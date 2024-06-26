/******************************************************************************
 JXImageMask.cpp

	Class representing a mask for an offscreen drawable.

	BASE CLASS = JXImage, JImageMask

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "JXImageMask.h"
#include "JXDisplay.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/JStdError.h>
#include <X11/Xutil.h>
#include <jx-af/jcore/jAssert.h>

const JColorID JXImageMask::kPixelOff = kJXTransparentColor;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageMask::JXImageMask
	(
	JXDisplay*			display,
	const JCoordinate	width,
	const JCoordinate	height,
	const bool			filled
	)
	:
	JXImage(display, width, height,
			(filled ? JColorManager::GetBlackColor() : kPixelOff), 1),
	JImageMask()
{
	SetDefaultState(kRemoteStorage);
}

JXImageMask::JXImageMask
	(
	JXDisplay*			display,
	const JConstBitmap&	bitmap
	)
	:
	JXImage(XCreateBitmapFromData(*display, display->GetRootWindow(),
								  (char*) bitmap.data, bitmap.w, bitmap.h),
			bitmap.w, bitmap.h, display),
	JImageMask()
{
	SetDefaultState(kRemoteStorage);
}

JXImageMask::JXImageMask
	(
	JXDisplay*	display,
	Drawable	source
	)
	:
	JXImage(display, source),
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
	Drawable		source,
	const JRect&	rect
	)
	:
	JXImage(display, source, rect),
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
	const JXImage&	image,
	const JColorID	color
	)
	:
	JXImage(image.GetDisplay(), image.GetWidth(), image.GetHeight(),
			JColorManager::GetBlackColor(), 1),
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
	JXDisplay*			display
	)
	:
	JXImage(bitmap, width, height, display),
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
	JXDisplay*		display,
	const JString&	fileName,
	JXImageMask**	mask
	)
{
	unsigned int w,h;
	Pixmap bitmap;
	int x_hot, y_hot;
	const int xbmErr =
		XReadBitmapFile(*display, display->GetRootWindow(),
						const_cast<JUtf8Byte*>(fileName.GetBytes()),
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

	*mask = jnew JXImageMask(bitmap, w,h, display);
	return JNoError();
}

/******************************************************************************
 WriteXBM

	Returns an error if we are unable to create the specified file.

 ******************************************************************************/

JError
JXImageMask::WriteXBM
	(
	const JString&	fileName,
	const JPoint&	hotSpot
	)
	const
{
	ConvertToPixmap();

	const int xbmErr =
		XWriteBitmapFile(*(GetDisplay()),
						 const_cast<JUtf8Byte*>(fileName.GetBytes()),
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
	auto* obj = jnew JXImageMask(*this);
	return obj;
}

/******************************************************************************
 ContainsPixel (virtual)

 ******************************************************************************/

bool
JXImageMask::ContainsPixel
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return GetColor(x,y) == JColorManager::GetBlackColor();
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
	SetColor(x,y, JColorManager::GetBlackColor());
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

	Convert the given bit from an X bitmap to a JColorID.

 ******************************************************************************/

JColorID
JXImageMask::BitToColor
	(
	const unsigned long	bit
	)
{
	return (bit == 0 ? kPixelOff : JColorManager::GetBlackColor());
}
