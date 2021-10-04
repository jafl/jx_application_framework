/******************************************************************************
 JXWidgetSet.h

	Interface for the JXWidgetSet class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWidgetSet
#define _H_JXWidgetSet

#include "jx-af/jx/JXWidget.h"

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

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	bool	NeedsInternalFTC() const override;

private:

	bool	itsNeedsInternalFTCFlag;
};


/******************************************************************************
 SetNeedsInternalFTC

 ******************************************************************************/

inline void
JXWidgetSet::SetNeedsInternalFTC()
{
	itsNeedsInternalFTCFlag = true;
}

#endif
