/******************************************************************************
 JXDecorRect.h

	Interface for the JXDecorRect class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDecorRect
#define _H_JXDecorRect

#include "JXWidget.h"

class JXDecorRect : public JXWidget
{
public:

	JXDecorRect(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~JXDecorRect() override;

	void	SetColor(const JColorID color);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	bool	NeedsInternalFTC() const override;
};

/******************************************************************************
 SetColor

 ******************************************************************************/

inline void
JXDecorRect::SetColor
	(
	const JColorID color
	)
{
	SetBackColor(color);
	SetFocusColor(color);
}

#endif
