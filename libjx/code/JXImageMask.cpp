/******************************************************************************
 JXImageMask.cpp

	Class representing a mask for an offscreen drawable.

	BASE CLASS = JXImage, JImageMask

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImageMask.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <jFileUtil.h>
#include <JStdError.h>
#include <jAssert.h>

const JColorIndex JXImageMask::kPixelOff = kJXTransparentColor;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageMask::JXImageMask
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCoordinate	width,
	const JCoordinate	height,
	const JBoolean		filled
	)
	:
	JXImage(display, colormap, width, height,
			(filled ? colormap->GetBlackColor() : kPixelOff), 1),
	JImageMask()
{
	SetDefaultState(kRemoteStorage);
}

JXImageMask::JXImageMask
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JConstBitmap&	bitmap
	)
	:
	JXImage(XCreateBitmapFromData(*display, display->GetRootWindow(),
								  (char*) bitmap.data, bitmap.w, bitmap.h),
			bitmap.w, bitmap.h, display, colormap),
	JImageMask()
{
	SetDefaultState(kRemoteStorage);
}

JXImageMask::JXImageMask
	(
	JXDisplay*	display,
	JXColormap*	colormap,
	Drawable	source
	)
	:
	JXImage(display, colormap, source),
	JImageMask()
{
#ifndef NDEBUG
	{
	Window rootWindow;
	int x,y;
	unsigned int w,h, bw, depth;
	const Status ok =
		XGetGeometry(*display, source, &rootWindow, &x, &y, &w, &h, &bw, &depth);
	assert( ok && depth == 1 );
	}
#endif

	SetDefaultState(kRemoteStorage);
}

JXImageMask::JXImageMask
	(
	JXDisplay*		display,
	JXColormap*		colormap,
	Drawable		source,
	const JRect&	rect
	)
	:
	JXImage(display, colormap, source, rect),
	JImageMask()
{
#ifndef NDEBUG
	{
	Window rootWindow;
	int x,y;
	unsigned int w,h, bw, depth;
	const Status ok =
		XGetGeometry(*display, source, &rootWindow, &x, &y, &w, &h, &bw, &depth);
	assert( ok && depth == 1 );
	}
#endif

	SetDefaultState(kRemoteStorage);
}

/******************************************************************************
 Constructor (mask out color)

	Calculate a mask to avoid drawing the specified color.

 ******************************************************************************/

JXImageMask::JXImageMask
	(
	const JXImage&		image,
	const JColorIndex	color
	)
	:
	JXImage(image.GetDisplay(), image.GetXColormap(),
			image.GetWidth(), image.GetHeight(),
			(image.GetColormap())->GetBlackColor(), 1),
	JImageMask()
{
	CalcMask(image, color);
	SetDefaultState(kRemoteStorage);
}

/******************************************************************************
 Constructor (private)

	This is used when reading from a file.

 ******************************************************************************/

JXImageMask::JXImageMask
	(
	const Pixmap		bitmap,
	const JCoordinate	width,
	const JCoordinate	height,
	JXDisplay*			display,
	JXColormap*			colormap
	)
	:
	JXImage(bitmap, width, height, display, colormap),
	JImageMask()
{
	SetDefaultState(kRemoteStorage);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JXImageMask::JXImageMask
	(
	const JXImageMask& source
	)
	:
	JXImage(source),
	JImageMask(source)
{
}

// partial image

JXImageMask::JXImageMask
	(
	const JXImageMask&	source,
	const JRect&		rect
	)
	:
	JXImage(source, rect),
	JImageMask()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageMask::~JXImageMask()
{
}

/******************************************************************************
 Constructor function (XBM file) (static)

	Returns an error if the file does not contain an XBM.

 ******************************************************************************/

JError
JXImageMask::CreateFromXBM
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCharacter*	fileName,
	JXImageMask**		mask
	)
{
	unsigned int w,h;
	Pixmap bitmap;
	int x_hot, y_hot;
	const int xbmErr =
		XReadBitmapFile(*display, display->GetRootWindow(),
						const_cast<JCharacter*>(fileName),
						&w, &h, &bitmap, &x_hot, &y_hot);
	if (xbmErr == BitmapOpenFailed && JFileExists(fileName))
		{
		return JAccessDenied(fileName);
		}
	else if (xbmErr == BitmapOpenFailed)
		{
		return JDirEntryDoesNotExist(fileName);
		}
	else if (xbmErr == BitmapFileInvalid)
		{
		return FileIsNotXBM(fileName);
		}
	else if (xbmErr == BitmapNoMemory)
		{
		return JNoProcessMemory();
		}

	*mask = new JXImageMask(bitmap, w,h, display, colormap);
	assert( *mask != NULL );

	return JNoError();
}

/******************************************************************************
 WriteXBM

	Returns an error if we are unable to create the specified file.

 ******************************************************************************/

JError
JXImageMask::WriteXBM
	(
	const JCharacter*	fileName,
	const JPoint&		hotSpot
	)
	const
{
	ConvertToPixmap();

	const int xbmErr =
		XWriteBitmapFile(*(GetDisplay()),
						 const_cast<JCharacter*>(fileName),
						 GetPixmap(), GetWidth(), GetHeight(),
						 hotSpot.x, hotSpot.y);
	if (xbmErr == BitmapOpenFailed)
		{
		return JAccessDenied(fileName);
		}
	else if (xbmErr == BitmapNoMemory)
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
JXImageMask::Copy()
	const
{
	JXImageMask* obj = new JXImageMask(*this);
	assert( obj != NULL );
	return obj;
}

/******************************************************************************
 ContainsPixel (virtual)

 ******************************************************************************/

JBoolean
JXImageMask::ContainsPixel
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JConvertToBoolean( GetColor(x,y) == (GetColormap())->GetBlackColor() );
}

/******************************************************************************
 AddPixel (virtual)

 ******************************************************************************/

void
JXImageMask::AddPixel
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	SetColor(x,y, (GetColormap())->GetBlackColor());
}

/******************************************************************************
 RemovePixel (virtual)

 ******************************************************************************/

void
JXImageMask::RemovePixel
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	SetColor(x,y, kPixelOff);
}

/******************************************************************************
 BitToColor (static)

	Convert the given bit from an X bitmap to a JColorIndex.

 ******************************************************************************/

JColorIndex
JXImageMask::BitToColor
	(
	const unsigned long	bit,
	JXColormap*			colormap
	)
{
	return (bit == 0 ? kPixelOff : colormap->GetBlackColor());
}
