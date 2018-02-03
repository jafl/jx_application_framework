/******************************************************************************
 JXDecorRect.h

	Interface for the JXDecorRect class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDecorRect
#define _H_JXDecorRect

#include <JXWidget.h>

class JXDecorRect : public JXWidget
{
public:

	JXDecorRect(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXDecorRect();

	void	SetColor(const JColorIndex color);

protected:

	virtual void		Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual JBoolean	NeedsInternalFTC() const override;

private:

	// not allowed

	JXDecorRect(const JXDecorRect& source);
	const JXDecorRect& operator=(const JXDecorRect& source);
};

/******************************************************************************
 SetColor

 ******************************************************************************/

inline void
JXDecorRect::SetColor
	(
	const JColorIndex color
	)
{
	SetBackColor(color);
	SetFocusColor(color);
}

#endif
