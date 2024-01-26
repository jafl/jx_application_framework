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

class InputFieldPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	InputFieldPanel(WidgetParametersDialog* dlog, const bool required,
					const JInteger minLength, const JInteger maxLength,
					const JString& regexPattern, const JString&	regexFlags,
					const JString& regexErrorMsg,
					const bool wordWrap, const bool newlines,
					const bool monospace, const JString& hint);

	~InputFieldPanel();

	bool	Validate() const override;

	void	GetValues(bool* required,
					  JInteger* minLength, JInteger* maxLength,
					  JString* regexPattern, JString* regexFlags,
					  JString* regexErrorMsg,
					  bool* wordWrap, bool* newlines,
					  bool* monospace, JString* hint);

private:

// begin Panel

	JXTextCheckbox* itsValueRequiredCB;
	JXTextCheckbox* itsWordWrapCB;
	JXTextCheckbox* itsAllowNewlinesCB;
	JXTextCheckbox* itsMonospaceCB;
	JXIntegerInput* itsMinLengthInput;
	JXIntegerInput* itsMaxLengthInput;
	JXRegexInput*   itsRegexInput;
	JXInputField*   itsRegexFlagsInput;
	JXInputField*   itsRegexErrorMsgInput;
	JXInputField*   itsHintInput;

// end Panel

private:

	void	ConfigureInput(JXIntegerInput* field, const JInteger value);
	void	UpdateDisplay() const;
};

#endif
