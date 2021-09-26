/******************************************************************************
 JXBorderRect.h

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXBorderRect
#define _H_JXBorderRect

#include "jx-af/jx/JXDecorRect.h"

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
};

#endif
