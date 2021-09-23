/******************************************************************************
 JXImageCheckbox.h

	Interface for the JXImageCheckbox class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageCheckbox
#define _H_JXImageCheckbox

#include "JXCheckbox.h"
#include "jXConstants.h"
#include <JConstBitmap.h>
#include "JXPM.h"

class JXImage;

class JXImageCheckbox : public JXCheckbox
{
public:

	JXImageCheckbox(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~JXImageCheckbox();

	void	SetBitmap(const JConstBitmap& bitmap,
					  const JColorID foreColor = kJXTransparentColor,  // placeholder for GetBlackColor()
					  const JColorID backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()
	void	SetImage(const JXPM& xpm,
					 const JColorID backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()
	void	SetImage(JXImage* image, const bool widgetOwnsImage,
					 const JColorID backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	JXImage*	itsImage;
	bool		itsOwnsImageFlag;
};

#endif
