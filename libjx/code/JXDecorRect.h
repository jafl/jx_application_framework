/******************************************************************************
 JXDecorRect.h

	Interface for the JXDecorRect class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDecorRect
#define _H_JXDecorRect

#include "jx-af/jx/JXWidget.h"

class JXDecorRect : public JXWidget
{
public:

	JXDecorRect(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXDecorRect();

	void	SetColor(const JColorID color);

protected:

	virtual void		Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual bool	NeedsInternalFTC() const override;
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
