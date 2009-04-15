/******************************************************************************
 JXImage.cpp

	Class representing an offscreen drawable and XImage.  These are combined
	into a single class because the issue of where the image is stored should
	be hidden, for convenience.  It is, however, *very* expensive to convert
	an image between a drawable (remote storage) and an XImage (local storage).
	Therefore, do not interleave GetColor()/SetColor() with drawing commands
	(via JXImagePainter).  Collect them into blocks of calls.  (This is usually
	what one does anyway.)

	Planned features:
		For image read from file: store raw rgb values separately and
			then rewrite JPSPrinter to ask for rgb instead of JColorIndex.

	BASE CLASS = JImage, virtual JBroadcaster

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImage.h>
#include <JXImageMask.h>
#include <JXImagePainter.h>
#include <JXDisplay.h>
#include <JXGC.h>
#include <JXColormap.h>
#include <jXUtil.h>
#include <JStdError.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <JOrderedSetUtil.h>

#ifdef _J_HAS_XPM
#include <xpm.h>
#endif

#include <jAssert.h>

/******************************************************************************
 Constructor (empty)

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCoordinate	width,
	const JCoordinate	height,
	const JColorIndex	origInitColor,
	const JSize			depth
	)
	:
	JImage(width, height, colormap)
{
	JXImageX(display, colormap, depth);

	const JColorIndex initColor =
		(origInitColor == kJXTransparentColor && itsDepth > 1 ?
		 colormap->GetDefaultBackColor() : origInitColor);

	itsPixmap =
		XCreatePixmap(*itsDisplay, itsDisplay->GetRootWindow(),
					  width, height, itsDepth);
	assert( itsPixmap != None );

	// We need a private GC so we can draw.

	itsGC = new JXGC(itsDisplay, itsColormap, itsPixmap);
	assert( itsGC != NULL );

	itsGC->SetDrawingColor(initColor);
	itsGC->FillRect(itsPixmap, 0,0, width, height);

	RegisterColor(initColor);

	// We don't convert to an image because the user probably wants
	// to draw to us.
}

/******************************************************************************
 Constructor (drawable)

	We assume that the given JXColormap is the colormap used by the
	given Drawable.  (X does not seem to provide a way to check this
	because XGetWindowAttributes() only works on a Window, not a Pixmap.)
	This means that we assume that all the colors used by the Drawable
	have been registered with the JXColormap.  If we didn't assume this,
	we would have to call XQueryColor for every pixel and that would
	be *really* slow.

	This routine can still be slow because we have to analyze the given
	Drawable and register all the colors that are used.

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*	display,
	JXColormap*	colormap,
	Drawable	source
	)
	:
	JImage(0,0, colormap)
{
	JXImageFromDrawable(display, colormap, source, JRect());
}

JXImage::JXImage
	(
	JXDisplay*		display,
	JXColormap*		colormap,
	Drawable		source,
	const JRect&	rect
	)
	:
	JImage(rect.width(), rect.height(), colormap)
{
	JXImageFromDrawable(display, colormap, source, rect);
}

// private

void
JXImage::JXImageFromDrawable
	(
	JXDisplay*		display,
	JXColormap*		colormap,
	Drawable		source,
	const JRect&	origRect
	)
{
	JXImageX(display, colormap);

	JRect rect = origRect;
	{
	Window rootWindow;
	int x,y;
	unsigned int w,h, bw, depth;
	XGetGeometry(*itsDisplay, source, &rootWindow, &x, &y, &w, &h, &bw, &depth);

	itsDepth = depth;
	if (rect.IsEmpty())
		{
		SetDimensions(w,h);
		rect = GetBounds();
		}
	}

	itsPixmap = XCreatePixmap(*itsDisplay, itsDisplay->GetRootWindow(),
							  GetWidth(), GetHeight(), itsDepth);
	assert( itsPixmap != None );

	if (itsDepth != itsDisplay->GetDepth())
		{
		itsGC = new JXGC(itsDisplay, itsColormap, itsPixmap);
		assert( itsGC != NULL );
		}

	(GetGC())->CopyPixels(source, rect.left, rect.top,
						  rect.width(), rect.height(), itsPixmap, 0,0);

	// register the colors that are used in the drawable

	const JBoolean needRegister =
		JConvertToBoolean( itsDepth > 1 && !itsColormap->AllColorsPreallocated() );

	if (needRegister && itsColormap->GetVisualClass() == DirectColor)
		{
		// With DirectColor, there could be just as many colors as pixels,
		// so we actually waste time (and memory!) by building a list of
		// X pixel values before converting them to JColorIndices.

		ConvertToImage();

		const JCoordinate w = GetWidth();
		const JCoordinate h = GetHeight();
		for (JCoordinate y=0; y<=h; y++)
			{
			for (JCoordinate x=0; x<=w; x++)
				{
				JColorIndex color;
				const unsigned long xPixel = XGetPixel(itsImage, x,y);
				itsColormap->AllocateStaticColor(xPixel, &color);
				RegisterColor(color, kJFalse);
				}
			}
		}
	else if (needRegister)	// using PseudoColor => small # of X pixels
		{
		// By building a list of unique X pixel values and then converting
		// each once, we reduce O(W H (alloc)) to O(W H) + O(P (alloc)),
		// where W=width, H=height, P=# of distinct pixels in Drawable.
		// Typically, W*H is much larger than P.  Alloc usually requires
		// a server call.

		JIndex i;

		// build boolean array telling which X pixel values are used

		ConvertToImage();

		const JSize maxColorCount = itsColormap->GetMaxColorCount();
		char* pixelUsed = new char [ maxColorCount ];
		assert( pixelUsed != NULL );
		for (i=0; i<maxColorCount; i++)
			{
			pixelUsed[i] = 0;
			}

		const JCoordinate w = GetWidth();
		const JCoordinate h = GetHeight();
		for (JCoordinate y=0; y<=h; y++)
			{
			for (JCoordinate x=0; x<=w; x++)
				{
				const unsigned long xPixel = XGetPixel(itsImage, x,y);
				assert( xPixel < maxColorCount );
				pixelUsed [ xPixel ] = 1;
				}
			}

		// register each X pixel value that is used

		JColorIndex color;
		for (i=0; i<maxColorCount; i++)
			{
			if (pixelUsed[i])
				{
				itsColormap->AllocateStaticColor(i, &color);
				RegisterColor(color, kJFalse);
				}
			}

		delete [] pixelUsed;
		}
}

/******************************************************************************
 Constructor (bitmap data)

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JConstBitmap&	bitmap,
	const JColorIndex	origForeColor,
	const JColorIndex	origBackColor,
	const JSize			depth
	)
	:
	JImage(bitmap.w, bitmap.h, colormap)
{
	JXImageX(display, colormap, depth);

	const JColorIndex foreColor =
		(origForeColor == kJXTransparentColor && itsDepth > 1 ?
		 colormap->GetBlackColor() : origForeColor);

	const JColorIndex backColor =
		(origBackColor == kJXTransparentColor && itsDepth > 1 ?
		 colormap->GetDefaultBackColor() : origBackColor);

	unsigned long forePixel, backPixel;
	if (itsDepth == 1)
		{
		forePixel = JXImageMask::ColorToBit(foreColor);
		backPixel = JXImageMask::ColorToBit(backColor);
		}
	else
		{
		forePixel = itsColormap->GetXPixel(foreColor);
		backPixel = itsColormap->GetXPixel(backColor);
		}

	itsPixmap =
		XCreatePixmapFromBitmapData(*itsDisplay, itsDisplay->GetRootWindow(),
									(char*) bitmap.data, bitmap.w, bitmap.h,
									forePixel, backPixel, itsDepth);
	assert( itsPixmap != None );

	if (itsDepth != itsDisplay->GetDepth())
		{
		itsGC = new JXGC(itsDisplay, itsColormap, itsPixmap);
		assert( itsGC != NULL );
		}

	RegisterColor(foreColor);
	RegisterColor(backColor);
}

/******************************************************************************
 Constructor (xpm data)

	This only works for XPM's with up to 256 colors.

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*	display,
	JXColormap*	colormap,
	const JXPM&	data
	)
	:
	JImage(0,0, colormap)
{
	JXImageX(display, colormap);
	ReadFromJXPM(data);
}

/******************************************************************************
 Constructor (private)

	This is used by JXImageMask.  The depth is assumed to be 1.

 ******************************************************************************/

JXImage::JXImage
	(
	const Pixmap		bitmap,
	const JCoordinate	width,
	const JCoordinate	height,
	JXDisplay*			display,
	JXColormap*			colormap
	)
	:
	JImage(width, height, colormap)
{
#ifndef NDEBUG
	{
	Window rootWindow;
	int x,y;
	unsigned int w,h, bw, depth;
	const Status ok =
		XGetGeometry(*display, bitmap, &rootWindow, &x, &y, &w, &h, &bw, &depth);
	assert( ok && depth == 1 );
	}
#endif

	JXImageX(display, colormap, 1);

	itsPixmap = bitmap;

	itsGC = new JXGC(itsDisplay, itsColormap, itsPixmap);
	assert( itsGC != NULL );
}

/******************************************************************************
 Constructor (private)

	This is used when reading from a file.

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*	display,
	JXColormap*	colormap
	)
	:
	JImage(0,0, colormap)
{
	JXImageX(display, colormap);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JXImage::JXImage
	(
	const JXImage& source
	)
	:
	JImage(source)
{
	JXImageX(source.itsDisplay, source.itsColormap, source.itsDepth);
	itsDefState = source.itsDefState;

	if (source.itsGC != NULL)
		{
		ForcePrivateGC();
		}

	if (source.itsPixmap != None)
		{
		itsPixmap = source.CreatePixmap();
		}

	if (source.itsImage != NULL)
		{
		Pixmap tempPixmap = source.CreatePixmap();

		itsImage = XGetImage(*itsDisplay, tempPixmap,
							 0,0, GetWidth(), GetHeight(), AllPlanes, ZPixmap);
		assert( itsImage != NULL );

		XFreePixmap(*itsDisplay, tempPixmap);
		}

	if (source.itsMask != NULL)
		{
		itsMask = new JXImageMask(*(source.itsMask));
		assert( itsMask != NULL );
		}

	CopyColorList(source);
}

/******************************************************************************
 Copy constructor (partial image)

 ******************************************************************************/

JXImage::JXImage
	(
	const JXImage&	source,
	const JRect&	rect
	)
	:
	JImage(rect.width(), rect.height(), source.GetColormap())
{
	JXImageX(source.itsDisplay, source.itsColormap, source.itsDepth);
	itsDefState = source.itsDefState;

	if (source.itsGC != NULL)
		{
		ForcePrivateGC();
		}

	Pixmap sourcePixmap;
	if (source.itsPixmap != None)
		{
		sourcePixmap = source.itsPixmap;
		}
	else
		{
		sourcePixmap = source.CreatePixmap();
		}

	itsPixmap = XCreatePixmap(*itsDisplay, itsDisplay->GetRootWindow(),
							  GetWidth(), GetHeight(), itsDepth);
	assert( itsPixmap != None );

	(GetGC())->CopyPixels(sourcePixmap, rect.left, rect.top,
						  GetWidth(), GetHeight(), itsPixmap, 0,0);

	if (source.itsPixmap == None)
		{
		XFreePixmap(*itsDisplay, sourcePixmap);
		}

	if (source.itsMask != NULL)
		{
		itsMask = new JXImageMask(*(source.itsMask), rect);
		assert( itsMask != NULL );
		}

	CopyColorList(source);
}

/******************************************************************************
 CopyColorList (private)

	We have to tell the colormap that we need the colors copied from source.

 ******************************************************************************/

void
JXImage::CopyColorList
	(
	const JXImage& source
	)
{
	assert( itsColorList == NULL );

	if (source.itsColorList != NULL)
		{
		itsColorList = new JArray<JColorIndex>(*(source.itsColorList));
		assert( itsColorList != NULL );

		const JSize colorCount = itsColorList->GetElementCount();
		for (JIndex i=1; i<=colorCount; i++)
			{
			itsColormap->UsingColor(itsColorList->GetElement(i));
			}
		}
}

/******************************************************************************
 JXImageX (private)

	Initializes instance variables.

 ******************************************************************************/

void
JXImage::JXImageX
	(
	JXDisplay*	display,
	JXColormap*	colormap,
	const JSize	depth
	)
{
	itsDisplay   = display;
	itsColormap  = colormap;
	itsGC        = NULL;
	itsDepth     = (depth > 0 ? depth : itsDisplay->GetDepth());

	itsDefState  = kRemoteStorage;
	itsPixmap    = None;
	itsImage     = NULL;
	itsMask      = NULL;

	itsColorList = NULL;

	ListenTo(itsColormap);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImage::~JXImage()
{
	if (itsPixmap != None)
		{
		XFreePixmap(*itsDisplay, itsPixmap);
		}

	if (itsImage != NULL)
		{
		XDestroyImage(itsImage);
		}

	delete itsMask;
	delete itsGC;

	if (itsColorList != NULL)
		{
		const JSize colorCount = itsColorList->GetElementCount();
		for (JIndex i=1; i<=colorCount; i++)
			{
			itsColormap->DeallocateColor(itsColorList->GetElement(i));
			}

		delete itsColorList;
		}
}

/******************************************************************************
 Constructor function (any file) (static)

	Returns an error if the file does not contain a recognizable image format.

 ******************************************************************************/

JError
JXImage::CreateFromFile
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCharacter*	fileName,
	JXImage**			image,
	const JBoolean		allowApproxColors
	)
{
	const FileType type = GetFileType(fileName);
	if (type == kGIFType)
		{
		return CreateFromGIF(display, colormap, fileName, image, allowApproxColors);
		}
	else if (type == kPNGType)
		{
		return CreateFromPNG(display, colormap, fileName, image, allowApproxColors);
		}
	else if (type == kJPEGType)
		{
		return CreateFromJPEG(display, colormap, fileName, image, allowApproxColors);
		}
	else if (type == kXPMType)
		{
		return CreateFromXPM(display, colormap, fileName, image, allowApproxColors);
		}
	else
		{
		*image = NULL;
		return UnknownFileType(fileName);
		}
}

/******************************************************************************
 Constructor function (GIF file) (static)

	Returns an error if the file does not contain a GIF.

 ******************************************************************************/

JError
JXImage::CreateFromGIF
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCharacter*	fileName,
	JXImage**			image,
	const JBoolean		allowApproxColors
	)
{
	*image = new JXImage(display, colormap);
	assert( *image != NULL );

	const JBoolean saveApprox = colormap->WillApproximateColors();
	const JBoolean savePre    = colormap->WillPreemptivelyApproximateColors();
	colormap->ShouldApproximateColors(allowApproxColors);
	colormap->ShouldPreemptivelyApproximateColors(kJTrue);

	const JError err = (**image).ReadGIF(fileName);

	colormap->ShouldApproximateColors(saveApprox);
	colormap->ShouldPreemptivelyApproximateColors(savePre);

	if (!err.OK())
		{
		delete *image;
		*image = NULL;
		}
	return err;
}

/******************************************************************************
 Constructor function (PNG file) (static)

	Returns an error if the file does not contain a PNG.

 ******************************************************************************/

JError
JXImage::CreateFromPNG
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCharacter*	fileName,
	JXImage**			image,
	const JBoolean		allowApproxColors
	)
{
	*image = new JXImage(display, colormap);
	assert( *image != NULL );

	const JBoolean saveApprox = colormap->WillApproximateColors();
	const JBoolean savePre    = colormap->WillPreemptivelyApproximateColors();
	colormap->ShouldApproximateColors(allowApproxColors);
	colormap->ShouldPreemptivelyApproximateColors(kJTrue);

	const JError err = (**image).ReadPNG(fileName);

	colormap->ShouldApproximateColors(saveApprox);
	colormap->ShouldPreemptivelyApproximateColors(savePre);

	if (!err.OK())
		{
		delete *image;
		*image = NULL;
		}
	return err;
}

/******************************************************************************
 Constructor function (JPEG file) (static)

	Returns an error if the file does not contain a JPEG.

 ******************************************************************************/

JError
JXImage::CreateFromJPEG
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCharacter*	fileName,
	JXImage**			image,
	const JBoolean		allowApproxColors
	)
{
	*image = new JXImage(display, colormap);
	assert( *image != NULL );

	const JBoolean saveApprox = colormap->WillApproximateColors();
	const JBoolean savePre    = colormap->WillPreemptivelyApproximateColors();
	colormap->ShouldApproximateColors(allowApproxColors);
	colormap->ShouldPreemptivelyApproximateColors(kJTrue);

	const JError err = (**image).ReadJPEG(fileName);

	colormap->ShouldApproximateColors(saveApprox);
	colormap->ShouldPreemptivelyApproximateColors(savePre);

	if (!err.OK())
		{
		delete *image;
		*image = NULL;
		}
	return err;
}

/******************************************************************************
 Constructor function (XPM file) (static)

	Returns an error if the file does not contain an XPM.

 ******************************************************************************/

JError
JXImage::CreateFromXPM
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCharacter*	fileName,
	JXImage**			image,
	const JBoolean		allowApproxColors
	)
{
#ifdef _J_HAS_XPM

	Pixmap image_pixmap = None;
	Pixmap mask_pixmap  = None;

	XpmAttributes attr;
	attr.valuemask          = XpmVisual | XpmColormap | XpmDepth |
							  XpmExactColors | XpmCloseness |
							  XpmColorKey | XpmAllocCloseColors |
							  XpmReturnAllocPixels;
	attr.visual             = colormap->GetVisual();
	attr.colormap           = colormap->GetXColormap();
	attr.depth              = display->GetDepth();
	attr.color_key          = XPM_COLOR;
	attr.alloc_pixels       = NULL;
	attr.nalloc_pixels      = 0;
	attr.alloc_close_colors = kJTrue;	// so we can free all resulting pixels

	if (allowApproxColors)
		{
		attr.exactColors = 0;
		attr.closeness   = 40000;		// recommended by XPM docs
		}
	else
		{
		attr.exactColors = 1;
		attr.closeness   = 0;
		}

	const int xpmErr =
		XpmReadFileToPixmap(*display, display->GetRootWindow(),
							const_cast<JCharacter*>(fileName),
							&image_pixmap, &mask_pixmap, &attr);
	if (xpmErr == XpmOpenFailed && JFileExists(fileName))
		{
		return JAccessDenied(fileName);
		}
	else if (xpmErr == XpmOpenFailed)
		{
		return JDirEntryDoesNotExist(fileName);
		}
	else if (xpmErr == XpmFileInvalid)
		{
		return FileIsNotXPM(fileName);
		}
	else if (xpmErr == XpmNoMemory)
		{
		return JNoProcessMemory();
		}
	else if (xpmErr == XpmColorFailed || xpmErr == XpmColorError)
		{
		if (image_pixmap != None)
			{
			XFreePixmap(*display, image_pixmap);
			}
		if (mask_pixmap != None)
			{
			XFreePixmap(*display, mask_pixmap);
			}
		if (attr.alloc_pixels != NULL)
			{
			XFreeColors(*display, attr.colormap, attr.alloc_pixels, attr.nalloc_pixels, 0);
			}
		XpmFreeAttributes(&attr);
		return TooManyColors();
		}

	// create image and mask

	*image = new JXImage(display, colormap, image_pixmap);
	assert( *image != NULL );

	XFreePixmap(*display, image_pixmap);

	if (mask_pixmap != None)
		{
		JXImageMask* mask = new JXImageMask(display, colormap, mask_pixmap);
		assert( mask != NULL );
		(**image).SetMask(mask);

		XFreePixmap(*display, mask_pixmap);
		}

	// free pixels so X has usage count of 1

	XFreeColors(*display, attr.colormap, attr.alloc_pixels, attr.nalloc_pixels, 0);

	// clean up

	XpmFreeAttributes(&attr);
	return JNoError();

#else

	return XPMNotAvailable();

#endif
}

/******************************************************************************
 WriteXPM

	Returns an error if we are unable to create the specified file.

 ******************************************************************************/

JError
JXImage::WriteXPM
	(
	const JCharacter* fileName
	)
	const
{
#ifdef _J_HAS_XPM

	XpmAttributes attr;
	attr.valuemask = XpmColormap;
	attr.colormap  = itsColormap->GetXColormap();

	int xpmErr;
	if (itsImage != NULL)
		{
		XImage* mask_image = NULL;
		if (itsMask != NULL)
			{
			itsMask->ConvertToImage();
			mask_image = ((JXImage*) itsMask)->itsImage;
			}

		xpmErr = XpmWriteFileFromImage(*itsDisplay, const_cast<JCharacter*>(fileName),
									   itsImage, mask_image, &attr);
		}
	else
		{
		assert( itsPixmap != None );

		Pixmap mask_pixmap = None;
		if (itsMask != NULL)
			{
			itsMask->ConvertToPixmap();
			mask_pixmap = ((JXImage*) itsMask)->itsPixmap;
			}

		xpmErr = XpmWriteFileFromPixmap(*itsDisplay, const_cast<JCharacter*>(fileName),
										itsPixmap, mask_pixmap, &attr);
		}

	if (xpmErr == XpmOpenFailed)
		{
		return JAccessDenied(fileName);
		}
	else if (xpmErr == XpmNoMemory)
		{
		return JNoProcessMemory();
		}
	else
		{
		return JNoError();
		}

#else

	return XPMNotAvailable();

#endif
}

/******************************************************************************
 Copy (virtual)

	We can't support operator= because there are derived classes.

 ******************************************************************************/

JXImage*
JXImage::Copy()
	const
{
	JXImage* obj = new JXImage(*this);
	assert( obj != NULL );
	return obj;
}

/******************************************************************************
 Mask

 ******************************************************************************/

JBoolean
JXImage::GetMask		// virtual
	(
	JImageMask** mask
	)
	const
{
	*mask = itsMask;
	return JConvertToBoolean( itsMask != NULL );
}

void
JXImage::SetMask
	(
	JXImageMask* mask
	)
{
	assert( ((JXImage*) mask)->itsDisplay == itsDisplay );
	assert( mask->GetWidth() == GetWidth() && mask->GetHeight() == GetHeight() );

	delete itsMask;
	itsMask = mask;
}

void
JXImage::ClearMask()
{
	delete itsMask;
	itsMask = NULL;
}

/******************************************************************************
 Draw

	Copy the specified portion of the image, centered within destRect.

 ******************************************************************************/

void
JXImage::Draw
	(
	const Drawable	drawable,
	JXGC*			gc,
	const JRect&	srcRect,
	const JRect&	destRect
	)
	const
{
	assert( gc->GetDisplay() == itsDisplay );

	// calculate position in destination drawable

	const JCoordinate destX = (destRect.left + destRect.right - srcRect.width())/2;
	const JCoordinate destY = (destRect.top + destRect.bottom - srcRect.height())/2;

	// intersect gc's clipping and our mask

	JPoint origClipOffset;
	Region origClipRegion = NULL;
	JXImageMask* origClipPixmap = NULL;
	if (itsMask != NULL)
		{
		itsMask->ConvertToPixmap();

		gc->GetClipping(&origClipOffset, &origClipRegion, &origClipPixmap);

		JPoint maskOffset(destX, destY);
		const JPoint tempClipOffset = origClipOffset - maskOffset;
		const JRect imageBounds     = GetBounds();

		if (origClipRegion != NULL)
			{
			JRect r = JXGetRegionBounds(origClipRegion);
			r.Shift(tempClipOffset);
			if (!JIntersection(r, imageBounds, &r))
				{
				XDestroyRegion(origClipRegion);
				return;
				}

			Pixmap mask = JXIntersection(itsDisplay, origClipRegion, tempClipOffset,
										 *itsMask, JPoint(0,0), imageBounds);
			gc->SetClipPixmap(maskOffset, mask);
			}
		else if (origClipPixmap != NULL)
			{
			JRect r = origClipPixmap->GetBounds();
			r.Shift(tempClipOffset);
			if (!JIntersection(r, imageBounds, &r))
				{
				delete origClipPixmap;
				return;
				}

			Pixmap mask = JXIntersection(itsDisplay, *origClipPixmap, tempClipOffset,
										 *itsMask, JPoint(0,0), imageBounds);
			gc->SetClipPixmap(maskOffset, mask);
			}
		else
			{
			gc->SetClipPixmap(maskOffset, *itsMask);
			}
		}

	// draw the image

	if (itsImage != NULL)
		{
		gc->CopyImage(itsImage, srcRect.left, srcRect.top,
					  srcRect.width(), srcRect.height(),
					  drawable, destX, destY);
		}
	else if (itsPixmap != None)
		{
		gc->CopyPixels(itsPixmap, srcRect.left, srcRect.top,
					   srcRect.width(), srcRect.height(),
					   drawable, destX, destY);
		}

	// restore the clipping

	if (itsMask != NULL && origClipRegion != NULL)
		{
		gc->SetClipRegion(origClipRegion);
		XDestroyRegion(origClipRegion);
		}
	else if (itsMask != NULL && origClipPixmap != NULL)
		{
		gc->SetClipPixmap(origClipOffset, *origClipPixmap);
		delete origClipPixmap;
		}
	else if (itsMask != NULL)
		{
		gc->ClearClipping();
		}
}

/******************************************************************************
 CreatePainter

 ******************************************************************************/

JXImagePainter*
JXImage::CreatePainter()
{
	ConvertToPixmap();
	JXImagePainter* p = new JXImagePainter(this, itsPixmap, GetBounds(), NULL);
	assert( p != NULL );
	return p;
}

/******************************************************************************
 GetColor (virtual)

 ******************************************************************************/

JColorIndex
JXImage::GetColor
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	if (itsDepth == 1)
		{
		return JXImageMask::BitToColor(GetSystemColor(x,y), itsColormap);
		}
	else
		{
		JColorIndex color;
		const JBoolean found = itsColormap->GetColorIndex(GetSystemColor(x,y), &color);
		assert( found );
		return color;
		}
}

/******************************************************************************
 SetColor (virtual)

	In the long run, it is usually more efficient to convert to an XImage
	once, rather than sending a large number of XDrawPoint() messages to
	the server.

 ******************************************************************************/

void
JXImage::SetColor
	(
	const JCoordinate x,
	const JCoordinate y,
	const JColorIndex color
	)
{
	ConvertToImage();

	unsigned long xPixel;
	if (itsDepth == 1)
		{
		xPixel = JXImageMask::ColorToBit(color);
		}
	else
		{
		xPixel = itsColormap->GetXPixel(color);
		}

	XPutPixel(itsImage, x,y, xPixel);
}

/******************************************************************************
 RegisterColor (private)

	Make a note that we are using the given color.  If tellColormap is kJTrue,
	tell the colormap that we need the specified color.

 ******************************************************************************/

void
JXImage::RegisterColor
	(
	const JColorIndex	color,
	const JBoolean		tellColormap
	)
{
	if (itsDepth > 1)
		{
		if (itsColorList == NULL)
			{
			itsColorList = new JArray<JColorIndex>;
			assert( itsColorList != NULL );
			itsColorList->SetCompareFunction(JCompareIndices);
			}

		JBoolean isDuplicate;
		const JIndex index =
			itsColorList->GetInsertionSortIndex(color, &isDuplicate);
		if (!isDuplicate)
			{
			itsColorList->InsertElementAtIndex(index, color);
			if (tellColormap)
				{
				itsColormap->UsingColor(color);
				}
			}
		}
}

// called by JXImagePainter

void
JXImage::RegisterColors
	(
	const JArray<JColorIndex>&	colorList,
	const JBoolean				tellColormap
	)
{
	if (itsDepth > 1)
		{
		const JSize count = colorList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			RegisterColor(colorList.GetElement(i), tellColormap);
			}
		}
}

/******************************************************************************
 GetSystemColor (virtual)

 ******************************************************************************/

unsigned long
JXImage::GetSystemColor
	(
	const JColorIndex color
	)
	const
{
	if (itsDepth == 1)
		{
		return JXImageMask::ColorToBit(color);
		}
	else
		{
		return itsColormap->GetXPixel(color);
		}
}

unsigned long
JXImage::GetSystemColor
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	if (itsImage == NULL)
		{
		JXImage* me = const_cast<JXImage*>(this);
		me->ConvertToImage();
		}

	return XGetPixel(itsImage, x,y);
}

/******************************************************************************
 CreatePixmap

	The caller is responsible for calling XFreePixmap().

 ******************************************************************************/

Pixmap
JXImage::CreatePixmap()
	const
{
	Pixmap newPixmap =
		XCreatePixmap(*itsDisplay, itsDisplay->GetRootWindow(),
					  GetWidth(), GetHeight(), itsDepth);
	assert( newPixmap != None );

	if (itsPixmap != None)
		{
		(GetGC())->CopyPixels(itsPixmap, 0,0, GetWidth(), GetHeight(),
							  newPixmap, 0,0);
		}
	else if (itsImage != NULL)
		{
		(GetGC())->CopyImage(itsImage, 0,0, GetWidth(), GetHeight(),
							 newPixmap, 0,0);
		}

	return newPixmap;
}

/******************************************************************************
 CreateXImage

	The caller is responsible for calling XDestroyImage().

 ******************************************************************************/

XImage*
JXImage::CreateXImage()
	const
{
	if (itsPixmap == None)
		{
		JXImage* me = const_cast<JXImage*>(this);
		me->ConvertToPixmap();
		}

	XImage* image = XGetImage(*itsDisplay, itsPixmap,
							  0,0, GetWidth(), GetHeight(), AllPlanes, ZPixmap);
	assert( image != NULL );

	return image;
}

/******************************************************************************
 GetGC (protected)

 ******************************************************************************/

const JXGC*
JXImage::GetGC()
	const
{
	if (itsGC == NULL)
		{
		return itsDisplay->GetDefaultGC();
		}
	else
		{
		return itsGC;
		}
}

/******************************************************************************
 ForcePrivateGC (protected)

 ******************************************************************************/

void
JXImage::ForcePrivateGC()
{
	if (itsGC == NULL && itsDepth != itsDisplay->GetDepth())
		{
		// We need a drawable of the appropriate depth in order to
		// create a usable GC.

		Pixmap tempPixmap =
			XCreatePixmap(*itsDisplay, itsDisplay->GetRootWindow(),
						  1,1, itsDepth);
		assert( tempPixmap != None );

		itsGC = new JXGC(itsDisplay, itsColormap, tempPixmap);
		assert( itsGC != NULL );

		XFreePixmap(*itsDisplay, tempPixmap);
		}
	else if (itsGC == NULL)
		{
		itsGC = new JXGC(itsDisplay, itsColormap, itsDisplay->GetRootWindow());
		assert( itsGC != NULL );
		}
}

/******************************************************************************
 GetCurrentState

 ******************************************************************************/

JXImage::State
JXImage::GetCurrentState()
	const
{
	if (itsImage != NULL)
		{
		return kLocalStorage;
		}
	else
		{
		assert( itsPixmap != None );
		return kRemoteStorage;
		}
}

/******************************************************************************
 ConvertToDefaultState

 ******************************************************************************/

void
JXImage::ConvertToDefaultState()
{
	if (itsDefState == kLocalStorage)
		{
		ConvertToLocalStorage();
		}
	else
		{
		assert( itsDefState == kRemoteStorage );
		ConvertToRemoteStorage();
		}
}

/******************************************************************************
 ConvertToPixmap (protected)

 ******************************************************************************/

void
JXImage::ConvertToPixmap()
	const
{
	JXImage* me = const_cast<JXImage*>(this);

	if (itsPixmap == None)
		{
		assert( itsImage != NULL );

		me->itsPixmap = CreatePixmap();

		XDestroyImage(itsImage);
		me->itsImage = NULL;
		}
}

/******************************************************************************
 ConvertToImage (protected)

 ******************************************************************************/

void
JXImage::ConvertToImage()
	const
{
	JXImage* me = const_cast<JXImage*>(this);

	if (itsImage == NULL)
		{
		assert( itsPixmap != None );

		me->itsImage = CreateXImage();

		XFreePixmap(*itsDisplay, itsPixmap);
		me->itsPixmap = None;
		}
}

/******************************************************************************
 Receive (protected)

	Update our pixel values.

 ******************************************************************************/

void
JXImage::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsColormap && message.Is(JXColormap::kNewColormap) &&
		itsDepth > 1 && (itsPixmap != None || itsImage != NULL))
		{
		const JXColormap::NewColormap* info =
			dynamic_cast(const JXColormap::NewColormap*, &message);
		assert( info != NULL );

		ConvertToImage();

		const JCoordinate width  = GetWidth();
		const JCoordinate height = GetHeight();
		for (JCoordinate y=0; y<height; y++)
			{
			for (JCoordinate x=0; x<width; x++)
				{
				const unsigned long xPixel = XGetPixel(itsImage, x,y);
				XPutPixel(itsImage, x,y, info->ConvertPixel(xPixel));
				}
			}

		ConvertToDefaultState();
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 SetImageData (virtual protected)

	colorTable[ imageData[x][y] ] is the JColorIndex for pixel (x,y)

	colorTable[ maskColor ] is the transparent color, which shouldn't be used

 ******************************************************************************/

void
JXImage::SetImageData
	(
	const JSize			colorCount,
	const JColorIndex*	colorTable,
	unsigned short**	imageData,
	const JBoolean		hasMask,
	const unsigned long	maskColor
	)
{
	const JCoordinate w = GetWidth();
	const JCoordinate h = GetHeight();

	PrepareForImageData();

	// convert color table to X pixel values

	unsigned long* xColorTable = new unsigned long [ colorCount ];
	assert( xColorTable != NULL );

	for (JIndex i=0; i<colorCount; i++)
		{
		if (!hasMask || i != maskColor)
			{
			RegisterColor(colorTable[i], kJFalse);
			xColorTable[i] = itsColormap->GetXPixel(colorTable[i]);
			}
		}

	// put data into image

	for (JCoordinate x=0; x<w; x++)
		{
		for (JCoordinate y=0; y<h; y++)
			{
			const unsigned short color = imageData[x][y];

			if (hasMask && color == maskColor)
				{
				if (itsMask == NULL)
					{
					itsMask = new JXImageMask(itsDisplay, itsColormap, w,h, kJTrue);
					assert( itsMask != NULL );
					}
				itsMask->RemovePixel(x,y);
				XPutPixel(itsImage, x,y, 0);
				}
			else
				{
				XPutPixel(itsImage, x,y, xColorTable[color]);
				}
			}
		}

	// clean up

	delete [] xColorTable;

	ImageDataFinished();
}

/******************************************************************************
 PrepareForImageData (virtual protected)

 ******************************************************************************/

void
JXImage::PrepareForImageData()
{
	const JCoordinate w = GetWidth();
	const JCoordinate h = GetHeight();

	const int bitmap_pad = (itsDepth > 16 ? 32 : (itsDepth > 8 ? 16 : 8));

	itsImage = XCreateImage(*itsDisplay, itsColormap->GetVisual(), itsDepth,
							ZPixmap, 0, NULL, w,h, bitmap_pad, 0);
    assert( itsImage != NULL );

    itsImage->data = (char*) malloc(h * itsImage->bytes_per_line);
    assert( itsImage->data != NULL );
}

/******************************************************************************
 ImageDataFinished (virtual protected)

 ******************************************************************************/

void
JXImage::ImageDataFinished()
{
	ConvertToDefaultState();

	if (itsMask != NULL)
		{
		itsMask->ConvertToDefaultState();
		}
}

#define JTemplateType JXImage
#include <JPtrArray.tmpls>
#undef JTemplateType
