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
class JXRadioGroup;
class WidgetPanelBase;

class WidgetParametersDialog : public JXModalDialogDirector
{
public:

	WidgetParametersDialog(const JString& varName, const bool isMemberVar,
							const bool isPredeclaredVar,
							const JXWidget::HSizingOption hSizing,
							const JXWidget::VSizingOption vSizing);

	~WidgetParametersDialog() override;

	void	AddPanel(WidgetPanelBase* panel, JXWidgetSet* container);

	const JString&	GetVarName(bool* isMemberData, bool* isPredeclared) const;

	JXWidget::HSizingOption	GetHSizing() const;
	JXWidget::VSizingOption	GetVSizing() const;

protected:

	bool	OKToDeactivate() override;

private:

	JPtrArray<WidgetPanelBase>*	itsPanelList;

// begin JXLayout

	JXWidgetSet*    itsLatestContainer;
	JXRadioGroup*   itsHSizingRG;
	JXRadioGroup*   itsVSizingRG;
	JXTextCheckbox* itsMemberVarCB;
	JXTextCheckbox* itsPredeclaredVarCB;
	JXInputField*   itsVarNameInput;

// end JXLayout

private:

	void	BuildWindow(const JString& varName, const bool isMemberVar,
						const bool isPredeclaredVar,
						const JXWidget::HSizingOption hSizing,
						const JXWidget::VSizingOption vSizing);
};

#endif
