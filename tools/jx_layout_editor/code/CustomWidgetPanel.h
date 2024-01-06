/******************************************************************************
 CustomWidgetPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_CustomWidgetPanel
#define _H_CustomWidgetPanel

class JString;
class JXWindow;
class JXInputField;
class JXTextCheckbox;
class WidgetParametersDialog;

class CustomWidgetPanel
{
public:

	CustomWidgetPanel(WidgetParametersDialog* dlog, const JString& className,
					  const JString& ctorArgs, const bool needsCreate);

	~CustomWidgetPanel();

private:

// begin Panel

	JXInputField*   itsClassNameInput;
	JXInputField*   itsCtorArgs;
	JXTextCheckbox* itsNeedsCreateCB;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, const JString& className,
						const JString& ctorArgs, const bool needsCreate);
};

#endif
