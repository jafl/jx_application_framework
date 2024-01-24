/******************************************************************************
 JXMacWinPrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include "JXMacWinPrefsDialog.h"
#include "JXWindow.h"
#include "JXMenu.h"
#include "JXStaticText.h"
#include "JXTextButton.h"
#include "JXTextCheckbox.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 EditPrefs (static)

 ******************************************************************************/

void
JXMacWinPrefsDialog::EditPrefs()
{
	auto* dlog = jnew JXMacWinPrefsDialog;
	if (dlog->DoDialog())
	{
		dlog->UpdateSettings();
	}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXMacWinPrefsDialog::JXMacWinPrefsDialog()
	:
	JXModalDialogDirector()
{
	BuildWindow();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMacWinPrefsDialog::~JXMacWinPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXMacWinPrefsDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 350,230, JGetString("WindowTitle::JXMacWinPrefsDialog::JXLayout"));

	itsMacStyleCB =
		jnew JXTextCheckbox(JGetString("itsMacStyleCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 320,20);

	auto* restartHint =
		jnew JXStaticText(JGetString("restartHint::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,40, 300,20);
	restartHint->SetToLabel(false);

	itsHomeEndCB =
		jnew JXTextCheckbox(JGetString("itsHomeEndCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 320,20);

	itsScrollCaretCB =
		jnew JXTextCheckbox(JGetString("itsScrollCaretCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 320,20);

	itsCopyWhenSelectCB =
		jnew JXTextCheckbox(JGetString("itsCopyWhenSelectCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 320,20);

	itsFocusInDockCB =
		jnew JXTextCheckbox(JGetString("itsFocusInDockCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 320,20);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,200, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 209,199, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXMacWinPrefsDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsMacStyleCB->SetState(JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle);
	itsHomeEndCB->SetState(JXTEBase::WillUseWindowsHomeEnd());
	itsScrollCaretCB->SetState(JXTEBase::CaretWillFollowScroll());
	itsCopyWhenSelectCB->SetState(JTextEditor::WillCopyWhenSelect());
	itsFocusInDockCB->SetState(JXWindow::WillFocusFollowCursorInDock());

	restartHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
}

/******************************************************************************
 UpdateSettings (private)

 ******************************************************************************/

void
JXMacWinPrefsDialog::UpdateSettings()
{
	JXMenu::SetDisplayStyle(itsMacStyleCB->IsChecked() ?
							JXMenu::kMacintoshStyle : JXMenu::kWindowsStyle);

	JXTEBase::ShouldUseWindowsHomeEnd(itsHomeEndCB->IsChecked());
	JXTEBase::CaretShouldFollowScroll(itsScrollCaretCB->IsChecked());
	JTextEditor::ShouldCopyWhenSelect(itsCopyWhenSelectCB->IsChecked());
	JXWindow::ShouldFocusFollowCursorInDock(itsFocusInDockCB->IsChecked());
}
