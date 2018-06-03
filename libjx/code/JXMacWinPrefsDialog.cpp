/******************************************************************************
 JXMacWinPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include "JXMacWinPrefsDialog.h"
#include <JXWindow.h>
#include <JXMenu.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <jXGlobals.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 EditPrefs (static)

 ******************************************************************************/

void
JXMacWinPrefsDialog::EditPrefs()
{
	JXMacWinPrefsDialog* dlog = jnew JXMacWinPrefsDialog;
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXMacWinPrefsDialog::JXMacWinPrefsDialog()
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
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

	JXWindow* window = jnew JXWindow(this, 350,230, JString::empty);
	assert( window != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,200, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXMacWinPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,200, 60,20);
	assert( cancelButton != NULL );

	itsMacStyleCB =
		jnew JXTextCheckbox(JGetString("itsMacStyleCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 320,20);
	assert( itsMacStyleCB != NULL );

	itsHomeEndCB =
		jnew JXTextCheckbox(JGetString("itsHomeEndCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 320,20);
	assert( itsHomeEndCB != NULL );

	JXStaticText* restartHint =
		jnew JXStaticText(JGetString("restartHint::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,40, 300,20);
	assert( restartHint != NULL );
	restartHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	restartHint->SetToLabel();

	itsScrollCaretCB =
		jnew JXTextCheckbox(JGetString("itsScrollCaretCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 320,20);
	assert( itsScrollCaretCB != NULL );

	itsCopyWhenSelectCB =
		jnew JXTextCheckbox(JGetString("itsCopyWhenSelectCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 320,20);
	assert( itsCopyWhenSelectCB != NULL );

	itsFocusInDockCB =
		jnew JXTextCheckbox(JGetString("itsFocusInDockCB::JXMacWinPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 320,20);
	assert( itsFocusInDockCB != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXMacWinPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsMacStyleCB->SetState(JI2B(JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle));
	itsHomeEndCB->SetState(JXTEBase::WillUseWindowsHomeEnd());
	itsScrollCaretCB->SetState(JXTEBase::CaretWillFollowScroll());
	itsCopyWhenSelectCB->SetState(JTextEditor::WillCopyWhenSelect());
	itsFocusInDockCB->SetState(JXWindow::WillFocusFollowCursorInDock());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXMacWinPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			UpdateSettings();
			}
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
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
