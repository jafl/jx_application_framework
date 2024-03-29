/******************************************************************************
 JXImageButton.h

	Interface for the JXImageButton class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageButton
#define _H_JXImageButton

#include "JXButton.h"
#include "jXConstants.h"		// for kJXTransparentColor
#include <jx-af/jcore/JConstBitmap.h>
#include "jx-af/jcore/JXPM.h"

class JXImage;

class JXImageButton : public JXButton
{
public:

	JXImageButton(JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	~JXImageButton() override;

	void	SetBitmap(const JConstBitmap& bitmap,
					  const JColorID foreColor = kJXTransparentColor,  // placeholder for GetBlackColor()
					  const JColorID backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()
	void	SetXPM(const JXPM& xpm,
				   const JColorID backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()
	void	SetImage(JXImage* image, const bool widgetOwnsImage,
					 const JColorID backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()

	void	FitToImage();

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

private:

	JXImage*	itsImage;
	bool		itsOwnsImageFlag;
};

#endif
