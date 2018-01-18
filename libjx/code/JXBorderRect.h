/******************************************************************************
 JXBorderRect.h

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXBorderRect
#define _H_JXBorderRect

#include <JXDecorRect.h>

class JXBorderRect : public JXDecorRect
{
public:

	JXBorderRect(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXBorderRect();

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;


private:

	// not allowed

	JXBorderRect(const JXBorderRect& source);
	const JXBorderRect& operator=(const JXBorderRect& source);
};

#endif
