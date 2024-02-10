/******************************************************************************
 RadioButtonIDPanel.h

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_RadioButtonIDPanel
#define _H_RadioButtonIDPanel

#include "WidgetPanelBase.h"

class JString;
class JXInputField;

class RadioButtonIDPanel : public WidgetPanelBase
{
public:

	RadioButtonIDPanel(WidgetParametersDialog* dlog, const JString& id);

	~RadioButtonIDPanel();

	void	GetValues(JString* id);

private:

// begin Panel

	JXInputField* itsIDInput;

// end Panel
};

#endif
