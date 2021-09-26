/******************************************************************************
 JXImageRadioButton.cpp

	BASE CLASS = JXRadioButton

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXImageRadioButton.h"
#include "jx-af/jx/JXImage.h"
#include "jx-af/jx/JXImageCache.h"
#include "jx-af/jx/JXWindowPainter.h"
#include "jx-af/jx/JXColorManager.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/jXPainterUtil.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageRadioButton::JXImageRadioButton
	(
	const JIndex		id,
	JXRadioGroup*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXRadioButton(id, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsImage         = nullptr;
	itsOwnsImageFlag = true;

	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageRadioButton::~JXImageRadioButton()
{
	if (itsOwnsImageFlag)
	{
		jdelete itsImage;
	}
}

/******************************************************************************
 SetBitmap

 ******************************************************************************/

void
JXImageRadioButton::SetBitmap
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

	itsOwnsImageFlag = true;

	SetBackColor(backColor);
	Refresh();
}

/******************************************************************************
 SetImage

 ******************************************************************************/

void
JXImageRadioButton::SetImage
	(
	const JXPM&		xpm,
	const JColorID	backColor
	)
{
	SetImage(GetDisplay()->GetImageCache()->GetImage(xpm), false, backColor);
}

/******************************************************************************
 SetImage

 ******************************************************************************/

void
JXImageRadioButton::SetImage
	(
	JXImage*			image,
	const bool		widgetOwnsImage,
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
		Refresh();
	}

	SetBackColor(backColor);
	itsOwnsImageFlag = widgetOwnsImage;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXImageRadioButton::Draw
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
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXImageRadioButton::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	const bool drawChecked = DrawChecked();
	const bool isActive    = IsActive();
	const JSize borderWidth    = GetBorderWidth();

	if (drawChecked && isActive)
	{
		JXDrawDownFrame(p, frame, borderWidth);
	}
	else if (isActive)
	{
		JXDrawUpFrame(p, frame, borderWidth);
	}
	else if (borderWidth > 0)
	{
		p.SetLineWidth(borderWidth);
		if (drawChecked)
		{
			p.SetPenColor(JColorManager::GetWhiteColor());
		}
		else
		{
			p.SetPenColor(JColorManager::GetInactiveLabelColor());
		}
		p.RectInside(frame);
	}
}
