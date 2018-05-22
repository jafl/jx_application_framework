/******************************************************************************
 JXImageButton.cpp

	Maintains a pushable button with a JXImage.

	BASE CLASS = JXButton

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXImageButton.h>
#include <JXWindowPainter.h>
#include <JXImage.h>
#include <JXColorManager.h>
#include <jAssert.h>

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
	itsImage         = NULL;
	itsOwnsImageFlag = kJTrue;
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
	assert( itsImage != NULL );

	itsOwnsImageFlag = kJTrue;

	SetBackColor(backColor);
	Refresh();
}

/******************************************************************************
 SetImage

 ******************************************************************************/

void
JXImageButton::SetImage
	(
	const JXPM&			xpm,
	const JColorID	backColor
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
JXImageButton::SetImage
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
	if (itsImage != NULL)
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
	if (itsImage != NULL)
		{
		p.Image(*itsImage, itsImage->GetBounds(), GetBounds());
		}
}
