/******************************************************************************
 JXImageWidget.cpp

	Maintains a scrollable image.

	BASE CLASS = JXScrollableWidget

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImageWidget.h>
#include <JXAdjustIWBoundsTask.h>
#include <JXWindowPainter.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageWidget::JXImageWidget
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXScrollableWidget(NULL, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsImage            = NULL;
	itsOwnsImageFlag    = kJTrue;
	itsAdjustBoundsTask = NULL;

	SetBorderWidth(0);
}

JXImageWidget::JXImageWidget
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsImage            = NULL;
	itsOwnsImageFlag    = kJTrue;
	itsAdjustBoundsTask = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageWidget::~JXImageWidget()
{
	if (itsOwnsImageFlag)
		{
		delete itsImage;
		}

	delete itsAdjustBoundsTask;
}

/******************************************************************************
 SetBitmap

 ******************************************************************************/

void
JXImageWidget::SetBitmap
	(
	const JConstBitmap&	bitmap,
	const JColorIndex	origForeColor,
	const JColorIndex	origBackColor
	)
{
	const JColorIndex foreColor =
		(origForeColor == kJXTransparentColor ?
		 (GetColormap())->GetBlackColor() : origForeColor);

	const JColorIndex backColor =
		(origBackColor == kJXTransparentColor ?
		 (GetColormap())->GetDefaultBackColor() : origBackColor);

	if (itsOwnsImageFlag)
		{
		delete itsImage;
		}

	itsImage = new JXImage(GetDisplay(), GetColormap(), bitmap, foreColor, backColor);
	assert( itsImage != NULL );

	itsOwnsImageFlag = kJTrue;

	AdjustBounds();

	SetBackColor(backColor);
	Refresh();
}

/******************************************************************************
 SetXPM

 ******************************************************************************/

void
JXImageWidget::SetXPM
	(
	const JXPM&			data,
	const JColorIndex	origBackColor
	)
{
	const JColorIndex backColor =
		(origBackColor == kJXTransparentColor ?
		 (GetColormap())->GetDefaultBackColor() : origBackColor);

	if (itsOwnsImageFlag)
		{
		delete itsImage;
		}

	itsImage = new JXImage(GetDisplay(), GetColormap(), data);
	assert( itsImage != NULL );

	itsOwnsImageFlag = kJTrue;

	AdjustBounds();

	SetBackColor(backColor);
	Refresh();
}

/******************************************************************************
 SetImage

 ******************************************************************************/

void
JXImageWidget::SetImage
	(
	JXImage*			image,
	const JBoolean		widgetOwnsImage,
	const JColorIndex	origBackColor
	)
{
	const JColorIndex backColor =
		(origBackColor == kJXTransparentColor ?
		 (GetColormap())->GetDefaultBackColor() : origBackColor);

	if (image != itsImage)
		{
		if (itsOwnsImageFlag)
			{
			delete itsImage;
			}

		itsImage = image;
		AdjustBounds();
		Refresh();
		}

	SetBackColor(backColor);
	itsOwnsImageFlag = widgetOwnsImage;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXImageWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsImage != NULL)
		{
		p.Image(*itsImage, itsImage->GetBounds(), GetBounds());
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXImageWidget::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXScrollableWidget::ApertureResized(dw,dh);
	NeedAdjustBounds();
}

/******************************************************************************
 NeedAdjustBounds (private)

	We can't call AdjustBounds() during a resize because it confuses
	everybody.  So we do it as soon as everything has settled down.

 ******************************************************************************/

void
JXImageWidget::NeedAdjustBounds()
{
	if (itsAdjustBoundsTask == NULL)
		{
		itsAdjustBoundsTask = new JXAdjustIWBoundsTask(this);
		assert( itsAdjustBoundsTask != NULL );
		(JXGetApplication())->InstallUrgentTask(itsAdjustBoundsTask);
		}
}

/******************************************************************************
 AdjustBounds (private)

	This works the same way as JXScrollableWidget::AdjustScrollbars().

 ******************************************************************************/

void
JXImageWidget::AdjustBounds()
{
	if (itsAdjustBoundsTask != NULL)
		{
		return;
		}

	JRect newBounds = GetAperture();
	if (itsImage != NULL)
		{
		newBounds = JCovering(newBounds, itsImage->GetBounds());
		}

	SetBounds(newBounds.width(), newBounds.height());
}
