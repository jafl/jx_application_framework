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
class WidgetPanelBase;

class WidgetParametersDialog : public JXModalDialogDirector
{
public:

	WidgetParametersDialog(const JString& varName, const bool isMemberVar,
							const JXWidget::HSizingOption hSizing,
							const JXWidget::VSizingOption vSizing);

	~WidgetParametersDialog() override;

	void	AddPanel(WidgetPanelBase* panel, JXWidgetSet* container);

	const JString&	GetVarName(bool* isMemberData) const;

	JXWidget::HSizingOption	GetHSizing() const;
	JXWidget::VSizingOption	GetVSizing() const;

protected:

	bool	OKToDeactivate() override;

private:

	JIndex	itsHSizingIndex;
	JIndex	itsVSizingIndex;

	JPtrArray<WidgetPanelBase>*	itsPanelList;

// begin JXLayout

	JXWidgetSet*    itsLatestContainer;
	JXTextCheckbox* itsMemberVarCB;
	JXTextMenu*     itsHSizingMenu;
	JXTextMenu*     itsVSizingMenu;
	JXInputField*   itsVarNameInput;

// end JXLayout

private:

	void	BuildWindow(const JString& varName, const bool isMemberVar);
};

#endif
