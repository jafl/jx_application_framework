/******************************************************************************
 GMessageTableDir.cc

	BASE CLASS = GMManagedDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GMessageTableDir.h"
#include "GMessageEditDir.h"
#include "GMessageTable.h"
#include "GMessageHeaderList.h"
#include "GMessageHeader.h"
#include "GMessageColHeader.h"
#include "GMessageDirUpdateTask.h"
#include "GAddressBookTreeDir.h"
#include "GMessageViewDir.h"
#include "SMTPMessage.h"
#include <GMGlobals.h>
#include <GMApp.h>
#include <GPrefsMgr.h>
#include <GInboxMgr.h>
#include <GHelpText.h>
#include <GXBlockingPG.h>
#include <gMailUtils.h>
#include "GMDummyText.h"
#include "GMChooseSaveFile.h"
#include "GMPTPrinter.h"
#include "GMDirectorManager.h"
#include "GMDirectorMenu.h"
#include "GMAccountManager.h"
#include "GMPOPRetrieverMenu.h"
#include "GMFilterManager.h"
#include "JXFSBindingManager.h"
#include "GMailboxTreeDir.h"
#include "GMMessageFindDialog.h"
#include "GMFindTableDir.h"
#include "GMMIMEParser.h"
#include "GMMailboxData.h"

#include "nomail.xpm"
#include "newmail.xpm"
#include "newmail_small.xpm"
#include "newmail_small_plus.xpm"
#include "newmail_small_minus.xpm"
#include "havemail.xpm"

#include "filenew.xpm"
#include "filefloppy.xpm"
#include "fileprint.xpm"
#include "fileopen.xpm"
#include "envelopes.xpm"
#include "check_mail.xpm"

#include "address_entry.xpm"
#include "mailbox.xpm"

#include "manual.xpm"
#include "jx_help_specific.xpm"

#include <JXToolBar.h>

#include <JXApplication.h>
#include <JXChooseSaveFile.h>
#include <JXFSDirMenu.h>
#include <JXDisplay.h>
#include <JXHelpManager.h>
#include <JXImage.h>
#include <JXMenuBar.h>
#include <JXProgressIndicator.h>
#include <JXScrollbar.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXTextMenu.h>
#include <JXWebBrowser.h>
#include <JXWindow.h>

#include <JFileArray.h>
#include <JLatentPG.h>
#include <JTableSelection.h>
#include <JDirInfo.h>
#include <JUserNotification.h>

#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <strstream>
#include <stdlib.h>
#include <jAssert.h>

//extern JUserNotification*	gUserNotification;
//extern JXApplication*		gApplication;
//extern JChooseSaveFile*	gChooseSaveFile;

extern const JIndex		kPriorityIndex;
extern const JIndex		kIconIndex;
extern const JIndex		kAttachIndex;
extern const JIndex		kFromIndex;
extern const JIndex		kSubjectIndex;
extern const JIndex		kDateIndex;

const JFileVersion kCurrentWindowPrefsVersion		= 0;

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"   New message %k Meta-N %i \"NewMessage::Arrow\""
	"  |New mailbox %k Meta-Shift-N %i \"NewMailbox::Arrow\""
	"  |Open mailbox... %k Meta-O %i \"OpenMailbox::Arrow\""
	"%l|Find messages... %k Meta-F %i \"FindMessages::Arrow\""
	"%l|Save messages... %k Meta-S %i \"SaveMessages::Arrow\""
	"  |Append messages... %i \"AppendMessages::Arrow\""
	"  |Transfer to"
	"  |Copy to"
	"%l|Page setup... %i \"PageSetup::Arrow\""
	"  |Print selected messages... %k Meta-P %i \"PrintMessages::Arrow\""
	"%l| Check all accounts %k Meta-M  %i \"CheckMail::Arrow\""
	"  |Check account %k Meta-M  %i \"CheckAccount::Arrow\""
	"%l| Save changes %i \"SaveChanges::Arrow\""
	"  |Flush deleted messages %k Meta-Shift-F %i \"FlushDeleted::Arrow\""
	"%l|Close %k Meta-W %i \"Close::Arrow\""
	"  |Quit %k Meta-Q %i \"Quit::Arrow\"";

enum
{
	kNewCmd = 1,
	kNewMBox,
	kOpenCmd,
	kFindMessageCmd,
	kSaveMsgCmd,
	kAppendMsgCmd,
	kTransferToCmd,
	kCopyToCmd,
	kPageSetupCmd,
	kPrintMsgCmd,
	kCheckMailCmd,
	kCheckAccountCmd,
	kSaveCmd,
	kFlushCmd,
	kCloseCmd,
	kQuitCmd
};

static const JCharacter* kInboxMenuTitleStr = "Inbox";
static const JCharacter* kInboxMenuStr =
	"Add as inbox %k Meta-plus %i \"AddInbox::Arrow\""
	"| Remove as inbox %k Meta-minus %i \"RemoveInbox::Arrow\""
	"|Remove all inboxes  %i \"RemoveAllInboxes::Arrow\""
	"| Open all inboxes %i \"OpenAllInboxes::Arrow\" %l";

enum
{
	kAddInboxCmd = 1,
	kRemoveInboxCmd,
	kRemoveAllInboxesCmd,
	kOpenAllInboxesCmd
};

static const JCharacter* kWindowsMenuTitleStr = "Windows";

static const JCharacter* kWindowMenuTitleStr = "Windows";
static const JCharacter* kWindowMenuStr =
	"Mailboxes %k Meta-Shift-M  %i \"MailboxWindow::Arrow\""
	"| Address book %k Meta-Shift-A  %i \"AddBookWindow::Arrow\""
	"| SMTP debug window %i \"SMTPDebugWindow::Arrow\"";

enum
{
	kMailboxCmd = 1,
	kAddressBookCmd,
	kSMTPDebugCmd
};

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

const JCharacter kStateDataEndDelimiter	= '\032';
const JCharacter kViewStateDataEndDelimiter     = '\031';

const JFileVersion kCurrentStateVersion	= 1;

const JCharacter* GMessageTableDir::kMessageClosed	= "kMessageClosed::GMessageTableDir";


/******************************************************************************
 Constructor

 ******************************************************************************/

JBoolean
GMessageTableDir::Create
	(
	JXDirector* supervisor,
	const JString& mailfile,
	GMessageTableDir** dir,
	const JBoolean iconify
	)
{
	*dir = new GMessageTableDir(supervisor, mailfile);
	assert(*dir != NULL);
	if (iconify)
		{
		(*dir)->GetWindow()->Iconify();
		}
	JBoolean success = CreateX(supervisor, mailfile, dir);
	if (success)
		{
		(*dir)->ListenTo((*dir)->GetWindow());
		}
	if (iconify && success)
		{
		JXImage* icon	= new JXImage(*((*dir)->itsNewMailIcon));
		assert(icon != NULL);
		(*dir)->GetWindow()->SetIcon(icon);
		(*dir)->itsData->ShouldHaveNewMail(kJTrue);
		(*dir)->AdjustWindowTitle();
		}
	return success;
}

JBoolean
GMessageTableDir::Create
	(
	JXDirector* supervisor,
	JFileArray& fileArray,
	GMessageTableDir** dir,
	const JFileVersion& version
	)
{
	JFAID id = 1;
	std::string data;
	fileArray.GetElement(id, &data);
	std::istringstream is(data);

	JString mailfile;
	is >> mailfile;

	JString lockfile = JString(mailfile) + ".lock";
	if (JFileExists(lockfile))
		{
		JString report	= "The mailbox \"" + JString(mailfile);
		report			+= "\" is locked. Do you want to forcibly remove the lock on mailbox";
		JBoolean yes	= JGetUserNotification()->AskUserYes(report);
		if (yes)
			{
			GUnlockFile(mailfile);
			}
		else
			{
			return kJFalse;
			}
		}

	*dir = new GMessageTableDir(supervisor, mailfile);
	assert(*dir != NULL);
	(*dir)->GetWindow()->ReadGeometry(is);
	JBoolean success = CreateX(supervisor, mailfile, dir);
	if (success)
		{
		(*dir)->ReadState(fileArray, version);
		(*dir)->ListenTo((*dir)->GetWindow());
		}
	return success;
}

// private

JBoolean
GMessageTableDir::CreateX
	(
	JXDirector* supervisor,
	const JString& mailfile,
	GMessageTableDir** dir
	)
{
	(*dir)->Activate();
	(*dir)->itsPath->Hide();
	(*dir)->itsIndicator->Show();
	(*dir)->GetWindow()->Refresh();
	(*dir)->GetDisplay()->Flush();
	(*dir)->GetDisplay()->Synchronize();
	GMessageHeaderList* list;
	JBoolean ok = GMessageHeaderList::Create(mailfile, &list, (*dir)->itsPG);
	(*dir)->itsIndicator->Hide();
	(*dir)->itsPath->Show();
	if (ok)
		{
		GMMailboxData* data	= new GMMailboxData(mailfile, list, (*dir)->itsPG);
		assert(data != NULL);
		(*dir)->itsData = data;
		(*dir)->ListenTo((*dir)->itsData);
		(*dir)->itsTable->SetData(data);
		(*dir)->itsColHeader->SetData(data);
		(*dir)->UpdateMessageCount();
		(*dir)->GetWindow()->Update();
		return kJTrue;
		}
	(*dir)->itsData = NULL;
	(*dir)->GMManagedDirector::Close();
	*dir = NULL;
	return kJFalse;
}

/******************************************************************************
 Constructor

 ******************************************************************************/

GMessageTableDir::GMessageTableDir
	(
	JXDirector* supervisor,
	const JString& mailfile
	)
	:
	GMManagedDirector(supervisor),
	itsDummyPrintText(NULL),
	itsFindDialog(NULL)
{
	BuildWindow(mailfile);
    GGetPrefsMgr()->GetTableWindowPrefs(this);

	itsViewDirs = new JPtrArray<GMessageViewDir>(JPtrArrayT::kForgetAll);
	assert(itsViewDirs != NULL);

	UpdateInboxMenu();
	ListenTo(GGetPrefsMgr());

	GGetDirMgr()->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMessageTableDir::~GMessageTableDir()
{
	delete itsNoMailIcon;
	delete itsNewMailIcon;
	delete itsSmallNewMailIcon;
	delete itsHaveMailIcon;
	delete itsViewDirs;
	delete itsPG;
	delete itsData;
	delete itsMenuIcon;
	GGetDirMgr()->DirectorDeleted(this);
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
GMessageTableDir::BuildWindow
	(
	const JString& mailfile
	)
{
	JSize w = 560;
	JSize h = 300;
	JString title;
	JString path;
	JSplitPathAndName(mailfile, &path, &title);
	itsWindow = new JXWindow(this, w,h, title);
	assert( itsWindow != NULL );
	SetWindow(itsWindow);
	itsWindow->SetWMClass(GMGetWMClassInstance(), GMGetTableWindowClass());
//	ListenTo(itsWindow);
    GGetPrefsMgr()->GetTableWindowSize(itsWindow);

	w = itsWindow->GetFrameWidth();
	h = itsWindow->GetFrameHeight();

	itsWindow->SetMinSize(w, 150);

	JXMenuBar* menuBar =
		new JXMenuBar(itsWindow,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0, 0, w, kJXDefaultMenuBarHeight);
	assert(menuBar != NULL);

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

	GMPOPRetrieverMenu* popmenu =
		new GMPOPRetrieverMenu(itsFileMenu, kCheckAccountCmd, menuBar);
	assert(popmenu != NULL);

//	itsInboxMenu = new JXTextMenu(itsFileMenu, kInboxCmd, menuBar);
	itsInboxMenu = menuBar->AppendTextMenu(kInboxMenuTitleStr);
	assert(itsInboxMenu != NULL);
	itsInboxMenu->SetMenuItems(kInboxMenuStr);
	itsInboxMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsInboxMenu);

	itsToolBar =
		new JXToolBar(GGetPrefsMgr(), kGTableToolBarID,
			menuBar, w, 150, itsWindow,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, kJXDefaultMenuBarHeight, w, h - kJXDefaultMenuBarHeight);
	assert(itsToolBar != NULL);

	const JCoordinate pathheight = 20;
	const JCoordinate scrollheight = itsToolBar->GetWidgetEnclosure()->GetBoundsHeight() - pathheight;

	itsSBSet =
		new JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,w,scrollheight);
	assert(itsSBSet != NULL);

	itsTable =
		new GMessageTable(this, menuBar,
			itsSBSet, itsSBSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,20,w,scrollheight-20);
	assert (itsTable != NULL);

	itsColHeader =
		new GMessageColHeader(itsTable, itsSBSet, itsSBSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0,w,20);
	assert(itsColHeader != NULL);

	itsColHeader->SetColTitle(4, "From");
	itsColHeader->SetColTitle(5, "Subject");
	itsColHeader->SetColTitle(6, "Date");
	itsColHeader->SetColTitle(7, "Size");

	const JCoordinate pathwidth = w - 150;

	itsPath =
		new JXStaticText(mailfile, itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedBottom,
			0,scrollheight,pathwidth,pathheight);
	assert(itsPath != NULL);
	itsPath->SetBorderWidth(1);

	itsMessageCount =
		new JXStaticText("", itsToolBar->GetWidgetEnclosure(),
			JXWidget::kFixedRight, JXWidget::kFixedBottom,
			pathwidth,scrollheight,w-pathwidth,pathheight);
	assert(itsMessageCount != NULL);
	itsMessageCount->SetBorderWidth(1);

	itsNoMailIcon = new JXImage(itsWindow->GetDisplay(), nomail_xpm);
	itsNoMailIcon->ConvertToRemoteStorage();

	itsNewMailIcon = new JXImage(itsWindow->GetDisplay(), newmail_xpm);
	itsNewMailIcon->ConvertToRemoteStorage();

	itsSmallNewMailIcon = new JXImage(itsWindow->GetDisplay(), newmail_small_xpm);
	itsSmallNewMailIcon->ConvertToRemoteStorage();

	itsHaveMailIcon = new JXImage(itsWindow->GetDisplay(), havemail_xpm);
	itsHaveMailIcon->ConvertToRemoteStorage();

	JXImage* icon	= new JXImage(*itsNoMailIcon);
	assert(icon != NULL);
	itsWindow->SetIcon(icon);

	itsIndicator =
        new JXProgressIndicator(itsToolBar->GetWidgetEnclosure(),
	JXWidget::kHElastic, JXWidget::kFixedBottom,
			5,scrollheight+5,pathwidth-10,pathheight/2);
	assert(itsIndicator != NULL);
	itsIndicator->Hide();

	itsPG = new GXBlockingPG(NULL, NULL, itsIndicator);
	assert(itsPG != NULL);
//	itsPG->SetItems(NULL, NULL, itsIndicator);
// begin JXLayout

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
	itsFileMenu->SetItemImage(kSaveMsgCmd, filefloppy);
	itsFileMenu->SetItemImage(kPrintMsgCmd, fileprint);
	itsFileMenu->SetItemImage(kCheckMailCmd, check_mail);
//	itsFileMenu->SetItemImage(kOpenCmd, fileopen);

	itsInboxMenu->SetItemImage(kAddInboxCmd, newmail_small_plus);
	itsInboxMenu->SetItemImage(kRemoveInboxCmd, newmail_small_minus);

	itsHelpMenu->SetItemImage(kTOCCmd, manual);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	itsDummyPrintText =
		new GMDummyText(GMGetAltPTPrinter(), "", itsWindow,
			JXWidget::kHElastic, JXWidget::kFixedBottom,
			0,0,500,10);
	assert(itsDummyPrintText != NULL);

	itsToolBar->LoadPrefs();

	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kNewCmd);
		itsToolBar->AppendButton(itsFileMenu, kNewMBox);
		itsToolBar->AppendButton(itsFileMenu, kOpenCmd);
		itsToolBar->NewGroup();

		itsToolBar->AppendButton(itsFileMenu, kSaveMsgCmd);
		itsToolBar->NewGroup();

		itsToolBar->AppendButton(itsFileMenu, kPrintMsgCmd);
		itsToolBar->NewGroup();

		itsToolBar->AppendButton(itsFileMenu, kCheckMailCmd);
		itsToolBar->NewGroup();

		JString id;
		if (GGetMailboxTreeDir()->GetID(&id))
			{
			itsToolBar->AppendButton(menu, id);
			}
		if (GMGetAddressBookDir()->GetID(&id))
			{
			itsToolBar->AppendButton(menu, id);
			}			
		itsToolBar->NewGroup();
		
		itsToolBar->AppendButton(itsHelpMenu, kTOCCmd);
		itsToolBar->AppendButton(itsHelpMenu, kThisWindowCmd);
		}

	itsMenuIcon = new JXImage(itsWindow->GetDisplay(), envelopes);
	assert(itsMenuIcon != NULL);
	itsMenuIcon->ConvertToRemoteStorage();
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GMessageTableDir::Receive
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

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsInboxMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleInboxMenu(selection->GetIndex());
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

	else if (sender == itsWindow && message.Is(JXWindow::kIconified))
		{
		itsData->ShouldBePostponingUpdate(kJTrue);
		}
	else if (sender == itsWindow && message.Is(JXWindow::kDeiconified))
		{
		itsData->ShouldBePostponingUpdate(kJFalse);
		itsData->Update();
		}
	else if (sender == GGetPrefsMgr())
		{
		if (message.Is(GPrefsMgr::kInboxesChanged))
			{
			UpdateInboxMenu();
			}
		else if (message.Is(GPrefsMgr::kShowStateChanged))
			{
			AdjustWindowTitle();
			}
		}
	else if (sender == GMGetAltPTPrinter() && message.Is(GMPTPrinter::kPrintStarting))
		{
		JString tempname;
		const JError err = JCreateTempFile(&tempname);
		if (!err.OK())
			{
			JGetUserNotification()->ReportError("Unable to print.");
			return;
			}
		ofstream os(tempname);
		if (!os.good())
			{
			JGetUserNotification()->ReportError("Unable to print.");
			os.close();
			JRemoveFile(tempname);
			return;
			}
		JBoolean headers = GMGetAltPTPrinter()->WillPrintHeader();
		SaveSelectedMessages(os, headers);
		os.close();
		JTextEditor::PlainTextFormat format;
		itsDummyPrintText->ReadPlainText(tempname, &format);
		JRemoveFile(tempname);
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
	else if (sender == itsFindDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert(info != NULL);
		if (info->Successful())
			{
			SearchMessages();
			}
		itsFindDialog	= NULL;
		}
	else if (sender == itsData)
		{
		if (message.Is(GMMailboxData::kStartingUpdate))
			{
			itsPath->Hide();
			itsIndicator->Show();
			GetWindow()->Deactivate();
			}
		else if (message.Is(GMMailboxData::kUpdateFinished))
			{
			itsIndicator->Hide();
			itsPath->Show();
			UpdateMessageCount();
			AdjustWindowTitle();
			GetWindow()->Activate();
			}
		else if (message.Is(GMMailboxData::kNewMail))
			{
			const GMMailboxData::NewMail* info	= 
				dynamic_cast(const GMMailboxData::NewMail*, &message);
			assert(info != NULL);
			
			if (GGetPrefsMgr()->IsBeepingOnNewMail())
				{
				GetWindow()->GetDisplay()->Beep();
				}

			if (info->IsModified() && 
				GetWindow()->IsIconified())
				{
				JXImage* icon	= new JXImage(*itsNewMailIcon);
				assert(icon != NULL);
				itsWindow->SetIcon(icon);
				AdjustWindowTitle();
				}
			}
		else if (message.Is(GMMailboxData::kMBoxState))
			{
			const GMMailboxData::MBoxState* info	= 
				dynamic_cast(const GMMailboxData::MBoxState*, &message);
			assert(info != NULL);
			if (info->Exists())
				{
				itsSBSet->Activate();
				}
			else
				{
				itsSBSet->Deactivate();
				}
			}
		else if (message.Is(GMMailboxData::kHeaderRemoved))
			{
			const GMMailboxData::HeaderRemoved* info	= 
				dynamic_cast(const GMMailboxData::HeaderRemoved*, &message);
			assert(info != NULL);
			CloseMessage(info->GetHeader());
			}
		else if (message.Is(GMMailboxData::kHeaderMarkedDeleted))
			{
			const GMMailboxData::HeaderMarkedDeleted* info	= 
				dynamic_cast(const GMMailboxData::HeaderMarkedDeleted*, &message);
			assert(info != NULL);
			CloseMessage(info->GetHeader());
			}
		else if (message.Is(GMMailboxData::kDataModified))
			{
			AdjustWindowTitle();
			}
		else if (message.Is(GMMailboxData::kDataReverted))
			{
			AdjustWindowTitle();
			}
		}
	else
		{
		GMManagedDirector::Receive(sender, message);
		}

}

/******************************************************************************
 DirectorClosed

 ******************************************************************************/

void
GMessageTableDir::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	GMessageViewDir* dir = (GMessageViewDir*) theDirector;
	if (itsViewDirs->Includes(dir))
		{
		itsViewDirs->Remove(dir);
		}
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
GMessageTableDir::UpdateFileMenu()
{
	JTableSelection& s = itsTable->GetTableSelection();
	if (s.HasSelection())
		{
		itsFileMenu->EnableItem(kSaveMsgCmd);
		itsFileMenu->EnableItem(kAppendMsgCmd);
		itsFileMenu->EnableItem(kTransferToCmd);
		itsFileMenu->EnableItem(kCopyToCmd);
		itsFileMenu->EnableItem(kPrintMsgCmd);
		}
	else
		{
		itsFileMenu->DisableItem(kSaveMsgCmd);
		itsFileMenu->DisableItem(kAppendMsgCmd);
		itsFileMenu->DisableItem(kTransferToCmd);
		itsFileMenu->DisableItem(kCopyToCmd);
		itsFileMenu->DisableItem(kPrintMsgCmd);
		}
	if (itsData->NeedsSave())
		{
		itsFileMenu->EnableItem(kSaveCmd);
		itsFileMenu->EnableItem(kFlushCmd);
		}
	else
		{
		itsFileMenu->DisableItem(kSaveCmd);
		itsFileMenu->DisableItem(kFlushCmd);
		}
	if ((GGetAccountMgr()->GetPOPAccountCount() > 0) &&
		(GGetAccountMgr()->FinishedChecking()))
		{
		itsFileMenu->EnableItem(kCheckMailCmd);
		}
	else
		{
		itsFileMenu->DisableItem(kCheckMailCmd);
		}
	itsFileMenu->EnableItem(kCheckAccountCmd);
}

/******************************************************************************
 HandleFileMenu


 ******************************************************************************/

void
GMessageTableDir::HandleFileMenu
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
	else if (index == kFindMessageCmd)
		{
		assert(itsFindDialog == NULL);
		itsFindDialog	= new GMMessageFindDialog(this);
		assert(itsFindDialog != NULL);
		ListenTo(itsFindDialog);
		itsFindDialog->BeginDialog();
		}
	else if (index == kSaveMsgCmd)
		{
		SaveSelectedMessages();
		}
	else if (index == kAppendMsgCmd)
		{
		AppendSelectedMessages();
		}
	else if (index == kPageSetupCmd)
		{
		GMGetAltPTPrinter()->BeginUserPageSetup();
		}
	else if (index == kPrintMsgCmd)
		{
		PrintSelectedMessages();
		}
	else if (index == kCheckMailCmd)
		{
		GGetAccountMgr()->CheckAllAccounts();
		}
	else if (index == kSaveCmd)
		{
		itsData->Save();
		}
	else if (index == kFlushCmd)
		{
		itsData->Flush();
		}
	else if (index == kCloseCmd)
		{
		Close();
		}
	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdateInboxMenu


 ******************************************************************************/

void
GMessageTableDir::UpdateInboxMenu()
{
	JSize count = itsInboxMenu->GetItemCount();
	for (JSize i = count; i >= 5; i--)
		{
		itsInboxMenu->RemoveItem(i);
		}

	JPtrArray<JString> inboxes(JPtrArrayT::kForgetAll);
	GGetPrefsMgr()->GetInboxes(inboxes);
	for (JSize i = 1; i <= inboxes.GetElementCount(); i++)
		{
		JString inbox;
		JString path;
		JSplitPathAndName(*(inboxes.NthElement(i)), &path, &inbox);
		itsInboxMenu->AppendItem(inbox);
		}
	inboxes.DeleteAll();

	for (JSize i = 5; i <= itsInboxMenu->GetItemCount(); i++)
		{
		itsInboxMenu->SetItemImage(i, itsSmallNewMailIcon, kJFalse);
		}
}

/******************************************************************************
 HandleInboxMenu


 ******************************************************************************/

void
GMessageTableDir::HandleInboxMenu
	(
	const JIndex index
	)
{
	if (index == kAddInboxCmd)
		{
		GGetPrefsMgr()->AddInbox(itsData->GetMailFile());
		}
	else if (index == kRemoveInboxCmd)
		{
		GGetPrefsMgr()->DeleteInbox(itsData->GetMailFile());
		}
	else if (index == kRemoveAllInboxesCmd)
		{
		JString notice = "Are you sure you want to delete all of the inboxes?";
		JBoolean ok = JGetUserNotification()->AskUserYes(notice);
		if (ok)
			{
			GGetPrefsMgr()->DeleteAllInboxes();
			}
		}
	else if (index == kOpenAllInboxesCmd)
		{
		JPtrArray<JString> inboxes(JPtrArrayT::kForgetAll);
		GGetPrefsMgr()->GetInboxes(inboxes);
		for (JSize i = 1; i <= inboxes.GetElementCount(); i++)
			{
			GMGetApplication()->OpenMailbox(*(inboxes.NthElement(i)));
			}
		}
	else
		{
		JPtrArray<JString> inboxes(JPtrArrayT::kForgetAll);
		GGetPrefsMgr()->GetInboxes(inboxes);
		JSize count = inboxes.GetElementCount();
		assert(count >= index - 4);
		JString filename = *(inboxes.NthElement(index - 4));
		if (JFileExists(filename))
			{
			GMGetApplication()->OpenMailbox(filename);
			}
		else
			{
			JString report = "The mailbox \"" + filename + "\" does not exist. Would you like to remove it from the inbox list?";
			if (JGetUserNotification()->AskUserYes(report))
				{
				GGetPrefsMgr()->DeleteInbox(filename);
				}
			}
		}
}

/******************************************************************************
 HandlePrefsMenu


 ******************************************************************************/

void
GMessageTableDir::HandlePrefsMenu
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
		GGetPrefsMgr()->SetTableWindowPrefs(this);
		}

}

/******************************************************************************
 HandleHelpMenu


 ******************************************************************************/

void
GMessageTableDir::HandleHelpMenu
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
		(JXGetHelpManager())->ShowSection(kGMailboxHelpName);
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
 Close

 ******************************************************************************/

JBoolean
GMessageTableDir::Close()
{
	itsData->Save();
	GMGetApplication()->BroadcastMailboxClosed(itsData->GetMailFile());
	return GMManagedDirector::Close();
}

/******************************************************************************
 UpdateMessageCount

 ******************************************************************************/

void
GMessageTableDir::UpdateMessageCount()
{
	JSize count = itsData->GetHeaderCount();
	JString messageCount(count, 0);
	messageCount += " messages";
	itsMessageCount->SetText(messageCount);
	if (count > 0)
		{
		JXImage* icon	= new JXImage(*itsHaveMailIcon);
		assert(icon != NULL);
		itsWindow->SetIcon(icon);
		}
	else
		{
		JXImage* icon	= new JXImage(*itsNoMailIcon);
		assert(icon != NULL);
		itsWindow->SetIcon(icon);
		}
	AdjustWindowTitle();
}

/******************************************************************************
 ViewMessage

 ******************************************************************************/

void
GMessageTableDir::ViewMessage
	(
	GMessageHeader* header
	)
{
	GMessageViewDir* dir;
	for (JSize i = 1; i <= itsViewDirs->GetElementCount(); i++)
		{
		dir = itsViewDirs->NthElement(i);
		if (header == dir->GetMessageHeader())
			{
			dir->Activate();
			return;
			}
		}
	JXGetApplication()->DisplayBusyCursor();
	dir = new GMessageViewDir(this, itsData->GetMailFile(), header);
	assert(dir != NULL);
	itsViewDirs->Append(dir);
	dir->Activate();
}

/******************************************************************************
 CloseMessage

 ******************************************************************************/

void
GMessageTableDir::CloseMessage
	(
	const GMessageHeader* header
	)
{
	const JSize count = itsViewDirs->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		GMessageViewDir* dir = itsViewDirs->NthElement(i);
		if (const_cast<GMessageHeader*>(header) == dir->GetMessageHeader())
			{
			dir->Close();
			}
		}
	Broadcast(MessageClosed(header));
}

/******************************************************************************
 SaveState

 ******************************************************************************/

void
GMessageTableDir::SaveState
	(
	JFileArray& fileArray
	)
{
	{
	std::ostringstream os;
	os << itsData->GetMailFile() << " ";
	GetWindow()->WriteGeometry(os);
	fileArray.AppendElement(os);
	}
	JSize count;
	{
	std::ostringstream os;
	JXScrollbar* hsb = itsSBSet->GetHScrollbar();
	JXScrollbar* vsb = itsSBSet->GetVScrollbar();
	os << hsb->GetValue() << " ";
	os << vsb->GetValue() << " ";
	JBoolean hasNewMail = kJFalse;
	os << itsData->HasNewMail() << " ";
	os << itsData->GetEntry().GetModTime() << " ";
	for (JSize i = 1; i <= itsTable->GetColCount(); i++)
		{
		os << itsTable->GetColWidth(i) << " ";
		}

	count = itsViewDirs->GetElementCount();
	os << count << " ";
	fileArray.AppendElement(os);
	}
	for (JSize i = 1; i <= count; i++)
		{
		std::ostringstream os;
		itsViewDirs->NthElement(i)->SaveState(os);
		fileArray.AppendElement(os);
		}
}

/******************************************************************************
 ReadState

 ******************************************************************************/

void
GMessageTableDir::ReadState
	(
	JFileArray& fileArray,
	const JFileVersion& version
	)
{
	JSize count;
	JFAID id = 2;

	{
	std::string data;
	fileArray.GetElement(id, &data);
	std::istringstream is(data);

	JCoordinate hval, vval;
	is >> hval;
	is >> vval;
	JXScrollbar* hsb = itsSBSet->GetHScrollbar();
	JXScrollbar* vsb = itsSBSet->GetVScrollbar();
	hsb->SetValue(hval);
	vsb->SetValue(vval);

	// check for had new mail
	JBoolean hadNewMail;
	is >> hadNewMail;
	if (hadNewMail)
		{
		if (GetWindow()->IsIconified())
			{
			JXImage* icon	= new JXImage(*itsNewMailIcon);
			assert(icon != NULL);
			itsWindow->SetIcon(icon);
			itsData->ShouldHaveNewMail(kJTrue);
			AdjustWindowTitle();
			}
		}

	// check for different mod time
	time_t time;
	is >> time;
	if (time < itsData->GetEntry().GetModTime())
		{
		if (GetWindow()->IsIconified())
			{
			JXImage* icon	= new JXImage(*itsNewMailIcon);
			assert(icon != NULL);
			itsWindow->SetIcon(icon);
			itsData->ShouldHaveNewMail(kJTrue);
			AdjustWindowTitle();
			}
		}

	for (JSize i = 1; i <= itsTable->GetColCount(); i++)
		{
		JCoordinate width;
		is >> width;
		itsTable->SetColWidth(i, width);
		}
	is >> count;
	}
	id.SetID(id.GetID() + 1);

	JString hdr, from, subject;
	for (JSize i = 1; i <= count; i++)
		{
		std::string data;
		fileArray.GetElement(id, &data);
		std::istringstream is(data);
		is >> hdr;
		if (version >= 5)
		{
			is >> from >> subject;
		}

		GMessageHeader* header;
		JBoolean found = kJFalse;
		for (JIndex index = 1; index <= itsData->GetHeaderCount(); index++)
			{
			header = itsData->GetHeader(index);
			if (header->GetHeader() == hdr &&
				(version < 5 ||
				 (header->GetFrom() == from &&
				  header->GetSubject() == subject)))
				{
				found = kJTrue;
				break;
				}
			}

		if (found)
			{
			GMessageViewDir* dir =
				new GMessageViewDir(this, itsData->GetMailFile(), header);
			assert(dir != NULL);
			itsViewDirs->Append(dir);
			dir->ReadState(is, version);
			dir->Activate();
			}
		id.SetID(id.GetID() + 1);
		}
}

/******************************************************************************
 WindowPrefs

 ******************************************************************************/

void
GMessageTableDir::ReadWindowPrefs
	(
	istream& is
	)
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentWindowPrefsVersion)
		{
		return;
		}
	GetWindow()->ReadGeometry(is);
	const JSize count	= itsTable->GetColCount();
	for (JSize i = 1; i <= count; i++)
		{
		JCoordinate width;
		is >> width;
		itsTable->SetColWidth(i, width);
		}
}

void
GMessageTableDir::WriteWindowPrefs
	(
	ostream& os
	)
{
	os << ' ' << kCurrentWindowPrefsVersion << ' ';
	GetWindow()->WriteGeometry(os);
	const JSize count	= itsTable->GetColCount();
	for (JSize i = 1; i <= count; i++)
		{
		os << itsTable->GetColWidth(i) << " ";
		}
}

/******************************************************************************
 AdjustWindowTitle

 ******************************************************************************/

void
GMessageTableDir::AdjustWindowTitle()
{
	JString title, path;
	JSplitPathAndName(itsData->GetMailFile(), &path, &title);

	if (GGetPrefsMgr()->ShowingStateInTitle())
		{
		const JSize count = itsData->GetHeaderCount();
		if (itsData->HasNewMail())
			{
			title.Prepend("**");
			}
		else if (count > 0)
			{
			title.Prepend("*");
			}
		GetWindow()->SetTitle(title);
		}
	else
		{
		GetWindow()->SetTitle(title);
		}
}

/******************************************************************************
 SaveSelectedMessages (private)

 ******************************************************************************/

void
GMessageTableDir::SaveSelectedMessages
	(
	const JBoolean headers
	)
{
	JString filename;
	if (GMGetAltChooseSaveFile()->SaveFile("File name:", "", "",  &filename))
		{
		ofstream os(filename);
		JBoolean ok = JConvertToBoolean(os.good());
		if (!ok)
			{
			JString notice = "The file \"" + filename + "\" could not be opened.";
			JGetUserNotification()->ReportError(notice);
			return;
			}
		JBoolean saveHeaders = GMGetAltChooseSaveFile()->IsSavingHeaders();
		SaveSelectedMessages(os, saveHeaders);
		os.close();
		}
	return;
}

/******************************************************************************
 AppendSelectedMessages (private)

 ******************************************************************************/

void
GMessageTableDir::AppendSelectedMessages
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
		SaveSelectedMessages(os, saveHeaders);
		is.close();
		os.close();

		GMReplaceFileWithTemp(tempname, filename);
		}
}

/******************************************************************************
 SaveSelectedMessages (private)

 ******************************************************************************/

void
GMessageTableDir::SaveSelectedMessages
	(
	ostream&		os,
	const JBoolean	headers
	)
{
	JTableSelection& s = itsTable->GetTableSelection();
	JTableSelectionIterator iter(&s);

	JPoint cell;
	GMessageHeaderList* list = new GMessageHeaderList();
	assert(list != NULL);
	while(iter.Next(&cell))
		{
		GMessageHeader* header = itsData->GetHeader(cell.y);
		list->Append(header);
		}

	itsData->SaveSelectedMessages(os, headers, list);
	delete list;
}

/******************************************************************************
 PrintSelectedMessages (private)

 ******************************************************************************/

void
GMessageTableDir::PrintSelectedMessages
	(
	const JBoolean headers
	)
{
	itsDummyPrintText->PrintPT();
	ListenTo(GMGetAltPTPrinter());
}

/******************************************************************************
 HandleMessageTransfer (private)

 ******************************************************************************/

void
GMessageTableDir::HandleMessageTransfer
	(
	const JCharacter*	dest,
	const JBoolean		move
	)
{

	JTableSelection& s = itsTable->GetTableSelection();
	JTableSelectionIterator iter(&s);
	if (!s.HasSelection())
		{
		return;
		}

	JPoint cell;
	GMessageHeaderList* headers = new GMessageHeaderList();
	assert(headers != NULL);
	while(iter.Next(&cell))
		{
		headers->Append(itsData->GetHeader(cell.y));
		}

	itsData->HandleMessageTransfer(dest, move, headers);
	delete headers;
}

/******************************************************************************
 SearchMessages (private)

 ******************************************************************************/

void
GMessageTableDir::SearchMessages()
{
	if (!GLockFile(itsData->GetMailFile()))
		{
		JString report = "This file is currently locked, please try again in a moment.";
		JGetUserNotification()->ReportError(report);
		return;
		}
	
	assert(itsFindDialog != NULL);
	JString searchStr	= itsFindDialog->GetSearchString();
	JBoolean searchMsg	= itsFindDialog->SearchMessage();

	if (searchStr.IsEmpty())
		{
		return;
		}

	std::istringstream pis(std::string(searchStr, searchStr.GetLength()));
	JPtrArray<JString> patterns(JPtrArrayT::kDeleteAll);

	JBoolean foundws	= kJTrue;
	while (foundws)
		{
		JString* str	= new JString(JReadUntilws(pis, &foundws));
		assert(str != NULL);
		patterns.Append(str);
		}

	mode_t perms;
	JError err = JGetPermissions(itsData->GetMailFile(), &perms);
	if (!err.OK())
		{
		perms = 0600;
		}

	ifstream is(itsData->GetMailFile());
	if (!is.good())
		{
		JGetUserNotification()->ReportError("There was an error opening this mailbox.");
		return;
		}
			
	GMessageHeaderList* list	= new GMessageHeaderList();
	assert(list != NULL);

	JSize count = itsData->GetHeaderCount();
	JLatentPG* pg	= new JLatentPG();
	assert(pg != NULL);
	const JString msg	= "Searching messages...";
	pg->FixedLengthProcessBeginning(count, msg, kJTrue, kJFalse);
	for (JIndex i = 1; i <= count; i++)
		{
		GMessageHeader* header = itsData->GetHeader(i);
		const JSize pcount	= patterns.GetElementCount();
		JBoolean match		= kJFalse;
		for (JIndex j = 1; j <= pcount; j++)
			{
			JString pattern	= *(patterns.NthElement(j));
			if (header->GetFrom().Contains(pattern, kJFalse) ||
				header->GetSubject().Contains(pattern, kJFalse))
				{
				match	= kJTrue;
				}
			else if (searchMsg)
				{
				if (header->IsMIME())
					{
					GMMIMEParser* parser	= header->GetMIMEParser();
					JString dir;
					JBoolean ok	= GMGetApplication()->GetFileDirectory(&dir);
					if (ok && parser == NULL)
						{
						is.seekg(header->GetHeaderStart());
						JString body = JRead(is, header->GetMessageEnd() - header->GetHeaderStart());
						parser	= new GMMIMEParser(&body, dir);
						assert(parser != NULL);
						header->SetMIMEParser(parser);
						}
					if (ok)
						{
						itsDummyPrintText->SetText("");
						JString data;
						const JSize tcount = parser->GetTextSegmentCount();
						for (JIndex k = 1; k <= tcount; k++)
							{
							GMMIMEParser::TextFormat format;
							JString charset;
							parser->GetTextSegment(k, &data, &format, &charset);
							data.AppendCharacter('\n');
							if (format == GMMIMEParser::kHTML)
								{
								std::istrstream is(data.GetCString(), data.GetLength());
								itsDummyPrintText->PasteHTML(is);
								}
							else
								{
								itsDummyPrintText->Paste(data);
								}
							}
						if (itsDummyPrintText->GetText().Contains(pattern, kJFalse))
							{
							match	= kJTrue;
							}
						else
							{
							match	= kJFalse;
							break;
							}
						}
					else
						{
						match	= kJFalse;
						break;
						}
					}
				else
					{
					is.seekg(header->GetHeaderEnd());
					JString body = JRead(is, header->GetMessageEnd() - header->GetHeaderEnd());
					if (body.Contains(pattern, kJFalse))
						{
						match	= kJTrue;
						}
					else
						{
						match	= kJFalse;
						break;
						}
					}
				}
			else
				{
				match		= kJFalse;
				break;
				}
			}
		if (match)
			{
			list->Append(header);
			}
		if (!pg->IncrementProgress())
			{
			break;
			}
		}

	pg->ProcessFinished();


	if (list->GetElementCount() > 0)
		{
		GMFindTableDir* dir	= new GMFindTableDir(this, itsData, list);
		assert(dir != NULL);
		dir->Activate();
		}
	else
		{
		delete list;
		JGetUserNotification()->DisplayMessage("No matches were found.");
		}
		
	GUnlockFile(itsData->GetMailFile());
}
