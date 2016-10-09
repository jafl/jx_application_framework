/******************************************************************************
 JXImageRadioButton.cpp

	BASE CLASS = JXRadioButton

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXImageRadioButton.h>
#include <JXImage.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jAssert.h>

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
	itsImage         = NULL;
	itsOwnsImageFlag = kJTrue;

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
	const JColorIndex	origForeColor,
	const JColorIndex	origBackColor
	)
{
	const JColorIndex foreColor =
		(origForeColor == kJXTransparentColor ?
		 GetColormap()->GetBlackColor() : origForeColor);

	const JColorIndex backColor =
		(origBackColor == kJXTransparentColor ?
		 GetColormap()->GetDefaultBackColor() : origBackColor);

	if (itsOwnsImageFlag)
		{
		jdelete itsImage;
		}

	itsImage = jnew JXImage(GetDisplay(), bitmap, foreColor, backColor);
	assert( itsImage != NULL );

	itsOwnsImageFlag = kJTrue;

	SetBackColor(backColor);
	Refresh();
}

/******************************************************************************
 SetImage

 ******************************************************************************/

void
JXImageRadioButton::SetImage
	(
	const JXPM&			xpm,
	const JColorIndex	backColor
	)
{
	JXImage* image = jnew JXImage(GetDisplay(), xpm);
	assert( image != NULL );
	SetImage(image, kJTrue, backColor);
}

/******************************************************************************
 SetImage

 ******************************************************************************/

void
JXImageRadioButton::SetImage
	(
	JXImage*			image,
	const JBoolean		widgetOwnsImage,
	const JColorIndex	origBackColor
	)
{
	const JColorIndex backColor =
		(origBackColor == kJXTransparentColor ?
		 GetColormap()->GetDefaultBackColor() : origBackColor);

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
	if (itsImage != NULL)
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
	const JBoolean drawChecked = DrawChecked();
	const JBoolean isActive    = IsActive();
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
			p.SetPenColor(GetColormap()->GetWhiteColor());
			}
		else
			{
			p.SetPenColor(GetColormap()->GetInactiveLabelColor());
			}
		p.RectInside(frame);
		}
}
