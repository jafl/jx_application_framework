/******************************************************************************
 WidgetLabelPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_WidgetLabelPanel
#define _H_WidgetLabelPanel

class JString;
class JXInputField;
class JXTextCheckbox;
class WidgetParametersDialog;

class WidgetLabelPanel
{
public:

	WidgetLabelPanel(WidgetParametersDialog* dlog, const JString& label,
					 const JString& shortcuts);

	~WidgetLabelPanel();

	void	GetValues(JString* label, JString* shortcuts);

private:

// begin Panel

	JXInputField* itsLabelInput;
	JXInputField* itsShortcutsInput;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, const JString& label,
						const JString& shortcuts);
};

#endif
