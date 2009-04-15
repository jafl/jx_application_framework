/******************************************************************************
 JPagePrinter.cpp

	This class abstracts the functions required to draw to a printer.
	Derived classes must implement Page Setup and Print Setup dialogs and
	the following routines:

		WillPrintBlackWhite
			Returns kJTrue if the output will be black&white.

		OpenDocument
			Prepare to print.  (Page Setup and Print Setup dialogs should
			already have been successfully completed.)  Return kJFalse
			if an error occurs.

		NewPage
			Open a new page.  Return kJFalse if printing was cancelled.

		CloseDocument
			Send the result to the printer.

		CancelDocument
			Abort the process.  Subsequent calls to NewPage should return kJFalse.

	BASE CLASS = JPainter, JPrinter

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPagePrinter.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JPagePrinter::JPagePrinter
	(
	const JFontManager*	fontManager,
	const JColormap*	colormap,
	const JRect&		defaultClipRect
	)
	:
	JPainter(fontManager, colormap, defaultClipRect),
	JPrinter(),
	itsPageIndex(0),
	itsPageWidth(0),
	itsPageHeight(0)
{
	itsOrientation = kPortrait;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPagePrinter::~JPagePrinter()
{
}

/******************************************************************************
 LockHeader

	Lock off the top of the page so no more drawing can be done
	there.  This is done by adjusting the clipping.  The page rect
	is adjusted to reflect the change.

 ******************************************************************************/

void
JPagePrinter::LockHeader
	(
	const JCoordinate headerHeight
	)
{
	ShiftOrigin(0, headerHeight);
	SetPageSize(GetPageWidth(), GetPageHeight() - headerHeight);

	JRect defClipRect = GetDefaultClipRect();
	defClipRect.top += headerHeight;
	SetDefaultClipRect(defClipRect);

	ResetClipRect();
}

/******************************************************************************
 LockFooter

	Lock off the bottom of the page so no more drawing can be done
	there.  This is done by adjusting the clipping.  The page rect
	is adjusted to reflect the change.

 ******************************************************************************/

void
JPagePrinter::LockFooter
	(
	const JCoordinate footerHeight
	)
{
	SetPageSize(GetPageWidth(), GetPageHeight() - footerHeight);

	JRect defClipRect = GetDefaultClipRect();
	defClipRect.bottom -= footerHeight;
	SetDefaultClipRect(defClipRect);

	ResetClipRect();
}

/******************************************************************************
 ImageOrientationChanged (virtual protected)

	The default is to do nothing.

 ******************************************************************************/

void
JPagePrinter::ImageOrientationChanged
	(
	const ImageOrientation orient
	)
{
}

/******************************************************************************
 Global functions for JPagePrinter::ImageOrientation

 ******************************************************************************/

istream&
operator>>
	(
	istream&						input,
	JPagePrinter::ImageOrientation&	orientation
	)
{
	long temp;
	input >> temp;
	orientation = (JPagePrinter::ImageOrientation) temp;
	assert( orientation == JPagePrinter::kPortrait ||
			orientation == JPagePrinter::kLandscape );
	return input;
}

ostream&
operator<<
	(
	ostream&								output,
	const JPagePrinter::ImageOrientation	orientation
	)
{
	output << (long) orientation;
	return output;
}
