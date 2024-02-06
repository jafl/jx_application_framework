/******************************************************************************
 WidgetParametersDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "WidgetParametersDialog.h"
#include "WidgetPanelBase.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXAtMostOneCBGroup.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WidgetParametersDialog::WidgetParametersDialog
	(
	const JString&					varName,
	const bool						isMemberVar,
	const bool						isPredeclaredVar,
	const JXWidget::HSizingOption	hSizing,
	const JXWidget::VSizingOption	vSizing
	)
	:
	JXModalDialogDirector(),
	itsHSizingIndex(hSizing+1),
	itsVSizingIndex(vSizing+1),
	itsPanelList(jnew JPtrArray<WidgetPanelBase>(JPtrArrayT::kForgetAll))
{
	BuildWindow(varName, isMemberVar, isPredeclaredVar);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WidgetParametersDialog::~WidgetParametersDialog()
{
	jdelete itsPanelList;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "WidgetParametersDialog-HSizing.h"
#include "WidgetParametersDialog-VSizing.h"

void
WidgetParametersDialog::BuildWindow
	(
	const JString&	varName,
	const bool		isMemberVar,
	const bool		isPredeclaredVar
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 460,170, JGetString("WindowTitle::WidgetParametersDialog::JXLayout"));

	itsLatestContainer =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,130);

	auto* variableNameLabel =
		jnew JXStaticText(JGetString("variableNameLabel::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	variableNameLabel->SetToLabel(false);

	itsHSizingMenu =
		jnew JXTextMenu(JString::empty, itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 280,30);

	itsMemberVarCB =
		jnew JXTextCheckbox(JGetString("itsMemberVarCB::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,50, 130,20);
	itsMemberVarCB->SetShortcuts(JGetString("itsMemberVarCB::shortcuts::WidgetParametersDialog::JXLayout"));

	itsPredeclaredVarCB =
		jnew JXTextCheckbox(JGetString("itsPredeclaredVarCB::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,80, 130,20);
	itsPredeclaredVarCB->SetShortcuts(JGetString("itsPredeclaredVarCB::shortcuts::WidgetParametersDialog::JXLayout"));

	itsVSizingMenu =
		jnew JXTextMenu(JString::empty, itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 280,30);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::WidgetParametersDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 120,140, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::WidgetParametersDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 289,139, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::WidgetParametersDialog::JXLayout"));

	itsVarNameInput =
		jnew JXInputField(itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,20, 310,20);
	itsVarNameInput->SetIsRequired();
	itsVarNameInput->SetValidationPattern(jnew JRegex("^[_a-z][_a-z0-9:\\[\\]]+$", "i"), "itsVarNameInput::validation::WidgetParametersDialog::JXLayout");

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsVarNameInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsVarNameInput->GetText()->SetText(varName);

	itsMemberVarCB->SetState(isMemberVar);
	itsPredeclaredVarCB->SetState(isPredeclaredVar);
	jnew JXAtMostOneCBGroup(2, itsMemberVarCB, itsPredeclaredVarCB);

	itsHSizingMenu->SetTitleText(JGetString("MenuTitle::WidgetParametersDialog_HSizing"));
	itsHSizingMenu->SetMenuItems(kHorizontalSizingMenuStr);
	itsHSizingMenu->SetUpdateAction(JXMenu::kDisableNone);
	ConfigureHorizontalSizingMenu(itsHSizingMenu);
	itsHSizingMenu->SetToPopupChoice(true, itsHSizingIndex);

	ListenTo(itsHSizingMenu, std::function([this](const JXMenu::NeedsUpdate&)
	{
		itsHSizingMenu->CheckItem(itsHSizingIndex);
	}));

	ListenTo(itsHSizingMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsHSizingIndex = msg.GetIndex();
	}));

	itsVSizingMenu->SetTitleText(JGetString("MenuTitle::WidgetParametersDialog_VSizing"));
	itsVSizingMenu->SetMenuItems(kVerticalSizingMenuStr);
	itsVSizingMenu->SetUpdateAction(JXMenu::kDisableNone);
	ConfigureVerticalSizingMenu(itsVSizingMenu);
	itsVSizingMenu->SetToPopupChoice(true, itsVSizingIndex);

	ListenTo(itsVSizingMenu, std::function([this](const JXMenu::NeedsUpdate&)
	{
		itsVSizingMenu->CheckItem(itsVSizingIndex);
	}));

	ListenTo(itsVSizingMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsVSizingIndex = msg.GetIndex();
	}));
}

/******************************************************************************
 AddPanel

 ******************************************************************************/

void
WidgetParametersDialog::AddPanel
	(
	WidgetPanelBase*	panel,
	JXWidgetSet*		container
	)
{
	itsPanelList->Append(panel);

	GetWindow()->AdjustSize(0, container->GetFrameHeight());
	container->Place(0, itsLatestContainer->GetFrame().bottom);
	itsLatestContainer = container;
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
WidgetParametersDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	for (auto* panel : *itsPanelList)
	{
		if (!panel->Validate())
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 GetVarName

 ******************************************************************************/

const JString&
WidgetParametersDialog::GetVarName
	(
	bool* isMemberData,
	bool* isPredeclared
	)
	const
{
	*isMemberData  = itsMemberVarCB->IsChecked();
	*isPredeclared = itsPredeclaredVarCB->IsChecked();
	return itsVarNameInput->GetText()->GetText();
}

/******************************************************************************
 Get sizing

 ******************************************************************************/

JXWidget::HSizingOption
WidgetParametersDialog::GetHSizing()
	const
{
	return (JXWidget::HSizingOption) (itsHSizingIndex-1);
}

JXWidget::VSizingOption
WidgetParametersDialog::GetVSizing()
	const
{
	return (JXWidget::VSizingOption) (itsVSizingIndex-1);
}
