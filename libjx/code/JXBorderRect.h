/******************************************************************************
 JXBorderRect.h

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXBorderRect
#define _H_JXBorderRect

#include "JXDecorRect.h"

class JXBorderRect : public JXDecorRect
{
public:

	JXBorderRect(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~JXBorderRect() override;

	JColorID	GetBorderColor() const;
	void		SetBorderColor(const JColorID color);

protected:

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	JColorID	itsColor;
};


/******************************************************************************
 Layout name

 ******************************************************************************/

inline JColorID
JXBorderRect::GetBorderColor()
	const
{
	return itsColor;
}

inline void
JXBorderRect::SetBorderColor
	(
	const JColorID color
	)
{
	itsColor = color;
}

#endif
