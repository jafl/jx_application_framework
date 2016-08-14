/******************************************************************************
 CBEditTreePrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

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

	JXWindow* window = new JXWindow(this, 320,290, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,260, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,260, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBEditTreePrefsDialog::shortcuts::JXLayout"));

	itsShowInheritedFnsCB =
		new JXTextCheckbox(JGetString("itsShowInheritedFnsCB::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 280,20);
	assert( itsShowInheritedFnsCB != NULL );

	itsFontSizeMenu =
		new JXFontSizeMenu(JGetDefaultFontName(), "Font size:", window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 280,30);
	assert( itsFontSizeMenu != NULL );

	itsAutoMinMILinkCB =
		new JXTextCheckbox(JGetString("itsAutoMinMILinkCB::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 280,20);
	assert( itsAutoMinMILinkCB != NULL );

	itsMILinkStyleRG =
		new JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,160, 134,74);
	assert( itsMILinkStyleRG != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 45,190, 90,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXTextRadioButton* obj2_JXLayout =
		new JXTextRadioButton(1, JGetString("obj2_JXLayout::CBEditTreePrefsDialog::JXLayout"), itsMILinkStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	assert( obj2_JXLayout != NULL );

	JXTextRadioButton* obj3_JXLayout =
		new JXTextRadioButton(2, JGetString("obj3_JXLayout::CBEditTreePrefsDialog::JXLayout"), itsMILinkStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 110,20);
	assert( obj3_JXLayout != NULL );

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 125,260, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditTreePrefsDialog::shortcuts::JXLayout"));

	itsRaiseSingleMatchCB =
		new JXTextCheckbox(JGetString("itsRaiseSingleMatchCB::CBEditTreePrefsDialog::JXLayout"), window,
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
	delete pg;
}
