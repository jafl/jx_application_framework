/******************************************************************************
 CBEditSymbolPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEditSymbolPrefsDialog.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "cbGlobals.h"
#include "cbHelpText.h"
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
	const JBoolean raiseTreeOnRightClick
	)
	:
	JXDialogDirector(CBGetApplication(), kJTrue)
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
	const JBoolean raiseTreeOnRightClick
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 470,90, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,60, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 340,60, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBEditSymbolPrefsDialog::shortcuts::JXLayout"));

	itsRaiseTreeOnRightClickCB =
		jnew JXTextCheckbox(JGetString("itsRaiseTreeOnRightClickCB::CBEditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 430,20);
	assert( itsRaiseTreeOnRightClickCB != NULL );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditSymbolPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 195,60, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditSymbolPrefsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("Symbol Preferences");
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
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			UpdateSettings();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBSymbolPrefsHelpName);
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
	(CBGetApplication())->DisplayBusyCursor();

	JPtrArray<CBProjectDocument>* docList =
		(CBGetDocumentManager())->GetProjectDocList();
	const JSize docCount = docList->GetElementCount();

//	JProgressDisplay* pg = JNewPG();
//	pg->FixedLengthProcessBeginning(docCount, "Updating preferences...",
//									kJFalse, kJFalse);

	for (JIndex i=1; i<=docCount; i++)
		{
		((docList->GetElement(i))->GetSymbolDirector())->
			SetPrefs(itsRaiseTreeOnRightClickCB->IsChecked(),
					 JI2B(i==1));

//		pg->IncrementProgress();
		}

//	pg->ProcessFinished();
//	jdelete pg;
}
