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

	auto* window = jnew JXWindow(this, 350,230, JString::empty);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,200, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXMacWinPrefsDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,200, 60,20);
	assert( cancelButton != nullptr );

	itsMacStyleCB =
		jnew JXTextCheckbox(JGetString("itsMacStyleCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 320,20);
	assert( itsMacStyleCB != nullptr );

	itsHomeEndCB =
		jnew JXTextCheckbox(JGetString("itsHomeEndCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 320,20);
	assert( itsHomeEndCB != nullptr );

	auto* restartHint =
		jnew JXStaticText(JGetString("restartHint::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,40, 300,20);
	assert( restartHint != nullptr );
	restartHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	restartHint->SetToLabel();

	itsScrollCaretCB =
		jnew JXTextCheckbox(JGetString("itsScrollCaretCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 320,20);
	assert( itsScrollCaretCB != nullptr );

	itsCopyWhenSelectCB =
		jnew JXTextCheckbox(JGetString("itsCopyWhenSelectCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 320,20);
	assert( itsCopyWhenSelectCB != nullptr );

	itsFocusInDockCB =
		jnew JXTextCheckbox(JGetString("itsFocusInDockCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 320,20);
	assert( itsFocusInDockCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXMacWinPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsMacStyleCB->SetState(JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle);
	itsHomeEndCB->SetState(JXTEBase::WillUseWindowsHomeEnd());
	itsScrollCaretCB->SetState(JXTEBase::CaretWillFollowScroll());
	itsCopyWhenSelectCB->SetState(JTextEditor::WillCopyWhenSelect());
	itsFocusInDockCB->SetState(JXWindow::WillFocusFollowCursorInDock());
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
