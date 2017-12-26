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

protected:

	virtual void		Draw(JXWindowPainter& p, const JRect& rect);
	virtual void		DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual JBoolean	NeedsInternalFTC() const;

private:

	JBoolean	itsNeedsInternalFTCFlag;

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

#endif
