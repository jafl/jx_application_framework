/******************************************************************************
 JXDownRect.h

	Interface for the JXDownRect class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDownRect
#define _H_JXDownRect

#include "jx-af/jx/JXDecorRect.h"

class JXDownRect : public JXDecorRect
{
public:

	JXDownRect(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	~JXDownRect();

protected:

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
};

#endif
