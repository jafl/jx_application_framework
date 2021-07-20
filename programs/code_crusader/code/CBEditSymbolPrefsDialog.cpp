/******************************************************************************
 CBEditSymbolPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "CBEditSymbolPrefsDialog.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXStaticText.h>
#include <JXHelpManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditSymbolPrefsDialog::CBEditSymbolPrefsDialog
	(
	const bool raiseTreeOnRightClick
	)
	:
	JXDialogDirector(CBGetApplication(), true)
{
	BuildWindow(raiseTreeOnRightClick);
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditSymbolPrefsDialog::~CBEditSymbolPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditSymbolPrefsDialog::BuildWindow
	(
	const bool raiseTreeOnRightClick
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 470,90, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,60, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 340,60, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBEditSymbolPrefsDialog::shortcuts::JXLayout"));

	itsRaiseTreeOnRightClickCB =
		jnew JXTextCheckbox(JGetString("itsRaiseTreeOnRightClickCB::CBEditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 430,20);
	assert( itsRaiseTreeOnRightClickCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 195,60, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditSymbolPrefsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditSymbolPrefsDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsRaiseTreeOnRightClickCB->SetState(raiseTreeOnRightClick);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditSymbolPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateSettings();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("CBSymbolHelp-Prefs");
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
CBEditSymbolPrefsDialog::UpdateSettings()
{
	CBGetApplication()->DisplayBusyCursor();

	JPtrArray<CBProjectDocument>* docList =
		CBGetDocumentManager()->GetProjectDocList();
	const JSize docCount = docList->GetElementCount();

//	JProgressDisplay* pg = JNewPG();
//	pg->FixedLengthProcessBeginning(docCount, "Updating preferences...",
//									false, false);

	for (JIndex i=1; i<=docCount; i++)
		{
		((docList->GetElement(i))->GetSymbolDirector())->
			SetPrefs(itsRaiseTreeOnRightClickCB->IsChecked(),
					 i==1);

//		pg->IncrementProgress();
		}

//	pg->ProcessFinished();
//	jdelete pg;
}
