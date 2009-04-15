/******************************************************************************
 JXImageCheckbox.h

	Interface for the JXImageCheckbox class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageCheckbox
#define _H_JXImageCheckbox

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXCheckbox.h>
#include <jXConstants.h>
#include <JConstBitmap.h>
#include <JXPM.h>

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
					  const JColorIndex foreColor = kJXTransparentColor,  // placeholder for GetBlackColor()
					  const JColorIndex backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()
	void	SetImage(const JXPM& xpm,
					 const JColorIndex backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()
	void	SetImage(JXImage* image, const JBoolean widgetOwnsImage,
					 const JColorIndex backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	JXImage*	itsImage;
	JBoolean	itsOwnsImageFlag;

private:

	// not allowed

	JXImageCheckbox(const JXImageCheckbox& source);
	const JXImageCheckbox& operator=(const JXImageCheckbox& source);
};

#endif
