/******************************************************************************
 RadioGroupPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_RadioGroupPanel
#define _H_RadioGroupPanel

#include "WidgetPanelBase.h"

class JXTextCheckbox;

class RadioGroupPanel : public WidgetPanelBase
{
public:

	RadioGroupPanel(WidgetParametersDialog* dlog, const bool hideBorder);

	~RadioGroupPanel();

	void	GetValues(bool* hideBorder);

private:

// begin Panel

	JXTextCheckbox* itsHideBorderCB;

// end Panel
};

#endif
