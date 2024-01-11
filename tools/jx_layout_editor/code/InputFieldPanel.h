/******************************************************************************
 InputFieldPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_InputFieldPanel
#define _H_InputFieldPanel

#include <jx-af/jcore/JBroadcaster.h>
#include "WidgetPanelBase.h"

class JString;
class JXInputField;
class JXIntegerInput;
class JXRegexInput;
class JXTextCheckbox;
class WidgetParametersDialog;

class InputFieldPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	InputFieldPanel(WidgetParametersDialog* dlog, const bool required,
					const JInteger minLength, const JInteger maxLength,
					const JString& regexPattern, const JString& regexErrorMsg,
					const bool wordWrap, const bool newlines);

	~InputFieldPanel();

	bool	Validate() const override;

	void	GetValues(bool* required,
					  JInteger* minLength, JInteger* maxLength,
					  JString* regexPattern, JString* regexErrorMsg,
					  bool* wordWrap, bool* newlines);

private:

// begin Panel

	JXTextCheckbox* itsValueRequiredCB;
	JXIntegerInput* itsMinLengthInput;
	JXIntegerInput* itsMaxLengthInput;
	JXRegexInput*   itsRegexInput;
	JXInputField*   itsRegexErrorMsgInput;
	JXTextCheckbox* itsWordWrapCB;
	JXTextCheckbox* itsAllowNewlinesCB;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, const bool required,
						const JInteger minLength, const JInteger maxLength,
						const JString& regexPattern, const JString& regexErrorMsg,
						const bool wordWrap, const bool newlines);

	void	ConfigureInput(JXIntegerInput* field, const JInteger value);
};

#endif
