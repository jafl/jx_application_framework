/******************************************************************************
 ComplexBorderWidthPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ComplexBorderWidthPanel
#define _H_ComplexBorderWidthPanel

#include "WidgetPanelBase.h"

class JXIntegerInput;

class ComplexBorderWidthPanel : public WidgetPanelBase
{
public:

	ComplexBorderWidthPanel(WidgetParametersDialog* dlog, const JSize outside,
							const JSize between, const JSize inside);

	~ComplexBorderWidthPanel();

	void	GetValues(JSize* outside, JSize* between, JSize* inside);

private:

// begin Panel

	JXIntegerInput* itsOutsideWidthInput;
	JXIntegerInput* itsBetweenWidthInput;
	JXIntegerInput* itsInsideWidthInput;

// end Panel
};

#endif
