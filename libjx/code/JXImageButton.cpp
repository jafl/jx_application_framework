/******************************************************************************
 JXImageButton.cpp

	Maintains a pushable button with a JXImage.

	BASE CLASS = JXButton

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXImageButton.h"
#include "JXWindowPainter.h"
#include "JXImage.h"
#include "JXImageCache.h"
#include "JXColorManager.h"
#include "JXDisplay.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageButton::JXImageButton
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
	JXButton(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsImage         = nullptr;
	itsOwnsImageFlag = true;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageButton::~JXImageButton()
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
JXImageButton::SetBitmap
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
JXImageButton::SetXPM
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
JXImageButton::SetImage
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
 FitToImage

 ******************************************************************************/

void
JXImageButton::FitToImage()
{
	if (itsImage != nullptr)
	{
		const JSize bw = GetBorderWidth();
		SetSize(itsImage->GetWidth() + 2*bw,
				itsImage->GetHeight() + 2*bw);
	}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXImageButton::Draw
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
