/******************************************************************************
 BorderWidthPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_BorderWidthPanel
#define _H_BorderWidthPanel

#include "WidgetPanelBase.h"

class JXIntegerInput;

class BorderWidthPanel : public WidgetPanelBase
{
public:

	BorderWidthPanel(WidgetParametersDialog* dlog, const JSize width);

	~BorderWidthPanel();

	void	GetValues(JSize* width);

private:

// begin Panel

	JXIntegerInput* itsBorderWidthInput;

// end Panel
};

#endif
