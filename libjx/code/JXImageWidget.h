/******************************************************************************
 JXImageWidget.h

	Interface for the JXImageWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageWidget
#define _H_JXImageWidget

#include "jx-af/jx/JXScrollableWidget.h"
#include "jx-af/jx/jXConstants.h"
#include <jx-af/jcore/JConstBitmap.h>
#include "jx-af/jcore/JXPM.h"

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

	~JXImageWidget() override;

	bool	HasImage() const;
	bool	GetImage(JXImage** image) const;
	void	SetImage(JXImage* image, const bool widgetOwnsImage,
					 const JColorID backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()
	void	SetBitmap(const JConstBitmap& bitmap,
					  const JColorID foreColor = kJXTransparentColor,  // placeholder for GetBlackColor()
					  const JColorID backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()
	void	SetXPM(const JXPM& data,
				   const JColorID backColor = kJXTransparentColor);    // placeholder for GetDefaultBackColor()

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JXImage*	itsImage;			// can be nullptr
	bool		itsOwnsImageFlag;

	JXAdjustIWBoundsTask*	itsAdjustBoundsTask;

private:

	void	NeedAdjustBounds();
	void	AdjustBounds();
};


/******************************************************************************
 HasImage

 ******************************************************************************/

inline bool
JXImageWidget::HasImage()
	const
{
	return itsImage != nullptr;
}

/******************************************************************************
 GetImage

 ******************************************************************************/

inline bool
JXImageWidget::GetImage
	(
	JXImage** image
	)
	const
{
	*image = itsImage;
	return itsImage != nullptr;
}

#endif
