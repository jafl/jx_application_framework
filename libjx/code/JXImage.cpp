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
			then rewrite JPSPrinter to ask for rgb instead of JColorID.

	BASE CLASS = JImage, public JBroadcaster

	Copyright (C) 1996-2010 by John Lindal.

 ******************************************************************************/

#include "JXImage.h"
#include "JXImageMask.h"
#include "JXImagePainter.h"
#include "JXDisplay.h"
#include "JXGC.h"
#include "JXColorManager.h"
#include "jXUtil.h"
#include <JStdError.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <JListUtil.h>
#include <X11/xpm.h>
#include <jAssert.h>

/******************************************************************************
 Constructor (empty)

	initState == kLocalStorage accepts origInitColor=0

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*			display,
	const JCoordinate	width,
	const JCoordinate	height,
	const JColorID		origInitColor,
	const JSize			depth,
	const State			initState
	)
	:
	JImage(width, height, display->GetColorManager())
{
	JXImageX(display, depth);

	const JColorID initColor =
		(origInitColor == kJXTransparentColor && itsDepth > 1 ?
		 JColorManager::GetDefaultBackColor() : origInitColor);

	if (initState == kRemoteStorage)
		{
		itsPixmap =
			XCreatePixmap(*itsDisplay, itsDisplay->GetRootWindow(),
						  width, height, itsDepth);
		assert( itsPixmap != None );

		// We need a private GC so we can draw.

		itsGC = jnew JXGC(itsDisplay, itsPixmap);
		assert( itsGC != nullptr );

		itsGC->SetDrawingColor(initColor);
		itsGC->FillRect(itsPixmap, 0,0, width, height);

		// We don't convert to an image because the user probably wants
		// to draw to us.
		}
	else
		{
		assert( initState == kLocalStorage );

		PrepareForImageData();

		if (origInitColor != 0)
			{
			for (JCoordinate x=0; x<width; x++)
				{
				for (JCoordinate y=0; y<height; y++)
					{
					SetColor(x,y, initColor);
					}
				}
			}
		}
}

/******************************************************************************
 Constructor (drawable)

	We assume that the given JXColorManager is the colormap used by the
	given Drawable.  (X does not seem to provide a way to check this
	because XGetWindowAttributes() only works on a Window, not a Pixmap.)
	This means that we assume that all the colors used by the Drawable
	have been registered with the JXColorManager.  If we didn't assume this,
	we would have to call XQueryColor for every pixel and that would
	be *really* slow.

	This routine can still be slow because we have to analyze the given
	Drawable and register all the colors that are used.

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*	display,
	Drawable	source
	)
	:
	JImage(0,0, display->GetColorManager())
{
	JXImageFromDrawable(display, source, JRect());
}

JXImage::JXImage
	(
	JXDisplay*		display,
	Drawable		source,
	const JRect&	rect
	)
	:
	JImage(rect.width(), rect.height(), display->GetColorManager())
{
	JXImageFromDrawable(display, source, rect);
}

// private

void
JXImage::JXImageFromDrawable
	(
	JXDisplay*		display,
	Drawable		source,
	const JRect&	origRect
	)
{
	JXImageX(display);

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
		itsGC = jnew JXGC(itsDisplay, itsPixmap);
		assert( itsGC != nullptr );
		}

	GetGC()->CopyPixels(source, rect.left, rect.top,
						  rect.width(), rect.height(), itsPixmap, 0,0);
}

/******************************************************************************
 Constructor (bitmap data)

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*			display,
	const JConstBitmap&	bitmap,
	const JColorID		origForeColor,
	const JColorID		origBackColor,
	const JSize			depth
	)
	:
	JImage(bitmap.w, bitmap.h, display->GetColorManager())
{
	JXImageX(display, depth);

	const JColorID foreColor =
		(origForeColor == kJXTransparentColor && itsDepth > 1 ?
		 JColorManager::GetBlackColor() : origForeColor);

	const JColorID backColor =
		(origBackColor == kJXTransparentColor && itsDepth > 1 ?
		 JColorManager::GetDefaultBackColor() : origBackColor);

	unsigned long forePixel, backPixel;
	if (itsDepth == 1)
		{
		forePixel = JXImageMask::ColorToBit(foreColor);
		backPixel = JXImageMask::ColorToBit(backColor);
		}
	else
		{
		forePixel = itsDisplay->GetColorManager()->GetXColor(foreColor);
		backPixel = itsDisplay->GetColorManager()->GetXColor(backColor);
		}

	itsPixmap =
		XCreatePixmapFromBitmapData(*itsDisplay, itsDisplay->GetRootWindow(),
									(char*) bitmap.data, bitmap.w, bitmap.h,
									forePixel, backPixel, itsDepth);
	assert( itsPixmap != None );

	if (itsDepth != itsDisplay->GetDepth())
		{
		itsGC = jnew JXGC(itsDisplay, itsPixmap);
		assert( itsGC != nullptr );
		}
}

/******************************************************************************
 Constructor (xpm data)

	This only works for XPM's with up to 256 colors.

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay*	display,
	const JXPM&	data
	)
	:
	JImage(0,0, display->GetColorManager())
{
	JXImageX(display);
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
	JXDisplay*			display
	)
	:
	JImage(width, height, display->GetColorManager())
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

	JXImageX(display, 1);

	itsPixmap = bitmap;

	itsGC = jnew JXGC(itsDisplay, itsPixmap);
	assert( itsGC != nullptr );
}

/******************************************************************************
 Constructor (private)

	This is used when reading from a file.

 ******************************************************************************/

JXImage::JXImage
	(
	JXDisplay* display
	)
	:
	JImage(0,0, display->GetColorManager())
{
	JXImageX(display);
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
	JXImageX(source.itsDisplay, source.itsDepth);
	itsDefState = source.itsDefState;

	if (source.itsGC != nullptr)
		{
		ForcePrivateGC();
		}

	if (source.itsPixmap != None)
		{
		itsPixmap = source.CreatePixmap();
		}

	if (source.itsImage != nullptr)
		{
		Pixmap tempPixmap = source.CreatePixmap();

		itsImage = XGetImage(*itsDisplay, tempPixmap,
							 0,0, GetWidth(), GetHeight(), AllPlanes, ZPixmap);
		assert( itsImage != nullptr );

		XFreePixmap(*itsDisplay, tempPixmap);
		}

	if (source.itsMask != nullptr)
		{
		itsMask = jnew JXImageMask(*(source.itsMask));
		assert( itsMask != nullptr );
		}
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
	JImage(rect.width(), rect.height(), source.GetColorManager())
{
	JXImageX(source.itsDisplay, source.itsDepth);
	itsDefState = source.itsDefState;

	if (source.itsGC != nullptr)
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

	GetGC()->CopyPixels(sourcePixmap, rect.left, rect.top,
						  GetWidth(), GetHeight(), itsPixmap, 0,0);

	if (source.itsPixmap == None)
		{
		XFreePixmap(*itsDisplay, sourcePixmap);
		}

	if (source.itsMask != nullptr)
		{
		itsMask = jnew JXImageMask(*(source.itsMask), rect);
		assert( itsMask != nullptr );
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
	const JSize	depth
	)
{
	itsDisplay  = display;
	itsGC       = nullptr;
	itsDepth    = (depth > 0 ? depth : itsDisplay->GetDepth());

	itsDefState = kRemoteStorage;
	itsPixmap   = None;
	itsImage    = nullptr;
	itsMask     = nullptr;
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

	if (itsImage != nullptr)
		{
		XDestroyImage(itsImage);
		}

	jdelete itsMask;
	jdelete itsGC;
}

/******************************************************************************
 Constructor function (GIF file) (static)

	Returns an error if the file does not contain a GIF.

 ******************************************************************************/

JError
JXImage::CreateFromGIF
	(
	JXDisplay*		display,
	const JString&	fileName,
	JXImage**		image
	)
{
	*image = jnew JXImage(display);
	assert( *image != nullptr );

	const JError err = (**image).ReadGIF(fileName);

	if (!err.OK())
		{
		jdelete *image;
		*image = nullptr;
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
	JXDisplay*		display,
	const JString&	fileName,
	JXImage**		image
	)
{
	JXColorManager* colorManager = display->GetColorManager();

	Pixmap image_pixmap = None;
	Pixmap mask_pixmap  = None;

	XpmAttributes attr;
	attr.valuemask          = XpmVisual | XpmColormap | XpmDepth |
							  XpmExactColors | XpmCloseness |
							  XpmColorKey | XpmAllocCloseColors |
							  XpmReturnAllocPixels;
	attr.visual             = colorManager->GetVisual();
	attr.colormap           = colorManager->GetXColormap();
	attr.depth              = display->GetDepth();
	attr.color_key          = XPM_COLOR;
	attr.alloc_pixels       = nullptr;
	attr.nalloc_pixels      = 0;
	attr.alloc_close_colors = kJTrue;	// so we can free all resulting pixels
	attr.exactColors        = 1;
	attr.closeness          = 0;

	const int xpmErr =
		XpmReadFileToPixmap(*display, display->GetRootWindow(),
							const_cast<JUtf8Byte*>(fileName.GetBytes()),
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
		if (attr.alloc_pixels != nullptr)
			{
			XFreeColors(*display, attr.colormap, attr.alloc_pixels, attr.nalloc_pixels, 0);
			}
		XpmFreeAttributes(&attr);
		return TooManyColors();
		}

	// create image and mask

	*image = jnew JXImage(display, image_pixmap);
	assert( *image != nullptr );

	XFreePixmap(*display, image_pixmap);

	if (mask_pixmap != None)
		{
		JXImageMask* mask = jnew JXImageMask(display, mask_pixmap);
		assert( mask != nullptr );
		(**image).SetMask(mask);

		XFreePixmap(*display, mask_pixmap);
		}

	// free pixels so X has usage count of 1

	XFreeColors(*display, attr.colormap, attr.alloc_pixels, attr.nalloc_pixels, 0);

	// clean up

	XpmFreeAttributes(&attr);
	return JNoError();
}

/******************************************************************************
 WriteXPM

	Returns an error if we are unable to create the specified file.

 ******************************************************************************/

JError
JXImage::WriteXPM
	(
	const JString& fileName
	)
	const
{
	XpmAttributes attr;
	attr.valuemask = XpmColormap;
	attr.colormap  = itsDisplay->GetColorManager()->GetXColormap();

	int xpmErr;
	if (itsImage != nullptr)
		{
		XImage* mask_image = nullptr;
		if (itsMask != nullptr)
			{
			itsMask->ConvertToImage();
			mask_image = ((JXImage*) itsMask)->itsImage;
			}

		xpmErr = XpmWriteFileFromImage(*itsDisplay,
									   const_cast<JUtf8Byte*>(fileName.GetBytes()),
									   itsImage, mask_image, &attr);
		}
	else
		{
		assert( itsPixmap != None );

		Pixmap mask_pixmap = None;
		if (itsMask != nullptr)
			{
			itsMask->ConvertToPixmap();
			mask_pixmap = ((JXImage*) itsMask)->itsPixmap;
			}

		xpmErr = XpmWriteFileFromPixmap(*itsDisplay,
										const_cast<JUtf8Byte*>(fileName.GetBytes()),
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
}

/******************************************************************************
 Copy (virtual)

	We can't support operator= because there are derived classes.

 ******************************************************************************/

JXImage*
JXImage::Copy()
	const
{
	JXImage* obj = jnew JXImage(*this);
	assert( obj != nullptr );
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
	return JConvertToBoolean( itsMask != nullptr );
}

void
JXImage::SetMask
	(
	JXImageMask* mask
	)
{
	assert( ((JXImage*) mask)->itsDisplay == itsDisplay );
	assert( mask->GetWidth() == GetWidth() && mask->GetHeight() == GetHeight() );

	jdelete itsMask;
	itsMask = mask;
}

void
JXImage::ClearMask()
{
	jdelete itsMask;
	itsMask = nullptr;
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
	Region origClipRegion = nullptr;
	JXImageMask* origClipPixmap = nullptr;
	if (itsMask != nullptr)
		{
		itsMask->ConvertToPixmap();

		gc->GetClipping(&origClipOffset, &origClipRegion, &origClipPixmap);

		JPoint maskOffset(destX, destY);
		const JPoint tempClipOffset = origClipOffset - maskOffset;
		const JRect imageBounds     = GetBounds();

		if (origClipRegion != nullptr)
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
		else if (origClipPixmap != nullptr)
			{
			JRect r = origClipPixmap->GetBounds();
			r.Shift(tempClipOffset);
			if (!JIntersection(r, imageBounds, &r))
				{
				jdelete origClipPixmap;
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

	if (itsImage != nullptr)
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

	if (itsMask != nullptr && origClipRegion != nullptr)
		{
		gc->SetClipRegion(origClipRegion);
		XDestroyRegion(origClipRegion);
		}
	else if (itsMask != nullptr && origClipPixmap != nullptr)
		{
		gc->SetClipPixmap(origClipOffset, *origClipPixmap);
		jdelete origClipPixmap;
		}
	else if (itsMask != nullptr)
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
	JXImagePainter* p = jnew JXImagePainter(this, itsPixmap, GetBounds(), nullptr);
	assert( p != nullptr );
	return p;
}

/******************************************************************************
 GetColor (virtual)

 ******************************************************************************/

JColorID
JXImage::GetColor
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	if (itsDepth == 1)
		{
		return JXImageMask::BitToColor(GetSystemColor(x,y));
		}
	else
		{
		return itsDisplay->GetColorManager()->GetColorID(GetSystemColor(x,y));
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
	const JCoordinate	x,
	const JCoordinate	y,
	const JColorID		color
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
		xPixel = itsDisplay->GetColorManager()->GetXColor(color);
		}

	XPutPixel(itsImage, x,y, xPixel);
}

/******************************************************************************
 GetSystemColor (virtual)

 ******************************************************************************/

unsigned long
JXImage::GetSystemColor
	(
	const JColorID color
	)
	const
{
	if (itsDepth == 1)
		{
		return JXImageMask::ColorToBit(color);
		}
	else
		{
		return itsDisplay->GetColorManager()->GetXColor(color);
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
	if (itsImage == nullptr)
		{
		ConvertToImage();
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
		GetGC()->CopyPixels(itsPixmap, 0,0, GetWidth(), GetHeight(),
							  newPixmap, 0,0);
		}
	else if (itsImage != nullptr)
		{
		GetGC()->CopyImage(itsImage, 0,0, GetWidth(), GetHeight(),
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
		ConvertToPixmap();
		}

	XImage* image = XGetImage(*itsDisplay, itsPixmap,
							  0,0, GetWidth(), GetHeight(), AllPlanes, ZPixmap);
	assert( image != nullptr );

	return image;
}

/******************************************************************************
 GetGC (protected)

 ******************************************************************************/

const JXGC*
JXImage::GetGC()
	const
{
	if (itsGC == nullptr)
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
	if (itsGC == nullptr && itsDepth != itsDisplay->GetDepth())
		{
		// We need a drawable of the appropriate depth in order to
		// create a usable GC.

		Pixmap tempPixmap =
			XCreatePixmap(*itsDisplay, itsDisplay->GetRootWindow(),
						  1,1, itsDepth);
		assert( tempPixmap != None );

		itsGC = jnew JXGC(itsDisplay, tempPixmap);
		assert( itsGC != nullptr );

		XFreePixmap(*itsDisplay, tempPixmap);
		}
	else if (itsGC == nullptr)
		{
		itsGC = jnew JXGC(itsDisplay, itsDisplay->GetRootWindow());
		assert( itsGC != nullptr );
		}
}

/******************************************************************************
 GetCurrentState

 ******************************************************************************/

JXImage::State
JXImage::GetCurrentState()
	const
{
	if (itsImage != nullptr)
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
	if (itsPixmap == None)
		{
		assert( itsImage != nullptr );

		JXImage* me = const_cast<JXImage*>(this);
		me->itsPixmap = CreatePixmap();

		XDestroyImage(itsImage);
		me->itsImage = nullptr;
		}
}

/******************************************************************************
 ConvertToImage (protected)

 ******************************************************************************/

void
JXImage::ConvertToImage()
	const
{
	if (itsImage == nullptr)
		{
		assert( itsPixmap != None );

		JXImage* me = const_cast<JXImage*>(this);
		me->itsImage = CreateXImage();

		XFreePixmap(*itsDisplay, itsPixmap);
		me->itsPixmap = None;
		}
}

/******************************************************************************
 SetImageData (virtual protected)

	colorTable[ imageData[x][y] ] is the JColorID for pixel (x,y)

	colorTable[ maskColor ] is the transparent color, which shouldn't be used

 ******************************************************************************/

void
JXImage::SetImageData
	(
	const JSize			colorCount,
	const JColorID*		colorTable,
	unsigned short**	imageData,
	const JBoolean		hasMask,
	const unsigned long	maskColor
	)
{
	const JCoordinate w = GetWidth();
	const JCoordinate h = GetHeight();

	PrepareForImageData();

	// convert color table to X pixel values

	unsigned long* xColorTable = jnew unsigned long [ colorCount ];
	assert( xColorTable != nullptr );

	for (JIndex i=0; i<colorCount; i++)
		{
		if (!hasMask || i != maskColor)
			{
			xColorTable[i] = itsDisplay->GetColorManager()->GetXColor(colorTable[i]);
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
				if (itsMask == nullptr)
					{
					itsMask = jnew JXImageMask(itsDisplay, w,h, kJTrue);
					assert( itsMask != nullptr );
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

	jdelete [] xColorTable;

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

	itsImage = XCreateImage(*itsDisplay, itsDisplay->GetColorManager()->GetVisual(), itsDepth,
							ZPixmap, 0, nullptr, w,h, bitmap_pad, 0);
	assert( itsImage != nullptr );

	itsImage->data = (char*) malloc(h * itsImage->bytes_per_line);
	assert( itsImage->data != nullptr );
}

/******************************************************************************
 ImageDataFinished (virtual protected)

 ******************************************************************************/

void
JXImage::ImageDataFinished()
{
	ConvertToDefaultState();

	if (itsMask != nullptr)
		{
		itsMask->ConvertToDefaultState();
		}
}
