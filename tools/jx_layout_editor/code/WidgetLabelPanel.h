/******************************************************************************
 WidgetLabelPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_WidgetLabelPanel
#define _H_WidgetLabelPanel

#include "WidgetPanelBase.h"

class JString;
class JXInputField;
class JXTextCheckbox;

class WidgetLabelPanel : public WidgetPanelBase
{
public:

	WidgetLabelPanel(WidgetParametersDialog* dlog, const JString& label,
					 const JString& shortcuts);

	~WidgetLabelPanel();

	bool	Validate() const override;

	void	GetValues(JString* label, JString* shortcuts);

private:

// begin Panel

	JXInputField* itsLabelInput;
	JXInputField* itsShortcutsInput;

// end Panel
};

#endif
