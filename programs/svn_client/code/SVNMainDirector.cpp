/******************************************************************************
 SVNMainDirector.cc

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "SVNMainDirector.h"
#include "SVNTabGroup.h"
#include "SVNRepoView.h"
#include "SVNRepoTree.h"
#include "SVNGetRepoDialog.h"
#include "SVNStatusList.h"
#include "SVNUpdateList.h"
#include "SVNInfoLog.h"
#include "SVNPropertiesList.h"
#include "SVNCommandLog.h"
#include "SVNRefreshStatusTask.h"
#include "SVNMDIServer.h"
#include "svnMenus.h"
#include "svnGlobals.h"
#include <JXHelpManager.h>
#include <JXFSBindingManager.h>
#include <JXWebBrowser.h>
#include <JXMacWinPrefsDialog.h>
#include <JXGetStringDialog.h>
#include <JXChooseSaveFile.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXWDMenu.h>
#include <JXToolBar.h>
#include <JXScrollbarSet.h>
#include <JXColorManager.h>
#include <JXImage.h>
#include <JProcess.h>
#include <jDirUtil.h>
#include <jWebUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    Open directory...                   %k Meta-O                   %i" kSVNOpenDirectoryAction
	"  | Browse repository...                %k Meta-Shift-O             %i" kSVNBrowseRepoAction
	"%l| Check out repository...                                         %i" kSVNCheckOutRepoAction
	"  | Check out current repository...                                 %i" kSVNCheckOutCurrentRepoAction
	"  | Check out selected directory...                                 %i" kSVNCheckOutSelectionAction
	"%l| Open selected files                 %k Left-dbl-click or Return %i" kSVNOpenFilesAction
	"  | Show selected files in file manager                             %i" kSVNShowFilesAction
	"%l| Close                               %k Meta-W                   %i" kJXCloseWindowAction
	"%l| Quit                                %k Meta-Q                   %i" kJXQuitAction;

enum
{
	kOpenDirectoryCmd = 1,
	kBrowseRepoCmd,
	kCheckOutRepoCmd,
	kCheckOutCurrentRepoCmd,
	kCheckOutSelectionCmd,
	kOpenFilesCmd,
	kShowFilesCmd,
	kCloseCmd,
	kQuitCmd
};

// Edit menu additions

static const JCharacter* kEditMenuAddStr =
	"Copy full path %k Meta-Shift-C %i" kSVNCopyFullPathAction;

// Actions menu

static const JCharacter* kActionsMenuTitleStr = "Actions";
static const JCharacter* kActionsMenuStr =
	"    Refresh display            %k F5     %i" kSVNRefreshStatusAction
	"  | Close tab                  %k Ctrl-W %i" kSVNCloseTabAction
	"%l| Update working copy        %k Meta-@ %i" kSVNUpdateWorkingCopyAction
	"  | Clean up working copy                %i" kSVNCleanUpWorkingCopyAction
	"%l| Add selected files         %k Meta-! %i" kSVNAddFilesAction
	"  | Remove selected files                %i" kSVNRemoveFilesAction
	"  | Remove selected files (force)        %i" kSVNForceRemoveFilesAction
	"%l| Resolve selected conflicts %k Meta-` %i" kSNVResolveSelectedConflictsAction
	"%l| Commit selected changes    %k Meta-~ %i" kSNVCommitSelectedFilesAction
	"  | Commit all changes                   %i" kSVNCommitAllFilesAction
	"%l| Revert selected changes              %i" kSVNRevertSelectedFilesAction
	"  | Revert all changes                   %i" kSVNRevertAllFilesAction
	"%l| New directory...                     %i" kSVNCreateDirectoryAction
	"  | Duplicate selected file/directory... %i" kSVNDuplicateSelectedItemAction
	"%l| New property...                      %i" kSVNCreatePropertyAction
	"  | Remove selected properties...        %i" kSVNRemovePropertiesAction
	"%l| Ignore selected item...              %i" kSVNIgnoreSelectionAction;

	/*
	 * Remember to keeup svnMenus.h in sync
	 */

// Info menu

static const JCharacter* kInfoMenuTitleStr = "Info";
static const JCharacter* kInfoMenuStr =
	"    Info & Log %k Meta-I     %i" kSVNInfoLogSelectedFilesAction
	"  | Properties               %i" kSVNPropSelectedFilesAction
	"%l| Compare with edited      %i" kSVNDiffEditedSelectedFilesAction
	"  | Compare with current     %i" kSVNDiffCurrentSelectedFilesAction
	"  | Compare with previous    %i" kSVNDiffPrevSelectedFilesAction
	"%l| Commit details           %i" kSVNCommitDetailsAction
	"%l| Browse revision...       %i" kSVNBrowseRevisionAction
	"  | Browse selected revision %i" kSVNBrowseSelectedRevisionAction;

	/*
	 * Remember to keeup svnMenus.h in sync
	 */

// Misc menus

static const JCharacter* kWindowsMenuTitleStr = "Windows";

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    Integration..."
	"  | Edit tool bar..."
	"  | File bindings..."
	"  | File manager & web browser..."
	"  | Mac/Win/X emulation..."
	"%l| Save window setup as default";

enum
{
	kIntegrationPrefsCmd = 1,
	kEditToolBarCmd,
	kEditBindingsCmd,
	kWebBrowserCmd,
	kEditMacWinPrefsCmd,
	kSaveWindSizeCmd
};

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About"
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor

 *****************************************************************************/

SVNMainDirector::SVNMainDirector
	(
	JXDirector*			supervisor,
	const JCharacter*	path
	)
	:
	JXWindowDirector(supervisor),
	itsPath(path)
{
	SVNMainDirectorX();

	JPoint desktopLoc;
	JCoordinate w,h;
	if ((SVNGetPrefsManager())->GetWindowSize(kSVNMainDirectorWindSizeID,
											  &desktopLoc, &w, &h))
		{
		JXWindow* window = GetWindow();
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
		}

	if (itsRepoWidget != NULL)
		{
		itsRepoWidget->RefreshContent();
		}
}

SVNMainDirector::SVNMainDirector
	(
	JXDirector*		supervisor,
	std::istream&		input,
	JFileVersion	vers
	)
	:
	JXWindowDirector(supervisor)
{
	input >> itsPath;

	SVNMainDirectorX();

	GetWindow()->ReadGeometry(input);

	if (vers >= 1)
		{
		JBoolean hadRepoWidget;
		input >> hadRepoWidget;

		if (itsRepoWidget != NULL)
			{
			itsRepoWidget->ReadSetup(hadRepoWidget, input, vers);
			}
		else if (hadRepoWidget)
			{
			SVNRepoView::SkipSetup(input, vers);
			}
		}
}

// private

void
SVNMainDirector::SVNMainDirectorX()
{
	itsRepoWidget               = NULL;
	itsStatusWidget             = NULL;
	itsActionProcess            = NULL;
	itsCheckOutProcess          = NULL;
	itsBrowseRepoDialog         = NULL;
	itsBrowseRepoRevisionDialog = NULL;
	itsCheckOutRepoDialog       = NULL;
	itsRefreshStatusTask        = NULL;

	itsTabList = jnew JPtrArray<SVNTabBase>(JPtrArrayT::kForgetAll);
	assert( itsTabList != NULL );

	BuildWindow();

	(GetDisplay()->GetWDManager())->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

SVNMainDirector::~SVNMainDirector()
{
	jdelete itsTabList;
}

/******************************************************************************
 StreamOut

 *****************************************************************************/

void
SVNMainDirector::StreamOut
	(
	std::ostream& output
	)
{
	// before BuildWindow()

	if (HasPath())
		{
		output << itsPath;
		}
	else
		{
		JString repoPath;
		const JBoolean hasRepo = GetRepoPath(&repoPath);
		assert( hasRepo );
		output << repoPath;
		}

	// after BuildWindow()

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	output << JI2B(itsRepoWidget != NULL);

	if (itsRepoWidget != NULL)
		{
		output << ' ';
		itsRepoWidget->WriteSetup(output);
		}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "svn_main_window_icon.xpm"
#include "svn_update.xpm"
#include "svn_add.xpm"
#include "svn_remove.xpm"
#include "svn_remove_force.xpm"
#include "svn_resolved.xpm"
#include "svn_commit.xpm"
#include "svn_commit_all.xpm"
#include "svn_revert.xpm"
#include "svn_revert_all.xpm"
#include <jx_folder_small.xpm>
#include "svn_info_log.xpm"
#include <jx_edit_undo.xpm>
#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>

void
SVNMainDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 500,300, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != NULL );

	itsToolBar =
		jnew JXToolBar(SVNGetPrefsManager(), kSVNMainToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
	assert( itsToolBar != NULL );

// end JXLayout

	const JBoolean isURL = JIsURL(itsPath);

	JString displayPath;
	if (isURL)
		{
		displayPath = itsPath;
		}
	else
		{
		displayPath = JConvertToHomeDirShortcut(itsPath);
		}
	UpdateWindowTitle(displayPath);
	window->SetMinSize(200, 200);
	window->SetWMClass(SVNGetWMClassInstance(), SVNGetMainWindowClass());

	JXImage* image = jnew JXImage(GetDisplay(), svn_main_window_icon);
	assert( image != NULL );
	window->SetIcon(image);
	ListenTo(window);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "SVNMainDirector");
	ListenTo(itsFileMenu);

	itsEditMenu = JXTEBase::StaticAppendEditMenu(menuBar, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse);

	const JSize count = itsEditMenu->GetItemCount();
	for (JIndex i=1; i<count; i++)
		{
		const JString* id;
		if (itsEditMenu->GetItemID(i, &id) && *id == kJXCopyAction)
			{
			itsEditMenu->InsertMenuItems(i+1, kEditMenuAddStr, "SVNMainDirector");
			break;
			}
		}

	itsActionsMenu = menuBar->AppendTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr, "SVNMainDirector");
	ListenTo(itsActionsMenu);

	itsActionsMenu->SetItemImage(kUpdateWorkingCopyCmd,        svn_update);
	itsActionsMenu->SetItemImage(kAddSelectedFilesCmd,         svn_add);
	itsActionsMenu->SetItemImage(kRemoveSelectedFilesCmd,      svn_remove);
	itsActionsMenu->SetItemImage(kForceRemoveSelectedFilesCmd, svn_remove_force);
	itsActionsMenu->SetItemImage(kResolveSelectedConflictsCmd, svn_resolved);
	itsActionsMenu->SetItemImage(kCommitSelectedChangesCmd,    svn_commit);
	itsActionsMenu->SetItemImage(kCommitAllChangesCmd,         svn_commit_all);
	itsActionsMenu->SetItemImage(kRevertSelectedChangesCmd,    svn_revert);
	itsActionsMenu->SetItemImage(kRevertAllChangesCmd,         svn_revert_all);
	itsActionsMenu->SetItemImage(kCreateDirectoryCmd,          jx_folder_small);

	itsInfoMenu = menuBar->AppendTextMenu(kInfoMenuTitleStr);
	itsInfoMenu->SetMenuItems(kInfoMenuStr, "SVNMainDirector");
	ListenTo(itsInfoMenu);

	itsInfoMenu->SetItemImage(kInfoLogSelectedFilesCmd, svn_info_log);

	JXWDMenu* wdMenu =
		jnew JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != NULL );
	menuBar->AppendMenu(wdMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "SVNMainDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "SVNMainDirector");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	// tab group

	itsTabGroup =
		jnew SVNTabGroup(itsToolBar->GetWidgetEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0, 100,100);
	assert( itsTabGroup != NULL );
	itsTabGroup->FitToEnclosure();
	ListenTo(itsTabGroup->GetCardEnclosure());

	// repository view

	JString repoPath = itsPath;
	if (isURL || JGetVCSRepositoryPath(itsPath, &repoPath))
		{
		JXContainer* card            = itsTabGroup->AppendTab(JGetString("RepoTab::SVNMainDirector"));
		JXScrollbarSet* scrollbarSet = BuildScrollbarSet(card);

		itsRepoWidget =
			jnew SVNRepoView(this, repoPath, 0, itsEditMenu,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
		assert( itsRepoWidget != NULL );

		itsTabList->Append(itsRepoWidget);
		}

	// svn status

	if (!isURL)
		{
		JXContainer* card            = itsTabGroup->AppendTab(JGetString("StatusTab::SVNMainDirector"));
		JXScrollbarSet* scrollbarSet = BuildScrollbarSet(card);

		itsStatusWidget =
			jnew SVNStatusList(this, itsEditMenu,
							  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kVElastic,
							  0,0, 100,100);
		assert( itsStatusWidget != NULL );

		itsTabList->Append(itsStatusWidget);
		itsStatusWidget->RefreshContent();

		itsTabGroup->ShowTab(card);
		}

	// if we opened a URL, disable all local functionality

	if (isURL)
		{
		itsPath.Clear();
		}

	// must be done after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsActionsMenu, kUpdateWorkingCopyCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsActionsMenu, kAddSelectedFilesCmd);
		itsToolBar->AppendButton(itsActionsMenu, kRemoveSelectedFilesCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsActionsMenu, kResolveSelectedConflictsCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsActionsMenu, kCommitSelectedChangesCmd);
		itsToolBar->AppendButton(itsActionsMenu, kCommitAllChangesCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsActionsMenu, kRevertSelectedChangesCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsInfoMenu, kInfoLogSelectedFilesCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsHelpMenu, kTOCCmd);
		itsToolBar->AppendButton(itsHelpMenu, kThisWindowCmd);
		}
}

/******************************************************************************
 BuildScrollbarSet (private)

 ******************************************************************************/

JXScrollbarSet*
SVNMainDirector::BuildScrollbarSet
	(
	JXContainer* widget
	)
{
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(widget, JXWidget::kHElastic,JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure();
	return scrollbarSet;
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
SVNMainDirector::UpdateWindowTitle
	(
	const JCharacter* path
	)
{
	const JCharacter* map[] =
		{
		"path", path
		};
	const JString title = JGetString("WindowTitleID::SVNMainDirector", map, sizeof(map));
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetPath

 ******************************************************************************/

const JString&
SVNMainDirector::GetPath()
	const
{
	assert( !itsPath.IsEmpty() );
	return itsPath;
}

/******************************************************************************
 GetRepoPath

 ******************************************************************************/

JBoolean
SVNMainDirector::GetRepoPath
	(
	JString* path
	)
	const
{
	if (itsRepoWidget != NULL)
		{
		*path = (itsRepoWidget->GetRepoTree())->GetRepoPath();
		JStripTrailingDirSeparator(path);
		return kJTrue;
		}
	else
		{
		path->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 RegisterActionProcess

 ******************************************************************************/

void
SVNMainDirector::RegisterActionProcess
	(
	SVNTabBase*		tab,
	JProcess*		p,
	const JBoolean	refreshRepo,
	const JBoolean	refreshStatus,
	const JBoolean	reload
	)
{
	assert( itsActionProcess == NULL );

	itsActionProcess = p;
	ListenTo(itsActionProcess);

	itsRefreshRepoFlag     = refreshRepo;
	itsRefreshStatusFlag   = refreshStatus;
	itsReloadOpenFilesFlag = reload;

	JIndex i;
	if (itsTabList->Find(tab, &i))
		{
		itsTabGroup->SetBusyIndex(i);
		}
}

/******************************************************************************
 ScheduleStatusRefresh

 ******************************************************************************/

void
SVNMainDirector::ScheduleStatusRefresh()
{
	if (itsRefreshStatusTask == NULL)
		{
		itsRefreshStatusTask = jnew SVNRefreshStatusTask(this);
		assert( itsRefreshStatusTask != NULL );
		itsRefreshStatusTask->Start();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNMainDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionsMenu();
		}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleActionsMenu(selection->GetIndex());
		}

	else if (sender == itsInfoMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateInfoMenu();
		}
	else if (sender == itsInfoMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleInfoMenu(selection->GetIndex());
		}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateHelpMenu();
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsActionProcess && message.Is(JProcess::kFinished))
		{
		const JProcess::Finished* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != NULL );

		itsActionProcess       = NULL;
		JBoolean refreshRepo   = itsRefreshRepoFlag;	// changed when refresh status
		JBoolean refreshStatus = itsRefreshStatusFlag;	// changed when refresh status
		JBoolean reload        = itsReloadOpenFilesFlag;

		itsTabGroup->ClearBusyIndex();
		if (info->Successful())
			{
			if (refreshRepo)
				{
				RefreshRepo();
				}

			if (refreshStatus)
				{
				RefreshStatus();
				}

			if (reload)
				{
				(SVNGetApplication())->ReloadOpenFilesInIDE();
				}

			if (sender == itsCheckOutProcess)
				{
				itsCheckOutProcess = NULL;
				CreateStatusTab();
				}
			}
		else
			{
			CleanUpWorkingCopy();
			}
		}

	else if (sender == itsBrowseRepoDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );

		if (info->Successful())
			{
			const JString& url = itsBrowseRepoDialog->GetRepo();

			JBoolean wasOpen;
			SVNMainDirector* dir =
				(SVNGetWDManager())->OpenDirectory(url, &wasOpen);
			if (wasOpen)
				{
				dir->RefreshRepo();
				(dir->itsRepoWidget->GetRepoTree())->SavePathToOpen(url);
				dir->RefreshStatus();
				}
			}

		itsBrowseRepoDialog = NULL;
		}

	else if (sender == itsBrowseRepoRevisionDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );

		if (info->Successful())
			{
			BrowseRepo(itsBrowseRepoRevisionDialog->GetString());
			}

		itsBrowseRepoRevisionDialog = NULL;
		}

	else if (sender == itsCheckOutRepoDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );

		if (info->Successful())
			{
			CheckOut(itsCheckOutRepoDialog->GetRepo());
			}

		itsCheckOutRepoDialog = NULL;
		}

	else if (message.Is(JXCardFile::kCardRemoved))
		{
		const JXCardFile::CardRemoved* info =
			dynamic_cast<const JXCardFile::CardRemoved*>(&message);
		assert( info != NULL );

		JIndex i;
		if (info->GetCardIndex(&i))
			{
			itsTabList->RemoveElement(i);
			}
		}

	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
		{
		ScheduleStatusRefresh();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
SVNMainDirector::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsActionProcess)
		{
		itsActionProcess   = NULL;
		itsCheckOutProcess = NULL;
		itsTabGroup->ClearBusyIndex();
		CleanUpWorkingCopy();
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
SVNMainDirector::UpdateFileMenu()
{
	itsFileMenu->EnableItem(kOpenDirectoryCmd);
	itsFileMenu->EnableItem(kBrowseRepoCmd);
	itsFileMenu->EnableItem(kCheckOutRepoCmd);
	itsFileMenu->EnableItem(kCloseCmd);
	itsFileMenu->EnableItem(kQuitCmd);

	if (itsRepoWidget != NULL && itsStatusWidget == NULL)
		{
		itsFileMenu->EnableItem(kCheckOutCurrentRepoCmd);
		}

	JIndex i;
	if (itsTabGroup->GetCurrentTabIndex(&i))
		{
		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
		(itsTabList->GetElement(i))->GetSelectedFiles(&list);

		if (!list.IsEmpty() && itsStatusWidget != NULL)
			{
			itsFileMenu->EnableItem(kOpenFilesCmd);
			itsFileMenu->EnableItem(kShowFilesCmd);
			}

		if ((itsTabList->GetElement(i))->CanCheckOutSelection())
			{
			itsFileMenu->EnableItem(kCheckOutSelectionCmd);
			}
		}
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
SVNMainDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenDirectoryCmd)
		{
		SVNMainDirector* dir;
		(SVNGetWDManager())->NewBrowser(&dir);
		}
	else if (index == kBrowseRepoCmd)
		{
		assert( itsBrowseRepoDialog == NULL );

		itsBrowseRepoDialog =
			jnew SVNGetRepoDialog(
				JXGetPersistentWindowOwner(), JGetString("BrowseRepoWindowTitle::SVNMainDirector"));
		assert( itsBrowseRepoDialog != NULL );
		ListenTo(itsBrowseRepoDialog);
		itsBrowseRepoDialog->BeginDialog();
		}

	else if (index == kCheckOutRepoCmd)
		{
		assert( itsCheckOutRepoDialog == NULL );

		itsCheckOutRepoDialog =
			jnew SVNGetRepoDialog(
				JXGetPersistentWindowOwner(), JGetString("CheckOutRepoWindowTitle::SVNMainDirector"));
		assert( itsCheckOutRepoDialog != NULL );
		ListenTo(itsCheckOutRepoDialog);
		itsCheckOutRepoDialog->BeginDialog();
		}
	else if (index == kCheckOutCurrentRepoCmd)
		{
		CheckOut();
		}
	else if (index == kCheckOutSelectionCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			SVNTabBase* tab = itsTabList->GetElement(i);
			tab->CheckOutSelection();
			}
		}

	else if (index == kOpenFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			SVNTabBase* tab = itsTabList->GetElement(i);
			tab->OpenFiles();
			}
		}
	else if (index == kShowFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			SVNTabBase* tab = itsTabList->GetElement(i);
			tab->ShowFiles();
			}
		}

	else if (index == kCloseCmd)
		{
		GetWindow()->Close();
		}
	else if (index == kQuitCmd)
		{
		SVNGetApplication()->Quit();
		}
}

/******************************************************************************
 CheckOut (static)

 ******************************************************************************/

void
SVNMainDirector::CheckOut
	(
	const JCharacter* url
	)
{
	SVNMainDirector* dir;
	if (!(SVNGetWDManager())->GetBrowserForExactURL(url, &dir))
		{
		dir = jnew SVNMainDirector(JXGetApplication(), url);
		assert( dir != NULL );
		dir->Activate();
		}
	dir->CheckOut();
}

/******************************************************************************
 CheckOut (private)

 ******************************************************************************/

void
SVNMainDirector::CheckOut()
{
	if (itsStatusWidget != NULL || !OKToStartActionProcess())
		{
		return;
		}

	JString repoPath;
	if (!GetRepoPath(&repoPath))
		{
		return;
		}

	JString path, name;
	JSplitPathAndName(repoPath, &path, &name);

	if (!(JXGetChooseSaveFile())->SaveFile(
			JGetString("CheckOutDirectoryPrompt::SVNMainDirector"), NULL, name, &path))
		{
		return;
		}

	if (JFileExists(path))
		{
		JRemoveFile(path);
		}

	JString repoCmd = "svn co ";
	repoCmd        += JPrepArgForExec(repoPath);
	repoCmd        += " ";
	repoCmd        += JPrepArgForExec(path);
	Execute("CheckOutTab::SVNMainDirector", repoCmd, kJFalse, kJTrue, kJFalse);

	itsPath            = path;
	itsCheckOutProcess = itsActionProcess;

	path = JConvertToHomeDirShortcut(path);
	UpdateWindowTitle(path);
}

/******************************************************************************
 CreateStatusTab (private)

 ******************************************************************************/

void
SVNMainDirector::CreateStatusTab()
{
	assert( itsStatusWidget == NULL );

	JIndex index;
	const JBoolean found = itsTabList->Find(itsRepoWidget, &index);
	assert( found );
	index++;

	JXContainer* card            = itsTabGroup->InsertTab(index, JGetString("StatusTab::SVNMainDirector"));
	JXScrollbarSet* scrollbarSet = BuildScrollbarSet(card);

	itsStatusWidget =
		jnew SVNStatusList(this, itsEditMenu,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic,
						  0,0, 100,100);
	assert( itsStatusWidget != NULL );

	itsTabList->InsertAtIndex(index, itsStatusWidget);
	itsStatusWidget->RefreshContent();
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
SVNMainDirector::UpdateActionsMenu()
{
	JIndex i;
	const JBoolean hasTab = itsTabGroup->GetCurrentTabIndex(&i);
	if (hasTab && itsTabGroup->TabCanClose(i))
		{
		itsActionsMenu->EnableItem(kCloseTabCmd);
		}

	if (itsActionProcess == NULL)
		{
		if (HasPath())
			{
			itsActionsMenu->EnableItem(kUpdateWorkingCopyCmd);
			itsActionsMenu->EnableItem(kCleanUpWorkingCopyCmd);
			itsActionsMenu->EnableItem(kCommitAllChangesCmd);
			itsActionsMenu->EnableItem(kRevertAllChangesCmd);
			}

		if (hasTab)
			{
			(itsTabList->GetElement(i))->UpdateActionsMenu(itsActionsMenu);
			}
		}
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
SVNMainDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kRefreshCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->RefreshContent();
			}
		}
	else if (index == kCloseTabCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i) && itsTabGroup->TabCanClose(i))
			{
			itsTabGroup->DeleteTab(i);
			}
		}

	else if (index == kUpdateWorkingCopyCmd)
		{
		UpdateWorkingCopy();
		}
	else if (index == kCleanUpWorkingCopyCmd)
		{
		CleanUpWorkingCopy();
		}

	else if (index == kAddSelectedFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->ScheduleForAdd();
			}
		}
	else if (index == kRemoveSelectedFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->ScheduleForRemove();
			}
		}
	else if (index == kForceRemoveSelectedFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->ForceScheduleForRemove();
			}
		}

	else if (index == kResolveSelectedConflictsCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->Resolved();
			}
		}

	else if (index == kCommitSelectedChangesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->Commit();
			}
		}
	else if (index == kCommitAllChangesCmd)
		{
		CommitAll();
		}

	else if (index == kRevertSelectedChangesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->Revert();
			}
		}
	else if (index == kRevertAllChangesCmd)
		{
		RevertAll();
		}

	else if (index == kCreateDirectoryCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->CreateDirectory();
			}
		}
	else if (index == kDuplicateSelectedItemCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->DuplicateItem();
			}
		}

	else if (index == kCreatePropertyCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->CreateProperty();
			}
		}
	else if (index == kRemoveSelectedPropertiesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i) &&
			(JGetUserNotification())->AskUserNo(JGetString("WarnRemoveProperties::SVNMainDirector")))
			{
			(itsTabList->GetElement(i))->SchedulePropertiesForRemove();
			}
		}
	else if (index == kIgnoreSelectionCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			(itsTabList->GetElement(i))->Ignore();
			}
		}
}

/******************************************************************************
 RefreshRepo

 ******************************************************************************/

void
SVNMainDirector::RefreshRepo()
{
	if (itsRepoWidget != NULL)
		{
		itsRepoWidget->RefreshContent();
		}
}

/******************************************************************************
 BrowseRepo

 ******************************************************************************/

void
SVNMainDirector::BrowseRepo
	(
	const JCharacter* rev
	)
{
	JString repoPath;
	const JBoolean hasRepo = GetRepoPath(&repoPath);
	assert( hasRepo );

	const JCharacter* map[] =
		{
		"rev", rev
		};
	const JString title = JGetString("RepoRevTab::SVNMainDirector", map, sizeof(map));

	JXContainer* card            = itsTabGroup->AppendTab(title, kJTrue);
	JXScrollbarSet* scrollbarSet = BuildScrollbarSet(card);

	SVNRepoView* widget =
		jnew SVNRepoView(this, repoPath, rev, itsEditMenu,
						scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0, 100,100);
	assert( widget != NULL );

	itsTabGroup->ShowTab(card);
	itsTabList->Append(widget);

	widget->RefreshContent();
}

/******************************************************************************
 RefreshStatus

 ******************************************************************************/

void
SVNMainDirector::RefreshStatus()
{
	jdelete itsRefreshStatusTask;
	itsRefreshStatusTask = NULL;

	if (itsStatusWidget != NULL)
		{
		itsStatusWidget->RefreshContent();
		}
}

/******************************************************************************
 UpdateWorkingCopy

 ******************************************************************************/

void
SVNMainDirector::UpdateWorkingCopy()
{
	if (!HasPath())
		{
		return;
		}

	JXContainer* card            = itsTabGroup->AppendTab(JGetString("UpdateTab::SVNMainDirector"), kJTrue);
	JXScrollbarSet* scrollbarSet = BuildScrollbarSet(card);

	SVNUpdateList* updateWidget =
		jnew SVNUpdateList(this, itsEditMenu,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic,
						  0,0, 100,100);
	assert( updateWidget != NULL );

	itsTabGroup->ShowTab(card);
	itsTabList->Append(updateWidget);

	updateWidget->RefreshContent();
}

/******************************************************************************
 CleanUpWorkingCopy (private)

 ******************************************************************************/

void
SVNMainDirector::CleanUpWorkingCopy()
{
	if (HasPath())
		{
		Execute("CleanUpTab::SVNMainDirector", "svn cleanup", kJFalse, kJTrue, kJTrue);
		}
}

/******************************************************************************
 Commit

 ******************************************************************************/

void
SVNMainDirector::Commit
	(
	const JCharacter* cmd
	)
{
	if (HasPath())
		{
		Execute("CommitTab::SVNMainDirector", cmd, kJTrue, kJTrue, kJFalse);
		}
}

/******************************************************************************
 CommitAll (private)

 ******************************************************************************/

void
SVNMainDirector::CommitAll()
{
	if (HasPath())
		{
		Execute("CommitAllTab::SVNMainDirector", "svn commit", kJTrue, kJTrue, kJFalse);
		}
}

/******************************************************************************
 RevertAll (private)

 ******************************************************************************/

void
SVNMainDirector::RevertAll()
{
	if (HasPath() &&
		(JGetUserNotification())->AskUserNo(JGetString("WarnRevertAll::SVNMainDirector")))
		{
		Execute("RevertAllTab::SVNMainDirector", "svn revert -R .", kJFalse, kJTrue, kJTrue);
		}
}

/******************************************************************************
 Execute

 ******************************************************************************/

void
SVNMainDirector::Execute
	(
	const JCharacter*	tabStringID,
	const JCharacter*	cmd,
	const JBoolean		refreshRepo,
	const JBoolean		refreshStatus,
	const JBoolean		reloadOpenFiles
	)
{
	JXContainer* card            = itsTabGroup->AppendTab(JGetString(tabStringID), kJTrue);
	JXScrollbarSet* scrollbarSet = BuildScrollbarSet(card);

	SVNCommandLog* widget =
		jnew SVNCommandLog(this, itsEditMenu, cmd, refreshRepo, refreshStatus, reloadOpenFiles,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic,
						  0,0, 100,100);
	assert( widget != NULL );

	itsTabGroup->ShowTab(card);
	itsTabList->Append(widget);

	widget->RefreshContent();
}

/******************************************************************************
 UpdateInfoMenu (private)

 ******************************************************************************/

void
SVNMainDirector::UpdateInfoMenu()
{
	itsInfoMenu->EnableItem(kBrowseRepoRevisionCmd);

	JIndex i;
	if (itsTabGroup->GetCurrentTabIndex(&i))
		{
		(itsTabList->GetElement(i))->UpdateInfoMenu(itsInfoMenu);
		}
}

/******************************************************************************
 HandleInfoMenu (private)

 ******************************************************************************/

void
SVNMainDirector::HandleInfoMenu
	(
	const JIndex index
	)
{
	if (index == kInfoLogSelectedFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			ShowInfoLog(itsTabList->GetElement(i));
			}
		}
	else if (index == kPropSelectedFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			ShowProperties(itsTabList->GetElement(i));
			}
		}

	else if (index == kDiffEditedSelectedFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			SVNTabBase* tab = itsTabList->GetElement(i);
			JString rev;
			if (tab->GetBaseRevision(&rev))
				{
				tab->CompareEdited(rev);
				}
			}
		}
	else if (index == kDiffCurrentSelectedFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			SVNTabBase* tab = itsTabList->GetElement(i);
			JString rev;
			tab->GetBaseRevision(&rev);
			tab->CompareCurrent(rev);
			}
		}
	else if (index == kDiffPrevSelectedFilesCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			SVNTabBase* tab = itsTabList->GetElement(i);
			JString rev;
			tab->GetBaseRevision(&rev);
			tab->ComparePrev(rev);
			}
		}

	else if (index == kBrowseRepoRevisionCmd)
		{
		assert( itsBrowseRepoRevisionDialog == NULL );

		itsBrowseRepoRevisionDialog =
			jnew JXGetStringDialog(
				this, JGetString("BrowseRepoRevWindowTitle::SVNMainDirector"),
				JGetString("BrowseRepoRevPrompt::SVNMainDirector"), "");
		assert( itsBrowseRepoRevisionDialog != NULL );
		ListenTo(itsBrowseRepoRevisionDialog);
		itsBrowseRepoRevisionDialog->BeginDialog();
		}
	else if (index == kBrowseSelectedRepoRevisionCmd)
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			SVNTabBase* tab = itsTabList->GetElement(i);
			JString rev;
			if (tab->GetBaseRevision(&rev))
				{
				BrowseRepo(rev);
				}
			}
		}
}

/******************************************************************************
 ShowInfoLog

 ******************************************************************************/

void
SVNMainDirector::ShowInfoLog
	(
	SVNTabBase* tab
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	tab->GetSelectedFiles(&list, kJTrue);

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		ShowInfoLog(*(list.GetElement(i)));
		}
}

/******************************************************************************
 ShowInfoLog

 ******************************************************************************/

void
SVNMainDirector::ShowInfoLog
	(
	const JCharacter* origFullName,
	const JCharacter* rev
	)
{
	JString fullName = origFullName;
	JStripTrailingDirSeparator(&fullName);

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JString tabTitle       = JGetString("LogInfoTab::SVNMainDirector") + name;
	JXContainer* card            = itsTabGroup->AppendTab(tabTitle, kJTrue);
	JXScrollbarSet* scrollbarSet = BuildScrollbarSet(card);

	SVNInfoLog* widget =
		jnew SVNInfoLog(this, itsEditMenu, fullName, rev,
					   scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					   JXWidget::kHElastic, JXWidget::kVElastic,
					   0,0, 100,100);
	assert( widget != NULL );

	itsTabGroup->ShowTab(card);
	itsTabList->Append(widget);

	widget->RefreshContent();
}

/******************************************************************************
 ShowProperties

 ******************************************************************************/

void
SVNMainDirector::ShowProperties
	(
	SVNTabBase* tab
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	tab->GetSelectedFiles(&list, kJTrue);

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		ShowProperties(*(list.GetElement(i)));
		}
}

/******************************************************************************
 ShowProperties

 ******************************************************************************/

void
SVNMainDirector::ShowProperties
	(
	const JCharacter* origFullName
	)
{
	JString fullName = origFullName;
	JStripTrailingDirSeparator(&fullName);

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JString tabTitle       = JGetString("PropTab::SVNMainDirector") + name;
	JXContainer* card            = itsTabGroup->AppendTab(tabTitle, kJTrue);
	JXScrollbarSet* scrollbarSet = BuildScrollbarSet(card);

	SVNPropertiesList* widget =
		jnew SVNPropertiesList(this, itsEditMenu, fullName,
							  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kVElastic,
							  0,0, 100,100);
	assert( widget != NULL );

	itsTabGroup->ShowTab(card);
	itsTabList->Append(widget);

	widget->RefreshContent();
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
SVNMainDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
SVNMainDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kIntegrationPrefsCmd)
		{
		(SVNGetPrefsManager())->EditPrefs();
		}
	else if (index == kEditToolBarCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kEditBindingsCmd)
		{
		(JXFSBindingManager::Instance())->EditBindings();
		}
	else if (index == kWebBrowserCmd)
		{
		(JXGetWebBrowser())->EditPrefs();
		}
	else if (index == kEditMacWinPrefsCmd)
		{
		JXMacWinPrefsDialog::EditPrefs();
		}

	else if (index == kSaveWindSizeCmd)
		{
		(SVNGetPrefsManager())->SaveWindowSize(kSVNMainDirectorWindSizeID, GetWindow());
		}
}

/******************************************************************************
 UpdateHelpMenu (private)

 ******************************************************************************/

void
SVNMainDirector::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
SVNMainDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		SVNGetApplication()->DisplayAbout();
		}

	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection("SVNOverviewHelp");
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection("SVNMainHelp");
		}

	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowChangeLog();
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowCredits();
		}
}
