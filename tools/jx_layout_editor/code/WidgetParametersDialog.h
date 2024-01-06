/******************************************************************************
 WidgetParametersDialog.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_WidgetParametersDialog
#define _H_WidgetParametersDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jx/JXWidget.h>	// need defn of sizing options

class JXWidgetSet;
class JXInputField;
class JXTextCheckbox;
class JXTextMenu;

class WidgetParametersDialog : public JXModalDialogDirector
{
public:

	WidgetParametersDialog(const JString& varName, const bool isMemberVar,
							const JXWidget::HSizingOption hSizing,
							const JXWidget::VSizingOption vSizing);

	~WidgetParametersDialog() override;

	void	AddPanel(JXWidgetSet* panel);

	const JString&	GetVarName(bool* isMemberData) const;

	JXWidget::HSizingOption	GetHSizing() const;
	JXWidget::VSizingOption	GetVSizing() const;

private:

	JIndex	itsHSizingIndex;
	JIndex	itsVSizingIndex;

// begin JXLayout

	JXWidgetSet*    itsLatestContainer;
	JXInputField*   itsVarNameInput;
	JXTextCheckbox* itsMemberVarCB;
	JXTextMenu*     itsHSizingMenu;
	JXTextMenu*     itsVSizingMenu;

// end JXLayout

private:

	void	BuildWindow(const JString& varName, const bool isMemberVar);
};

#endif
