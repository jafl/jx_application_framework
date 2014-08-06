/******************************************************************************
 GMFindTableDir.cc

	BASE CLASS = GMManagedDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GMFindTableDir.h"
#include "GMessageEditDir.h"
#include "GMessageTable.h"
#include "GMessageHeaderList.h"
#include "GMessageHeader.h"
#include "GMessageColHeader.h"
#include "GMessageFrom.h"
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
#include "GMessageTableDir.h"
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
#include <JTableSelection.h>
#include <JDirInfo.h>
#include <JUserNotification.h>

#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
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
	"%l|Save messages... %k Meta-S %i \"SaveMessages::Arrow\""
	"  |Append messages... %i \"AppendMessages::Arrow\""
	"  |Transfer to"
	"  |Copy to"
	"%l|Page setup... %i \"PageSetup::Arrow\""
	"  |Print selected messages... %k Meta-P %i \"PrintMessages::Arrow\""
	"%l| Check all accounts %k Meta-M  %i \"CheckMail::Arrow\""
	"  |Check account %k Meta-M  %i \"CheckAccount::Arrow\""
	"%l| Save changes %i \"SaveChanges::Arrow\""
	"  |Flush deleted messages %k Meta-F %i \"FlushDeleted::Arrow\""
	"%l|Close %k Meta-W %i \"Close::Arrow\""
	"  |Quit %k Meta-Q %i \"Quit::Arrow\"";

enum
{
	kNewCmd = 1,
	kNewMBox,
	kOpenCmd,
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

/******************************************************************************
 Constructor

 ******************************************************************************/

GMFindTableDir::GMFindTableDir
	(
	GMessageTableDir* 	supervisor,
	GMMailboxData* 		data,
	GMessageHeaderList*	list
	)
	:
	GMManagedDirector(supervisor),
	itsDummyPrintText(NULL),
	itsDir(supervisor)
{
	itsData	= new GMMailboxData(data, list);
	assert(itsData != NULL);

	BuildWindow();
	itsTable->SetData(itsData);
	ListenTo(itsDir);

    GGetPrefsMgr()->GetFindDirWindowPrefs(this);
	GGetDirMgr()->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMFindTableDir::~GMFindTableDir()
{
	delete itsData;
	GGetDirMgr()->DirectorDeleted(this);
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
GMFindTableDir::BuildWindow()
{
	JSize w = 560;
	JSize h = 300;
	JString title;
	JString path;
	JSplitPathAndName(itsData->GetMailFile(), &path, &title);
	title.Prepend("Find messages in: ");

	itsWindow = new JXWindow(this, w,h, title);
	assert( itsWindow != NULL );

	itsWindow->SetWMClass(GMGetWMClassInstance(), GMGetFindDirWindowClass());
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

	itsToolBar =
		new JXToolBar(GGetPrefsMgr(), kGFindDirToolBarID,
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
		new GMessageTable(itsDir, menuBar,
			itsSBSet, itsSBSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,20,w,scrollheight-20);
	assert (itsTable != NULL);

	GMessageColHeader* header =
		new GMessageColHeader(itsTable, itsSBSet, itsSBSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0,w,20);
	assert(header != NULL);

	header->SetColTitle(4, "From");
	header->SetColTitle(5, "Subject");
	header->SetColTitle(6, "Date");
	header->SetColTitle(7, "Size");

	const JCoordinate pathwidth = w - 150;

//	JXImage* icon	= new JXImage(*itsNoMailIcon);
//	assert(icon != NULL);
//	itsWindow->SetIcon(icon);
//	itsHasNewMail = kJFalse;

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
GMFindTableDir::Receive
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

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
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

	else if (sender == GMGetAltPTPrinter() && message.Is(GMPTPrinter::kPrintStarting))
		{
		JString tempname;
		const JError err = JCreateTempFile(&tempname);
		if (!err.OK())
			{
			JGetUserNotification()->ReportError("Unable to print!");
			return;
			}
		ofstream os(tempname);
		if (!os.good())
			{
			JGetUserNotification()->ReportError("Unable to print!");
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
			dynamic_cast<const JXFSDirMenu::FileSelected*>(&message);
		assert(info != NULL);
		HandleMessageTransfer(info->GetFileName(), kJTrue);
		}
	else if (sender == itsCopyMenu && message.Is(JXFSDirMenu::kFileSelected))
		{
		const JXFSDirMenu::FileSelected* info =
			dynamic_cast<const JXFSDirMenu::FileSelected*>(&message);
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
	else
		{
		GMManagedDirector::Receive(sender, message);
		}

}

/******************************************************************************
 UpdateFileMenu


 ******************************************************************************/

void
GMFindTableDir::UpdateFileMenu()
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
	if (itsNeedsSave)
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
GMFindTableDir::HandleFileMenu
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
 HandlePrefsMenu


 ******************************************************************************/

void
GMFindTableDir::HandlePrefsMenu
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
		GGetPrefsMgr()->SetFindDirWindowPrefs(this);
		}

}

/******************************************************************************
 HandleHelpMenu


 ******************************************************************************/

void
GMFindTableDir::HandleHelpMenu
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
 WindowPrefs

 ******************************************************************************/

void
GMFindTableDir::ReadWindowPrefs
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
GMFindTableDir::WriteWindowPrefs
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
 SaveSelectedMessages (private)

 ******************************************************************************/

void
GMFindTableDir::SaveSelectedMessages
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
GMFindTableDir::AppendSelectedMessages
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
			return;
			}
		JString temp;
		temp.Read(is, size);
		if (size != 0 && !temp.EndsWith("\n\n"))
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
GMFindTableDir::SaveSelectedMessages
	(
	ostream&		os,
	const JBoolean	headers
	)
{
	JTableSelection& s = itsTable->GetTableSelection();
	JTableSelectionIterator iter(&s);

	GMessageHeaderList* list	= new GMessageHeaderList();
	assert(list != NULL);

	JPoint cell;
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
GMFindTableDir::PrintSelectedMessages
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
GMFindTableDir::HandleMessageTransfer
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
	GMessageHeaderList* headers	= new GMessageHeaderList();
	assert(headers != NULL);
	while(iter.Next(&cell))
		{
		headers->Append(itsData->GetHeader(cell.y));
		}
	itsData->HandleMessageTransfer(dest, move, headers);
	delete headers;
}

