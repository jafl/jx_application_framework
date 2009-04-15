/******************************************************************************
 GMessageViewDir.cc

	BASE CLASS = GMManagedDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GMessageViewDir.h"
#include "GMessageView.h"
#include "GMessageEditor.h"
#include "GMessageHeader.h"
#include "GMessageTableDir.h"
#include "GMessageTable.h"
#include "GMessageEditDir.h"
#include "GMessageHeaderList.h"
#include "JXGetPasswordDialog.h"
#include "gMailUtils.h"
#include <GMApp.h>
#include <GHelpText.h>
#include <GPrefsMgr.h>
#include "GMChooseSaveFile.h"
#include "GMPTPrinter.h"
#include "GMDirectorManager.h"
#include "GMDirectorMenu.h"
#include "GMAttachmentTable.h"
#include "GMMIMEParser.h"
#include "GMAccountManager.h"
#include "GMMessageDragSource.h"
#include "GMFilterManager.h"
#include "JXFSBindingManager.h"
#include "GMailboxTreeDir.h"
#include "GMMailboxData.h"
#include "GMMarkHeaderDeletedTask.h"

#include "filenew.xpm"
#include "filefloppy.xpm"
#include "fileprint.xpm"
#include "fileopen.xpm"
#include "envelopes.xpm"
#include "check_mail.xpm"
#include "mini-left.xpm"
#include "mini-right.xpm"
#include "mini-del-right.xpm"
#include "paperclip.xpm"

#include "unlock.xpm"
#include "reply.xpm"
#include "reply_all.xpm"
#include "reply_sender.xpm"
#include "forward.xpm"
#include "redirect.xpm"

#include "address_entry.xpm"
#include "mailbox.xpm"

#include "jx_help_specific.xpm"
#include "jx_help_toc.xpm"

#include <JXToolBar.h>

#include <JXApplication.h>
#include <JXColormap.h>
#include <JXFSDirMenu.h>
#include <JXDownRect.h>
#include <JXFontManager.h>
#include <JXHelpManager.h>
#include <JXHorizPartition.h>
#include <JXKeyModifiers.h>
#include <JXImage.h>
#include <JXMenuBar.h>
#include <JXScrollbar.h>
#include <JXScrollbarSet.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXEngravedRect.h>
#include <JXVertPartition.h>
#include <JXWindow.h>
#include <JXWebBrowser.h>

#include <JColormap.h>
#include <JError.h>
#include <JPtrArray-JString.h>
#include <JRegex.h>
#include <JRunArray.h>
#include <JTableSelection.h>
#include <JDirInfo.h>
#include <JUserNotification.h>

#include <GMGlobals.h>

#include <jx_plain_file_small.xpm>

#include <jXConstants.h>

#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jDirUtil.h>
#include <jMissingProto.h>

#include <jFStreamUtil.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strstream>
#include <jAssert.h>

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"   New message %k Meta-N %i \"NewMessage::Arrow\""
	"  |New mailbox %k Meta-Shift-N %i \"NewMailbox::Arrow\""
	"  |Open mailbox... %k Meta-O %i \"OpenMailbox::Arrow\""
	"%l|Save... %k Meta-S %i \"Save::Arrow\" "
	"  |Append... %i \"Append::Arrow\" "
	"  |Transfer to"
	"  |Copy to"
	"%l|Page setup... %i \"PageSetup::Arrow\""
	"  |Print... %k Meta-P  %i \"Print::Arrow\""
	"%l|Show next %k Meta-plus %i \"ShowNect::Arrow\""
	"  |Show previous %k Meta-minus %i \"ShowPrev::Arrow\""
	"  |Delete and show next %k Meta-Shift-D %i \"DelShowNext::Arrow\""
	"%l|Close %k Meta-W %i \"Close::Arrow\""
	"  |Close and delete %k Meta-D %i \"CloseDelete::Arrow\""
	"  |Quit %k Meta-Q %i \"Quit::Arrow\"";

enum
{
	kNewCmd = 1,
	kNewMBox,
	kOpenCmd,
	kSaveCmd,
	kAppendCmd,
	kTransferToCmd,
	kCopyToCmd,
	kPageSetupCmd,
	kPrintCmd,
	kShowNextCmd,
	kShowPrevCmd,
	kDeleteShowNextCmd,
	kCloseCmd,
	kCloseDeleteCmd,
	kQuitCmd
};

static const JCharacter* kMessageMenuTitleStr = "Message";
static const JCharacter* kMessageMenuStr =
	"Show full headers %b  %i \"ShowFullHead::Arrow\""
	"| Decrypt %i \"Decrypt::Arrow\""
	"| Open selected attachments... %k Meta-Shift-O %i \"OpenAttach::Arrow\""
	"| Save selected attachments... %k Meta-Shift-S %i \"SaveAttach::Arrow\""
	"| View MIME source %i \"ViewSource::Arrow\""
	"%l|Reply %k Meta-R %i \"Reply::Arrow\""
	"| Reply to sender %i \"ReplySender::Arrow\""
	"|Reply to all %i \"ReplyAll::Arrow\""
	"| Forward %i \"Forward::Arrow\""
	"| Redirect %i \"Redirect::Arrow\"";

enum
{
	kShowHeaderCmd = 1,
	kDecryptCmd,
	kOpenAttachCmd,
	kSaveAttachCmd,
	kViewSourceCmd,
	kReplyCmd,
	kReplySenderCmd,
	kReplyAllCmd,
	kForwardCmd,
	kRedirectCmd
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

const JCoordinate kAttachPartHeight		= 50;
const JCoordinate kAttachPartMinHeight	= 30;
const JCoordinate kAttachTablePartIndex	= 2;

const JFileVersion kCurrentPrefsVersion	= 3;

static const JCharacter* kPasswordPrompt		= "Password:";

/******************************************************************************
 Constructor

 ******************************************************************************/

GMessageViewDir::GMessageViewDir
	(
	GMessageTableDir* supervisor,
	const JString& mailfile,
	GMessageHeader* header,
	const JBoolean source
	)
	:
	GMManagedDirector(supervisor),
	itsSourceOnly(source)
{
	itsPasswdDialog			= NULL;
	itsAttachTable				= NULL;
	itsPrefsIncludeAttachTable	= kJFalse;

	BuildWindow(mailfile);
	itsDir = supervisor;
	itsShowFullHeaders = kJFalse;
	GGetPrefsMgr()->GetViewWindowPrefs(this);
	ShowHeader(header);

	GGetDirMgr()->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMessageViewDir::~GMessageViewDir()
{
	delete itsMenuIcon;
	GGetDirMgr()->DirectorDeleted(this);
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
GMessageViewDir::BuildWindow
	(
	const JString& mailfile
	)
{
	JSize w = 500;
	JSize h = 300;
	JXWindow* window =
		new JXWindow(this, w,h, mailfile);
    assert( window != NULL );
    SetWindow(window);
	window->SetWMClass(GMGetWMClassInstance(), GMGetViewWindowClass());
    GGetPrefsMgr()->GetViewWindowSize(window);
	w = window->GetFrameWidth();
	h = window->GetFrameHeight();

    window->SetMinSize(w, 150);
    window->ShouldFocusWhenShow(kJTrue);

	JXMenuBar* menuBar =
		new JXMenuBar(window,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0, 0, w - kJXDefaultMenuBarHeight, kJXDefaultMenuBarHeight);
	assert(menuBar != NULL);

	JXEngravedRect* eRect =
		new JXEngravedRect(window,
			JXWidget::kFixedRight, JXWidget::kFixedTop,
			w - kJXDefaultMenuBarHeight, 0, kJXDefaultMenuBarHeight, kJXDefaultMenuBarHeight);
	assert(eRect != NULL);

	GMMessageDragSource* mds =
		new GMMessageDragSource(this, window,
			JXWidget::kFixedRight, JXWidget::kFixedTop,
			w - kJXDefaultMenuBarHeight + kJXDefaultBorderWidth,
			0 + kJXDefaultBorderWidth,
			kJXDefaultMenuBarHeight - 2 * kJXDefaultBorderWidth + 1,
			kJXDefaultMenuBarHeight - 2 * kJXDefaultBorderWidth + 1);
	assert(mds != NULL);

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	JPtrArray<JString> nodes(JPtrArrayT::kDeleteAll);
	GGetMailboxTreeDir()->GetTopLevelNodes(&nodes);

	itsTransferMenu = new JXFSDirMenu(nodes, itsFileMenu, kTransferToCmd, menuBar);
	assert(itsTransferMenu != NULL);
	ListenTo(itsTransferMenu);

	JDirInfo* info;
	if (itsTransferMenu->GetDirInfo(&info))
		{
		info->SetContentFilter(GMGetMailRegexStr());
		}

	itsCopyMenu = new JXFSDirMenu(nodes, itsFileMenu, kCopyToCmd, menuBar);
	assert(itsCopyMenu != NULL);
	ListenTo(itsCopyMenu);

	if (itsCopyMenu->GetDirInfo(&info))
		{
		info->SetContentFilter(GMGetMailRegexStr());
		}

	ListenTo(GGetMailboxTreeDir());

	itsMessageMenu = menuBar->AppendTextMenu(kMessageMenuTitleStr);
	itsMessageMenu->SetMenuItems(kMessageMenuStr);
	itsMessageMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsMessageMenu);

	itsToolBar =
		new JXToolBar(GGetPrefsMgr(), kGViewToolBarID,
			menuBar, w, 150, window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, kJXDefaultMenuBarHeight, w, h - kJXDefaultMenuBarHeight);
	assert(itsToolBar != NULL);

	const JSize newHeight	= itsToolBar->GetWidgetEnclosure()->GetBoundsHeight();

	const JFontManager* fm	= window->GetFontManager();
	JSize lineHeight =
		fm->GetLineHeight(GGetPrefsMgr()->GetDefaultMonoFont(),
						  GGetPrefsMgr()->GetDefaultFontSize(),
						  JFontStyle());


	const JCoordinate headerheight  = (lineHeight * 4) + (lineHeight/2); //58;

	JArray<JCoordinate> sizes;
	JArray<JCoordinate> minSizes;

	sizes.AppendElement(headerheight);
	minSizes.AppendElement(20);
	sizes.AppendElement(w - headerheight);
	minSizes.AppendElement(50);
	JIndex elasticIndex = 2;

	itsPart =
		new JXVertPartition(sizes, elasticIndex,
			minSizes, itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0,
			w, newHeight);
	assert(itsPart != NULL);

	itsSBSet =
		new JXScrollbarSet(itsPart->GetCompartment(2),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,
			100,100);
	assert(itsSBSet != NULL);
	itsSBSet->FitToEnclosure(kJTrue, kJTrue);

	itsView =
		new GMessageView(menuBar, itsSBSet, itsSBSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,10,10);
	assert (itsView != NULL);
	itsView->FitToEnclosure(kJTrue, kJTrue);
	itsView->SetPTPrinter(GMGetAltPTPrinter());

	window->InstallShortcuts(itsView, "#_");
	ListenTo(itsView);

	JXScrollbarSet* sbs =
		new JXScrollbarSet(itsPart->GetCompartment(1),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,
			100,50);
	assert(sbs != NULL);
	sbs->FitToEnclosure(kJTrue, kJTrue);

	itsHeader =
		new GMessageView(sbs, sbs->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,10,10);
	assert (itsHeader != NULL);
	itsHeader->FitToEnclosure(kJTrue, kJTrue);

	itsHeader->ShareEditMenu(itsView);
	itsHeader->ShareSearchMenu(itsView);

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
	itsFileMenu->SetItemImage(kPrintCmd, fileprint);
	itsFileMenu->SetItemImage(kShowNextCmd, mini_right);
	itsFileMenu->SetItemImage(kDeleteShowNextCmd, mini_del_right);
	itsFileMenu->SetItemImage(kShowPrevCmd, mini_left);

	itsMessageMenu->SetItemImage(kDecryptCmd, unlock_xpm);
	itsMessageMenu->SetItemImage(kSaveAttachCmd, paperclip);
	itsMessageMenu->SetItemImage(kReplyCmd, reply_xpm);
	itsMessageMenu->SetItemImage(kReplySenderCmd, reply_sender_xpm);
	itsMessageMenu->SetItemImage(kReplyAllCmd, reply_all_xpm);
	itsMessageMenu->SetItemImage(kForwardCmd, forward_xpm);
	itsMessageMenu->SetItemImage(kRedirectCmd, redirect_xpm);

	itsHelpMenu->SetItemImage(kTOCCmd, jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, JXPM(jx_help_specific));

	itsToolBar->LoadPrefs();

	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kNewCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenCmd);
		itsToolBar->NewGroup();

		itsToolBar->AppendButton(itsFileMenu, kSaveCmd);
		itsToolBar->NewGroup();

		itsToolBar->AppendButton(itsFileMenu, kPrintCmd);
		itsToolBar->NewGroup();

		itsToolBar->AppendButton(itsFileMenu, kShowNextCmd);
		itsToolBar->AppendButton(itsFileMenu, kShowPrevCmd);
		itsToolBar->AppendButton(itsFileMenu, kDeleteShowNextCmd);
		itsToolBar->NewGroup();

		itsToolBar->AppendButton(itsMessageMenu, kReplyCmd);
		itsToolBar->AppendButton(itsMessageMenu, kReplySenderCmd);
		itsToolBar->AppendButton(itsMessageMenu, kReplyAllCmd);
		itsToolBar->AppendButton(itsMessageMenu, kForwardCmd);
		itsToolBar->AppendButton(itsMessageMenu, kRedirectCmd);
		}

	itsMenuIcon = new JXImage(window->GetDisplay(), window->GetColormap(), JXPM(jx_plain_file_small));
	assert(itsMenuIcon != NULL);
	itsMenuIcon->ConvertToRemoteStorage();
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GMessageViewDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsMessageMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleMessageMenu(selection->GetIndex());
		}
	else if (sender == itsMessageMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMessageMenu();
		}
	else if (sender == itsPasswdDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			JString passwd = itsPasswdDialog->GetPassword();
			JString buffer;
			JBoolean ok;
			if (GGetPrefsMgr()->GetEncryptionType() == GPrefsMgr::kPGP2_6)
				{
				ok = GDecryptPGP2_6_2(itsView->GetText(), passwd, &buffer);
				}
			else
				{
				ok = GDecryptGPG1_0(itsView->GetText(), passwd, &buffer);
				}
			if (ok && !buffer.IsEmpty())
				{
				itsView->SelectAll();
				PasteClean(buffer);
				itsView->ParseURLs();
				}
			}
		itsPasswdDialog = NULL;
		}
	else if (sender == GMGetAltPTPrinter() && message.Is(GMPTPrinter::kPrintStarting))
		{
		JBoolean headers = GMGetAltPTPrinter()->WillPrintHeader();
		if (headers)
			{
			AdjustView(kJTrue);
			}
		}
	else if (sender == itsTransferMenu && message.Is(JXFSDirMenu::kFileSelected))
		{
		const JXFSDirMenu::FileSelected* info =
			dynamic_cast(const JXFSDirMenu::FileSelected*, &message);
		assert(info != NULL);
		HandleMessageTransfer(info->GetFileName(), kJTrue);
		}
	else if (sender == itsCopyMenu && message.Is(JXFSDirMenu::kFileSelected))
		{
		const JXFSDirMenu::FileSelected* info =
			dynamic_cast(const JXFSDirMenu::FileSelected*, &message);
		assert(info != NULL);
		HandleMessageTransfer(info->GetFileName(), kJFalse);
		}
	else if (sender == GGetMailboxTreeDir() && message.Is(GMailboxTreeDir::kMailTreeChanged))
		{
		JPtrArray<JString> nodes(JPtrArrayT::kDeleteAll);
		GGetMailboxTreeDir()->GetTopLevelNodes(&nodes);
		itsTransferMenu->SetFileList(nodes);
		itsCopyMenu->SetFileList(nodes);
		}
	else if (sender == itsView && message.Is(GMessageView::kUnderBarShortcutCaught))
		{
		ShowPrev();
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
GMessageViewDir::HandleFileMenu
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
		if (JXGetChooseSaveFile()->SaveFile("Name of new mailbox:", "", "",  &mbox))
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
		JString name;
		if (GMGetAltChooseSaveFile()->SaveFile("Choose file name:", "", "",  &name))
			{
			JBoolean headers = GMGetAltChooseSaveFile()->IsSavingHeaders();
			if (!headers)
				{
				itsView->WritePlainText(name, JTextEditor::kUNIXText);
				}
			else
				{
				if (itsShowFullHeaders)
					{
					itsView->WritePlainText(name, JTextEditor::kUNIXText);
					}
				else
					{
					AdjustView(kJTrue);
					itsView->WritePlainText(name, JTextEditor::kUNIXText);
					AdjustView(kJFalse);
					}
				}
			}
		}
	else if (index == kAppendCmd)
		{
		AppendMessagesToFile(kJFalse);
		}
	else if (index == kPageSetupCmd)
		{
		GMGetAltPTPrinter()->BeginUserPageSetup();
		}
	else if (index == kPrintCmd)
		{
		itsView->PrintPT();
		ListenTo(GMGetAltPTPrinter());
		}
	else if (index == kCloseCmd)
		{
		Close();
		}
	else if (index == kCloseDeleteCmd)
		{
		GMMarkHeaderDeletedTask* task	=
			new GMMarkHeaderDeletedTask(itsDir->GetData(), itsMessageHeader);
		assert(task != NULL);
		JXGetApplication()->InstallUrgentTask(task);		
		Close();
		}
	else if (index == kDeleteShowNextCmd)
		{
		GMMarkHeaderDeletedTask* task	=
			new GMMarkHeaderDeletedTask(itsDir->GetData(), itsMessageHeader);
		assert(task != NULL);
		JXGetApplication()->InstallUrgentTask(task);		

		GMessageHeader* header = itsMessageHeader;
		if (!itsSourceOnly)
			{
			ShowNext();
			}
		if (header == itsMessageHeader)
			{
			Close();
			}
		}
	else if (index == kShowNextCmd)
		{
		ShowNext();
		}
	else if (index == kShowPrevCmd)
		{
		ShowPrev();
		}
	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdateFileMenu


 ******************************************************************************/

void
GMessageViewDir::UpdateFileMenu()
{
	GMMailboxData* data = itsDir->GetData();
	JIndex findex;
	data->Includes(itsMessageHeader, &findex);

	if (findex > 1 && !itsSourceOnly)
		{
		itsFileMenu->EnableItem(kShowPrevCmd);
		}
	else
		{
		itsFileMenu->DisableItem(kShowPrevCmd);
		}
	if (findex < data->GetHeaderCount() && !itsSourceOnly)
		{
		itsFileMenu->EnableItem(kShowNextCmd);
		}
	else
		{
		itsFileMenu->DisableItem(kShowNextCmd);
		}
}

/******************************************************************************
 UpdateMessageMenu


 ******************************************************************************/

void
GMessageViewDir::UpdateMessageMenu()
{
	if (itsShowFullHeaders)
		{
		itsMessageMenu->CheckItem(kShowHeaderCmd);
		}
	if (itsAttachTable != NULL && itsAttachTable->GetTableSelection().HasSelection())
		{
		itsMessageMenu->EnableItem(kSaveAttachCmd);
		itsMessageMenu->EnableItem(kOpenAttachCmd);
		}
	else
		{
		itsMessageMenu->DisableItem(kSaveAttachCmd);
		itsMessageMenu->DisableItem(kOpenAttachCmd);
		}
	if (itsMessageHeader->IsMIME() && !itsSourceOnly)
		{
		itsMessageMenu->EnableItem(kViewSourceCmd);
		}
	else
		{
		itsMessageMenu->DisableItem(kViewSourceCmd);
		}
}

/******************************************************************************
 HandleMessageMenu


 ******************************************************************************/

void
GMessageViewDir::HandleMessageMenu
	(
	const JIndex index
	)
{
	if (index == kShowHeaderCmd)
		{
		AdjustView(!itsShowFullHeaders);
		}
	else if (index == kDecryptCmd)
		{
		assert(itsPasswdDialog == NULL);
		itsPasswdDialog = new JXGetPasswordDialog(this, kPasswordPrompt);
		assert(itsPasswdDialog != NULL);
		ListenTo(itsPasswdDialog);
		itsPasswdDialog->BeginDialog();
		}
	else if (index == kOpenAttachCmd)
		{
		itsAttachTable->OpenSelectedAttachments();
		}
	else if (index == kSaveAttachCmd)
		{
		itsAttachTable->SaveSelectedAttachments();
		}
	else if (index == kViewSourceCmd)
		{
		GMessageViewDir* dir =
			new GMessageViewDir(itsDir, itsDir->GetData()->GetMailFile(), itsMessageHeader, kJTrue);
		assert(dir != NULL);
		dir->Activate();
		}
	else if (index == kReplyCmd)
		{
		itsDir->GetTable().Reply(itsMessageHeader);
		}
	else if (index == kReplySenderCmd)
		{
		itsDir->GetTable().ReplySender(itsMessageHeader);
		}
	else if (index == kReplyAllCmd)
		{
		itsDir->GetTable().ReplyAll(itsMessageHeader);
		}
	else if (index == kForwardCmd)
		{
		itsDir->GetTable().Forward(itsMessageHeader);
		}
	else if (index == kRedirectCmd)
		{
		itsDir->GetTable().Redirect(itsMessageHeader);
		}
}

/******************************************************************************
 HandlePrefsMenu


 ******************************************************************************/

void
GMessageViewDir::HandlePrefsMenu
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
		GGetPrefsMgr()->SetViewWindowPrefs(this);
		}


}

/******************************************************************************
 HandleHelpMenu


 ******************************************************************************/

void
GMessageViewDir::HandleHelpMenu
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
		(JXGetHelpManager())->ShowSection(kGViewHelpName);
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
 ShowNext


 ******************************************************************************/

void
GMessageViewDir::ShowNext()
{
		GMMailboxData* data = itsDir->GetData();
		JIndex findex;
		data->Includes(itsMessageHeader, &findex);
		if (findex < data->GetHeaderCount())
			{
			JBoolean found = kJFalse;
			JIndex i = findex + 1;
			GMessageHeader* header;
			while (!found && i <= data->GetHeaderCount())
				{
				header = data->GetHeader(i);
				if (header->GetMessageStatus() != GMessageHeader::kDelete)
					{
					found = kJTrue;
					}
				i++;
				}
			if (found)
				{
				itsDir->GetTable().UnSelectHeader(itsMessageHeader);
				ShowHeader(header);
				itsDir->GetTable().SelectHeader(header);
				itsDir->GetData()->ChangeMessageStatus(header, GMessageHeader::kRead);
				GGetDirMgr()->DirectorChanged(this);
				}
			}
}

/******************************************************************************
 ShowPrev


 ******************************************************************************/

void
GMessageViewDir::ShowPrev()
{
		GMMailboxData* data = itsDir->GetData();
		JIndex findex;
		data->Includes(itsMessageHeader, &findex);
		if (findex > 1)
			{
			JBoolean found = kJFalse;
			JIndex i = findex - 1;
			GMessageHeader* header;
			while (!found && i > 0)
				{
				header = data->GetHeader(i);
				if (header->GetMessageStatus() != GMessageHeader::kDelete)
					{
					found = kJTrue;
					}
				i--;
				}
			if (found)
				{
				itsDir->GetTable().UnSelectHeader(itsMessageHeader);
				ShowHeader(header);
				itsDir->GetTable().SelectHeader(header);
				itsDir->GetData()->ChangeMessageStatus(header, GMessageHeader::kRead);
				GGetDirMgr()->DirectorChanged(this);
				}
			}
}

/******************************************************************************
 ShowHeader


 ******************************************************************************/

void
GMessageViewDir::ShowHeader
	(
	GMessageHeader* header
	)
{
	itsMessageHeader = header;

	mode_t perms;
	JError err = JGetPermissions(itsDir->GetData()->GetMailFile(), &perms);
	if (!err.OK())
		{
		perms = 0600;
		}

	ifstream is(itsDir->GetData()->GetMailFile());
	if (!is.good())
		{
		JGetUserNotification()->ReportError("There was an error opening this mailbox.");
		return;
		}

	is.seekg(header->GetHeaderStart());
	itsFullHeader = JRead(is, header->GetHeaderEnd() - header->GetHeaderStart());

	itsFrom = header->GetFrom();
	itsTo = header->GetTo();
	itsDate = header->GetDate();
	itsSubject = header->GetSubject();
	itsReplyTo = header->GetReplyTo();
	itsCC = header->GetCC();
	JString head = "From: " + itsFrom;
	if (!itsSubject.IsEmpty())
		{
		if (head.GetLastCharacter() != '\n')
			{
			head += "\n";
			}
		head += "Subject: " + itsSubject;
		}
	if (head.GetLastCharacter() != '\n')
		{
		head += "\n";
		}
	head += "Date: " + itsDate;
	if (!itsTo.IsEmpty())
		{
		if (head.GetLastCharacter() != '\n')
			{
			head += "\n";
			}
		head += "To: " + itsTo;
		}
	if (!itsReplyTo.IsEmpty())
		{
		if (head.GetLastCharacter() != '\n')
			{
			head += "\n";
			}
		head += "Reply-To: " + itsReplyTo;
		}
	if (!itsCC.IsEmpty())
		{
		if (head.GetLastCharacter() != '\n')
			{
			head += "\n";
			}
		head += "Cc: " + itsCC;
		}
	head.TrimWhitespace();
	itsHeader->SetText(head);
	itsView->SetText("");

	GMMIMEParser* parser	= itsMessageHeader->GetMIMEParser();
	JString dir;
	JBoolean ok	= GMGetApplication()->GetFileDirectory(&dir);
	if (itsSourceOnly)
		{
		ok	= kJFalse;
		}
	if ( ok &&
		(parser == NULL) &&
		(itsMessageHeader->IsMIME()))
		{
		JString data;
		is.seekg(header->GetHeaderStart());
		data.Read(is, header->GetMessageEnd() - header->GetHeaderStart());
		parser	= new GMMIMEParser(&data, dir);
		assert(parser != NULL);
		itsMessageHeader->SetMIMEParser(parser);
		}

	if (ok && itsMessageHeader->IsMIME())
		{
		itsView->RemoveLinks();
		JString data;
		const JSize count = parser->GetTextSegmentCount();
		for (JIndex i = 1; i <= count; i++)
			{
			GMMIMEParser::TextFormat format;
			JString charset;
			parser->GetTextSegment(i, &data, &format, &charset);
			data.AppendCharacter('\n');
			if (format == GMMIMEParser::kHTML)
				{
				std::istrstream is(data.GetCString(), data.GetLength());
				itsView->PasteHTML(is);
				}
			else
				{
				PasteClean(data);
				}
			}
		itsView->ParseURLs(kJFalse);
		}
	else
		{
		ok	= kJFalse;
		}

	if ( ok &&
		(parser != NULL) &&
		(parser->GetAttachmentCount() > 0))
		{
		if (itsPart->GetCompartmentCount() == 2)
			{
			JSize height	= kAttachPartHeight;
			if (itsPrefsIncludeAttachTable)
				{
				height	= itsAttachTableHeight;
				}
			JXContainer* container;
			if (itsPart->InsertCompartment(kAttachTablePartIndex, height, kAttachPartMinHeight, &container))
				{
				JXScrollbarSet* sbset =
					new JXScrollbarSet(container,
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0,
						container->GetBoundsWidth(),container->GetBoundsHeight());
				assert(sbset != NULL);
//				itsSBSet->FitToEnclosure(kJTrue, kJTrue);

				itsAttachTable =
					new GMAttachmentTable(kJFalse, sbset, sbset->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0,
						10,10);
				assert(itsAttachTable != NULL);
				itsAttachTable->FitToEnclosure(kJTrue, kJTrue);
				itsAttachTable->SetEditMenuHandler(itsView);

				}
			else
				{
				ok	= kJFalse;
				}
			}
		if (ok && (itsAttachTable != NULL))
			{
			itsAttachTable->SetParser(parser);
			}
		}
	else if (itsPart->GetCompartmentCount() > 2)
		{
		itsPart->DeleteCompartment(itsPart->GetCompartment(kAttachTablePartIndex));
		itsAttachTable	= NULL;
		}

	if (!ok)
		{
		is.seekg(header->GetHeaderEnd());
		JString body = JRead(is, header->GetMessageEnd() - header->GetHeaderEnd());
		PasteClean(body);
		itsView->ParseURLs();
		}
	AdjustView(itsShowFullHeaders, kJTrue);

	is.close();
	JString title = itsFrom;
	JIndex findindex;
	if (title.LocateSubstring("@", &findindex))
		{
		title.RemoveSubstring(findindex, title.GetLength());
		}
	JString sub = ":" + itsSubject;
	if (sub.GetLength() > 11)
		{
		sub.RemoveSubstring(10, sub.GetLength());
		}
	title.Append(sub);
	GetWindow()->SetTitle(title);
	itsView->ScrollTo(0,0);
}

/******************************************************************************
 FixHeaderForReply


 ******************************************************************************/

void
GMessageViewDir::FixHeaderForReply
	(
	JString* sub
	)
{
	JRegex regex;
	JError err = regex.SetPattern("^[rR][eE][.:]");
	assert(err.OK());
	JBoolean matched;
	JArray<JIndexRange>* subList = new JArray<JIndexRange>;
	assert(subList != NULL);
	matched = regex.Match(*sub, subList);
	if (!matched)
		{
		sub->Prepend("Re: ");
		}
}

/******************************************************************************
 SaveState


 ******************************************************************************/

void
GMessageViewDir::SaveState
	(
	ostream& os
	)
{
	os << itsMessageHeader->GetHeader() << " ";
	GetWindow()->WriteGeometry(os);

	JXScrollbar* hsb = itsSBSet->GetHScrollbar();
	JXScrollbar* vsb = itsSBSet->GetVScrollbar();
	os << hsb->GetValue() << " ";
	os << vsb->GetValue() << " ";

	const JArray<JCoordinate>& sizes	= itsPart->GetCompartmentSizes();
	const JSize count					= itsPart->GetCompartmentCount();
	os << count << ' ';
	for (JSize i = 1; i <= count; i++)
		{
		os << sizes.GetElement(i) << " ";
		}
}

/******************************************************************************
 ReadState

 ******************************************************************************/

void
GMessageViewDir::ReadState
	(
	istream& is,
	const JFileVersion& version
	)
{
	GetWindow()->ReadGeometry(is);
	JCoordinate hval, vval;
	is >> hval;
	is >> vval;
	JXScrollbar* hsb = itsSBSet->GetHScrollbar();
	JXScrollbar* vsb = itsSBSet->GetVScrollbar();
	hsb->SetValue(hval);
	vsb->SetValue(vval);



	JArray<JCoordinate> sizes;
	JSize count	= 2;
	if (version > 1)
		{
		is >> count;
		}
	for (JIndex i = 1; i <= count; i++)
		{
		JCoordinate value;
		is >> value;
		sizes.AppendElement(value);
		}
	if (sizes.GetElementCount() == itsPart->GetCompartmentCount())
		{
		itsPart->JPartition::SetCompartmentSizes(sizes);
		}
}

/******************************************************************************
 AppendMessagesToFile (private)

 ******************************************************************************/

void
GMessageViewDir::AppendMessagesToFile
	(
	const JBoolean headers
	)
{
	JString filename;
	if (GMGetAltChooseSaveFile()->ChooseFile("Choose file:", "", &filename))
		{
		JString path, name;
		JSplitPathAndName(filename, &path, &name);
		JString tempname;
		JError err = JCreateTempFile(path, NULL, &tempname);
		if (!err.OK())
			{
			err = JCreateTempFile(&tempname);
			if (!err.OK())
				{
				err.ReportIfError();
				return;
				}
			}

		mode_t perms;
		err = JGetPermissions(filename, &perms);
		if (!err.OK())
			{
			perms = 0600;
			}
		ifstream is(filename);
		ofstream os(tempname);
		JSize size;
		err = JGetFileLength(filename, &size);
		if (!err.OK())
			{
			JGetUserNotification()->ReportError("There was an unknown problem saving the file.");
			os.close();
			JRemoveFile(tempname);
			return;
			}
		JString temp;
		temp.Read(is, size);
		if ((size != 0) && !temp.EndsWith("\n\n"))
			{
			temp.Append("\n\n");
			}
		temp.Print(os);
		JBoolean saveHeaders = GMGetAltChooseSaveFile()->IsSavingHeaders();
		if (saveHeaders)
			{
			if (!itsShowFullHeaders)
				{
				itsFullHeader.Print(os);
				}
			itsView->WritePlainText(os, JTextEditor::kUNIXText);
			}
		else
			{
			if (itsShowFullHeaders)
				{
				AdjustView(kJFalse);
				itsView->WritePlainText(os, JTextEditor::kUNIXText);
				AdjustView(kJTrue);
				}
			else
				{
				itsView->WritePlainText(os, JTextEditor::kUNIXText);
				}
			}

		is.close();
		os.close();

		GMReplaceFileWithTemp(tempname, filename);
		}
}

/******************************************************************************
 WindowPrefs (public)

 ******************************************************************************/

void
GMessageViewDir::ReadWindowPrefs
	(
	istream& is
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
	if (version == 2)
		{
		if (count > 1)
			{
			// these are junk values left over from when the attachments were
			// on the right
			JCoordinate value;
			is >> value;
			is >> value;
			}
		else
			{
			JCoordinate value;
			is >> value;
			}
		itsPrefsIncludeAttachTable	= kJFalse;
		itsPart->ReadGeometry(is);
		}
	else if (version > 2)
		{
		JArray<JCoordinate> sizes;
		for (JIndex i = 1; i <= count; i++)
			{
			JCoordinate value;
			is >> value;
			sizes.AppendElement(value);
			}
		if (count == 3)
			{
			itsAttachTableHeight		= sizes.GetElement(kAttachTablePartIndex);
			sizes.RemoveElement(kAttachTablePartIndex);
			itsPrefsIncludeAttachTable	= kJTrue;
			}
		itsPart->JPartition::SetCompartmentSizes(sizes);
		}
}

void
GMessageViewDir::WriteWindowPrefs
	(
	ostream& os
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
	if (count == 3)
		{
		itsPrefsIncludeAttachTable	= kJTrue;
		itsAttachTableHeight		= sizes.GetElement(kAttachTablePartIndex);
		}
}

/******************************************************************************
 HandleMessageTransfer (private)

 ******************************************************************************/

void
GMessageViewDir::HandleMessageTransfer
	(
	const JCharacter*	dest,
	const JBoolean		move
	)
{
	GMessageHeaderList* headers = new GMessageHeaderList();
	assert(headers != NULL);
	headers->Append(itsMessageHeader);
	itsDir->GetData()->HandleMessageTransfer(dest, move, headers);
	delete headers;
	Close();
}

/******************************************************************************
 AdjustView (private)

 ******************************************************************************/

void
GMessageViewDir::AdjustView
	(
	const JBoolean showFull,
	const JBoolean firstTime
	)
{
	if (itsShowFullHeaders == showFull && !firstTime)
		{
		return;
		}
	itsShowFullHeaders	= showFull;
	if (itsShowFullHeaders)
		{
		itsView->SetCaretLocation(1);
		PasteClean(itsFullHeader);
		}
	else if (!firstTime)
		{
		itsView->SetSelection(1, itsFullHeader.GetLength());
		itsView->DeleteSelection();
		}
}

/******************************************************************************
 PasteClean

 ******************************************************************************/

void
GMessageViewDir::PasteClean
	(
	const JString& buffer
	)
{
	JRunArray<JTextEditor::Font> runs;
	JFontID id;
	JSize size;
	JFontStyle style;
	itsView->GetDefaultFont(&id, &size, &style);
	JTextEditor::Font font(id, size, style);
	runs.AppendElements(font, buffer.GetLength());
	itsView->Paste(buffer, &runs);
}
