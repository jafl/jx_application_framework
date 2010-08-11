/******************************************************************************
 GMailPrefsDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GMailPrefsDialog.h"
#include <GHelpText.h>

#include <gMailUtils.h>

#include <JXChooseMonoFont.h>
#include <JXChooseSaveFile.h>
#include <JXFileInput.h>
#include <JXInputField.h>
#include <JXHelpManager.h>
#include <JXIntegerInput.h>
#include <JXRadioGroup.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXTextRadioButton.h>
#include <JXWindow.h>

#include <JColormap.h>
#include <JString.h>

#include <jDirUtil.h>
#include <jXGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMailPrefsDialog::GMailPrefsDialog
	(
	JXDirector*	supervisor,
	const JBoolean	showstate,
	const JBoolean	autoQuote,
	const JBoolean	showSender,
	const JString&	senderString,
	const JString&	fontname,
	const JSize	fontsize,
	const JSize		maxQuotedReply,
	const JBoolean	reportSCErrors,
	const JSize		spacesPerTab,
	const JBoolean	beepOnNewMail,
	const JIndex	encType,
	const JString&	filterFile,
	const JString&	defInbox,
	const JBoolean 	openMailboxWindow
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(showstate, autoQuote, showSender,
					senderString, fontname, fontsize,
					maxQuotedReply, reportSCErrors, spacesPerTab, beepOnNewMail,
					encType, filterFile, defInbox, openMailboxWindow);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMailPrefsDialog::~GMailPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GMailPrefsDialog::BuildWindow
	(
	const JBoolean	showstate,
	const JBoolean	autoQuote,
	const JBoolean	showSender,
	const JString&	senderString,
	const JString&	fontname,
	const JSize	fontsize,
	const JSize		maxQuotedReply,
	const JBoolean	reportSCErrors,
	const JSize		spacesPerTab,
	const JBoolean	beepOnNewMail,
	const JIndex	encType,
	const JString&	filterFile,
	const JString&	defInbox,
	const JBoolean 	openMailboxWindow
	)
{

// begin JXLayout

    JXWindow* window = new JXWindow(this, 360,550, "");
    assert( window != NULL );

    itsMaxReplyInput =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 270,335, 40,20);
    assert( itsMaxReplyInput != NULL );

    itsSenderStringInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 100,385, 240,20);
    assert( itsSenderStringInput != NULL );

    itsSpacesPerTabInput =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 140,80, 40,20);
    assert( itsSpacesPerTabInput != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 260,515, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::GMailPrefsDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 30,515, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::GMailPrefsDialog::shortcuts::JXLayout"));

    itsShowStateInTitle =
        new JXTextCheckbox(JGetString("itsShowStateInTitle::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,310, 250,20);
    assert( itsShowStateInTitle != NULL );

    itsAutoQuote =
        new JXTextCheckbox(JGetString("itsAutoQuote::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,335, 250,20);
    assert( itsAutoQuote != NULL );

    itsShowSenderString =
        new JXTextCheckbox(JGetString("itsShowSenderString::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,360, 250,20);
    assert( itsShowSenderString != NULL );

    itsSenderText =
        new JXStaticText(JGetString("itsSenderText::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 50,385, 50,20);
    assert( itsSenderText != NULL );
    itsSenderText->SetToLabel();

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 145,515, 70,20);
    assert( itsHelpButton != NULL );

    itsFontMenu =
        new JXChooseMonoFont(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,10, 310,60);
    assert( itsFontMenu != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 315,335, 30,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    itsReportSCErrorsCB =
        new JXTextCheckbox(JGetString("itsReportSCErrorsCB::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,420, 250,20);
    assert( itsReportSCErrorsCB != NULL );

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 40,80, 100,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetToLabel();

    itsBeepOnNewMailCB =
        new JXTextCheckbox(JGetString("itsBeepOnNewMailCB::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,285, 250,20);
    assert( itsBeepOnNewMailCB != NULL );

    itsEncryptionType =
        new JXRadioGroup(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 45,175, 145,70);
    assert( itsEncryptionType != NULL );

    JXTextRadioButton* obj3_JXLayout =
        new JXTextRadioButton(1, JGetString("obj3_JXLayout::GMailPrefsDialog::JXLayout"), itsEncryptionType,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,15, 100,20);
    assert( obj3_JXLayout != NULL );

    JXTextRadioButton* obj4_JXLayout =
        new JXTextRadioButton(2, JGetString("obj4_JXLayout::GMailPrefsDialog::JXLayout"), itsEncryptionType,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,35, 100,20);
    assert( obj4_JXLayout != NULL );

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 55,168, 120,20);
    assert( obj5_JXLayout != NULL );
    obj5_JXLayout->SetToLabel();

    JXStaticText* obj6_JXLayout =
        new JXStaticText(JGetString("obj6_JXLayout::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,480, 90,20);
    assert( obj6_JXLayout != NULL );
    obj6_JXLayout->SetToLabel();

    itsFilterButton =
        new JXTextButton(JGetString("itsFilterButton::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 290,480, 60,20);
    assert( itsFilterButton != NULL );

    itsDefaultInbox =
        new JXFileInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 110,450, 180,20);
    assert( itsDefaultInbox != NULL );

    itsInboxButton =
        new JXTextButton(JGetString("itsInboxButton::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 290,450, 60,20);
    assert( itsInboxButton != NULL );

    JXStaticText* obj7_JXLayout =
        new JXStaticText(JGetString("obj7_JXLayout::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,450, 90,20);
    assert( obj7_JXLayout != NULL );
    obj7_JXLayout->SetToLabel();

    itsFilterFileInput =
        new JXFileInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 110,480, 180,20);
    assert( itsFilterFileInput != NULL );

    itsCopySelectedCB =
        new JXTextCheckbox(JGetString("itsCopySelectedCB::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,110, 270,20);
    assert( itsCopySelectedCB != NULL );

    itsHomeEndCB =
        new JXTextCheckbox(JGetString("itsHomeEndCB::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,135, 270,20);
    assert( itsHomeEndCB != NULL );

    itsOpenMailTreeCB =
        new JXTextCheckbox(JGetString("itsOpenMailTreeCB::GMailPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,260, 250,20);
    assert( itsOpenMailTreeCB != NULL );

// end JXLayout

	window->SetTitle("Mail Preferences");
	SetButtons(okButton, cancelButton);

	ListenTo(itsShowSenderString);
	ListenTo(itsHelpButton);
	ListenTo(itsAutoQuote);

	itsShowStateInTitle->SetState(showstate);
	itsAutoQuote->SetState(autoQuote);

	itsSenderStringInput->SetText(senderString);
	itsShowSenderString->SetState(showSender);

	itsFontMenu->SetFont(fontname, fontsize);

	itsMaxReplyInput->SetValue(maxQuotedReply);

	itsSpacesPerTabInput->SetValue(spacesPerTab);

	itsReportSCErrorsCB->SetState(reportSCErrors);

	itsBeepOnNewMailCB->SetState(beepOnNewMail);
	itsOpenMailTreeCB->SetState(openMailboxWindow);

	AdjustForSenderString(showSender);
	AdjustForQuotedReply(autoQuote);

	itsEncryptionType->SelectItem(encType);

	JString home;
	if (JGetHomeDirectory(&home))
		{
		JAppendDirSeparator(&home);
		itsDefaultInbox->SetBasePath(home);
		itsFilterFileInput->SetBasePath(home);
		}

	itsDefaultInbox->SetText(defInbox);
	itsDefaultInbox->ShouldAllowInvalidFile();
	ListenTo(itsInboxButton);

	itsFilterFileInput->SetText(filterFile);
	itsFilterFileInput->ShouldAllowInvalidFile();
	ListenTo(itsFilterButton);

	itsCopySelectedCB->SetState(JTextEditor::WillCopyWhenSelect());
	itsHomeEndCB->SetState(JXTEBase::WillUseWindowsHomeEnd());
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
GMailPrefsDialog::GetValues
	(
	JBoolean*	showstate,
	JBoolean*	autoQuote,
	JBoolean*	showSender,
	JString*	senderString,
	JString*	fontname,
	JSize*		fontsize,
	JSize*		maxQuotedReply,
	JBoolean*	reportSCErrors,
	JSize*		spacesPerTab,
	JBoolean*	beepOnNewMail,
	JIndex*		encType,
	JString*	filterFile,
	JString*	defInbox,
	JBoolean*	openMailboxWindow
	)
{
	*showstate	= itsShowStateInTitle->IsChecked();
	*autoQuote	= itsAutoQuote->IsChecked();

	*showSender			= itsShowSenderString->IsChecked();
	*senderString		= itsSenderStringInput->GetText();
	*beepOnNewMail		= itsBeepOnNewMailCB->IsChecked();
	*openMailboxWindow	= itsOpenMailTreeCB->IsChecked();

	JInteger value;
	if (itsMaxReplyInput->GetValue(&value))
		{
		*maxQuotedReply = value;
		}
	else
		{
		*maxQuotedReply = 0;
		}

	itsFontMenu->GetFont(fontname, fontsize);

	*reportSCErrors	= itsReportSCErrorsCB->IsChecked();

	if (itsSpacesPerTabInput->GetValue(&value))
		{
		*spacesPerTab	= value;
		}
	else
		{
		*spacesPerTab	= 0;
		}

	*encType		= itsEncryptionType->GetSelectedItem();

	JString name	= itsFilterFileInput->GetText();
	*filterFile		= GGetRelativeToHome(name);

	name			= itsDefaultInbox->GetText();
	*defInbox		= GGetRelativeToHome(name);

	JTextEditor::ShouldCopyWhenSelect(itsCopySelectedCB->IsChecked());
	JXTEBase::ShouldUseWindowsHomeEnd(itsHomeEndCB->IsChecked());
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GMailPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kGPrefsHelpName);
		}
	else if (sender == itsShowSenderString && message.Is(JXCheckbox::kPushed))
		{
		const JXCheckbox::Pushed* info =
			dynamic_cast(const JXCheckbox::Pushed*, &message);
		assert(info != NULL);
		AdjustForSenderString(info->IsChecked());
		}
	else if (sender == itsAutoQuote && message.Is(JXCheckbox::kPushed))
		{
		const JXCheckbox::Pushed* info =
			dynamic_cast(const JXCheckbox::Pushed*, &message);
		assert(info != NULL);
		AdjustForQuotedReply(info->IsChecked());
		}
	else if (sender == itsFilterButton && message.Is(JXButton::kPushed))
		{
		JString fullname;
		if (JXGetChooseSaveFile()->ChooseFile("","",&fullname))
			{
			JString name	= GGetRelativeToHome(fullname);
			itsFilterFileInput->SetText(name);
			}
		}
	else if (sender == itsInboxButton && message.Is(JXButton::kPushed))
		{
		JString fullname;
		if (JXGetChooseSaveFile()->ChooseFile("","",&fullname))
			{
			JString name	= GGetRelativeToHome(fullname);
			itsDefaultInbox->SetText(name);
			}
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 AdjustForSenderString (private)

 ******************************************************************************/

void
GMailPrefsDialog::AdjustForSenderString
	(
	const JBoolean showSender
	)
{
	JFontStyle style(kJFalse, kJFalse, 0, kJFalse, JGetCurrColormap()->GetInactiveLabelColor());
	if (showSender)
		{
		itsSenderStringInput->Activate();
		itsSenderText->SetFontStyle(JFontStyle());
		itsSenderStringInput->SetFontStyle(JFontStyle());
		}
	else
		{
		itsSenderStringInput->Deactivate();
		itsSenderText->SetFontStyle(style);
		itsSenderStringInput->SetFontStyle(style);
		}
}
/******************************************************************************
 AdjustForQuotedReply (private)

 ******************************************************************************/

void
GMailPrefsDialog::AdjustForQuotedReply
	(
	const JBoolean quote
	)
{
	if (quote)
		{
		itsMaxReplyInput->Activate();
		}
	else
		{
		itsMaxReplyInput->Deactivate();
		}
}
