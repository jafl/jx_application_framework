/******************************************************************************
 JXEmbossedRect.h

	Interface for the JXEmbossedRect class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEmbossedRect
#define _H_JXEmbossedRect

#include <JXDecorRect.h>

class JXEmbossedRect : public JXDecorRect
{
public:

	JXEmbossedRect(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXEmbossedRect();

	void	SetUpWidth(const JSize upWidth);
	void	SetBetweenWidth(const JSize betweenWidth);
	void	SetDownWidth(const JSize downWidth);
	void	SetWidths(const JSize upWidth, const JSize betweenWidth,
					  const JSize downWidth);

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

private:

	JSize itsUpWidth, itsBetweenWidth, itsDownWidth;

private:

	void	UpdateBorderWidth();

	// not allowed

	JXEmbossedRect(const JXEmbossedRect& source);
	const JXEmbossedRect& operator=(const JXEmbossedRect& source);
};


/******************************************************************************
 Set widths

 ******************************************************************************/

inline void
JXEmbossedRect::SetUpWidth
	(
	const JSize upWidth
	)
{
	itsUpWidth = upWidth;
	UpdateBorderWidth();
}

inline void
JXEmbossedRect::SetBetweenWidth
	(
	const JSize betweenWidth
	)
{
	itsBetweenWidth = betweenWidth;
	UpdateBorderWidth();
}

inline void
JXEmbossedRect::SetDownWidth
	(
	const JSize downWidth
	)
{
	itsDownWidth = downWidth;
	UpdateBorderWidth();
}

inline void
JXEmbossedRect::SetWidths
	(
	const JSize upWidth,
	const JSize betweenWidth,
	const JSize downWidth
	)
{
	itsUpWidth      = upWidth;
	itsBetweenWidth = betweenWidth;
	itsDownWidth    = downWidth;
	UpdateBorderWidth();
}

/******************************************************************************
 UpdateBorderWidth (private)

 ******************************************************************************/

inline void
JXEmbossedRect::UpdateBorderWidth()
{
	SetBorderWidth(itsDownWidth + itsBetweenWidth + itsUpWidth);
}

#endif
