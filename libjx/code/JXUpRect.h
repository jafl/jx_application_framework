/******************************************************************************
 JXUpRect.h

	Interface for the JXUpRect class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUpRect
#define _H_JXUpRect

#include "JXDecorRect.h"

class JXUpRect : public JXDecorRect
{
public:

	JXUpRect(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual ~JXUpRect();

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	// not allowed

	JXUpRect(const JXUpRect& source);
	const JXUpRect& operator=(const JXUpRect& source);
};

#endif
