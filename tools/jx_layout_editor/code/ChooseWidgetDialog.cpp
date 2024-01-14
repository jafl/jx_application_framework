/******************************************************************************
 ChooseWidgetDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ChooseWidgetDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

static JIndex theSelectedIndex = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

ChooseWidgetDialog::ChooseWidgetDialog()
	:
	JXModalDialogDirector(),
	itsSelectedIndex(theSelectedIndex)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ChooseWidgetDialog::~ChooseWidgetDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "ChooseWidgetDialog-Widget.h"

void
ChooseWidgetDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,90, JString::empty);

	itsWidgetMenu =
		jnew JXTextMenu(JGetString("itsWidgetMenu::ChooseWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 270,20);
	assert( itsWidgetMenu != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ChooseWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 180,60, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::ChooseWidgetDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ChooseWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,60, 60,20);
	assert( cancelButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::ChooseWidgetDialog"));
	SetButtons(okButton, cancelButton);

	itsWidgetMenu->SetMenuItems(kWidgetMenuStr);
	itsWidgetMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsWidgetMenu->SetToPopupChoice(true, itsSelectedIndex);
	ConfigureWidgetMenu(itsWidgetMenu);

	ListenTo(itsWidgetMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsSelectedIndex = msg.GetIndex();
	}));
}

/******************************************************************************
 GetWidgetIndex

 ******************************************************************************/

JIndex
ChooseWidgetDialog::GetWidgetIndex()
	const
{
	theSelectedIndex = itsSelectedIndex;
	return itsSelectedIndex;
}
