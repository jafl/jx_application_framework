/******************************************************************************
 CustomWidgetPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_CustomWidgetPanel
#define _H_CustomWidgetPanel

#include "WidgetPanelBase.h"

class JString;
class JXInputField;
class JXTextCheckbox;

class CustomWidgetPanel : public WidgetPanelBase
{
public:

	CustomWidgetPanel(WidgetParametersDialog* dlog, const JString& className,
					  const JString& ctorArgs, const bool needsCreate,
					  const JString& deps, const bool wantsInput);

	~CustomWidgetPanel();

	void	GetValues(JString* className, JString* ctorArgs, bool* needsCreate,
					  JString* deps, bool* wantsInput);

private:

// begin Panel

	JXTextCheckbox* itsWantsInputCB;
	JXTextCheckbox* itsNeedsCreateCB;
	JXInputField*   itsClassNameInput;
	JXInputField*   itsCtorArgs;
	JXInputField*   itsDependencyInput;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, const JString& className,
						const JString& ctorArgs, const bool needsCreate,
						const JString& deps, const bool wantsInput);
};

#endif
