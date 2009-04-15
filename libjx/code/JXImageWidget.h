/******************************************************************************
 JXImageWidget.h

	Interface for the JXImageWidget class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageWidget
#define _H_JXImageWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXScrollableWidget.h>
#include <jXConstants.h>
#include <JConstBitmap.h>
#include "JXPM.h"

class JXImage;
class JXAdjustIWBoundsTask;

class JXImageWidget : public JXScrollableWidget
{
	friend class JXAdjustIWBoundsTask;

public:

	JXImageWidget(JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	JXImageWidget(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~JXImageWidget();

	JBoolean	HasImage() const;
	JBoolean	GetImage(JXImage** image) const;
	void		SetImage(JXImage* image, const JBoolean widgetOwnsImage,
						 const JColorIndex backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()
	void		SetBitmap(const JConstBitmap& bitmap,
						  const JColorIndex foreColor = kJXTransparentColor,  // placeholder for GetBlackColor()
						  const JColorIndex backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()
	void		SetXPM(const JXPM& data,
					   const JColorIndex backColor = kJXTransparentColor);    // placeholder for GetDefaultBackColor()

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

private:

	JXImage*	itsImage;			// can be NULL
	JBoolean	itsOwnsImageFlag;

	JXAdjustIWBoundsTask*	itsAdjustBoundsTask;

private:

	void	NeedAdjustBounds();
	void	AdjustBounds();

	// not allowed

	JXImageWidget(const JXImageWidget& source);
	const JXImageWidget& operator=(const JXImageWidget& source);
};


/******************************************************************************
 HasImage

 ******************************************************************************/

inline JBoolean
JXImageWidget::HasImage()
	const
{
	return JConvertToBoolean( itsImage != NULL );
}

/******************************************************************************
 GetImage

 ******************************************************************************/

inline JBoolean
JXImageWidget::GetImage
	(
	JXImage** image
	)
	const
{
	*image = itsImage;
	return JConvertToBoolean( itsImage != NULL );
}

#endif
