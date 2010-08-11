/******************************************************************************
 GMAccountDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GMAccountDialog.h"
#include <GMAccount.h>
#include <GMAccountList.h>
#include "GMailboxTreeDir.h"
#include <GHelpText.h>
#include <GMGlobals.h>

#include <JXCardFile.h>
#include <JXChooseSaveFile.h>
#include <JXFileInput.h>
#include <JXHelpManager.h>
#include <JXInputField.h>
#include <JXIntegerInput.h>
#include <JXPathInput.h>
#include <JXRadioGroup.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXTextMenu.h>
#include <JXTextRadioButton.h>
#include <JXWidgetSet.h>
#include <JXWindow.h>

#include <JString.h>
#include <JColormap.h>

#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

const JInteger kCheckMailMinDelay	= 1;

static const JCharacter* kTypeMenuStr =
	" Local mailboxes"
	"|POP server";

enum
{
	kLocalMboxCmd = 1,
	kPOPServerCmd
};
/******************************************************************************
 Constructor

 ******************************************************************************/

GMAccountDialog::GMAccountDialog
	(
	JXDirector*				supervisor,
	JPtrArray<GMAccount>&	array,
	const JIndex			defIndex
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsDefaultIndex(defIndex)
{
	const JSize count = array.GetElementCount();

	itsAccountInfo = new JPtrArray<GMAccount>(JPtrArrayT::kForgetAll);
	assert(itsAccountInfo != NULL);

	itsAccountInfo->SetCompareFunction(GMAccount::CompareNicknames);

	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* account	= new GMAccount(*(array.NthElement(i)));
		assert(account != NULL);
		account->SetReadList(array.NthElement(i)->GetReadList());
		itsAccountInfo->Append(account);
		}

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMAccountDialog::~GMAccountDialog()
{
	itsAccountInfo->DeleteAll();
	delete itsAccountInfo;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GMAccountDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 530,570, "");
    assert( window != NULL );

    itsFullNameInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 280,15, 240,20);
    assert( itsFullNameInput != NULL );

    itsSMTPUserName =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 280,40, 240,20);
    assert( itsSMTPUserName != NULL );

    itsSMTPServerInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 280,65, 240,20);
    assert( itsSMTPServerInput != NULL );

    itsReplyToAddress =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 280,90, 240,20);
    assert( itsReplyToAddress != NULL );

    itsCCInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 280,115, 240,20);
    assert( itsCCInput != NULL );

    itsBCCInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 280,140, 240,20);
    assert( itsBCCInput != NULL );

    itsSigInput =
        new JXInputField(kJFalse, kJTrue, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 280,165, 240,40);
    assert( itsSigInput != NULL );

    JXScrollbarSet* scrollbarset =
        new JXScrollbarSet(window,
                    JXWidget::kFixedLeft, JXWidget::kVElastic, 10,10, 170,170);
    assert( scrollbarset != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 355,540, 70,20);
    assert( okButton != NULL );

    itsCancelButton =
        new JXTextButton(JGetString("itsCancelButton::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 105,540, 70,20);
    assert( itsCancelButton != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,65, 90,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,40, 90,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetToLabel();

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,90, 90,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetToLabel();

    itsNewButton =
        new JXTextButton(JGetString("itsNewButton::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 15,195, 60,20);
    assert( itsNewButton != NULL );
    itsNewButton->SetShortcuts(JGetString("itsNewButton::GMAccountDialog::shortcuts::JXLayout"));

    itsDeleteButton =
        new JXTextButton(JGetString("itsDeleteButton::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 15,220, 60,20);
    assert( itsDeleteButton != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,15, 90,20);
    assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetToLabel();

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,115, 90,20);
    assert( obj5_JXLayout != NULL );
    obj5_JXLayout->SetToLabel();

    JXStaticText* obj6_JXLayout =
        new JXStaticText(JGetString("obj6_JXLayout::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,140, 90,20);
    assert( obj6_JXLayout != NULL );
    obj6_JXLayout->SetToLabel();

    JXStaticText* obj7_JXLayout =
        new JXStaticText(JGetString("obj7_JXLayout::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,165, 90,20);
    assert( obj7_JXLayout != NULL );
    obj7_JXLayout->SetToLabel();

    itsSigType =
        new JXRadioGroup(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,210, 240,30);
    assert( itsSigType != NULL );

    JXTextRadioButton* obj8_JXLayout =
        new JXTextRadioButton(1, JGetString("obj8_JXLayout::GMAccountDialog::JXLayout"), itsSigType,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,5, 75,20);
    assert( obj8_JXLayout != NULL );

    JXTextRadioButton* obj9_JXLayout =
        new JXTextRadioButton(2, JGetString("obj9_JXLayout::GMAccountDialog::JXLayout"), itsSigType,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 85,5, 75,20);
    assert( obj9_JXLayout != NULL );

    JXTextRadioButton* obj10_JXLayout =
        new JXTextRadioButton(3, JGetString("obj10_JXLayout::GMAccountDialog::JXLayout"), itsSigType,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 165,5, 70,20);
    assert( obj10_JXLayout != NULL );

    itsDupButton =
        new JXTextButton(JGetString("itsDupButton::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 85,195, 90,20);
    assert( itsDupButton != NULL );

    itsDefButton =
        new JXTextButton(JGetString("itsDefButton::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 85,220, 90,20);
    assert( itsDefButton != NULL );

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 230,540, 70,20);
    assert( itsHelpButton != NULL );

    itsAccountTypeCardFile =
        new JXCardFile(window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 30,280, 470,250);
    assert( itsAccountTypeCardFile != NULL );

    itsAccountTypeMenu =
        new JXTextMenu(JGetString("itsAccountTypeMenu::GMAccountDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 15,255, 120,20);
    assert( itsAccountTypeMenu != NULL );

// end JXLayout

	JXWidgetSet* widgetSet =
        new JXWidgetSet(itsAccountTypeCardFile,
                    	JXWidget::kHElastic, JXWidget::kFixedBottom, 30,280, 470,250);
    assert( widgetSet != NULL );

// begin localLayout

    const JRect localLayout_Frame    = widgetSet->GetFrame();
    const JRect localLayout_Aperture = widgetSet->GetAperture();
    widgetSet->AdjustSize(470 - localLayout_Aperture.width(), 250 - localLayout_Aperture.height());

    itsLocalDefaultInboxInput =
        new JXFileInput(widgetSet,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 150,55, 180,20);
    assert( itsLocalDefaultInboxInput != NULL );

    itsChooseLocalInboxButton =
        new JXTextButton(JGetString("itsChooseLocalInboxButton::GMAccountDialog::localLayout"), widgetSet,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 330,55, 60,20);
    assert( itsChooseLocalInboxButton != NULL );

    JXStaticText* obj1_localLayout =
        new JXStaticText(JGetString("obj1_localLayout::GMAccountDialog::localLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,55, 110,20);
    assert( obj1_localLayout != NULL );
    obj1_localLayout->SetToLabel();

    itsLocalPathInput =
        new JXPathInput(widgetSet,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 150,30, 180,20);
    assert( itsLocalPathInput != NULL );

    itsChooseLocalPathButton =
        new JXTextButton(JGetString("itsChooseLocalPathButton::GMAccountDialog::localLayout"), widgetSet,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 330,30, 60,20);
    assert( itsChooseLocalPathButton != NULL );

    JXStaticText* obj2_localLayout =
        new JXStaticText(JGetString("obj2_localLayout::GMAccountDialog::localLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,30, 110,20);
    assert( obj2_localLayout != NULL );
    obj2_localLayout->SetToLabel();

    widgetSet->SetSize(localLayout_Frame.width(), localLayout_Frame.height());

// end localLayout

	itsAccountTypeCardFile->AppendCard(widgetSet);

	widgetSet =
        new JXWidgetSet(itsAccountTypeCardFile,
                    	JXWidget::kHElastic, JXWidget::kFixedBottom, 30,280, 470,250);
    assert( widgetSet != NULL );

// begin popLayout

    const JRect popLayout_Frame    = widgetSet->GetFrame();
    const JRect popLayout_Aperture = widgetSet->GetAperture();
    widgetSet->AdjustSize(470 - popLayout_Aperture.width(), 250 - popLayout_Aperture.height());

    itsDeleteDaysInput =
        new JXIntegerInput(widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 218,37, 40,20);
    assert( itsDeleteDaysInput != NULL );

    itsAutoCheckMinutesInput =
        new JXIntegerInput(widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 245,97, 40,20);
    assert( itsAutoCheckMinutesInput != NULL );

    itsPopServerInput =
        new JXInputField(widgetSet,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 140,130, 240,20);
    assert( itsPopServerInput != NULL );

    itsPopAccountInput =
        new JXInputField(widgetSet,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 140,155, 240,20);
    assert( itsPopAccountInput != NULL );

    itsDefaultInboxInput =
        new JXFileInput(widgetSet,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 140,205, 180,20);
    assert( itsDefaultInboxInput != NULL );

    itsChooseInboxButton =
        new JXTextButton(JGetString("itsChooseInboxButton::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 320,205, 60,20);
    assert( itsChooseInboxButton != NULL );

    JXStaticText* obj1_popLayout =
        new JXStaticText(JGetString("obj1_popLayout::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,205, 110,20);
    assert( obj1_popLayout != NULL );
    obj1_popLayout->SetToLabel();

    JXStaticText* obj2_popLayout =
        new JXStaticText(JGetString("obj2_popLayout::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,130, 110,20);
    assert( obj2_popLayout != NULL );
    obj2_popLayout->SetToLabel();

    itsLeaveMailCB =
        new JXTextCheckbox(JGetString("itsLeaveMailCB::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,15, 205,20);
    assert( itsLeaveMailCB != NULL );

    itsSavePasswdCB =
        new JXTextCheckbox(JGetString("itsSavePasswdCB::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,55, 150,20);
    assert( itsSavePasswdCB != NULL );

    JXStaticText* obj3_popLayout =
        new JXStaticText(JGetString("obj3_popLayout::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,155, 110,20);
    assert( obj3_popLayout != NULL );
    obj3_popLayout->SetToLabel();

    itsUseAPOPCB =
        new JXTextCheckbox(JGetString("itsUseAPOPCB::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,75, 205,20);
    assert( itsUseAPOPCB != NULL );

    itsAutoCheckCB =
        new JXTextCheckbox(JGetString("itsAutoCheckCB::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,95, 205,20);
    assert( itsAutoCheckCB != NULL );

    JXStaticText* obj4_popLayout =
        new JXStaticText(JGetString("obj4_popLayout::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 292,98, 55,19);
    assert( obj4_popLayout != NULL );
    obj4_popLayout->SetToLabel();

    itsDeleteFromServerCB =
        new JXTextCheckbox(JGetString("itsDeleteFromServerCB::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,35, 165,20);
    assert( itsDeleteFromServerCB != NULL );

    JXStaticText* obj5_popLayout =
        new JXStaticText(JGetString("obj5_popLayout::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 265,38, 55,19);
    assert( obj5_popLayout != NULL );
    obj5_popLayout->SetToLabel();

    itsAccountFolderInput =
        new JXPathInput(widgetSet,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 140,180, 180,20);
    assert( itsAccountFolderInput != NULL );

    itsChoosePathButton =
        new JXTextButton(JGetString("itsChoosePathButton::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 320,180, 60,20);
    assert( itsChoosePathButton != NULL );

    JXStaticText* obj6_popLayout =
        new JXStaticText(JGetString("obj6_popLayout::GMAccountDialog::popLayout"), widgetSet,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,180, 110,20);
    assert( obj6_popLayout != NULL );
    obj6_popLayout->SetToLabel();

    widgetSet->SetSize(popLayout_Frame.width(), popLayout_Frame.height());

// end popLayout

	itsAccountTypeCardFile->AppendCard(widgetSet);

	//itsAccountTypeCardFile->ShowCard(kPOPServerCmd);

	itsAccountList =
		GMAccountList::Create(this, itsAccountInfo,
			scrollbarset, scrollbarset->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 135,240);
    assert( itsAccountList != NULL );
    itsAccountList->FitToEnclosure(kJTrue, kJTrue);
	ListenTo(itsAccountList);

	window->SetTitle("Accounts");
	SetButtons(okButton, itsCancelButton);

	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	itsSigInput->SetDefaultFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	//itsDefaultInboxInput->SetBorderWidth(2);
	itsSMTPServerInput->SetIsRequired();
	itsSMTPUserName->SetIsRequired();
	itsAutoCheckMinutesInput->SetLowerLimit(kCheckMailMinDelay);

	JString home;
	if (JGetHomeDirectory(&home))
		{
		itsLocalPathInput->SetBasePath(home);
		itsLocalPathInput->ShouldAllowInvalidPath(kJTrue);
		itsAccountFolderInput->SetBasePath(home);
		itsAccountFolderInput->ShouldAllowInvalidPath(kJTrue);
		itsLocalDefaultInboxInput->SetBasePath(home);
		itsLocalDefaultInboxInput->ShouldAllowInvalidFile(kJTrue);
		itsDefaultInboxInput->SetBasePath(home);
		itsDefaultInboxInput->ShouldAllowInvalidFile(kJTrue);
		}
		
	ListenTo(itsNewButton);
	ListenTo(itsDeleteButton);
	ListenTo(itsAutoCheckCB);
	ListenTo(itsCancelButton);
	
	ListenTo(itsLeaveMailCB);
	ListenTo(itsDeleteFromServerCB);
	ListenTo(itsHelpButton);
	ListenTo(itsDefButton);
	ListenTo(itsDupButton);
	ListenTo(itsAccountTypeMenu);

	ListenTo(itsChooseInboxButton);
	ListenTo(itsChoosePathButton);

	ListenTo(itsChooseLocalInboxButton);
	ListenTo(itsChooseLocalPathButton);

	itsAccountTypeMenu->SetMenuItems(kTypeMenuStr);
	//itsAccountTypeMenu->SetToPopupChoice(kJTrue, kPOPServerCmd);
	itsAccountTypeMenu->SetUpdateAction(JXMenu::kDisableNone);

	AdjustWidgets();

	if (itsAccountInfo->GetElementCount() <= 1)
		{
		itsDeleteButton->Deactivate();
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
GMAccountDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAccountList && message.Is(GMAccountList::kNameSelected))
		{
		AdjustWidgets();
		}
	else if (sender == itsNewButton && message.Is(JXButton::kPushed))
		{
		NewAccount();
		}
	else if (sender == itsDeleteButton && message.Is(JXButton::kPushed))
		{
		const JIndex current = itsAccountList->GetCurrentIndex();
		if (itsDefaultIndex > current)
			{
			itsDefaultIndex --;
			}
		else if (itsDefaultIndex == current)
			{
			itsDefaultIndex = 1;
			}
		itsAccountInfo->DeleteElement(current);
		itsAccountList->SetCurrentAccount(1);
		AdjustWidgets();
		if (itsAccountInfo->GetElementCount() <= 1)
			{
			itsDeleteButton->Deactivate();
			}
		itsAccountList->Refresh();
		}
	else if (sender == itsCancelButton && message.Is(JXButton::kPushed))
		{
		if (itsAccountList->IsEditing())
			{
			itsAccountList->CancelEditing();
			}
		else
			{
			EndDialog(kJFalse);
			}
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		JXGetHelpManager()->ShowSection(kGAccountHelpName);
		}
	else if (sender == itsDupButton && message.Is(JXButton::kPushed))
		{
		const JIndex current  = itsAccountList->GetCurrentIndex();
		GMAccount* curAccount = itsAccountInfo->NthElement(current);
		NewAccount(curAccount);
		}
	else if (sender == itsDefButton && message.Is(JXButton::kPushed))
		{
		itsDefaultIndex	= itsAccountList->GetCurrentIndex();
		itsAccountList->SetCurrentAsDefault();
		}
	else if (sender == itsAccountTypeMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* info	= 
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert(info != NULL);
		itsAccountTypeCardFile->ShowCard(info->GetIndex());
		itsCurrentType	= info->GetIndex();
		}
	else if (sender == itsChooseLocalInboxButton && message.Is(JXButton::kPushed))
		{
		itsLocalDefaultInboxInput->ChooseFile("Choose default inbox:");
		}
	else if (sender == itsChooseLocalPathButton && message.Is(JXButton::kPushed))
		{
		itsLocalPathInput->ChoosePath("Choose account folder:");
		}
	else if (sender == itsChooseInboxButton && message.Is(JXButton::kPushed))
		{
		itsDefaultInboxInput->ChooseFile("Choose default inbox:");
		}
	else if (sender == itsChoosePathButton && message.Is(JXButton::kPushed))
		{
		itsAccountFolderInput->ChoosePath("Choose account folder:");
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 AdjustWidgets (private)

 ******************************************************************************/

void
GMAccountDialog::AdjustWidgets()
{
	JIndex current		= itsAccountList->GetCurrentIndex();
	GMAccount* account	= itsAccountInfo->NthElement(current);
	itsSMTPServerInput->SetText(account->GetSMTPServer());
	itsSMTPUserName->SetText(account->GetSMTPUser());
	itsReplyToAddress->SetText(account->GetReplyTo());
	itsCCInput->SetText(account->GetCC());
	itsBCCInput->SetText(account->GetBCC());
	itsSigInput->SetText(account->GetSig());
	itsSigType->SelectItem(account->GetSigType());

	JString home;
	if (JGetHomeDirectory(&home))
		{
		if (JIsAbsolutePath(account->GetDefaultInbox()))
			{
			itsLocalDefaultInboxInput->SetText(JConvertToRelativePath(account->GetDefaultInbox(), home));
			}
		else
			{
			itsLocalDefaultInboxInput->SetText(account->GetDefaultInbox());
			}

		JString popInbox	= account->GetPOPInbox();
		if (popInbox.IsEmpty())
			{
			popInbox		= account->GetDefaultInbox();
			}

		if (JIsAbsolutePath(popInbox))
			{
			itsDefaultInboxInput->SetText(JConvertToRelativePath(popInbox, home));
			}
		else
			{
			itsDefaultInboxInput->SetText(popInbox);
			}

		JString dir	= account->GetMailDir();
		if (dir.IsEmpty())
			{
			dir	= "mail";
			}
		else if (JIsAbsolutePath(dir))
			{
			dir	= JConvertToRelativePath(dir, home);
			}

		itsLocalPathInput->SetText(dir);
		itsAccountFolderInput->SetText(dir);
		}
	
	itsFullNameInput->SetText(account->GetFullName());
	itsPopServerInput->SetText(account->GetPOPServer());
	itsPopAccountInput->SetText(account->GetPOPAccount());
	//itsUsePopButton->SetState(account->IsUsingPOP());
	itsAutoCheckMinutesInput->SetValue(account->GetCheckInterval());
	itsLeaveMailCB->SetState(account->IsLeavingMailOnServer());
	itsSavePasswdCB->SetState(account->IsSavingPass());
	itsUseAPOPCB->SetState(account->IsUsingAPOP());
	itsAutoCheckCB->SetState(account->IsAutoChecking());
	itsDeleteFromServerCB->Deactivate();
	itsDeleteDaysInput->Deactivate();
	if (account->IsUsingPOP())
		{
		itsAccountTypeMenu->SetToPopupChoice(kJTrue, kPOPServerCmd);
		itsAccountTypeCardFile->ShowCard(kPOPServerCmd);
		itsCurrentType	= kPOPServerCmd;
		}
	else
		{
		itsAccountTypeMenu->SetToPopupChoice(kJTrue, kLocalMboxCmd);
		itsAccountTypeCardFile->ShowCard(kLocalMboxCmd);
		itsCurrentType	= kLocalMboxCmd;
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
GMAccountDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return kJTrue;
		}

	JBoolean ok	= OKToSwitchAccounts();
	if (!ok)
		{
		return kJFalse;
		}

	const JSize count	= itsAccountInfo->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* account	= itsAccountInfo->NthElement(i);
		JString path		= account->GetMailDir();
		if (!JDirectoryExists(path))
			{
			JString report	= "Not all of the account folders exist. Please check to make sure that all of the information is correct for each account.";
			JGetUserNotification()->ReportError(report);
			itsAccountList->SetCurrentAccount(i);
			AdjustWidgets();
			return kJFalse;
			}
		else
			{
			CreateItemsIfNeeded(path);
			}
		}
		
	JString defNickname	= itsAccountInfo->NthElement(itsDefaultIndex)->GetNickname();
	itsAccountInfo->Sort();

	GMAccount account;
	account.SetNickname(defNickname);
	JIndex findex;
	ok	= itsAccountInfo->SearchSorted(&account, JOrderedSetT::kFirstMatch, &findex);
	assert(ok);
	itsDefaultIndex	= findex;
	return kJTrue;
}

/******************************************************************************
 OKToSwitchAccounts (public)

 ******************************************************************************/

JBoolean
GMAccountDialog::OKToSwitchAccounts()
{
	if ((itsAccountList->IsEditing() && !itsAccountList->EndEditing()) ||
		!itsSMTPServerInput->InputValid() ||
		!itsSMTPUserName->InputValid())
		{
		return kJFalse;
		}

	if (itsCurrentType == kLocalMboxCmd)
		{
		if (itsLocalDefaultInboxInput->GetText().IsEmpty())
			{
			JGetUserNotification()->ReportError("You must specify a default inbox.");
			return kJFalse;
			}
		if (itsLocalPathInput->GetText().IsEmpty())
			{
			JGetUserNotification()->ReportError("You must specify a folder for this account.");
			return kJFalse;
			}

		JString path;
		if (!itsLocalPathInput->GetPath(&path))
			{
			JString str	= "The Account folder does not exist, would you like to create it now?";
			if (JGetUserNotification()->AskUserYes(str))
				{
				if (!CreateMailDir(itsLocalPathInput->GetText()))
					{
					return kJFalse;
					}				
				}
			else
				{
				return kJFalse;
				}
			}

		if (itsLocalPathInput->GetPath(&path))
			{
			CreateItemsIfNeeded(path);
			CreateInboxLink(path, itsLocalDefaultInboxInput->GetText(), kJTrue);
			GGetMailboxTreeDir()->AddDirectory(path);
			}		
		
		SetCurrentAccount();
		return kJTrue;
		}

	if (itsDefaultInboxInput->GetText().IsEmpty())
		{
		JGetUserNotification()->ReportError("You must specify a default inbox.");
		return kJFalse;
		}
	if (itsAccountFolderInput->GetText().IsEmpty())
		{
		JGetUserNotification()->ReportError("You must specify a folder for this account.");
		return kJFalse;
		}

	JString path;
	if (!itsAccountFolderInput->GetPath(&path))
		{
		JString str	= "The Account folder does not exist, would you like to create it now?";
		if (JGetUserNotification()->AskUserYes(str))
			{
			if (!CreateMailDir(itsAccountFolderInput->GetText()))
				{
				return kJFalse;
				}
			}
		else
			{
			return kJFalse;
			}
		}

	JString file;
	if (!itsDefaultInboxInput->GetFile(&file))
		{
		JString realPath	= itsDefaultInboxInput->GetText();
		JString home;
		if (JIsRelativePath(realPath))
			{
			if (JGetHomeDirectory(&home))
				{
				realPath	= JCombinePathAndName(home, realPath);
				}
			else
				{
				JGetUserNotification()->ReportError("There was an error creating the inbox.");
				return kJFalse;
				}
			}
			
		ofstream os(realPath);
		if (!os.good())
			{
			JGetUserNotification()->ReportError("There was an error creating the inbox.");
			return kJFalse;
			}
		os.close();		
		}

	if (itsAccountFolderInput->GetPath(&path))
		{
		CreateItemsIfNeeded(path);
		CreateInboxLink(path, itsDefaultInboxInput->GetText(), kJFalse);
		GGetMailboxTreeDir()->AddDirectory(path);
		}

	if (itsAutoCheckCB->IsChecked() && !itsAutoCheckMinutesInput->InputValid())
		{
		return kJFalse;
		}
	JIndex currentIndex	= itsAccountList->GetCurrentIndex();
	JString user		= itsPopAccountInput->GetText();
	JString server		= itsPopServerInput->GetText();
	user.TrimWhitespace();
	server.TrimWhitespace();
	if (server.IsEmpty())
		{
		JGetUserNotification()->ReportError("You must specify a valid POP server.");
		return kJFalse;
		}
	if (user.IsEmpty())
		{
		JGetUserNotification()->ReportError("You must specify a valid POP user.");
		return kJFalse;
		}
	JString accountName	= user + "@" + server;
	const JSize count	= itsAccountInfo->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* account	= itsAccountInfo->NthElement(i);
		if (i != currentIndex && account->IsUsingPOP() &&
			account->GetFullPOPAccountName() == accountName)
			{

			JString report = "These POP settings are already in use by account " + account->GetNickname();
			JGetUserNotification()->ReportError(report);
			return kJFalse;
			}
		}
	SetCurrentAccount();
	return kJTrue;
}

/******************************************************************************
 SetCurrentAccount (private)

 ******************************************************************************/

void
GMAccountDialog::SetCurrentAccount()
{
	JIndex current		= itsAccountList->GetCurrentIndex();
	GMAccount* account	= itsAccountInfo->NthElement(current);
	account->SetSMTPServer(itsSMTPServerInput->GetText());
	account->SetSMTPUser(itsSMTPUserName->GetText());
	account->SetReplyTo(itsReplyToAddress->GetText());
	account->SetCC(itsCCInput->GetText());
	account->SetBCC(itsBCCInput->GetText());
	account->SetSig(itsSigInput->GetText());
	account->SetSigType((GMAccount::SigType)itsSigType->GetSelectedItem());
	
	account->SetFullName(itsFullNameInput->GetText());
	account->SetPOPServer(itsPopServerInput->GetText());
	account->SetPOPAccount(itsPopAccountInput->GetText());
	account->ShouldUsePOP(JI2B(itsCurrentType == kPOPServerCmd));
	JInteger value = 0;
	itsAutoCheckMinutesInput->GetValue(&value);
	account->SetCheckInterval(value);
	if (account->IsUsingPOP())
		{
		account->ShouldLeaveMailOnServer(itsLeaveMailCB->IsChecked());
		account->ShouldSavePass(itsSavePasswdCB->IsChecked());
		account->ShouldUseAPOP(itsUseAPOPCB->IsChecked());
		account->ShouldAutoCheck(itsAutoCheckCB->IsChecked());
		account->SetPOPInbox(itsDefaultInboxInput->GetText());
		JString path;
		JBoolean ok	= itsAccountFolderInput->GetPath(&path);
		assert(ok);
		account->SetMailDir(path);
		}
	else
		{
		account->ShouldLeaveMailOnServer(kJFalse);
		account->ShouldSavePass(kJFalse);
		account->ShouldUseAPOP(kJFalse);
		account->ShouldAutoCheck(kJFalse);
		account->SetDefaultInbox(itsLocalDefaultInboxInput->GetText());
		JString path;
		JBoolean ok	= itsLocalPathInput->GetPath(&path);
		assert(ok);
		account->SetMailDir(path);
		}
}

/******************************************************************************
 NewAccount (private)

 ******************************************************************************/

void
GMAccountDialog::NewAccount
	(
	GMAccount* src
	)
{
	if (OKToSwitchAccounts())
		{
		GMAccount* account;
		if (src == NULL)
			{
			account	= new GMAccount();
			}
		else
			{
			account	= new GMAccount(*src);
			}
		assert(account != NULL);
		itsAccountInfo->Append(account);
		itsAccountList->SetCurrentAccount(itsAccountInfo->GetElementCount());
		AdjustWidgets();
		itsDeleteButton->Activate();
		}
}

/******************************************************************************
 CreateMailDir

 *****************************************************************************/

JBoolean
GMAccountDialog::CreateMailDir
	(
	const JCharacter* path
	)
{
	JString realPath	= path;
	JString home;
	if (JIsRelativePath(realPath))
		{
		if (JGetHomeDirectory(&home))
			{
			realPath	= JCombinePathAndName(home, path);
			}
		else
			{
			return kJFalse;
			}
		}

	JError err	= JCreateDirectory(realPath);
	if (!err.OK())
		{
		err.ReportIfError();
		return kJFalse;
		}

	CreateItemsIfNeeded(realPath);
			
	return kJTrue;
}

/******************************************************************************
 CreateInboxLink

	// create symbolic link in mail dir to system mailbox, may need to
	// have System_Inbox, System_Inbox_1, System_Inbox_2 ...
	// 
	// No, let's keep it simple. how many system mailboxes will they have?
	// 
	// check if inbox is in mail directory (need to clean the path) if ! in 
	// mail dir, then create link.
			
 *****************************************************************************/

void	
GMAccountDialog::CreateInboxLink
	(
	const JCharacter* 	path, 
	const JCharacter* 	inboxStr,
	const JBoolean 		system
	)
{

	JString home;
	if (!JGetHomeDirectory(&home))
		{
		return;
		}

	JString inbox;
	if (JIsRelativePath(inboxStr))
		{
		if (!JConvertToAbsolutePath(inboxStr, home, &inbox))
			{
			return;
			}
		}
	else
		{
		inbox	= inboxStr;
		}

	JString realPath	= path;

	JCleanPath(&realPath);
	JCleanPath(&inbox);

	JString link;
	if (system)
		{
		JString relInbox	= JConvertToRelativePath(inbox, realPath);
		if (relInbox.BeginsWith("..") ||
			relInbox.BeginsWith("/"))
			{
			JString name	= "System_Inbox";
			link	= JCombinePathAndName(realPath, name);
			}
		else
			{
			return;
			}
		}
	else
		{
		JString ipath, iname;
		JSplitPathAndName(inbox, &ipath, &iname);
		if (ipath == realPath)
			{
			return;
			}
		link	= JCombinePathAndName(realPath, "Inbox");
		}

	if (JNameUsed(link))
		{
		return;
		}
	JCreateSymbolicLink(inbox, link);
}

/******************************************************************************
 CreateItemsIfNeeded

 *****************************************************************************/

void	
GMAccountDialog::CreateItemsIfNeeded
	(
	const JCharacter* path
	)
{
	JString sentDir	= JCombinePathAndName(path, "Sent");
	if (!JNameUsed(sentDir))
		{
		JError err	= JCreateDirectory(sentDir);
		if (!err.OK())
			{
			err.ReportIfError();
			return;
			}
		}

	JString outbox	= JCombinePathAndName(path, "Outbox");
	JString drafts	= JCombinePathAndName(path, "Drafts");

	if (!JNameUsed(outbox))
		{
		ofstream os(outbox);
		os.close();
		}

	if (!JNameUsed(drafts))
		{
		ofstream os(drafts);
		os.close();
		}

}
