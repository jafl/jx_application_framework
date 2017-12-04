/******************************************************************************
 JXWidgetSet.h

	Interface for the JXWidgetSet class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWidgetSet
#define _H_JXWidgetSet

#include <JXWidget.h>

class JXWidgetSet : public JXWidget
{
public:

	JXWidgetSet(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXWidgetSet();

	void	SetNeedsInternalFTC();
	void	SetExtraNeededSpace(const JCoordinate dw, const JCoordinate dh);

protected:

	virtual void		Draw(JXWindowPainter& p, const JRect& rect);
	virtual void		DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual JBoolean	NeedsInternalFTC() const;
	virtual JBoolean	RunInternalFTC(const JBoolean horizontal, JCoordinate* newSize);

private:

	JBoolean	itsNeedsInternalFTCFlag;
	JPoint		itsExtraNeededSpace;	// space required to fit original size

private:

	// not allowed

	JXWidgetSet(const JXWidgetSet& source);
	const JXWidgetSet& operator=(const JXWidgetSet& source);
};


/******************************************************************************
 SetNeedsInternalFTC

 ******************************************************************************/

inline void
JXWidgetSet::SetNeedsInternalFTC()
{
	itsNeedsInternalFTCFlag = kJTrue;
}

/******************************************************************************
 SetNeedsInternalFTC

 ******************************************************************************/

inline void
JXWidgetSet::SetExtraNeededSpace
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	itsExtraNeededSpace.Set(dw, dh);
}

#endif
