/******************************************************************************
 CBEditTreePrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBEditTreePrefsDialog.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXFontSizeMenu.h>
#include <JXStaticText.h>
#include <JXHelpManager.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditTreePrefsDialog::CBEditTreePrefsDialog
	(
	const JSize		fontSize,
	const bool	showInheritedFns,
	const bool	autoMinMILinks,
	const bool	drawMILinksOnTop,
	const bool	raiseWhenSingleMatch
	)
	:
	JXDialogDirector(CBGetApplication(), true)
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
	const bool	showInheritedFns,
	const bool	autoMinMILinks,
	const bool	drawMILinksOnTop,
	const bool	raiseWhenSingleMatch
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 320,290, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,260, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,260, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBEditTreePrefsDialog::shortcuts::JXLayout"));

	itsShowInheritedFnsCB =
		jnew JXTextCheckbox(JGetString("itsShowInheritedFnsCB::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 280,20);
	assert( itsShowInheritedFnsCB != nullptr );

	itsFontSizeMenu =
		jnew JXFontSizeMenu(JFontManager::GetDefaultFontName(), JGetString("FontSize::CBEditTreePrefsDialog"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 280,30);
	assert( itsFontSizeMenu != nullptr );

	itsAutoMinMILinkCB =
		jnew JXTextCheckbox(JGetString("itsAutoMinMILinkCB::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 280,20);
	assert( itsAutoMinMILinkCB != nullptr );

	itsMILinkStyleRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,160, 134,74);
	assert( itsMILinkStyleRG != nullptr );

	auto* miLinksLabel =
		jnew JXStaticText(JGetString("miLinksLabel::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 45,190, 90,20);
	assert( miLinksLabel != nullptr );
	miLinksLabel->SetToLabel();

	auto* aboveLabel =
		jnew JXTextRadioButton(1, JGetString("aboveLabel::CBEditTreePrefsDialog::JXLayout"), itsMILinkStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	assert( aboveLabel != nullptr );

	auto* belowLabel =
		jnew JXTextRadioButton(2, JGetString("belowLabel::CBEditTreePrefsDialog::JXLayout"), itsMILinkStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 110,20);
	assert( belowLabel != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 125,260, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditTreePrefsDialog::shortcuts::JXLayout"));

	itsRaiseSingleMatchCB =
		jnew JXTextCheckbox(JGetString("itsRaiseSingleMatchCB::CBEditTreePrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 280,20);
	assert( itsRaiseSingleMatchCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditTreePrefsDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsFontSizeMenu->SetFontSize(fontSize);
	itsFontSizeMenu->SetToPopupChoice();

	itsShowInheritedFnsCB->SetState(showInheritedFns);
	itsAutoMinMILinkCB->SetState(autoMinMILinks);
	itsRaiseSingleMatchCB->SetState(raiseWhenSingleMatch);

	itsMILinkStyleRG->SelectItem(drawMILinksOnTop ? kDrawMILinksAbove : kDrawMILinksBelow);
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
		(JXGetHelpManager())->ShowSection("CBTreePrefsHelp");
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
	CBGetApplication()->DisplayBusyCursor();

	JPtrArray<CBProjectDocument>* docList =
		CBGetDocumentManager()->GetProjectDocList();
	const JSize docCount = docList->GetElementCount();

	JProgressDisplay* pg = JNewPG();
	pg->FixedLengthProcessBeginning(docCount, JGetString("UpdatingPrefs::CBEditTreePrefsDialog"),
									false, false);

	for (JIndex i=1; i<=docCount; i++)
		{
		(docList->GetElement(i))->
			SetTreePrefs(itsFontSizeMenu->GetFontSize(),
						 itsShowInheritedFnsCB->IsChecked(),
						 itsAutoMinMILinkCB->IsChecked(),
						 itsMILinkStyleRG->GetSelectedItem() == kDrawMILinksAbove,
						 itsRaiseSingleMatchCB->IsChecked(),
						 i==1);

		pg->IncrementProgress();
		}

	pg->ProcessFinished();
	jdelete pg;
}
