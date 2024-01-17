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
	const JXWidget::HSizingOption	hSizing,
	const JXWidget::VSizingOption	vSizing
	)
	:
	JXModalDialogDirector(),
	itsHSizingIndex(hSizing+1),
	itsVSizingIndex(vSizing+1),
	itsPanelList(jnew JPtrArray<WidgetPanelBase>(JPtrArrayT::kForgetAll))
{
	BuildWindow(varName, isMemberVar);
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
	const bool		isMemberVar
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 460,150, JString::empty);

	itsLatestContainer =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,110);
	assert( itsLatestContainer != nullptr );

	itsVarNameInput =
		jnew JXInputField(itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,20, 170,20);
	assert( itsVarNameInput != nullptr );

	auto* variableNameLabel =
		jnew JXStaticText(JGetString("variableNameLabel::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	assert( variableNameLabel != nullptr );
	variableNameLabel->SetToLabel();

	itsMemberVarCB =
		jnew JXTextCheckbox(JGetString("itsMemberVarCB::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,20, 130,20);
	assert( itsMemberVarCB != nullptr );
	itsMemberVarCB->SetShortcuts(JGetString("itsMemberVarCB::WidgetParametersDialog::shortcuts::JXLayout"));

	itsHSizingMenu =
		jnew JXTextMenu(JGetString("itsHSizingMenu::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 420,20);
	assert( itsHSizingMenu != nullptr );

	itsVSizingMenu =
		jnew JXTextMenu(JGetString("itsVSizingMenu::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 420,20);
	assert( itsVSizingMenu != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::WidgetParametersDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 290,120, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::WidgetParametersDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::WidgetParametersDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 120,120, 60,20);
	assert( cancelButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::WidgetParametersDialog"));
	SetButtons(okButton, cancelButton);

	itsVarNameInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsVarNameInput->GetText()->SetText(varName);
	itsVarNameInput->SetIsRequired();
	itsVarNameInput->SetValidationPattern(
		jnew JRegex("^[_a-z][_a-z0-9]+$", "i"),
		"VariableNameMustBeValidIdentifier::WidgetParametersDialog");

	itsMemberVarCB->SetState(isMemberVar);

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
	bool* isMemberData
	)
	const
{
	*isMemberData = itsMemberVarCB->IsChecked();
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
