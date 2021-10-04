/******************************************************************************
 JXImageRadioButton.h

	Interface for the JXImageRadioButton class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageRadioButton
#define _H_JXImageRadioButton

#include "jx-af/jx/JXRadioButton.h"
#include "jx-af/jx/jXConstants.h"
#include <jx-af/jcore/JConstBitmap.h>
#include "jx-af/jcore/JXPM.h"

class JXImage;

class JXImageRadioButton : public JXRadioButton
{
public:

	JXImageRadioButton(const JIndex id, JXRadioGroup* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~JXImageRadioButton();

	void	SetBitmap(const JConstBitmap& bitmap,
					  const JColorID foreColor = kJXTransparentColor,  // placeholder for GetBlackColor()
					  const JColorID backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()
	void	SetImage(const JXPM& xpm,
					 const JColorID backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()
	void	SetImage(JXImage* image, const bool widgetOwnsImage,
					 const JColorID backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	JXImage*	itsImage;
	bool	itsOwnsImageFlag;
};

#endif
