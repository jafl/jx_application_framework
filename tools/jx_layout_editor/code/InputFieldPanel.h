/******************************************************************************
 InputFieldPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_InputFieldPanel
#define _H_InputFieldPanel

#include <jx-af/jcore/JBroadcaster.h>

class JString;
class JXInputField;
class JXIntegerInput;
class JXRegexInput;
class JXTextCheckbox;
class WidgetParametersDialog;

class InputFieldPanel : public JBroadcaster
{
public:

	InputFieldPanel(WidgetParametersDialog* dlog, const bool required,
					const JInteger minLength, const JInteger maxLength,
					const JString& regexPattern, const JString& regexErrorMsg);

	~InputFieldPanel();

	void	GetValues(bool* required,
					  JInteger* minLength, JInteger* maxLength,
					  JString* regexPattern, JString* regexErrorMsg);

private:

// begin Panel

	JXTextCheckbox* itsValueRequiredCB;
	JXIntegerInput* itsMinLengthInput;
	JXIntegerInput* itsMaxLengthInput;
	JXRegexInput*   itsRegexInput;
	JXInputField*   itsRegexErrorMsgInput;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, const bool required,
						const JInteger minLength, const JInteger maxLength,
						const JString& regexPattern, const JString& regexErrorMsg);

	void	ConfigureInput(JXIntegerInput* field, const JInteger value);
};

#endif
