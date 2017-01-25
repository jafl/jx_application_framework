/******************************************************************************
 GMessageEditDir.cc

	BASE CLASS = GMManagedDirector

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GMessageEditDir.h"
#include <SMTPMessage.h>
#include <GMessageEditor.h>
#include "gMailUtils.h"
#include <GMApp.h>
#include <GPrefsMgr.h>
#include <GHelpText.h>
#include "GMDirectorManager.h"
#include "GMDirectorMenu.h"
#include "GMAttachmentTable.h"
#include "GMAccountManager.h"
#include "GMAccountMenu.h"
#include "GMAccount.h"
#include "GMApp.h"
#include "GMAccountManager.h"
#include "GMFilterManager.h"
#include <GMGlobals.h>

#include "filenew.xpm"
#include "filefloppy.xpm"
#include "fileprint.xpm"
#include "fileopen.xpm"
#include "envelopes.xpm"

#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>
#include <jx_plain_file_small.xpm>

#include "lock.xpm"
#include "send.xpm"
#include "signature.xpm"
#include "paperclip.xpm"

#include <JXToolBar.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXChooseSaveFile.h>
#include <JXColormap.h>
#include <JXApplication.h>
#include <JXTextButton.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXHelpManager.h>
#include <JXEngravedRect.h>
#include <JXImage.h>
#include <JXVertPartition.h>
#include <JXWebBrowser.h>
#include <JXGetStringDialog.h>
#include <JXFSBindingManager.h>

#include <JUserNotification.h>
#include <JColormap.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jMissingProto.h>
#include <stdlib.h>
#include <unistd.h>
#include <jAssert.h>

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"New message %k Meta-N  %i \"NewMessage::Arrow\""
	"| New mailbox %k Meta-Shift-N  %i \"NewMailbox::Arrow\" "
	"|Open mailbox... %k Meta-O %i \"Open::Arrow\""
	"%l|Save message %k Meta-S  %i \"Save::Arrow\""
	"| Save message as... %i \"SaveAs::Arrow\""
	"%l|Discard and close %i \"Discard&Clase::Arrow\""
	"|Quit %k Meta-Q %i \"Quit::Arrow\"";

enum
{
	kNewCmd = 1,
	kNewMBox,
	kOpenCmd,
	kSaveCmd,
	kSaveAsCmd,
	kDiscardCmd,
	kQuitCmd
};

// Message menu

static const JCharacter* kMessageMenuTitleStr = "Message";
static const JCharacter* kMessageMenuStr =
	"Insert quoted %k Meta-I %i \"InsertQuoted::Arrow\""
	"| Insert unquoted %i \"InsertUnQuoted::Arrow\""
	"| Insert signature %k Meta-Shift-G %i \"InsertSig::Arrow\""
	"|Insert text file %i \"InsertTextFile::Arrow\""
	"|Attach files... %k Meta-Shift-F %i \"AttachFile::Arrow\""
	"%l|Encrypt %i \"Encrypt::Arrow\""
	"%l|Show Reply-To %b %i \"ShowReplyTo::Arrow\""
	"| Show Cc %b %k Meta-Shift-C %i \"ShowCC::Arrow\""
	"| Show Bcc %b %i \"ShowBCC::Arrow\"";

enum
{
	kQuoteCmd = 1,
	kUnquoteCmd,
	kSigCmd,
	kInsertTextCmd,
	kAttachFileCmd,
	kEncryptCmd,
	kShowRepToCmd,
	kShowCcCmd,
	kShowBccCmd
};

static const JCharacter* kWindowsMenuTitleStr = "Windows";

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"   Accounts... %i \"Accounts::Arrow\""
	"  |Filters... %i \"Filters::Arrow\""
	"  |URL preferences... %i \"URLPreferences::Arrow\""
	"  |Toolbar buttons... %i \"EditToolBar::Arrow\""
	"  |File bindings... %i \"EditFileBindings::Arrow\""
	"  |Miscellaneous preferences... %i \"Preferences::Arrow\""
	"%l|Save window size as default %i \"SaveWindowDefault::Arrow\"";

enum
{
	kEditAccountsCmd =1,
	kEditFiltersCmd,
	kURLPrefsCmd,
	kEditToolBarCmd,
	kFileBindingsCmd,
	kPrefsCmd,
	kSaveWindowSizeCmd
};

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"About"
	"%l|Table of Contents %i \"TOCHelp::Arrow\""
	"|Overview|This window %i \"ThisWindowHelp::Arrow\""
	"%l|Changes|Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

static const JCharacter* kDefaultReplyString	= "On $d, $f wrote:";
static const JCharacter* kDefaultMark			= ">";

static const JCoordinate kMinEditorHeight		= 100;

const JCoordinate kAttachPartHeight		= 50;
const JCoordinate kAttachPartMinHeight	= 30;
const JCoordinate kAttachTablePartIndex	= 1;

const JFileVersion kCurrentPrefsVersion	= 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

GMessageEditDir::GMessageEditDir
	(
	JXDirector* supervisor
	)
	:
	GMManagedDirector(supervisor)
{
	itsPasswdDialog	= NULL;
	itsMessage		= NULL;
	itsAttachTable	= NULL;
	itsHeader		= NULL;
	itsStatus		= GMessageHeader::kNoReply;
	itsAccount		= GGetAccountMgr()->GetDefaultAccount();

	itsAttachTableHeight	= kAttachPartHeight;

	BuildWindow();
	GGetPrefsMgr()->GetEditorWindowPrefs(this);

	itsReplyString =		new JString(kDefaultReplyString);
	assert(itsReplyString != NULL);

	itsReplyMark =			new JString(kDefaultMark);
	assert(itsReplyMark != NULL);

	itsSentFrom =			new JString();
	assert(itsSentFrom != NULL);

	itsSentDate =			new JString();
	assert(itsSentDate != NULL);

	itsSentTextFileName =	new JString();
	assert(itsSentTextFileName != NULL);

	itsShowReplyTo	= kJFalse;
	itsShowCC		= kJFalse;
	itsShowBCC		= kJFalse;

	SetAccount(itsAccount);

	itsShowReplyString = GGetPrefsMgr()->ShowingSenderString();
	if (itsShowReplyString)
		{
		*itsReplyString = GGetPrefsMgr()->GetSenderString();
		}

	AdjustInputFields();

	GMGetApplication()->AppendNewMessage(this);

	GGetDirMgr()->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMessageEditDir::~GMessageEditDir()
{
	delete itsReplyString;
	delete itsReplyMark;
	delete itsSentFrom;
	delete itsSentDate;
	if (JFileExists(*itsSentTextFileName))
		{
		JRemoveFile(*itsSentTextFileName);
		}
	delete itsSentTextFileName;
	delete itsMenuIcon;

	GGetDirMgr()->DirectorDeleted(this);
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
GMessageEditDir::BuildWindow()
{
	JSize w				= 510;
	JSize h				= 400;
	const JSize inputAdjust	= 260;
	const JSize kSendBuffer	= 130;

	JString title;
	JString path;
	JXWindow* window = new JXWindow(this, w,h, "New Message");
	assert( window != NULL );

	window->SetMinSize(w, 250);
	window->ShouldFocusWhenShow(kJTrue);

	window->SetWMClass(GMGetWMClassInstance(), GMGetEditorWindowClass());
	GGetPrefsMgr()->GetEditorWindowSize(window);

	JSize origW	= w;
	w			= window->GetFrameWidth();
	h			= window->GetFrameHeight();

	JXMenuBar* menuBar =
		new JXMenuBar(window,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0, 0, w, kJXDefaultMenuBarHeight);
	assert(menuBar != NULL);

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsMessageMenu = menuBar->AppendTextMenu(kMessageMenuTitleStr);
	itsMessageMenu->SetMenuItems(kMessageMenuStr);
	itsMessageMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsMessageMenu);

	itsMessageMenu->SetItemImage(kEncryptCmd, lock_xpm);
	itsMessageMenu->SetItemImage(kSigCmd, signature_xpm);
	itsMessageMenu->SetItemImage(kInsertTextCmd, jx_plain_file_small);
	itsMessageMenu->SetItemImage(kAttachFileCmd, paperclip);

	itsToolBar =
		new JXToolBar(GGetPrefsMgr(), kGEditToolBarID,
			menuBar, origW, 250, window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, kJXDefaultMenuBarHeight, w, h - kJXDefaultMenuBarHeight);
	assert(itsToolBar != NULL);

	const JCoordinate newHeight = itsToolBar->GetWidgetEnclosure()->GetBoundsHeight();

	JXEngravedRect* uprect =
		new JXEngravedRect(itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,
			w ,newHeight);
	assert(uprect != NULL);

	itsAccountMenu =
		new GMAccountMenu("From: ", itsToolBar->GetWidgetEnclosure(),
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			100,5,80,20);
	assert(itsAccountMenu != NULL);
	itsAccountMenu->SetToPopupChoice(kJTrue, 1);
	ListenTo(itsAccountMenu);

	itsToLabel =
		new JXStaticText("To:", itsToolBar->GetWidgetEnclosure(),
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			60,33,30,17);
	assert(itsToLabel != NULL);

	itsToInput =
		new JXInputField(itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			100,30,w - inputAdjust,20);
	assert(itsToInput != NULL);
	ListenTo(itsToInput);
	itsToInput->SetDefaultFont(GGetPrefsMgr()->GetDefaultMonoFont(), kJDefaultFontSize);

	itsReplyToLabel =
		new JXStaticText("Reply-To:", itsToolBar->GetWidgetEnclosure(),
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			20,58,70,17);
	assert(itsReplyToLabel != NULL);

	itsReplyToInput =
		new JXInputField(itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			100,55,w - inputAdjust,20);
	assert(itsReplyToInput != NULL);
	itsReplyToInput->SetDefaultFont(GGetPrefsMgr()->GetDefaultMonoFont(), kJDefaultFontSize);

	itsCCLabel =
		new JXStaticText("Cc:", itsToolBar->GetWidgetEnclosure(),
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			58,83,30,17);
	assert(itsCCLabel != NULL);

	itsCCInput =
		new JXInputField(itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			100,80,w - inputAdjust,20);
	assert(itsCCInput != NULL);
	itsCCInput->SetDefaultFont(GGetPrefsMgr()->GetDefaultMonoFont(), kJDefaultFontSize);

	itsBCCLabel =
		new JXStaticText("Bcc:", itsToolBar->GetWidgetEnclosure(),
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			52,108,40,17);
	assert(itsBCCLabel != NULL);

	itsBCCInput =
		new JXInputField(itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			100,105,w - inputAdjust,20);
	assert(itsBCCInput != NULL);
	itsBCCInput->SetDefaultFont(GGetPrefsMgr()->GetDefaultMonoFont(), kJDefaultFontSize);

	itsSubjectLabel =
		new JXStaticText("Subject:", itsToolBar->GetWidgetEnclosure(),
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			32,133,60,17);
	assert(itsSubjectLabel != NULL);

	itsSubjectInput =
		new JXInputField(itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			100,130,w - inputAdjust,20);
	assert(itsSubjectInput != NULL);
	ListenTo(itsSubjectInput);
	itsSubjectInput->SetDefaultFont(GGetPrefsMgr()->GetDefaultMonoFont(), kJDefaultFontSize);

	JArray<JCoordinate> sizes;
	JArray<JCoordinate> minSizes;
	sizes.AppendElement(newHeight - 160);
	minSizes.AppendElement(kMinEditorHeight);
	JIndex elasticIndex	= 1;

	itsPart =
		new JXVertPartition(sizes, elasticIndex,
			minSizes, itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,160,w,newHeight - 160);
	assert(itsPart != NULL);
//	itsPart->FitToEnclosure(kJTrue, kJTrue);

	itsEditorSet =
		new JXScrollbarSet(itsPart->GetCompartment(1),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,w,newHeight - 160);
	assert(itsEditorSet != NULL);

	itsEditor =
		new GMessageEditor(this, menuBar,
			itsEditorSet, itsEditorSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,w,newHeight - 160);
	assert (itsEditor != NULL);

	itsToInput->ShareEditMenu(itsEditor);
	itsToInput->ShouldAllowDragAndDrop(kJTrue);

	itsReplyToInput->ShareEditMenu(itsEditor);
	itsReplyToInput->ShouldAllowDragAndDrop(kJTrue);

	itsCCInput->ShareEditMenu(itsEditor);
	itsCCInput->ShouldAllowDragAndDrop(kJTrue);

	itsBCCInput->ShareEditMenu(itsEditor);
	itsBCCInput->ShouldAllowDragAndDrop(kJTrue);

	itsSubjectInput->ShareEditMenu(itsEditor);
	itsSubjectInput->ShouldAllowDragAndDrop(kJTrue);

	itsSendButton =
		new JXTextButton("Send", itsToolBar->GetWidgetEnclosure(),
			JXWidget::kFixedRight, JXWidget::kFixedTop,
			w - kSendBuffer,5,70,20);
	assert(itsSubjectInput != NULL);
//	itsSendButton->SetShortcuts("#S");
	ListenTo(itsSendButton);

	GMDirectorMenu* menu =
		new GMDirectorMenu(kWindowsMenuTitleStr, menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert(menu != NULL);
	menuBar->AppendMenu(menu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsFileMenu->SetItemImage(kNewCmd, filenew);
	itsFileMenu->SetItemImage(kNewMBox, envelopes);
	itsFileMenu->SetItemImage(kOpenCmd, fileopen);
	itsFileMenu->SetItemImage(kSaveCmd, filefloppy);

	itsHelpMenu->SetItemImage(kTOCCmd, jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, JXPM(jx_help_specific));

	itsToolBar->LoadPrefs();

	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kNewCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenCmd);
		itsToolBar->NewGroup();

		itsToolBar->AppendButton(itsFileMenu, kSaveCmd);
		}

	itsMenuIcon = new JXImage(window->GetDisplay(), filenew);
	assert(itsMenuIcon != NULL);
	itsMenuIcon->ConvertToRemoteStorage();
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GMessageEditDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}
	else if (sender == itsMessageMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleMessageMenu(selection->GetIndex());
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}
	else if (sender == itsMessageMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMessageMenu();
		}
	else if (sender == itsAccountMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		SetAccount(GGetAccountMgr()->GetAccount(selection->GetIndex()));
		}
	else if (sender == itsAccountMenu && message.Is(GMAccountMenu::kItemsChanged))
		{
		SetAccount(GGetAccountMgr()->GetDefaultAccount());
		}
	else if (sender == itsSendButton && message.Is(JXButton::kPushed))
		{
		Send();
		}
	else if (sender == itsToInput && message.Is(JXWidget::kLostFocus))
		{
		JString text = itsToInput->GetText();
		if (!text.IsEmpty())
			{
			AdjustWindowTitle();
			}
		}
	else if (sender == itsSubjectInput && message.Is(JXWidget::kLostFocus))
		{
		JString text = itsSubjectInput->GetText();
		if (!text.IsEmpty())
			{
			AdjustWindowTitle();
			}
		}
	else if (sender == itsPasswdDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			JString passwd = itsPasswdDialog->GetString();
			JPtrArray<JString> names(JPtrArrayT::kForgetAll);
			GParseNameLists(itsToInput->GetText(), itsCCInput->GetText(),
							itsBCCInput->GetText(), names);

			JString buffer;
			JBoolean ok =
				GEncryptPGP2_6_2(itsEditor->GetText(), passwd, &names, &buffer);
			if (ok && !buffer.IsEmpty())
				{
				itsEditor->SelectAll();
				itsEditor->Paste(buffer);
				}
			}
		itsPasswdDialog = NULL;
		}
	else if (sender == itsMessage)
		{
		if (message.Is(SMTPMessage::kMessageSent))
			{
			itsMessage = NULL;
			if (itsHeader != NULL && itsStatus != GMessageHeader::kNoReply)
				{
				itsHeader->SetReplyStatus(itsStatus);
				GMGetApplication()->HeaderStateChanged(itsHeader);
				}
			Close();
			}
		else if (message.Is(SMTPMessage::kSendFailure))
			{
			GetWindow()->Show();
			StopListening(itsMessage);
			itsMessage = NULL;
			}
		}
	else
		{
		GMManagedDirector::Receive(sender, message);
		}

}

/******************************************************************************
 HandleFileMenu


 ******************************************************************************/

void
GMessageEditDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kNewCmd)
		{
		GMessageEditDir* dir = new GMessageEditDir(JXGetApplication());
		assert(dir != NULL);
		dir->Activate();
		}
	else if (index == kNewMBox)
		{
		JString mbox;
		if (JXGetChooseSaveFile()->SaveFile("Name of new mailbox:","", "",  &mbox))
			{
			GMGetApplication()->NewMailbox(mbox);
			}
		}
	else if (index == kOpenCmd)
		{
		GMGetApplication()->OpenMailbox();
		}
	else if (index == kSaveCmd)
		{
		if (itsFileName.IsEmpty())
			{
			JXGetChooseSaveFile()->SaveFile("Save text as:","", "",  &itsFileName);
			}
		if (!itsFileName.IsEmpty())
			{
			std::ofstream os(itsFileName);
			if (os.good())
				{
				itsEditor->GetText().Print(os);
				JString path;
				JString name;
				JSplitPathAndName(itsFileName, &path, &name);
				JString menuName = "Save message (" + name + ")";
				itsFileMenu->SetItemText(kSaveCmd, menuName);
				}
			else
				{
				JString notice = "Unable to open file " + itsFileName;
				JGetUserNotification()->ReportError(notice);
				}
			}
		}
	else if (index == kSaveAsCmd)
		{
		JXGetChooseSaveFile()->SaveFile("Save text as:","", "",  &itsFileName);
		if (!itsFileName.IsEmpty())
			{
			std::ofstream os(itsFileName);
			if (os.good())
				{
				itsEditor->GetText().Print(os);
				JString path;
				JString name;
				JSplitPathAndName(itsFileName, &path, &name);
				JString menuName = "Save message (" + name + ")";
				itsFileMenu->SetItemText(kSaveCmd, menuName);
				}
			else
				{
				JString notice = "Unable to open file " + itsFileName;
				JGetUserNotification()->ReportError(notice);
				}
			}
		}
	else if (index == kDiscardCmd)
		{
		Close();
		}
	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdateMessageMenu


 ******************************************************************************/

void
GMessageEditDir::UpdateMessageMenu()
{
	itsMessageMenu->EnableItem(kSigCmd);
	if (itsSentTextFileName->IsEmpty())
		{
		itsMessageMenu->DisableItem(kQuoteCmd);
		itsMessageMenu->DisableItem(kUnquoteCmd);
		}
	else
		{
		itsMessageMenu->EnableItem(kQuoteCmd);
		itsMessageMenu->EnableItem(kUnquoteCmd);
		}
	if (GEncryptionEnabled())
		{
		itsMessageMenu->EnableItem(kEncryptCmd);
		}
	else
		{
		itsMessageMenu->DisableItem(kEncryptCmd);
		}
	if (itsShowReplyTo)
		{
		itsMessageMenu->CheckItem(kShowRepToCmd);
		}
	if (itsShowCC)
		{
		itsMessageMenu->CheckItem(kShowCcCmd);
		}
	if (itsShowBCC)
		{
		itsMessageMenu->CheckItem(kShowBccCmd);
		}
}

/******************************************************************************
 HandleMessageMenu


 ******************************************************************************/

void
GMessageEditDir::HandleMessageMenu
	(
	const JIndex index
	)
{
	if (index == kQuoteCmd)
		{
		InsertText(kJTrue);
		}
	else if (index == kUnquoteCmd)
		{
		InsertText(kJFalse);
		}
	else if (index == kSigCmd)
		{
		itsSig	= itsAccount->GetSig();
		itsSig.Prepend("--\n");
		if (!itsSig.IsEmpty())
			{
			itsEditor->Paste(itsSig);
			}
		itsSigType	= GMAccount::kManualSig;
		}
	else if (index == kInsertTextCmd)
		{
		JString filename;
		if (JXGetChooseSaveFile()->ChooseFile("Text file","",&filename))
			{
			JString buffer;
			JReadFile(filename, &buffer);
			JBoolean ok = kJTrue;
			if (itsEditor->ContainsIllegalChars(buffer))
				{
				ok = JGetUserNotification()->AskUserYes("This file contains illegal characters. It is probably a binary file. Are you sure you want to insert it?");
				}
			if (ok)
				{
				itsEditor->Paste(buffer);
				}
			}
		}
	else if (index == kAttachFileCmd)
		{
		JPtrArray<JString> files(JPtrArrayT::kForgetAll);
		if (JXGetChooseSaveFile()->ChooseFiles("Text file","",&files))
			{
			const JSize count	= files.GetElementCount();
			for (JIndex i = 1; i <= count; i++)
				{
				FileDropped(*(files.GetElement(i)), kJFalse);
				}
			}
		}
	else if (index == kEncryptCmd)
		{
		if (itsToInput->GetText().IsEmpty())
			{
			JGetUserNotification()->ReportError("You must specify a recipient.");
			}
		else if (GGetPrefsMgr()->GetEncryptionType() == GPrefsMgr::kPGP2_6)
			{
			assert(itsPasswdDialog == NULL);
			itsPasswdDialog =
				new JXGetStringDialog(this, JGetString("GMGetPasswordTitle"),
									  JGetString("GMGetPasswordPrompt"),
									  NULL, kJTrue, kJTrue);
			assert(itsPasswdDialog != NULL);
			ListenTo(itsPasswdDialog);
			itsPasswdDialog->BeginDialog();
			}
		else
			{
			JPtrArray<JString> names(JPtrArrayT::kForgetAll);
			GParseNameLists(itsToInput->GetText(), itsCCInput->GetText(),
							itsBCCInput->GetText(), names);

			JString buffer;
			JBoolean ok =
				GEncryptGPG1_0(itsEditor->GetText(), &names, &buffer);
			if (ok && !buffer.IsEmpty())
				{
				itsEditor->SelectAll();
				itsEditor->Paste(buffer);
				}
			}
		}
	else if (index == kShowRepToCmd)
		{
		itsShowReplyTo = !itsShowReplyTo;
		AdjustInputFields();
		if (itsShowReplyTo)
			{
			itsReplyToInput->Focus();
			}
		}
	else if (index == kShowCcCmd)
		{
		itsShowCC = !itsShowCC;
		AdjustInputFields();
		if (itsShowCC)
			{
			itsCCInput->Focus();
			}
		}
	else if (index == kShowBccCmd)
		{
		itsShowBCC = !itsShowBCC;
		AdjustInputFields();
		if (itsShowBCC)
			{
			itsBCCInput->Focus();
			}
		}
}

/******************************************************************************
 HandlePrefsMenu


 ******************************************************************************/

void
GMessageEditDir::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kPrefsCmd)
		{
		GGetPrefsMgr()->EditPrefs();
		}
	else if (index == kEditAccountsCmd)
		{
		GGetAccountMgr()->EditAccounts();
		}
	else if (index == kEditFiltersCmd)
		{
		GGetFilterMgr()->EditFilters();
		}
	else if (index == kURLPrefsCmd)
		{
		JXGetWebBrowser()->EditPrefs();
		}
	else if (index == kEditToolBarCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kFileBindingsCmd)
		{
		JXFSBindingManager::Instance()->EditBindings();
		}
	else if (index == kSaveWindowSizeCmd)
		{
		GGetPrefsMgr()->SetEditorWindowPrefs(this);
		}

}

/******************************************************************************
 HandleHelpMenu


 ******************************************************************************/

void
GMessageEditDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		GMGetApplication()->DisplayAbout();
		}
	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(kGTOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kGOverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kGSendHelpName);
		}
	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowSection(kGChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kGCreditsName);
		}
}

/******************************************************************************
 Send

 ******************************************************************************/

void
GMessageEditDir::Send()
{
	if (itsToInput->GetText().IsEmpty())
		{
		JGetUserNotification()->ReportError("You must specify a recipient.");
		itsToInput->Focus();
		return;
		}

	GetWindow()->Hide();
	JXGetApplication()->DisplayBusyCursor();
	itsMessage = new SMTPMessage();
	assert(itsMessage != NULL);
	ListenTo(itsMessage);
	itsMessage->SetTo(itsToInput->GetText());
	itsMessage->SetFrom(itsAccount->GetSMTPUser());
	itsMessage->SetSubject(itsSubjectInput->GetText());
	itsMessage->SetCC(itsCCInput->GetText());
	itsMessage->SetBCC(itsBCCInput->GetText());
	itsMessage->SetReplyTo(itsReplyToInput->GetText());
	itsMessage->SetData(itsAttachTable, itsEditor);
	itsMessage->SetAccount(itsAccount);
	itsMessage->Send();
}

/******************************************************************************
 Close

 ******************************************************************************/

JBoolean
GMessageEditDir::Close()
{
	return GMManagedDirector::Close();
}

/******************************************************************************
 SetTo

 ******************************************************************************/

void
GMessageEditDir::SetTo
	(
	const JString& to
	)
{
	JString temp = to;
	temp.TrimWhitespace();
	GCompressWhitespace(&temp);
	itsToInput->SetText(temp);
	AdjustWindowTitle();
	itsEditor->Focus();
}

/******************************************************************************
 SetCC

 ******************************************************************************/

void
GMessageEditDir::SetCC
	(
	const JString& cc
	)
{
	JString temp(cc);
	temp.TrimWhitespace();
	GCompressWhitespace(&temp);
	if (!temp.IsEmpty())
		{
		itsCCInput->SetText(temp);
		itsShowCC = kJTrue;
		AdjustInputFields();
		}
}

/******************************************************************************
 SetSubject

 ******************************************************************************/

void
GMessageEditDir::SetSubject
	(
	const JString& subject
	)
{
	itsSubjectInput->SetText(subject);
	AdjustWindowTitle();
}

/******************************************************************************
 SetSentFrom

 ******************************************************************************/

void
GMessageEditDir::SetSentFrom
	(
	const JString& sentfrom
	)
{
	*itsSentFrom = sentfrom;
	itsSentFrom->TrimWhitespace();
}

/******************************************************************************
 SetSentDate

 ******************************************************************************/

void
GMessageEditDir::SetSentDate
	(
	const JString& sentdate
	)
{
	*itsSentDate = sentdate;
}

/******************************************************************************
 SetText

 ******************************************************************************/

void
GMessageEditDir::SetText
	(
	const JString& filename
	)
{
	*itsSentTextFileName = filename;
}

/******************************************************************************
 InsertText

 ******************************************************************************/

void
GMessageEditDir::InsertText
	(
	const JBoolean marked
	)
{
	JString text;
	std::ifstream is(*itsSentTextFileName);
	JReadAll(is, &text);
	JIndex startIndex;
	itsEditor->GetCaretLocation(&startIndex);

	if (!marked)
		{
		itsEditor->Paste(text);
		}
	else
		{
		if (itsShowReplyString)
			{
			JString replystring(*itsReplyString);
			JIndex findindex;
			if (replystring.LocateSubstring("$d", &findindex))
				{
				replystring.ReplaceSubstring(findindex, findindex+1, *itsSentDate);
				}
			if (replystring.LocateSubstring("$f", &findindex))
				{
				replystring.ReplaceSubstring(findindex, findindex+1, *itsSentFrom);
				}
			if (replystring.GetLastCharacter() != '\n')
				{
				replystring.Append("\n");
				}

			replystring.Append("\n");
			text.Prepend(replystring);
			}
		GQuoteString(&text, !itsShowReplyString);
		itsEditor->Paste(text);
		}
	itsEditor->SelectLine(itsEditor->GetLineForChar(startIndex));
	itsEditor->TEScrollToSelection(kJFalse);
	itsEditor->SetCaretLocation(startIndex);
}

/******************************************************************************
 AdjustInputFields

 ******************************************************************************/

void
GMessageEditDir::AdjustInputFields()
{
	JSize inputcount = 0;
	const JCoordinate kHeightBase	= 55;
	const JCoordinate kTextOffset	= 3;
	if (itsShowReplyTo)
		{
		itsReplyToInput->Place(100,kHeightBase);
		itsReplyToLabel->Place(20,kHeightBase + kTextOffset);
		inputcount++;
		itsReplyToInput->Show();
		itsReplyToLabel->Show();
		}
	else
		{
		itsReplyToInput->Hide();
		itsReplyToLabel->Hide();
		}
	if (itsShowCC)
		{
		itsCCInput->Place(100,kHeightBase + inputcount*25);
		itsCCLabel->Place(58,kHeightBase + kTextOffset + inputcount*25);
		inputcount++;
		itsCCInput->Show();
		itsCCLabel->Show();
		}
	else
		{
		itsCCInput->Hide();
		itsCCLabel->Hide();
		}
	if (itsShowBCC)
		{
		itsBCCInput->Place(100,kHeightBase + inputcount*25);
		itsBCCLabel->Place(52,kHeightBase + kTextOffset + inputcount*25);
		inputcount++;
		itsBCCInput->Show();
		itsBCCLabel->Show();
		}
	else
		{
		itsBCCInput->Hide();
		itsBCCLabel->Hide();
		}
	itsSubjectInput->Place(100, kHeightBase + inputcount*25);
	itsSubjectLabel->Place(32, kHeightBase + kTextOffset + inputcount*25);
	inputcount++;

	JSize currentheight = itsPart->GetFrameHeight();
	JRect frame = itsPart->GetFrame();
	JCoordinate newpos = kHeightBase + inputcount*25;
	JCoordinate oldpos = frame.top;
	itsPart->Move(0, newpos - oldpos);
	itsPart->AdjustSize(0, oldpos - newpos);
//	GetWindow()->AdjustSize(0, newpos-oldpos);
}

/******************************************************************************
 AdjustWindowTitle

 ******************************************************************************/

void
GMessageEditDir::AdjustWindowTitle()
{
	JString title;
	if (!(itsToInput->GetText().IsEmpty()))
		{
		title = itsToInput->GetText();
		JIndex findindex;
		if (title.LocateSubstring("@", &findindex))
			{
			title.RemoveSubstring(findindex, title.GetLength());
			}
		}
	if (!(itsSubjectInput->GetText().IsEmpty()))
		{
		if (!title.IsEmpty())
			{
			title.Append(":");
			}
		JString sub = itsSubjectInput->GetText();
		const JSize kMaxSubjectWidth = 10;
		if (sub.GetLength() > kMaxSubjectWidth)
			{
			sub.RemoveSubstring(10, sub.GetLength());
			}
		title.Append(sub);
		}
	GetWindow()->SetTitle(title);
	GGetDirMgr()->DirectorChanged(this);
}

/******************************************************************************
 SaveState


 ******************************************************************************/

void
GMessageEditDir::SaveState
	(
	std::ostream& os
	)
{
	os << itsToInput->GetText();
	os << itsReplyToInput->GetText();
	os << itsCCInput->GetText();
	os << itsBCCInput->GetText();
	os << itsSubjectInput->GetText();

	os << itsShowReplyTo << " ";
	os << itsShowCC << " ";
	os << itsShowBCC << " ";

	os << *itsReplyString;
	os << *itsReplyMark;
	os << *itsSentFrom;
	os << *itsSentDate;

	os << itsEditor->GetText();

	JString buffer;
	if (JFileExists(*itsSentTextFileName))
		{
		JReadFile(*itsSentTextFileName, &buffer);
		}
	os << buffer;

	GetWindow()->WriteGeometry(os);

	JXScrollbar* hsb = itsEditorSet->GetHScrollbar();
	JXScrollbar* vsb = itsEditorSet->GetVScrollbar();
	os << hsb->GetValue() << " ";
	os << vsb->GetValue() << " ";

	if (itsAttachTable != NULL)
		{
		const JSize count	= itsAttachTable->GetAttachmentCount();
		os << ' ' << count << ' ';
		for (JIndex i = 1; i <= count; i++)
			{
			JBoolean binary;
			const JString& name	= itsAttachTable->GetAttachmentName(i, &binary);
			os << name << ' ';
			}
		}
	else
		{
		os << ' ' << 0 << ' ';
		}
	os << itsAccount->GetNickname() << ' ';
}

/******************************************************************************
 ReadState


 ******************************************************************************/

void
GMessageEditDir::ReadState
	(
	std::istream& 			is,
	const JFileVersion&	version
	)
{
	JString buffer;
	is >> buffer;
	itsToInput->SetText(buffer);
	is >> buffer;
	itsReplyToInput->SetText(buffer);
	is >> buffer;
	itsCCInput->SetText(buffer);
	is >> buffer;
	itsBCCInput->SetText(buffer);
	is >> buffer;
	itsSubjectInput->SetText(buffer);

	JBoolean visible;
	is >> visible;
	itsShowReplyTo = visible;
	is >> visible;
	itsShowCC = visible;
	is >> visible;
	itsShowBCC = visible;

	is >> *itsReplyString;
	is >> *itsReplyMark;
	is >> *itsSentFrom;
	is >> *itsSentDate;

	is >> buffer;
	itsEditor->SetText(buffer);

	is >> buffer;
	if (!buffer.IsEmpty())
		{
		const JError err = JCreateTempFile(itsSentTextFileName);
		if (err.OK())
			{
			std::ofstream os(*itsSentTextFileName);
			buffer.Print(os);
			}
		}

	GetWindow()->ReadGeometry(is);
	JCoordinate hval, vval;
	is >> hval;
	is >> vval;
	itsEditor->UpdateScrollbars();
	JXScrollbar* hsb = itsEditorSet->GetHScrollbar();
	JXScrollbar* vsb = itsEditorSet->GetVScrollbar();
	hsb->SetValue(hval);
	vsb->SetValue(vval);

	AdjustInputFields();

	if (version > 2)
		{
		JSize count;
		is >> count;
		for (JIndex i = 1; i <= count; i++)
			{
			JString name;
			is >> name;
			if (JFileReadable(name))
				{
				FileDropped(name, kJFalse);
				}			
			}
		}
	if (version > 3)
		{
		JString name;
		is >> name;
		GMAccount* account;
		GGetAccountMgr()->GetAccountWithNickname(name, &account);
		SetAccount(account);
		}
}

/******************************************************************************
 QuoteLineRange

 ******************************************************************************/

void
GMessageEditDir::QuoteLineRange
	(
	const JIndex linestart,
	const JIndex lineend
	)
{
	for (JSize i = linestart+1; i <= lineend; i++)
		{
		itsEditor->SelectLine(i-1);
		JString test;
		itsEditor->GetSelection(&test);
		test.TrimWhitespace();
		if ((test == "\n") || test.IsEmpty())
			{
			itsEditor->SelectLine(i);
			itsEditor->GetSelection(&test);
			test.TrimWhitespace();
			JString mark;
			if (test.BeginsWith(">"))
				{
				mark = ">";
				}
			else if (!test.IsEmpty())
				{
				mark = "> ";
				}
			itsEditor->GoToLine(i);
			itsEditor->Paste(mark);
			}
		}
}


/******************************************************************************
 CheckForPendingMessage

 ******************************************************************************/

void
GMessageEditDir::CheckForPendingMessage()
{
	if (itsMessage != NULL)
		{
		itsMessage->BlockUntilOkOrFail();
		}
}

/******************************************************************************
 FileDropped (public)

 ******************************************************************************/

void
GMessageEditDir::FileDropped
	(
	const JString& filename,
	const JBoolean local
	)
{
	if (itsPart->GetCompartmentCount() == 1)
		{
		JSize height	= itsAttachTableHeight;
		JXContainer* container;
		if (itsPart->PrependCompartment(height, kAttachPartMinHeight, &container))
			{
			JXScrollbarSet* sbset =
				new JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic,
					0,0,
					container->GetBoundsWidth(),container->GetBoundsHeight());
			assert(sbset != NULL);

			itsAttachTable =
				new GMAttachmentTable(kJTrue, sbset, sbset->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic,
					0,0,
					10,10);
			assert(itsAttachTable != NULL);
			itsAttachTable->FitToEnclosure(kJTrue, kJTrue);
			itsAttachTable->SetEditMenuHandler(itsEditor);
			}
		}
	if (itsAttachTable != NULL)
		{
		itsAttachTable->AddFile(filename, local);
		}
}

/******************************************************************************
 SetReplyType (public)

 ******************************************************************************/

void
GMessageEditDir::SetReplyType
	(
	GMessageHeader::ReplyStatus status
	)
{
	itsStatus	= status;
}

/******************************************************************************
 SetHeader (public)

 ******************************************************************************/

void
GMessageEditDir::SetHeader
	(
	GMessageHeader* header,
	GMessageHeader::ReplyStatus status
	)
{
	itsHeader	= header;
	if (status == GMessageHeader::kForwarded ||
		status == GMessageHeader::kRedirected)
		{
		return;
		}
		
	const JString& account	= header->GetAccount();

	JPtrArray<JString> names(JPtrArrayT::kDeleteAll);
	GParseNameList(header->GetTo(), names);
	if (names.GetElementCount() > 0 &&
		GGetAccountMgr()->GetReplyAccount(*(names.GetElement(1)), &itsAccount))
		{
		SetAccount(itsAccount);
		}
	else if (!account.IsEmpty() && GGetAccountMgr()->GetPOPAccount(account, &itsAccount))
		{
		SetAccount(itsAccount);
		return;
		}
}

/******************************************************************************
 SetAccount (public)

 ******************************************************************************/

void
GMessageEditDir::SetAccount
	(
	GMAccount* account
	)
{
	itsAccount	= account;
	JString replyto = itsAccount->GetReplyTo();
	if (itsStatus < GMessageHeader::kForwarded)
		{
		if (!replyto.IsEmpty())
			{
			itsShowReplyTo = kJTrue;
			itsReplyToInput->SetText(replyto);
			}
		else
			{
			itsShowReplyTo = kJFalse;
			itsReplyToInput->SetText("");
			}

		JString cc = itsAccount->GetCC();
		if (!cc.IsEmpty())
			{
			itsShowCC = kJTrue;
			itsCCInput->SetText(cc);
			}
		else
			{
			itsShowCC = kJFalse;
			itsCCInput->SetText("");
			}

		JString bcc = itsAccount->GetBCC();
		if (!bcc.IsEmpty())
			{
			itsShowBCC = kJTrue;
			itsBCCInput->SetText(bcc);
			}
		else
			{
			itsShowBCC = kJFalse;
			itsBCCInput->SetText("");
			}
		}

	const JSize count	= itsAccountMenu->GetItemCount();
	for (JIndex i = 1; i <= count; i++)
		{
		if (itsAccountMenu->GetItemText(i) == itsAccount->GetNickname())
			{
			itsAccountMenu->SetPopupChoice(i);
			break;
			}
		}
	AdjustInputFields();

	if (!itsSig.IsEmpty() && itsSigType != GMAccount::kManualSig)
		{
		if (itsSigType == GMAccount::kPrependSig)
			{
			if (itsEditor->GetText().BeginsWith(itsSig))
				{
				itsEditor->SetSelection(1, itsSig.GetLength());
				itsEditor->DeleteSelection();
				}
			}
		else if (itsSigType == GMAccount::kAppendSig)
			{
			if (itsEditor->GetText().EndsWith(itsSig))
				{
				itsEditor->SetSelection(itsEditor->GetTextLength() - itsSig.GetLength() + 1, itsEditor->GetTextLength());
				itsEditor->DeleteSelection();
				}
			}
		}
	itsSig		= itsAccount->GetSig();
	itsSig.Prepend("--\n");
	itsSigType	= itsAccount->GetSigType();
	if (itsSigType == GMAccount::kPrependSig)
		{
		itsSig.PrependCharacter('\n');
		itsSig.AppendCharacter('\n');
		itsSig.AppendCharacter('\n');
		if (itsEditor->GetTextLength() > 1)
			{
			itsEditor->SetCaretLocation(1);
			}
		itsEditor->Paste(itsSig);
		itsEditor->SetCaretLocation(1);
		}
	else if (itsSigType == GMAccount::kAppendSig)
		{
		itsSig.PrependCharacter('\n');
		itsSig.PrependCharacter('\n');
		JIndex caretLoc	= 1;
		if (itsEditor->GetTextLength() > 1)
			{
			itsEditor->SetCaretLocation(itsEditor->GetTextLength() + 1);
			caretLoc	= itsEditor->GetTextLength() + 1;
			}
		itsEditor->Paste(itsSig);
		itsEditor->SetCaretLocation(caretLoc);
		}
	else
		{
		itsSig.Clear();
		}
}

/******************************************************************************
 WindowPrefs (public)

 ******************************************************************************/

void
GMessageEditDir::ReadWindowPrefs
	(
	std::istream& is
	)
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentPrefsVersion)
		{
		return;
		}
	GetWindow()->ReadGeometry(is);
	JSize count;
	is >> count;
	if (version == 1)
		{
		if (count > 1)
			{
			// read in junk values from when the attachment was to the right.
			JCoordinate value;
			is >> value;
			is >> value;
			}
		}
	else if (version > 1)
		{
		JArray<JCoordinate> sizes;
		for (JIndex i = 1; i <= count; i++)
			{
			JCoordinate value;
			is >> value;
			sizes.AppendElement(value);
			}
		if (count == 2)
			{
			itsAttachTableHeight		= sizes.GetElement(kAttachTablePartIndex);
			sizes.RemoveElement(kAttachTablePartIndex);
			}
		itsPart->JPartition::SetCompartmentSizes(sizes);
		}
}

void
GMessageEditDir::WriteWindowPrefs
	(
	std::ostream& os
	)
{
	os << ' ' << kCurrentPrefsVersion << ' ';
	GetWindow()->WriteGeometry(os);

	const JArray<JCoordinate>& sizes = itsPart->GetCompartmentSizes();
	const JSize count	= itsPart->GetCompartmentCount();
	os << ' ' << count << ' ';
	for (JSize i = 1; i <= count; i++)
		{
		os << sizes.GetElement(i) << " ";
		}
	if (count == 2)
		{
		itsAttachTableHeight			= sizes.GetElement(2);
		}
}
