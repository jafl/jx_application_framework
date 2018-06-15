/******************************************************************************
 JXImageWidget.cpp

	Maintains a scrollable image.

	BASE CLASS = JXScrollableWidget

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXImageWidget.h"
#include "JXAdjustIWBoundsTask.h"
#include "JXWindowPainter.h"
#include "JXImage.h"
#include "JXColorManager.h"
#include "jXGlobals.h"
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
	JXScrollableWidget(nullptr, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsImage            = nullptr;
	itsOwnsImageFlag    = kJTrue;
	itsAdjustBoundsTask = nullptr;

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
	itsImage            = nullptr;
	itsOwnsImageFlag    = kJTrue;
	itsAdjustBoundsTask = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageWidget::~JXImageWidget()
{
	if (itsOwnsImageFlag)
		{
		jdelete itsImage;
		}

	jdelete itsAdjustBoundsTask;
}

/******************************************************************************
 SetBitmap

 ******************************************************************************/

void
JXImageWidget::SetBitmap
	(
	const JConstBitmap&	bitmap,
	const JColorID	origForeColor,
	const JColorID	origBackColor
	)
{
	const JColorID foreColor =
		(origForeColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origForeColor);

	const JColorID backColor =
		(origBackColor == kJXTransparentColor ?
		 JColorManager::GetDefaultBackColor() : origBackColor);

	if (itsOwnsImageFlag)
		{
		jdelete itsImage;
		}

	itsImage = jnew JXImage(GetDisplay(), bitmap, foreColor, backColor);
	assert( itsImage != nullptr );

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
	const JColorID	origBackColor
	)
{
	const JColorID backColor =
		(origBackColor == kJXTransparentColor ?
		 JColorManager::GetDefaultBackColor() : origBackColor);

	if (itsOwnsImageFlag)
		{
		jdelete itsImage;
		}

	itsImage = jnew JXImage(GetDisplay(), data);
	assert( itsImage != nullptr );

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
	const JColorID	origBackColor
	)
{
	const JColorID backColor =
		(origBackColor == kJXTransparentColor ?
		 JColorManager::GetDefaultBackColor() : origBackColor);

	if (image != itsImage)
		{
		if (itsOwnsImageFlag)
			{
			jdelete itsImage;
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
	if (itsImage != nullptr)
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
	if (itsAdjustBoundsTask == nullptr)
		{
		itsAdjustBoundsTask = jnew JXAdjustIWBoundsTask(this);
		assert( itsAdjustBoundsTask != nullptr );
		itsAdjustBoundsTask->Go();
		}
}

/******************************************************************************
 AdjustBounds (private)

	This works the same way as JXScrollableWidget::AdjustScrollbars().

 ******************************************************************************/

void
JXImageWidget::AdjustBounds()
{
	if (itsAdjustBoundsTask != nullptr)
		{
		return;
		}

	JRect newBounds = GetAperture();
	if (itsImage != nullptr)
		{
		newBounds = JCovering(newBounds, itsImage->GetBounds());
		}

	SetBounds(newBounds.width(), newBounds.height());
}
