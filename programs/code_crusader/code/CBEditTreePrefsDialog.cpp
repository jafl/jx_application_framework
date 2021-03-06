/******************************************************************************
 CBEditTreePrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEditTreePrefsDialog.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include "cbHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXFontSizeMenu.h>
#include <JXStaticText.h>
#include <JXHelpManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditTreePrefsDialog::CBEditTreePrefsDialog
	(
	const JSize		fontSize,
	const JBoolean	showInheritedFns,
	const JBoolean	autoMinMILinks,
	const JBoolean	drawMILinksOnTop,
	const JBoolean	raiseWhenSingleMatch
	)
	:
	JXDialogDirector(CBGetApplication(), kJTrue)
{
	BuildWindow(fontSize, showInheritedFns, autoMinMILinks, drawMILinksOnTop,
				raiseWhenSingleMatch);
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditTreePrefsDialog::~CBEditTreePrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditTreePrefsDialog::BuildWindow
	(
	const JSize		fontSize,
	const JBoolean	showInheritedFns,
	const JBoolean	autoMinMILinks,
	const JBoolean	drawMILinksOnTop,
	const JBoolean	raiseWhenSingleMatch
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 320,290, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,260, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,260, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBEditTreePrefsDialog::shortcuts::JXLayout"));

	itsShowInheritedFnsCB =
		jnew JXTextCheckbox(JGetString("itsShowInheritedFnsCB::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 280,20);
	assert( itsShowInheritedFnsCB != NULL );

	itsFontSizeMenu =
		jnew JXFontSizeMenu(JGetDefaultFontName(), "Font size:", window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 280,30);
	assert( itsFontSizeMenu != NULL );

	itsAutoMinMILinkCB =
		jnew JXTextCheckbox(JGetString("itsAutoMinMILinkCB::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 280,20);
	assert( itsAutoMinMILinkCB != NULL );

	itsMILinkStyleRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,160, 134,74);
	assert( itsMILinkStyleRG != NULL );

	JXStaticText* miLinksLabel =
		jnew JXStaticText(JGetString("miLinksLabel::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 45,190, 90,20);
	assert( miLinksLabel != NULL );
	miLinksLabel->SetToLabel();

	JXTextRadioButton* aboveLabel =
		jnew JXTextRadioButton(1, JGetString("aboveLabel::CBEditTreePrefsDialog::JXLayout"), itsMILinkStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	assert( aboveLabel != NULL );

	JXTextRadioButton* belowLabel =
		jnew JXTextRadioButton(2, JGetString("belowLabel::CBEditTreePrefsDialog::JXLayout"), itsMILinkStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 110,20);
	assert( belowLabel != NULL );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 125,260, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditTreePrefsDialog::shortcuts::JXLayout"));

	itsRaiseSingleMatchCB =
		jnew JXTextCheckbox(JGetString("itsRaiseSingleMatchCB::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 280,20);
	assert( itsRaiseSingleMatchCB != NULL );

// end JXLayout

	window->SetTitle("Tree Preferences");
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsFontSizeMenu->SetFontSize(fontSize);
	itsFontSizeMenu->SetToPopupChoice();

	itsShowInheritedFnsCB->SetState(showInheritedFns);
	itsAutoMinMILinkCB->SetState(autoMinMILinks);
	itsRaiseSingleMatchCB->SetState(raiseWhenSingleMatch);

	if (drawMILinksOnTop)
		{
		itsMILinkStyleRG->SelectItem(kDrawMILinksAbove);
		}
	else
		{
		itsMILinkStyleRG->SelectItem(kDrawMILinksBelow);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditTreePrefsDialog::Receive
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
		(JXGetHelpManager())->ShowSection(kCBTreePrefsHelpName);
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
CBEditTreePrefsDialog::UpdateSettings()
{
	(CBGetApplication())->DisplayBusyCursor();

	JPtrArray<CBProjectDocument>* docList =
		(CBGetDocumentManager())->GetProjectDocList();
	const JSize docCount = docList->GetElementCount();

	JProgressDisplay* pg = JNewPG();
	pg->FixedLengthProcessBeginning(docCount, "Updating preferences...",
									kJFalse, kJFalse);

	for (JIndex i=1; i<=docCount; i++)
		{
		(docList->NthElement(i))->
			SetTreePrefs(itsFontSizeMenu->GetFontSize(),
						 itsShowInheritedFnsCB->IsChecked(),
						 itsAutoMinMILinkCB->IsChecked(),
						 JI2B(itsMILinkStyleRG->GetSelectedItem() == kDrawMILinksAbove),
						 itsRaiseSingleMatchCB->IsChecked(),
						 JI2B(i==1));

		pg->IncrementProgress();
		}

	pg->ProcessFinished();
	jdelete pg;
}
