/******************************************************************************
 JXFlatRect.h

	Interface for the JXFlatRect class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFlatRect
#define _H_JXFlatRect

#include "JXDecorRect.h"

class JXFlatRect : public JXDecorRect
{
public:

	JXFlatRect(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	~JXFlatRect() override;

protected:

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
};

#endif
