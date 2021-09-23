/******************************************************************************
 JXEngravedRect.h

	Interface for the JXEngravedRect class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEngravedRect
#define _H_JXEngravedRect

#include "JXDecorRect.h"

class JXEngravedRect : public JXDecorRect
{
public:

	JXEngravedRect(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXEngravedRect();

	void	SetDownWidth(const JSize downWidth);
	void	SetBetweenWidth(const JSize betweenWidth);
	void	SetUpWidth(const JSize upWidth);
	void	SetWidths(const JSize downWidth, const JSize betweenWidth,
					  const JSize upWidth);

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	JSize itsDownWidth, itsBetweenWidth, itsUpWidth;

private:

	void	UpdateBorderWidth();
};


/******************************************************************************
 Set widths

 ******************************************************************************/

inline void
JXEngravedRect::SetDownWidth
	(
	const JSize downWidth
	)
{
	itsDownWidth = downWidth;
	UpdateBorderWidth();
}

inline void
JXEngravedRect::SetBetweenWidth
	(
	const JSize betweenWidth
	)
{
	itsBetweenWidth = betweenWidth;
	UpdateBorderWidth();
}

inline void
JXEngravedRect::SetUpWidth
	(
	const JSize upWidth
	)
{
	itsUpWidth = upWidth;
	UpdateBorderWidth();
}

inline void
JXEngravedRect::SetWidths
	(
	const JSize downWidth,
	const JSize betweenWidth,
	const JSize upWidth
	)
{
	itsDownWidth    = downWidth;
	itsBetweenWidth = betweenWidth;
	itsUpWidth      = upWidth;
	UpdateBorderWidth();
}

/******************************************************************************
 UpdateBorderWidth (private)

 ******************************************************************************/

inline void
JXEngravedRect::UpdateBorderWidth()
{
	SetBorderWidth(itsDownWidth + itsBetweenWidth + itsUpWidth);
}

#endif
