/******************************************************************************
 SyGFileTreeTable.cpp

	BASE CLASS = JXNamedTreeListWidget

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGFileTreeTable.h"
#include "SyGHeaderWidget.h"
#include "SyGFileTreeList.h"
#include "SyGFileTree.h"
#include "SyGTreeDir.h"
#include "SyGTreeSet.h"
#include "SyGTrashButton.h"
#include "SyGPrefsMgr.h"
#include "SyGViewManPageDialog.h"
#include "SyGFindFileDialog.h"
#include "SyGChooseSaveFile.h"
#include "SyGBeginEditingTask.h"
#include "SyGCopyProcess.h"
#include "SyGDuplicateProcess.h"
#include "SyGGlobals.h"
#include "SyGActionDefs.h"

#include <JXFSBindingManager.h>
#include <JXFSDirMenu.h>
#include <JXFSRunCommandDialog.h>

#include <JXApplication.h>
#include <JXWindowDirector.h>
#include <JXCloseDirectorTask.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWindowIcon.h>
#include <JXColormap.h>
#include <JXDragPainter.h>
#include <JXDNDManager.h>
#include <JXSelectionManager.h>
#include <JXFileSelection.h>
#include <JXTextSelection.h>
#include <JXInputField.h>
#include <JXMenuBar.h>
#include <JXSaveFileInput.h>
#include <JXTextMenu.h>
#include <JXFSDirMenu.h>
#include <JXRadioGroupDialog.h>
#include <JXGetStringDialog.h>
#include <JXTimerTask.h>
#include <JXToolBar.h>
#include <JXImage.h>
#include <jXUtil.h>
#include <X11/keysym.h>

#include <JFontManager.h>
#include <JTableSelection.h>
#include <JSimpleProcess.h>
#include <JDirInfo.h>
#include <jMountUtil.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jMouseUtil.h>
#include <jVCSUtil.h>
#include <jTime.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

#include <jx_file_new.xpm>
#include <jx_file_open.xpm>
#include <jx_folder_small.xpm>
#include <jx_find.xpm>
#include "mini_term.xpm"
#include "man.xpm"
#include "home.xpm"
#include "filter.xpm"
#include "hidden.xpm"
#include "user.xpm"
#include "group.xpm"
#include "size.xpm"
#include "mode.xpm"
#include "month.xpm"

const Time kDirUpdateInterval  = 10;	// milliseconds
const JCharacter kPermTestChar = 'w';

static const JCharacter* kDNDClassID = "SyGFileTreeTable";

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    New folder                  %k Meta-N           %i" kSGNewFolderAction
	"  | New text file               %k Meta-Shift-N     %i" kSGNewTextFileAction
	"  | Open                        %k Meta-O           %i" kSGOpenAction
	"  | Open ... and close this     %k Ctrl-O           %i" kSGOpenAndCloseAction
	"  | Open ... and iconify this   %k Ctrl-Shift-O     %i" kSGOpenAndIconifyAction
	"  | Alternate open              %k Meta-Shift-O     %i" kSGAlternateOpenAction
	"  | Run command on selection... %k Middle-dbl-click %i" kSGRunOnFileAction
	"%l| Find...                     %k Meta-F           %i" kSGFindAction
	"  | Open recent"
	"%l| Rename                      %k Return / F2      %i" kSGRenameAction
	"  | Convert to file             %k Meta-Shift-F     %i" kSGConvertToFile
	"  | Convert to program          %k Meta-Shift-P     %i" kSGConvertToProgram
	"%l| Duplicate                   %k Meta-D           %i" kSGDuplicateAction
	"  | Make alias                                      %i" kSGMakeAliasAction
	"  | Find original                                   %i" kSGFindOriginalAction
	"%l| Mount                                           %i" kSGMountAction
	"  | Erase                                           %i" kSGEraseAction
	"%l| Home window                 %k Meta-H           %i" kSGOpenHomeWindowAction
	"%l| Look up man page            %k Meta-I           %i" kSGViewManPageAction
	"  | Open terminal               %k Meta-T           %i" kSGOpenTerminalAction
	"  | Run command...              %k Meta-R           %i" kSGRunCommandAction
	"%l| Close                       %k Meta-W           %i" kJXCloseWindowAction
	"  | Quit                        %k Meta-Q           %i" kJXQuitAction;

enum
{
	kNewDirCmd = 1,
	kNewTextFileCmd,
	kOpenCmd,
	kOpenCloseCmd,
	kOpenIconifyCmd,
	kAltOpenCmd,
	kRunOnSelCmd,
	kFindCmd,
	kOpenRecentItemIndex,
	kRenameCmd,
	kConvertToFileCmd,
	kConvertToProgramCmd,
	kDuplicateCmd,
	kMakeAlias,
	kFindOriginalCmd,
	kToggleMountCmd,
	kEraseDiskCmd,
	kHomeWindowCmd,
	kViewManCmd,
	kOpenTermCmd,
	kRunCmdCmd,
	kCloseCmd,
	kQuitCmd
};

// Edit menu additions

static const JCharacter* kEditMenuAddStr =
	"Copy with path  %k Meta-Shift-C  %i" kSGCopyWithPathAction;

// View menu

static const JCharacter* kViewMenuTitleStr = "View";
static const JCharacter* kViewMenuStr =
	"    Wildcard filter     %b    %i" kSGShowFilterAction
	"  | Hidden files        %b    %i" kSGShowHiddenAction
	"%l| User name           %b    %i" kSGShowUserAction
	"  | Group name          %b    %i" kSGShowGroupAction
	"  | Size                %b    %i" kSGShowSizeAction
	"  | Permissions         %b    %i" kSGShowPermissionsAction
	"  | Modification date   %b    %i" kSGShowDateAction
	"%l| Show all attributes       %i" kSGShowAllAction
	"  | Hide all attributes       %i" kSGShowNoneAction
	"%l| Refresh             %k F5 %i" kSGRefreshAction;

enum
{
	kShowFilterCmd = 1,
	kShowHiddenCmd,
	kShowUserCmd,
	kShowGroupCmd,
	kShowSizeCmd,
	kShowModeCmd,
	kShowDateCmd,
	kShowAllAttrCmd,
	kHideAllAttrCmd,
	kRefreshCmd
};

static const JCharacter* kMountStr   = "Mount";
static const JCharacter* kUnmountStr = "Unmount";

// Git Branch menu

static const JCharacter* kGitBranchMenuTitleStr = "Branch";

// Shortcuts menu

static const JCharacter* kShortcutMenuTitleStr = "Shortcuts";
static const JCharacter* kShortcutMenuStr =
	"    Add Shortcut         %k Meta-plus  %i" kSGAddShortcutAction
	"  | Remove Shortcut      %k Meta-minus %i" kSGRemoveShortcutAction
	"  | Remove All Shortcuts               %i" kSGRemoveAllShortcutsAction
	"%l";

enum
{
	kAddShortcutCmd = 1,
	kRemoveShortcutCmd,
	kRemoveAllShortcutsCmd
};

const JIndex kShortcutCmdOffset = 3;

// Format disk

static const JCharacter* kFormatWindowTitle = "Erase disk";
static const JCharacter* kFormatPrompt      = "Choose the type of disk to create:";

static const JCharacter* kFormatName[] =
{
	"Linux", "DOS"
};

static const JCharacter* kFormatType[] =
{
	"ext2", "msdos"
};

const JSize kFormatCount = sizeof(kFormatName) / sizeof(JCharacter*);

// string ID's

static const JCharacter* kWarnDeleteSingleFileID   = "WarnDeleteSingleFile::SyGFileTreeTable";
static const JCharacter* kWarnDeleteSingleDirID    = "WarnDeleteSingleDir::SyGFileTreeTable";
static const JCharacter* kWarnDeleteMultipleFileID = "WarnDeleteMultipleFile::SyGFileTreeTable";
static const JCharacter* kWarnDeleteMultipleDirID  = "WarnDeleteMultipleDir::SyGFileTreeTable";
static const JCharacter* kCreateFolderErrorID      = "CreateFolderError::SyGFileTreeTable";
static const JCharacter* kCreateTextFileErrorID    = "CreateTextFileError::SyGFileTreeTable";
static const JCharacter* kCreateAliasErrorID       = "CreateAliasError::SyGFileTreeTable";
static const JCharacter* kWarnEraseDiskID          = "WarnEraseDisk::SyGFileTreeTable";
static const JCharacter* kRenameErrorID            = "RenameError::SyGFileTreeTable";
static const JCharacter* kNoBranchInfoID           = "NoBranchInfo::SyGFileTreeTable";
static const JCharacter* kCreateBranchID           = "CreateBranch::SyGFileTreeTable";
static const JCharacter* kCreateBranchTitleID      = "CreateBranchTitle::SyGFileTreeTable";
static const JCharacter* kCreateBranchPromptID     = "CreateBranchPrompt::SyGFileTreeTable";

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFileTreeTable::SyGFileTreeTable
	(
	JXMenuBar*			menuBar,
	SyGFileTree*		fileTree,
	SyGFileTreeList*	treeList,
	SyGTreeSet*			treeSet,
	SyGTrashButton*		trashButton,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXNamedTreeListWidget(treeList, scrollbarSet, enclosure,
						  hSizing,vSizing, x,y, w,h)
{
	itsFileTree					= fileTree;
	itsFileTreeList				= treeList;
	itsTreeSet					= treeSet;
	itsTrashButton				= trashButton;
	itsMenuBar					= menuBar;
	itsGitBranchMenu            = NULL;
	itsUpdateTask				= NULL;
	itsEditTask					= NULL;
	itsUpdateNode				= NULL;
	itsSortNode					= NULL;
	itsChooseDiskFormatDialog	= NULL;
	itsFormatProcess			= NULL;
	itsCreateBranchDialog       = NULL;
	itsIconWidget				= NULL;
	itsWindowIconType			= 0;

	for (JIndex i=0; i<5; i++)
		{
		itsVisibleCols[i] = kJFalse;
		}
	itsCurrentColType = kGFMName;

	ShouldHilightTextOnly(kJTrue);
	WantInput(kJTrue);

	const JSize rgb = JRound(kJMaxRGBValue * 0.95);
	if (!GetColormap()->AllocateStaticColor(rgb, rgb, rgb, &itsAltRowColor))
		{
		itsAltRowColor = GetColormap()->GetGray90Color();
		}

	itsPermCharWidth =
		GetFontManager()->GetCharWidth( JGetDefaultFontName(), kJDefaultFontSize,
										JFontStyle(), kPermTestChar);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "SyGFileTreeTable");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kNewDirCmd,      jx_folder_small);
	itsFileMenu->SetItemImage(kNewTextFileCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenCmd,        jx_file_open);
	itsFileMenu->SetItemImage(kFindCmd,        jx_find);
	itsFileMenu->SetItemImage(kHomeWindowCmd,  home);
	itsFileMenu->SetItemImage(kViewManCmd,     man);
	itsFileMenu->SetItemImage(kOpenTermCmd,    mini_term);

	JString recentDir;
	if (SyGGetRecentFileDirectory(&recentDir))
		{
		itsRecentFilesMenu =
			new JXFSDirMenu(recentDir, itsFileMenu, kOpenRecentItemIndex, menuBar);
		assert( itsRecentFilesMenu != NULL );
		itsRecentFilesMenu->ShouldShowPath(kJTrue);
		itsRecentFilesMenu->ShouldDereferenceLinks(kJTrue);
		itsRecentFilesMenu->ShouldDeleteBrokenLinks(kJTrue);
		ListenTo(itsRecentFilesMenu);

		JDirInfo* info;
		if (itsRecentFilesMenu->GetDirInfo(&info))
			{
			info->ChangeSort(JDirEntry::CompareModTimes, JOrderedSetT::kSortDescending);
			}
		}
	else
		{
		itsFileMenu->DisableItem(kOpenRecentItemIndex);
		}

	JXTEBase* te         = GetEditMenuHandler();
	itsEditMenu          = te->AppendEditMenu(menuBar);
	const JBoolean found = te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &itsCopyPathCmdIndex);
	assert( found );
	itsCopyPathCmdIndex++;
	itsEditMenu->InsertMenuItems(itsCopyPathCmdIndex, kEditMenuAddStr, "SyGFileTreeTable");
	ListenTo(itsEditMenu);

	itsViewMenu = menuBar->AppendTextMenu(kViewMenuTitleStr);
	itsViewMenu->SetMenuItems(kViewMenuStr, "SyGFileTreeTable");
	itsViewMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsViewMenu);

	itsViewMenu->SetItemImage(kShowFilterCmd, filter);
	itsViewMenu->SetItemImage(kShowHiddenCmd, hidden);
	itsViewMenu->SetItemImage(kShowUserCmd,   user);
	itsViewMenu->SetItemImage(kShowGroupCmd,  group);
	itsViewMenu->SetItemImage(kShowSizeCmd,   size);
	itsViewMenu->SetItemImage(kShowModeCmd,   mode);
	itsViewMenu->SetItemImage(kShowDateCmd,   month);

	itsShortcutMenu = menuBar->AppendTextMenu(kShortcutMenuTitleStr);
	assert (itsShortcutMenu != NULL);
	itsShortcutMenu->SetMenuItems(kShortcutMenuStr, "SyGFileTreeTable");
	itsShortcutMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsShortcutMenu);
	ListenTo(SyGGetApplication());

	UpdateShortcutMenu();

	// updating

	itsUpdateTask = new JXTimerTask(kDirUpdateInterval);
	assert( itsUpdateTask != NULL );
	ListenTo(itsUpdateTask);

	JXWindow* window = GetWindow();
//	if (!window->IsIconified())		// update window icon while iconified
//		{
		JXGetApplication()->InstallIdleTask(itsUpdateTask);
//		}
	ListenTo(window);

	ListenTo(itsFileTree);
	ListenTo(itsFileTree->GetRootDirInfo());

	// shortcuts

	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, kJTrue);
	window->InstallShortcut(this, kJUpArrow, modifiers);
	window->InstallShortcut(this, kJDownArrow, modifiers);

	modifiers.SetState(kJXShiftKeyIndex, kJTrue);
	window->InstallShortcut(this, kJUpArrow, modifiers);
	window->InstallShortcut(this, kJDownArrow, modifiers);

	// show file size by default

	TogglePref(4);
	UpdateInfo();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFileTreeTable::~SyGFileTreeTable()
{
	SyGSetDNDSource(NULL);		// avoid dangling pointers
	SyGSetDNDTarget(NULL);

	delete itsEditTask;
	delete itsUpdateTask;

	(GetColormap())->DeallocateColor(itsAltRowColor);
}

/******************************************************************************
 LoadToolBarDefaults

 ******************************************************************************/

void
SyGFileTreeTable::LoadToolBarDefaults
	(
	JXToolBar* toolBar
	)
{
	toolBar->AppendButton(itsFileMenu, kNewDirCmd);
	toolBar->AppendButton(itsFileMenu, kOpenCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsFileMenu, kFindCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsFileMenu, kHomeWindowCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsFileMenu, kViewManCmd);
	toolBar->AppendButton(itsFileMenu, kOpenTermCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsViewMenu, kShowFilterCmd);
	toolBar->AppendButton(itsViewMenu, kShowHiddenCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsViewMenu, kShowUserCmd);
	toolBar->AppendButton(itsViewMenu, kShowGroupCmd);
	toolBar->AppendButton(itsViewMenu, kShowSizeCmd);
	toolBar->AppendButton(itsViewMenu, kShowModeCmd);
	toolBar->AppendButton(itsViewMenu, kShowDateCmd);
	toolBar->NewGroup();
	(SyGGetApplication())->LoadToolBarDefaults(toolBar, itsShortcutMenu,
											   kShortcutCmdOffset+1);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if (cell.y % 2 == 0)
		{
		const JColorIndex origColor = p.GetPenColor();
		p.SetPenColor(itsAltRowColor);
		p.SetFilling(kJTrue);
		p.Rect(rect);
		p.SetFilling(kJFalse);
		p.SetPenColor(origColor);
		}

	if (JIndex(cell.x) == GetToggleOpenColIndex() ||
		JIndex(cell.x) == GetNodeColIndex())
		{
		JXNamedTreeListWidget::TableDrawCell(p,cell,rect);
		return;
		}

	const GFMColType type = GetFMColType(cell.x, GetNodeColIndex(), itsVisibleCols);
	const JString str     = GetCellString(cell);
	if (type == kGFMMode && !str.IsEmpty())
		{
		JSize fontSize;
		const JString& fontName = GetFont(&fontSize);
		p.SetFont(fontName, fontSize, JFontStyle());

		JRect pRect = rect;
		pRect.right = pRect.left + itsPermCharWidth;

		JCharacter s[2] = { '\0', '\0' };
		assert( str.GetLength() == 9 );
		for (JIndex i=1; i<=9; i++)
			{
			s[0] = str.GetCharacter(i);
			p.String(pRect, s, JPainter::kHAlignCenter, JPainter::kVAlignCenter);

			pRect.Shift(itsPermCharWidth, 0);
			}
		}
	else if (!str.IsEmpty())
		{
		JSize fontSize;
		const JString& fontName = GetFont(&fontSize);
		p.SetFont(fontName, fontSize, JFontStyle());

		JRect sRect = rect;
		const JCoordinate theBuffer = GetFMBufferWidth(itsVisibleCols, GetNodeColIndex(), cell.x);
		if (theBuffer > 0)
			{
			sRect.left += theBuffer;
			}
		else
			{
			sRect.right += theBuffer;
			}

		p.String(sRect, str,
				 type == kGFMSize ? JPainter::kHAlignRight : JPainter::kHAlignLeft,
				 JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 GetImage (virtual protected)

 ******************************************************************************/

JBoolean
SyGFileTreeTable::GetImage
	(
	const JIndex	index,
	const JXImage** image
	)
	const
{
	JBoolean selected = (GetTableSelection()).IsSelected(index, GetNodeColIndex());

	JIndex dndIndex;
	if (GetDNDTargetIndex(&dndIndex) && dndIndex == index)
		{
		selected = kJTrue;
		}

	const SyGFileTreeNode* node = itsFileTreeList->GetSyGNode(index);
	const JDirEntry* entry      = node->GetDirEntry();
	if (node->IsOpenable() && entry->IsWritable())
		{
		*image = SyGGetFolderSmallIcon(selected);
		}
	else if (node->IsOpenable())
		{
		*image = SyGGetReadOnlyFolderSmallIcon(selected);
		}
	else if (entry->IsDirectory())
		{
		*image = SyGGetLockedFolderSmallIcon(selected);
		}
	else if (!itsIgnoreExecPermFlag && entry->IsExecutable() && entry->IsFile())
		{
		*image = SyGGetExecSmallIcon(selected);
		}
	else if (entry->IsFile() || entry->IsBrokenLink())
		{
		*image = SyGGetFileSmallIcon(selected);
		}
	else
		{
		*image = SyGGetUnknownSmallIcon(selected);
		}
	return kJTrue;
}

/******************************************************************************
 GetCellString (private)

 ******************************************************************************/

JString
SyGFileTreeTable::GetCellString
	(
	const JPoint& cell
	)
	const
{
	const SyGFileTreeNode* entry = itsFileTreeList->GetSyGNode(cell.y);
	const JDirEntry* dirEntry    = entry->GetDirEntry();

	const GFMColType type = GetFMColType(cell.x, GetNodeColIndex(), itsVisibleCols);

	if (type == kGFMUID)
		{
		return dirEntry->GetUserName();
		}
	else if (type == kGFMGID)
		{
		return dirEntry->GetGroupName();
		}
	else if (type == kGFMSize && !dirEntry->IsLink())
		{
		if (dirEntry->IsDirectory())
			{
			return "-";
			}
		else
			{
			return JPrintFileSize(dirEntry->GetSize());
			}
		}
	else if (type == kGFMMode && !dirEntry->IsLink())
		{
		mode_t mode = dirEntry->GetMode();
		if (itsIgnoreExecPermFlag)
			{
			mode &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
			}
		return JGetPermissionsString(mode);
		}
	else if (type == kGFMDate)
		{
		return JConvertToTimeStamp(dirEntry->GetModTime());
		}
	else
		{
		return JString();
		}
}

/******************************************************************************
 SelectName

	This function forces an update of the list before searching for the
	given name.

	If parent == NULL, the root node is assumed.

 ******************************************************************************/

JBoolean
SyGFileTreeTable::SelectName
	(
	const JCharacter*		name,
	const SyGFileTreeNode*	parent,
	JPoint*					cell
	)
{
	if (!JStringEmpty(name))
		{
		itsFileTree->Update(kJTrue);

		if (parent == NULL)
			{
			parent = itsFileTree->GetSyGRoot();
			}

		const JNamedTreeNode* node;
		JIndex index;
		if (parent->FindNamedChild(name, &node) &&
			itsFileTreeList->FindNode(node, &index))
			{
			cell->Set(GetNodeColIndex(), index);
			(GetTableSelection()).SelectCell(*cell);
			ForceAdjustToTree();	// make sure col widths are correct
			UpdateScrollbars();		// make sure they are visible
			TableScrollToCell(*cell);
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 SelectName

	Drill down to find the name, following pathList.

 ******************************************************************************/

JBoolean
SyGFileTreeTable::SelectName
	(
	const JPtrArray<JString>&	pathList,
	const JCharacter*			name,
	JPoint*						cell,
	const JBoolean				clearSelection
	)
{
	itsFileTree->Update(kJTrue);

	JTableSelection& s      = GetTableSelection();
	SyGFileTreeNode* parent = itsFileTree->GetSyGRoot();

	const JSize count = pathList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString& name1 = *(pathList.NthElement(i));

		const JNamedTreeNode* node;
		JIndex index;
		if (parent->FindNamedChild(name1, &node) &&
			itsFileTreeList->FindNode(node, &index))
			{
			cell->Set(GetNodeColIndex(), index);

			if (i == count && JStringEmpty(name))
				{
				if (clearSelection)
					{
					s.ClearSelection();
					}
				SelectName(name1, parent, cell);
				}
			}
		else
			{
			if (clearSelection)
				{
				s.ClearSelection();
				}
			SelectName(name1, parent, cell);
			return kJFalse;
			}

		itsFileTreeList->Open(cell->y);
		parent = itsFileTreeList->GetSyGNode(cell->y);
		}

	if (clearSelection && !JStringEmpty(name))
		{
		s.ClearSelection();
		}
	return SelectName(name, parent, cell);
}

/******************************************************************************
 IsEditable (virtual)

	Derived classes should override this if there are some cells that
	cannot be edited.

 ******************************************************************************/

JBoolean
SyGFileTreeTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return JI2B( JIndex(cell.x) == GetNodeColIndex() );
}

/******************************************************************************
 HandleMouseHere (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::HandleMouseHere
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsEditTask != NULL && JMouseMoved(itsStartPt, pt))
		{
		itsEditTask->Perform();
		}
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
SyGFileTreeTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();
	ClearIncrementalSearchBuffer();

	itsStartPt = itsPrevPt = pt;
	itsLastClickCount      = 0;
	itsWaitingForDragFlag  = kJFalse;
	itsClearIfNotDNDFlag   = kJFalse;
	itsWaitingToEditFlag   = kJFalse;

	delete itsEditTask;
	itsEditTask	= NULL;

	JPoint cell;
	NodePart part;
	if (!GetNode(pt, &cell, &part))
		{
		if (!ScrollForWheel(button, modifiers))
			{
			StartDragRect(pt, button, modifiers);
			}
		}
	else if (part == kToggleColumn || button > kJXRightButton)
		{
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount,
											   buttonStates, modifiers);
		if (!IsDraggingToggle())
			{
			StartDragRect(pt, button, modifiers);
			}
		}
	else if (GetFMColType(cell.x, GetNodeColIndex(), itsVisibleCols) == kGFMMode &&
			 !(itsFileTreeList->GetDirEntry(cell.y))->IsLink())
		{
		if (!s.IsSelected(cell.y, GetNodeColIndex()))
			{
			s.ClearSelection();
			}

		const JRect cellRect  = GetCellRect(cell);
		const JIndex subIndex = 1 + (pt.x - cellRect.left)/itsPermCharWidth;

		JString permsStr;
		const JDirEntry* dirEntry = itsFileTreeList->GetDirEntry(cell.y);

		static const JCharacter userChar[] = { 'u', 'g', 'o' };
		permsStr.AppendCharacter(userChar [ (subIndex-1)/3 ]);

		const mode_t theMode  = dirEntry->GetMode() & 0x01FF;
		const mode_t posMask  = 1 << (9 - subIndex);
		const JBoolean usePos = JNegate( (theMode & posMask) == posMask );
		permsStr.AppendCharacter(usePos ? '+' : '-');

		static const JCharacter permChar[] = { 'x', 'r', 'w' };
		permsStr.AppendCharacter(permChar [ subIndex % 3 ]);

		s.SelectCell(cell.y, GetNodeColIndex());

		JTableSelectionIterator iter(&s);
		JPoint testCell;
		while (iter.Next(&testCell))
			{
			SyGFileTreeNode* node  = itsFileTreeList->GetSyGNode(testCell.y);
			const JString fullName = (node->GetDirEntry())->GetFullName();

			JString sysCmd = "chmod ";
			if (modifiers.meta())
				{
				sysCmd += "-R ";
				}
			sysCmd += permsStr;
			sysCmd += " ";
			sysCmd += JPrepArgForExec(fullName);
			SyGExec(sysCmd);
			node->Update(kJTrue);
			}
		}
	else if (part == kBeforeImage || part == kAfterText ||
			 (part == kOtherColumn && (GetCellString(cell)).IsEmpty()))
		{
		StartDragRect(pt, button, modifiers);
		}
	else if (button == kJXRightButton)
		{
		// open context menu
		}
	else if (modifiers.shift() && !modifiers.control() && !modifiers.meta())
		{
		s.InvertCell(cell.y, GetNodeColIndex());
		}
	else if (clickCount > 1)
		{
		itsWaitingForDragFlag = s.HasSelection();
		itsLastClickCount     = clickCount;		// save for HandleMouseUp()
		}
	else if (modifiers.control() && !modifiers.shift() && !modifiers.meta())
		{
		// after checking for double-click, since Ctrl inverts selection

		s.InvertCell(cell.y, GetNodeColIndex());
		}
	else
		{
		itsWaitingForDragFlag = kJTrue;
		itsWaitingToEditFlag  = JI2B(part == kInText);
		itsEditCell           = cell;

		if (s.IsSelected(cell.y, GetNodeColIndex()))
			{
			itsClearIfNotDNDFlag = kJTrue;
			}
		else
			{
			s.ClearSelection();
			s.SelectCell(cell.y, GetNodeColIndex());
			}
		}
}

/******************************************************************************
 StartDragRect (private)

 ******************************************************************************/

void
SyGFileTreeTable::StartDragRect
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton)
		{
		if (!modifiers.shift())
			{
			(GetTableSelection()).ClearSelection();
			}

		JPainter* p = CreateDragInsidePainter();
		p->Rect(JRect(pt, pt));
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPainter* p = NULL;
	if (GetDragPainter(&p))
		{
		if (pt != itsPrevPt)
			{
			if (!ScrollForDrag(pt))
				{
				p->Rect(JRect(itsStartPt, itsPrevPt));
				}
			p->Rect(JRect(itsStartPt, pt));
			itsPrevPt = pt;
			}
		}
	else if (itsWaitingForDragFlag)
		{
		assert( (GetTableSelection()).HasSelection() );

		if (JMouseMoved(itsStartPt, pt))
			{
			itsWaitingForDragFlag = kJFalse;
			itsClearIfNotDNDFlag  = kJFalse;
			itsWaitingToEditFlag  = kJFalse;

			JXFileSelection* data = new JXFileSelection(this, kDNDClassID);
			assert(data != NULL);

			ChooseDNDCursors();
			if (BeginDND(pt, buttonStates, modifiers, data))
				{
				SyGSetDNDSource(this);
				}
			}
		}
	else
		{
		JXNamedTreeListWidget::HandleMouseDrag(pt,buttonStates,modifiers);
		}
}

/******************************************************************************
 HandleMouseUp

 ******************************************************************************/

void
SyGFileTreeTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();

	JPainter* p = NULL;
	if (GetDragPainter(&p))
		{
		const JRect selRect = JRect(itsStartPt, itsPrevPt);
		p->Rect(selRect);		// erase rectangle

		// Pin in bounds so points below bounds don't change but points
		// above bounds do.

		JPoint startCell, endCell;
		const JBoolean startOK = GetCell(JPinInRect(itsStartPt, GetBounds()), &startCell);
		const JBoolean endOK = GetCell(JPinInRect(itsPrevPt, GetBounds()), &endCell);

		const JIndex start = JMin(startCell.y, endCell.y);
		const JIndex end   = JMax(startCell.y, endCell.y);

		if (startOK || endOK)
			{
			for (JIndex i=start; i<=end; i++)
				{
				JRect r = GetNodeRect(i);
				if (JIntersection(selRect, r, &r))
					{
					if (modifiers.shift())
						{
						s.InvertCell(i, GetNodeColIndex());
						}
					else
						{
						s.SelectCell(i, GetNodeColIndex());
						}
					}
				}
			}

		DeleteDragPainter();
		Refresh();
		}
	else if (itsWaitingForDragFlag && itsLastClickCount > 1)
		{
		OpenSelection(modifiers.meta(), JI2B(button == kJXMiddleButton),
					  JI2B(modifiers.control() && modifiers.shift()),
					  modifiers.control());
		}
	else if (itsWaitingToEditFlag)
		{
		JPoint cell;
		if (GetCell(itsStartPt, &cell))		// might update dir contents
			{
			s.ClearSelection();
			s.SelectCell(itsEditCell);

			assert( itsEditTask == NULL );
			itsEditTask = new SyGBeginEditingTask(this, itsEditCell);
			assert( itsEditTask != NULL );
			JXGetApplication()->InstallIdleTask(itsEditTask);
			}
		}
	else if (itsClearIfNotDNDFlag)
		{
		JPoint cell;
		if (GetCell(itsStartPt, &cell))		// might update dir contents
			{
			s.ClearSelection();
			s.SelectCell(cell.y, GetNodeColIndex());
			}
		}
	else
		{
		JXNamedTreeListWidget::HandleMouseUp(pt, button, buttonStates, modifiers);
		}

	if (itsSortNode != NULL)
		{
		itsSortNode->SortChildren();
		itsSortNode = NULL;
		}

	itsWaitingToEditFlag = kJFalse;
	itsClearIfNotDNDFlag = kJFalse;
}

/******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
SyGFileTreeTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (!((GetDisplay())->GetLatestButtonStates()).AllOff())
		{
		return;		// don't let selection change during DND
		}

	if (key == kJReturnKey || key == XK_F2)
		{
		ClearIncrementalSearchBuffer();

		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (IsEditing())
			{
			EndEditing();
			}
		else if (s.GetSingleSelectedCell(&cell))
			{
			BeginEditing(cell);
			}
		}

	else if ((key == kJUpArrow || key == kJDownArrow) && !IsEditing())
		{
		const JBoolean hasSelection = (GetTableSelection()).HasSelection();
		if (!hasSelection && key == kJUpArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), GetRowCount()));
			}
		else if (!hasSelection && key == kJDownArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), 1));
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			}
		ClearIncrementalSearchBuffer();
		}

	else if (key == kJForwardDeleteKey && (SyGGetPrefsMgr())->DelWillDelete())
		{
		ClearIncrementalSearchBuffer();
		DeleteSelected();
		}

	else
		{
		JXNamedTreeListWidget::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 DeleteSelected

 ******************************************************************************/

void
SyGFileTreeTable::DeleteSelected()
{
	if (!EndEditing() || !WarnForDelete())
		{
		return;
		}

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
		{
		SyGFileTreeNode* node = itsFileTreeList->GetSyGNode(cell.y);
		const JString file    = (node->GetDirEntry())->GetFullName();
		const JError err      = JRemoveVCS(file);
		if (err.OK())
			{
			delete node;
			}
		else
			{
			err.ReportIfError();
			}
		}
}

/******************************************************************************
 WarnForDelete (private)

	We check for JDirEntry::kDir because deleting a link doesn't delete
	the directory's contents.

 ******************************************************************************/

JBoolean
SyGFileTreeTable::WarnForDelete()
	const
{
	const JTableSelection& s = GetTableSelection();
	if (!s.HasSelection())
		{
		return kJFalse;
		}

	JString msg;
	JPoint cell;
	if (s.GetSingleSelectedCell(&cell))
		{
		const SyGFileTreeNode* node = itsFileTreeList->GetSyGNode(cell.y);
		const JCharacter* map[] =
			{
			"name", node->GetName()
			};
		msg = JGetString((node->GetDirEntry())->GetType() == JDirEntry::kDir ?
							kWarnDeleteSingleDirID :
							kWarnDeleteSingleFileID,
						 map, sizeof(map));
		}
	else
		{
		JBoolean hasDirs = kJFalse;
		JTableSelectionIterator iter(&s);
		while (iter.Next(&cell))
			{
			if ((itsFileTreeList->GetDirEntry(cell.y))->GetType() == JDirEntry::kDir)
				{
				hasDirs = kJTrue;
				break;
				}
			}

		const JString countStr(s.GetSelectedCellCount(), 0);
		const JCharacter* map[] =
			{
			"count", countStr
			};
		msg = JGetString(hasDirs ? kWarnDeleteMultipleDirID :
								   kWarnDeleteMultipleFileID,
						 map, sizeof(map));
		}

	return JGetUserNotification()->AskUserNo(msg);
}

/******************************************************************************
 HandleShortcut

 ******************************************************************************/

void
SyGFileTreeTable::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJUpArrow && modifiers.meta())
		{
		GoUp(modifiers.shift());
		}
	else if (key == kJDownArrow && modifiers.meta())
		{
		OpenSelection(modifiers.shift(), kJFalse, kJFalse, kJFalse);
		}
	else
		{
		JXNamedTreeListWidget::HandleShortcut(key, modifiers);
		}
}

/******************************************************************************
 GoUp

 ******************************************************************************/

void
SyGFileTreeTable::GoUp
	(
	const JBoolean sameWindow
	)
{
	ClearIncrementalSearchBuffer();

	if (sameWindow)
		{
		SyGFileTreeNode* root = itsFileTree->GetSyGRoot();
		const JString origDir = root->GetName();
		const JError err      = root->GoUp();
		if (err.OK())
			{
			JPoint cell;
			SelectName(origDir, NULL, &cell);
			}
		else
			{
			err.ReportIfError();
			}
		}
	else
		{
		const JString path = JCombinePathAndName(itsFileTree->GetDirectory(), "..");
		(SyGGetApplication())->OpenDirectory(path);
		}
}

/******************************************************************************
 GoTo

 ******************************************************************************/

void
SyGFileTreeTable::GoTo
	(
	const JString&	path,
	const JBoolean	sameWindow
	)
{
	ClearIncrementalSearchBuffer();

	if (sameWindow)
		{
		const JError err = (itsFileTree->GetSyGRoot())->GoTo(path);
		err.ReportIfError();
		}
	else
		{
		(SyGGetApplication())->OpenDirectory(path);
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
SyGFileTreeTable::WillAcceptDrop
	(
	const JArray<Atom>& typeList,
	Atom*				action,
	const Time			time,
	const JXWidget*		source
	)
{
	const JString dest = ((itsFileTree->GetSyGRoot())->GetDirEntry())->GetFullName();
	if (!EndEditing() || !JDirectoryWritable(dest))
		{
		return kJFalse;
		}

	const Atom urlXAtom = (GetSelectionManager())->GetURLXAtom();
	const Atom xdsXAtom = (GetDNDManager())->GetDNDDirectSave0XAtom();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		const Atom type = typeList.GetElement(i);
		if (type == urlXAtom || type == xdsXAtom)
			{
			if (SyGIsTrashDirectory(itsFileTree->GetDirectory()))
				{
				*action = (GetDNDManager())->GetDNDActionPrivateXAtom();
				}
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::HandleDNDEnter()
{
	ClearDNDTargetIndex();
	SyGSetDNDTarget(this);
}

/******************************************************************************
 HandleDNDHere (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget* source
	)
{
	ScrollForDrag(pt);

	JPoint cell;
	NodePart part;
	if (!GetNode(pt, &cell, &part) ||
		part == kToggleColumn || part == kBeforeImage || part == kAfterText)
		{
		ClearDNDTargetIndex();
		}
	else
		{
		SetDNDTargetIndex(GetNearestDirIndex(cell.y, kJTrue));
		}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::HandleDNDLeave()
{
	ClearDNDTargetIndex();
	SyGSetDNDTarget(NULL);
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>& typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	Atom realAction                = action;
	JXDNDManager* dndMgr           = GetDNDManager();
	const Atom dndName             = dndMgr->GetDNDSelectionName();
	JXSelectionManager* selManager = GetSelectionManager();

	JBoolean isXDS = JI2B(action == dndMgr->GetDNDActionDirectSaveXAtom());
	if (!isXDS)
		{
		const Atom xdsXAtom   = dndMgr->GetDNDDirectSave0XAtom();
		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i = 1; i <= typeCount; i++)
			{
			if (typeList.GetElement(i) == xdsXAtom)
				{
				isXDS = kJTrue;
				break;
				}
			}
		}

	JIndex dndIndex = 0;
	if (isXDS)
		{
		const Window dragWindow = dndMgr->GetDraggerWindow();

		Atom actualType;
		int actualFormat;
		unsigned long itemCount, remainingBytes;
		unsigned char* rawData = NULL;
		XGetWindowProperty(*(GetDisplay()), dragWindow,
						   dndMgr->GetDNDDirectSave0XAtom(),
						   0, LONG_MAX, True, selManager->GetMimePlainTextXAtom(),
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &rawData);

		if (actualType == selManager->GetMimePlainTextXAtom() &&
			actualFormat == 8 && itemCount > 0)
			{
			SyGFileTreeNode* node =
				GetDNDTargetIndex(&dndIndex) ?
				itsFileTreeList->GetSyGNode(dndIndex) :
				itsFileTree->GetSyGRoot();

			JString path = (node->GetDirEntry())->GetFullName();
			path         = JCombinePathAndName(path, (char*) rawData);
			JString url  = JFileNameToURL(path);
			XChangeProperty(*(GetDisplay()), dragWindow,
							dndMgr->GetDNDDirectSave0XAtom(),
							(GetSelectionManager())->GetMimePlainTextXAtom(), 8,
							PropModeReplace,
							(unsigned char*) url.GetCString(), url.GetLength());

			unsigned char* data = NULL;
			JSize dataLength;
			Atom returnType;
			JXSelectionManager::DeleteMethod delMethod;
			if (selManager->GetData(dndName, time, dndMgr->GetDNDDirectSave0XAtom(),
									&returnType, &data, &dataLength, &delMethod))
				{
				if (*data == 0x46)		// F: failure
					{
					XChangeProperty(*(GetDisplay()), dragWindow,
									dndMgr->GetDNDDirectSave0XAtom(),
									(GetSelectionManager())->GetMimePlainTextXAtom(), 8,
									PropModeReplace,
									(unsigned char*) "", 0);
					(JGetUserNotification())->ReportError(
						"Remote saving is not yet supported.");
					}
				selManager->DeleteData(&data, delMethod);
				}

			}
		}
	else if (!GetDNDTargetIndex(&dndIndex) &&
			 SyGIsTrashDirectory(itsFileTree->GetDirectory()))
		{
		SyGTrashButton::MoveFilesToTrash(time, source);
		}
	else if (GetTrueDropAction(&realAction))
		{
		unsigned char* data = NULL;
		JSize dataLength;
		Atom returnType;
		JXSelectionManager::DeleteMethod delMethod;
		if (selManager->GetData(dndName, time, selManager->GetURLXAtom(),
								&returnType, &data, &dataLength, &delMethod))
			{
			if (returnType == selManager->GetURLXAtom())
				{
				JPtrArray<JString>* fileNameList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
				assert( fileNameList != NULL );
				JPtrArray<JString> urlList(JPtrArrayT::kDeleteAll);
				JXUnpackFileNames((char*) data, dataLength, fileNameList, &urlList);

				SyGFileTreeTable* srcTable = NULL;
				SyGGetDNDSource(source, &srcTable);

				SyGFileTreeNode* destNode =
					GetDNDTargetIndex(&dndIndex) ?
					itsFileTreeList->GetSyGNode(dndIndex) :
					itsFileTree->GetSyGRoot();

				if (realAction == dndMgr->GetDNDActionLinkXAtom())
					{
					(GetTableSelection()).ClearSelection();

					const JSize count = fileNameList->GetElementCount();
					for (JIndex i=1; i<=count; i++)
						{
						MakeLinkToFile(*(fileNameList->NthElement(i)), destNode);
						}

					delete fileNameList;
					}
				else if (realAction == dndMgr->GetDNDActionMoveXAtom())
					{
					SyGCopyProcess::Move(srcTable, fileNameList, this, destNode);
					}
				else
					{
					SyGCopyProcess::Copy(srcTable, fileNameList, this, destNode);
					}

				JXReportUnreachableHosts(urlList);
				}

			selManager->DeleteData(&data, delMethod);
			}
		}

	ClearDNDTargetIndex();
	SyGSetDNDSource(NULL);
	SyGSetDNDTarget(NULL);

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 GetTrueDropAction (private)

 ******************************************************************************/

JBoolean
SyGFileTreeTable::GetTrueDropAction
	(
	Atom* action
	)
{
	JXDNDManager* dndMgr = GetDNDManager();
	if (*action == dndMgr->GetDNDActionAskXAtom())
		{
		JArray<Atom> actionList;
		JPtrArray<JString> descriptionList(JPtrArrayT::kDeleteAll);
		if (!dndMgr->GetAskActions(&actionList, &descriptionList))
			{
			*action = dndMgr->GetDNDActionMoveXAtom();
			}
		else if (!dndMgr->ChooseDropAction(actionList, descriptionList, action))
			{
			return kJFalse;
			}
		}
	else if (SyGIsTrashDirectory(itsFileTree->GetDirectory()))
		{
		// handle special case when dropping on folder inside trash

		*action = dndMgr->GetDNDActionMoveXAtom();
		}
	else if (*action == dndMgr->GetDNDActionPrivateXAtom())		// ask
		{
		JArray<Atom> actionList;
		JPtrArray<JString> descriptionList(JPtrArrayT::kDeleteAll);
		GetDNDAskActions(JXButtonStates(), JXKeyModifiers(GetDisplay()),
						 &actionList, &descriptionList);
		if (!dndMgr->ChooseDropAction(actionList, descriptionList, action))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
SyGFileTreeTable::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	if (strcmp(id, kDNDClassID) == 0)
		{
		assert( GetTableSelection().HasSelection() );

		JXFileSelection* fileData = dynamic_cast(JXFileSelection*, data);
		assert( fileData != NULL );

		JPtrArray<JString>* fileList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( fileList != NULL );

		JTableSelectionIterator iter(&(GetTableSelection()));
		JPoint cell;
		while (iter.Next(&cell))
			{
			const JDirEntry* entry = itsFileTreeList->GetDirEntry(cell.y);
			fileList->Append(entry->GetFullName());
			}

		fileData->SetData(fileList);
		}
	else
		{
		JXNamedTreeListWidget::GetSelectionData(data, id);
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly during the drag so the drop action can be
	changed based on the current target, buttons, and modifier keys.

 ******************************************************************************/

Atom
SyGFileTreeTable::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDAction(this, itsFileTree->GetDirectory(), target, modifiers);
}

/******************************************************************************
 GetDNDAction (static)

	This is used by SyGFolderDragSource.

 ******************************************************************************/

Atom
SyGFileTreeTable::GetDNDAction
	(
	const JXWidget*			source,
	const JCharacter*		sourcePath,
	const JXContainer*		target,
	const JXKeyModifiers&	modifiers
	)
{
	JBoolean moveDef = kJTrue;

	SyGFileTreeTable* sygTarget = NULL;
	if (SyGGetDNDTarget(target, &sygTarget))
		{
		JIndex dndIndex;
		SyGFileTreeNode* node =
			sygTarget->GetDNDTargetIndex(&dndIndex) ?
			sygTarget->itsFileTreeList->GetSyGNode(dndIndex) :
			sygTarget->itsFileTree->GetSyGRoot();

		if (!JIsSamePartition(sourcePath, (node->GetDirEntry())->GetFullName()))
			{
			moveDef = kJFalse;
			}
		}

	const JXMenu::Style style = JXMenu::GetDisplayStyle();
	const JBoolean toggleMoveToCopy = JI2B(
		((style == JXMenu::kMacintoshStyle && modifiers.meta()) ||
		 (style == JXMenu::kWindowsStyle   && modifiers.control())));

	if ((style == JXMenu::kMacintoshStyle && modifiers.control()) ||
		(style == JXMenu::kWindowsStyle   && modifiers.shift()))
		{
		return (source->GetDNDManager())->GetDNDActionAskXAtom();
		}
	else if ((style == JXMenu::kMacintoshStyle && modifiers.shift()) ||
			 (style == JXMenu::kWindowsStyle   && modifiers.meta()))
		{
		return (source->GetDNDManager())->GetDNDActionLinkXAtom();
		}
	else if (( toggleMoveToCopy &&  moveDef) ||
			 (!toggleMoveToCopy && !moveDef))
		{
		return (source->GetDNDManager())->GetDNDActionCopyXAtom();
		}
	else
		{
		return (source->GetDNDManager())->GetDNDActionMoveXAtom();
		}
}

/******************************************************************************
 GetDNDAskActions (virtual protected)

	This is called when the value returned by GetDropAction() changes to
	XdndActionAsk.  If GetDropAction() repeatedly returns XdndActionAsk,
	this function is not called again because it is assumed that the
	actions are the same within a single DND session.

	This function must place at least 2 elements in askActionList and
	askDescriptionList.

	The first element should be the default action.

 ******************************************************************************/

void
SyGFileTreeTable::GetDNDAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JArray<Atom>*			askActionList,
	JPtrArray<JString>*		askDescriptionList
	)
{
	SyGGetDNDAskActions(askActionList, askDescriptionList);
}

/******************************************************************************
 ChooseDNDCursors (private)

	Choose the appropriate set of cursors to use for DND.

 ******************************************************************************/

void
SyGFileTreeTable::ChooseDNDCursors()
{
	JBoolean hasDir = kJFalse, hasFile = kJFalse;

	JTreeList* treeList = GetTreeList();
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell) && JIndex(cell.x) == GetNodeColIndex())
		{
		if ((itsFileTreeList->GetDirEntry(cell.y))->IsDirectory())
			{
			hasDir = kJTrue;
			}
		else
			{
			hasFile = kJTrue;
			}

		if (hasDir && hasFile)
			{
			break;
			}
		}

	if (hasDir && hasFile)
		{
		itsDNDCursorType = kDNDMixedCursor;
		}
	else if (hasDir)
		{
		itsDNDCursorType = kDNDDirCursor;
		}
	else
		{
		itsDNDCursorType = kDNDFileCursor;
		}
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

	This is called when the target indicates whether or not it will accept
	the drop.  If !dropAccepted, the action is undefined.  If the drop target
	is not within the same application, target is NULL.

 ******************************************************************************/

void
SyGFileTreeTable::HandleDNDResponse
	(
	const JXContainer*	target,
	const JBoolean		dropAccepted,
	const Atom			action
	)
{
	JXDNDManager* dndMgr = GetDNDManager();
	JCursorIndex cursor;
	if (itsDNDCursorType == kDNDMixedCursor)
		{
		cursor = dndMgr->GetDNDFileAndDirectoryCursor(dropAccepted, action);
		}
	else if (itsDNDCursorType == kDNDDirCursor)
		{
		cursor = dndMgr->GetDNDDirectoryCursor(dropAccepted, action);
		}
	else
		{
		cursor = dndMgr->GetDNDFileCursor(dropAccepted, action);
		}
	DisplayCursor(cursor);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::Receive
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
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsRecentFilesMenu && message.Is(JXFSDirMenu::kFileSelected))
		{
		const JXFSDirMenu::FileSelected* info	= 
			dynamic_cast(const JXFSDirMenu::FileSelected*, &message);
		assert( info != NULL );

		const JDirEntry entry(info->GetFileName());
		const JString* link;
		if (entry.GetLinkName(&link))
			{
			SyGAddRecentFile(*link);
			}

		JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
		fileList.Append(info->GetFileName());
		JXFSBindingManager::Exec(fileList);
		}

	else if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateEditMenu();
			}
		}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast(const JXMenu::ItemSelected*, &message);
			assert( selection != NULL );
			HandleEditMenu(selection->GetIndex());
			}
		}

	else if (sender == itsGitBranchMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateGitBranchMenu();
		}
	else if (sender == itsGitBranchMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleGitBranchMenu(selection->GetIndex());
		}

	else if (sender == itsViewMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateViewMenu();
		}
	else if (sender == itsViewMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleViewMenu(selection->GetIndex());
		}

	else if (sender == itsShortcutMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateShortcutMenu();
		}
	else if (sender == itsShortcutMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleShortcutMenu(selection->GetIndex());
		}

	else if (sender == itsUpdateTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		UpdateDisplay();
		}

	else if (sender == itsChooseDiskFormatDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert(info != NULL);
		if (info->Successful())
			{
			const JIndex i   = itsChooseDiskFormatDialog->GetSelectedItem();
			const JError err = JFormatPartition(itsFileTree->GetDirectory(),
												kFormatType[i-1], &itsFormatProcess);
			if (err.OK())
				{
				ListenTo(itsFormatProcess);
				}
			else
				{
				err.ReportIfError();
				}
			}
		itsChooseDiskFormatDialog = NULL;
		}
	else if (sender == itsFormatProcess && message.Is(JProcess::kFinished))
		{
		JMount(itsFileTree->GetDirectory());
		delete itsFormatProcess;
		itsFormatProcess = NULL;
		}

	else if (sender == itsCreateBranchDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert(info != NULL);
		if (info->Successful())
			{
			JString cmd      = "git checkout -b ";
			cmd             += itsCreateBranchDialog->GetString();
			const JError err = JSimpleProcess::Create(itsFileTree->GetDirectory(), cmd, kJFalse);
			err.ReportIfError();
			}
		itsCreateBranchDialog = NULL;
		}

	else if (sender == itsIconWidget && message.Is(JXWindowIcon::kHandleEnter))
		{
		HandleDNDEnter();
		}
	else if (sender == itsIconWidget && message.Is(JXWindowIcon::kHandleLeave))
		{
		HandleDNDLeave();
		}
	else if (sender == itsIconWidget && message.Is(JXWindowIcon::kHandleDrop))
		{
		const JXWindowIcon::HandleDrop* info =
			dynamic_cast(const JXWindowIcon::HandleDrop*, &message);
		assert( info != NULL );
		HandleDNDDrop(JPoint(0,0), info->GetTypeList(), info->GetAction(),
					  info->GetTime(), info->GetSource());
		}

	else if (sender == SyGGetApplication() && message.Is(SyGApplication::kTrashNeedsUpdate))
		{
		if (SyGIsTrashDirectory(itsFileTree->GetDirectory()))
			{
			UpdateDisplay(kJTrue);
			}
		}
	else if (sender == SyGGetApplication() && message.Is(SyGApplication::kShortcutsChanged))
		{
		UpdateShortcutMenu();
		}

	else if (sender == SyGGetApplication() && message.Is(JFSFileTree::kDirectoryRenamed))
		{
		(itsFileTree->GetSyGRoot())->UpdatePath(message);
		}

	else
		{
		if (sender == GetWindow() && message.Is(JXWindow::kIconified))
			{
//			UpdateDisplay();
//			JXGetApplication()->RemoveIdleTask(itsUpdateTask);
			}
		else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
			{
			ClearIncrementalSearchBuffer();
//			UpdateDisplay();
//			JXGetApplication()->InstallIdleTask(itsUpdateTask);
			}

		else if (sender == itsFileTree && message.Is(JFSFileTree::kDirectoryRenamed))
			{
			(SyGGetApplication())->DirectoryRenamed(message);
			}
		else if (sender == itsFileTree->GetRootDirInfo() &&
				 message.Is(JDirInfo::kPathChanged))
			{
			UpdateInfo();
			}

		JXNamedTreeListWidget::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsIconWidget && message->Is(JXWindowIcon::kAcceptDrop))
		{
		JXWindowIcon::AcceptDrop* info =
			dynamic_cast(JXWindowIcon::AcceptDrop*, message);
		assert( info != NULL );
		info->ShouldAcceptDrop(WillAcceptDrop(
									info->GetTypeList(), info->GetActionPtr(),
									info->GetTime(), info->GetSource()));
		}
	else
		{
		JXNamedTreeListWidget::ReceiveWithFeedback(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsSortNode)
		{
		itsSortNode = NULL;
		}
	if (sender == itsUpdateNode)
		{
		itsUpdateNode = NULL;
		}

	JBroadcaster::ReceiveGoingAway(sender);
}

/******************************************************************************
 UpdateInfo (private)

 ******************************************************************************/

void
SyGFileTreeTable::UpdateInfo()
{
	JBoolean writable, isTop;
	JString device, fsType;

	itsIgnoreExecPermFlag = JI2B(
		JIsMounted((itsFileTree->GetRootDirInfo())->GetDirectory(),
				   &writable, &isTop, &device, &fsType) &&
		JStringCompare(fsType, "vfat", kJFalse) == 0);
	Refresh();

	SetWindowIcon();
}

/******************************************************************************
 UpdateDisplay

 ******************************************************************************/

void
SyGFileTreeTable::UpdateDisplay
	(
	const JBoolean force
	)
{
	JPainter* p = NULL;
	if (GetDragPainter(&p))
		{
		return;
		}

	if (!JFSFileTreeNode::CanHaveChildren(itsFileTree->GetDirectory()))
		{
		if ((SyGGetApplication())->GetWindowCount() == 1)
			{
			JString p;
			if (!JGetHomeDirectory(&p) || !JFSFileTreeNode::CanHaveChildren(p))
				{
				p = JGetRootDirectory();
				}
			(itsFileTree->GetSyGRoot())->GoTo(p);
			}
		else
			{
			JXCloseDirectorTask::Close((GetWindow())->GetDirector());
			}
		}
	else
		{
		if (force || itsUpdateNode == NULL)
			{
			itsUpdateNode = itsFileTree->GetSyGRoot();
			}

		const JBoolean updateMenus = JI2B(itsUpdateNode == itsFileTree->GetSyGRoot());

		StopListening(itsUpdateNode);
		itsFileTree->Update(force, &itsUpdateNode);
		ListenTo(itsUpdateNode);

		if (updateMenus)
			{
			UpdateMenus();
			}
		}

	// trash button

	itsTrashButton->UpdateDisplay();
}

/******************************************************************************
 UpdateMenus (private)

 ******************************************************************************/

void
SyGFileTreeTable::UpdateMenus()
{
	if (JGetVCSType(itsFileTree->GetDirectory()) == kJGitType)
		{
		if (itsGitBranchMenu == NULL)
			{
			itsGitBranchMenu = itsMenuBar->AppendTextMenu(kGitBranchMenuTitleStr);
			ListenTo(itsGitBranchMenu);
			}

		JIndex index;
		if (!itsMenuBar->FindMenu(itsGitBranchMenu, &index))
			{
			itsMenuBar->InsertMenuBefore(itsShortcutMenu, itsGitBranchMenu);
			UpdateGitBranchMenu();
			}
		}
	else if (itsGitBranchMenu != NULL)
		{
		itsMenuBar->RemoveMenu(itsGitBranchMenu);
		}
}

/******************************************************************************
 SetWindowIcon

 ******************************************************************************/

void
SyGFileTreeTable::SetWindowIcon()
{
	JXPM plain(NULL), selected(NULL);
	const JIndex type = SyGGetMountPointLargeIcon(itsFileTree->GetDirectory(),
												  itsFileTreeList, &plain, &selected);
	if (type != itsWindowIconType || itsIconWidget == NULL)
		{
		itsWindowIconType = type;

		JXDisplay* display = GetDisplay();
		JXColormap* cmap   = GetColormap();
/*		JXImage* icon1 = new JXImage(display, cmap, plain);
		assert(icon1 != NULL);
		JXImage* icon2 = new JXImage(display, cmap, selected);
		assert(icon2 != NULL);

		itsIconWidget = (GetWindow())->SetIcon(icon1, icon2);
		ListenTo(itsIconWidget);
*/
		JXImage* icon3 = new JXImage(display, cmap, plain);
		assert(icon3 != NULL);

		(GetWindow())->SetIcon(icon3);
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
SyGFileTreeTable::UpdateFileMenu()
{
	JTableSelection& s          = GetTableSelection();
	const JBoolean hasSelection = s.HasSelection();
	const JBoolean isWritable   = (itsFileTree->GetRootDirInfo())->IsWritable();

	JPoint singleCell;
	itsFileMenu->SetItemEnable(kAltOpenCmd,   hasSelection);
	itsFileMenu->SetItemEnable(kRunOnSelCmd,  hasSelection);
	itsFileMenu->SetItemEnable(kRenameCmd,    s.GetSingleSelectedCell(&singleCell));
	itsFileMenu->SetItemEnable(kDuplicateCmd, hasSelection);

	// symbolic links

	JBoolean findOriginal = kJFalse;
	if (hasSelection)
		{
		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			if ((itsFileTreeList->GetDirEntry(cell.y))->IsWorkingLink())
				{
				findOriginal = kJTrue;
				}
			}
		}

	itsFileMenu->SetItemEnable(kMakeAlias,           hasSelection);
	itsFileMenu->SetItemEnable(kFindOriginalCmd,     findOriginal);
	itsFileMenu->SetItemEnable(kConvertToFileCmd,    hasSelection);
	itsFileMenu->SetItemEnable(kConvertToProgramCmd, hasSelection);

	// mount point

	const JString& path = itsFileTree->GetDirectory();
	itsFileMenu->SetItemEnable(kToggleMountCmd,
		(SyGGetApplication())->IsMountPoint(path));
	JBoolean writable;
	if (JIsMounted(path, &writable))
		{
		itsFileMenu->SetItemText(kToggleMountCmd, kUnmountStr);
		itsFileMenu->SetItemEnable(kEraseDiskCmd, writable);
		}
	else	// including if not a mount point
		{
		itsFileMenu->SetItemText(kToggleMountCmd, kMountStr);
		itsFileMenu->DisableItem(kEraseDiskCmd);
		}
}

/******************************************************************************
 HandleFileMenu

 ******************************************************************************/

void
SyGFileTreeTable::HandleFileMenu
	(
	const JIndex index
	)
{
	ClearIncrementalSearchBuffer();

	if (index == kNewDirCmd)
		{
		CreateNewDirectory();
		}
	else if (index == kNewTextFileCmd)
		{
		CreateNewTextFile();
		}
	else if (index == kOpenCmd)
		{
		OpenSelection(kJFalse, kJFalse, kJFalse, kJFalse);
		}
	else if (index == kOpenCloseCmd)
		{
		OpenSelection(kJFalse, kJFalse, kJFalse, kJTrue);
		}
	else if (index == kOpenIconifyCmd)
		{
		OpenSelection(kJFalse, kJFalse, kJTrue, kJFalse);
		}
	else if (index == kAltOpenCmd)
		{
		OpenSelection(kJTrue, kJFalse, kJFalse, kJFalse);
		}
	else if (index == kRunOnSelCmd)
		{
		OpenSelection(kJFalse, kJTrue, kJFalse, kJFalse);
		}

	else if (index == kFindCmd)
		{
		const JString path = GetCommandPath();
		(GetWindow())->Deiconify();
		(SyGGetFindFileDialog())->Search(path);
		}

	else if (index == kRenameCmd)
		{
		JPoint cell;
		if ((GetTableSelection()).GetSingleSelectedCell(&cell))
			{
			BeginEditing(cell);
			}
		}

	else if (index == kDuplicateCmd)
		{
		DuplicateSelected();
		}
	else if (index == kMakeAlias)
		{
		MakeLinks();
		}
	else if (index == kFindOriginalCmd)
		{
		FindOriginals();
		}

	else if (index == kConvertToFileCmd)
		{
		ChangeExecPermission(kJFalse);
		}
	else if (index == kConvertToProgramCmd)
		{
		ChangeExecPermission(kJTrue);
		}

	else if (index == kToggleMountCmd)
		{
		const JString& path = itsFileTree->GetDirectory();
		JMount(path, !JIsMounted(path));
		}
	else if (index == kEraseDiskCmd)
		{
		FormatDisk();
		}

	else if (index == kHomeWindowCmd)
		{
		JString homeDir;
		if (JGetHomeDirectory(&homeDir))
			{
			GoTo(homeDir, kJFalse);
			}
		}

	else if (index == kViewManCmd)
		{
		ViewManPage();
		}
	else if (index == kOpenTermCmd)
		{
		OpenTerminal();
		}
	else if (index == kRunCmdCmd)
		{
		RunCommand();
		}

	else if (index == kCloseCmd)
		{
		(GetWindow())->Close();
		}
	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 CreateNewDirectory

 ******************************************************************************/

void
SyGFileTreeTable::CreateNewDirectory()
{
	if (!EndEditing())
		{
		return;
		}
	ClearIncrementalSearchBuffer();

	SyGFileTreeNode* node = NULL;

	JTableSelection& s = GetTableSelection();
	JPoint cell;
	if (s.GetFirstSelectedCell(&cell))
		{
		const JIndex index = GetNearestDirIndex(cell.y, kJTrue);
		if (index == 0)
			{
			node = itsFileTree->GetSyGRoot();
			}
		else
			{
			node = itsFileTreeList->GetSyGNode(index);
			(GetTreeList())->Open(node);
			}
		}
	else
		{
		node = itsFileTree->GetSyGRoot();
		}
	assert( node != NULL );

	JString dirName  = (node->GetDirEntry())->GetFullName();
	dirName          = JGetUniqueDirEntryName(dirName, "Untitled");
	const JError err = JCreateDirectory(dirName);
	if (err.OK())
		{
		JString p, n;
		JSplitPathAndName(dirName, &p, &n);
		JPoint cell;
		if (SelectName(n, node, &cell))		// updates table
			{
			BeginEditing(cell);
			}
		}
	else
		{
		(JGetStringManager())->ReportError(kCreateFolderErrorID, err);
		}
}

/******************************************************************************
 CreateNewTextFile

 ******************************************************************************/

void
SyGFileTreeTable::CreateNewTextFile()
{
	if (!EndEditing())
		{
		return;
		}
	ClearIncrementalSearchBuffer();

	SyGFileTreeNode* node = NULL;

	JTableSelection& s = GetTableSelection();
	JPoint cell;
	if (s.GetFirstSelectedCell(&cell))
		{
		const JIndex index = GetNearestDirIndex(cell.y, kJTrue);
		if (index == 0)
			{
			node = itsFileTree->GetSyGRoot();
			}
		else
			{
			node = itsFileTreeList->GetSyGNode(index);
			(GetTreeList())->Open(node);
			}
		}
	else
		{
		node = itsFileTree->GetSyGRoot();
		}
	assert( node != NULL );

	JString name = (node->GetDirEntry())->GetFullName();
	name         = JGetUniqueDirEntryName(name, "Untitled", ".txt");
	{
	ofstream output(name);
	}
	if (JFileExists(name))
		{
		JString p, n;
		JSplitPathAndName(name, &p, &n);
		JPoint cell;
		if (SelectName(n, node, &cell))		// updates table
			{
			BeginEditing(cell);
			}
		}
	else
		{
		(JGetUserNotification())->ReportError(kCreateTextFileErrorID);
		}
}

/******************************************************************************
 GetNearestDirIndex (private)

 ******************************************************************************/

JIndex
SyGFileTreeTable::GetNearestDirIndex
	(
	const JIndex	index,
	const JBoolean	requireWritable
	)
{
	const JTreeList* treeList = GetTreeList();

	JIndex dirIndex = index;

	const JTreeNode* node = treeList->GetNode(index);
	if (!node->IsOpenable())
		{
		const JTreeNode* parent = node->GetParent();
		if (!treeList->FindNode(parent, &dirIndex))
			{
			return 0;	// if file not in subdirectory
			}
		}

	if (requireWritable)
		{
		return ((itsFileTreeList->GetDirEntry(dirIndex))->IsWritable() ? dirIndex : 0);
		}
	else
		{
		return dirIndex;
		}
}

/******************************************************************************
 OpenSelection (private)

 ******************************************************************************/

void
SyGFileTreeTable::OpenSelection
	(
	const JBoolean alternate,
	const JBoolean alwaysRunCmd,
	const JBoolean iconifyAfter,
	const JBoolean closeAfter
	)
{
	ClearIncrementalSearchBuffer();

	if (!EndEditing())
		{
		return;
		}

	JBoolean found = kJFalse;

	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	fileList.SetCompareFunction(JCompareStringsCaseSensitive);

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
		{
		SyGFileTreeNode* node  = itsFileTreeList->GetSyGNode(cell.y);
		const JDirEntry* entry = node->GetDirEntry();
		if (!alwaysRunCmd && entry->IsDirectory())
			{
			if (node->IsOpenable())
				{
				const JString newPath = (node->GetDirEntry())->GetFullName();
				GoTo(newPath, alternate);
				if (alternate)
					{
					return;
					}
				}
			}
		else if (!alwaysRunCmd && !itsIgnoreExecPermFlag && entry->IsExecutable())
			{
			const JString file = (node->GetDirEntry())->GetFullName();
			JXFSBindingManager::Exec(file, alternate);
			}
		else
			{
			JString* s = new JString((node->GetDirEntry())->GetFullName());
			assert( s != NULL );
			if (!fileList.InsertSorted(s, kJFalse))
				{
				delete s;
				}
			}

		found = kJTrue;
		}

	if (found && !fileList.IsEmpty())
		{
		JXFSBindingManager::Exec(fileList, JI2B(alternate || alwaysRunCmd));

		if (fileList.GetElementCount() == 1)
			{
			SyGAddRecentFile(*(fileList.NthElement(1)));
			}
		}
	else if (!found)
		{
		JString path;
		if ((SyGGetChooseSaveFile())->
				ChooseRPath("Select path", NULL, itsFileTree->GetDirectory(), &path))
			{
			const JBoolean sameWindow = !(SyGGetChooseSaveFile())->IsOpeningInNewWindow();
			GoTo(path, sameWindow);
			if (sameWindow)
				{
				((GetWindow())->GetDirector())->Activate();
				return;
				}
			}
		}

	// not done if display new directory in same window

	if (iconifyAfter)	// takes precedence because more modifiers than closeAfter
		{
		(GetWindow())->Iconify();
		}
	else if (closeAfter)
		{
		// wait until safe to commit suicide

		JXCloseDirectorTask::Close((GetWindow())->GetDirector());
		}
}

/******************************************************************************
 DuplicateSelected

 ******************************************************************************/

void
SyGFileTreeTable::DuplicateSelected()
{
	if (!EndEditing())
		{
		return;
		}

	JPtrArray<SyGFileTreeNode> nodeList(JPtrArrayT::kForgetAll);

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
		{
		nodeList.Append(itsFileTreeList->GetSyGNode(cell.y));
		}

	SyGDuplicateProcess::Duplicate(this, nodeList);
}

/******************************************************************************
 MakeLinks

 ******************************************************************************/

void
SyGFileTreeTable::MakeLinks()
{
	if (!EndEditing())
		{
		return;
		}

	JPtrArray<SyGFileTreeNode> nodeList(JPtrArrayT::kForgetAll);

	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	JPoint cell;
	while (iter.Next(&cell))
		{
		// can't call MakeLinkToFile() here because it changes selection
		nodeList.Append(itsFileTreeList->GetSyGNode(cell.y));
		}

	s.ClearSelection();

	const JSize count = nodeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const SyGFileTreeNode* node = nodeList.NthElement(i);
		MakeLinkToFile((node->GetDirEntry())->GetFullName(), node->GetSyGParent());
		}

	if (s.GetSingleSelectedCell(&cell))
		{
		BeginEditing(cell);
		}
}

/******************************************************************************
 MakeLinkToFile (private)

 ******************************************************************************/

void
SyGFileTreeTable::MakeLinkToFile
	(
	const JString&			src,
	const SyGFileTreeNode*	parentNode
	)
{
	JString srcPath, srcName;
	JSplitPathAndName(src, &srcPath, &srcName);

	const JString destPath = (parentNode->GetDirEntry())->GetFullName();

	JString root, suffix;
	if (JSplitRootAndSuffix(srcName, &root, &suffix))
		{
		suffix.PrependCharacter('.');
		}
	root += "_alias";

	const JString dest = JGetUniqueDirEntryName(destPath, root, suffix);
	const JError err   = JCreateSymbolicLink(src, dest);
	if (err.OK())
		{
		JString p, n;
		JSplitPathAndName(dest, &p, &n);
		JPoint cell;
		SelectName(n, parentNode, &cell);		// updates table
		}
	else
		{
		(JGetStringManager())->ReportError(kCreateAliasErrorID, err);
		}
}

/******************************************************************************
 FindOriginals

 ******************************************************************************/

void
SyGFileTreeTable::FindOriginals()
{
	JTableSelectionIterator iter(&(GetTableSelection()));

	JPoint cell;
	while (iter.Next(&cell))
		{
		const JDirEntry* entry = itsFileTreeList->GetDirEntry(cell.y);
		const JString* linkData;
		if (entry->IsWorkingLink() && entry->GetLinkName(&linkData))
			{
			JString fullName = *linkData;
			if (JIsRelativePath(fullName))
				{
				fullName = JCombinePathAndName(entry->GetPath(), fullName);
				}

			if (JDirectoryExists(fullName))
				{
				// open the containing directory, not the directory itself

				JString path, name;
				JSplitPathAndName(fullName, &path, &name);

				SyGTreeDir* dir;
				if ((SyGGetApplication())->OpenDirectory(path, &dir))
					{
					dir->SelectName(name);
					}
				}
			else
				{
				(SyGGetApplication())->OpenDirectory(fullName);
				}
			}
		}
}

/******************************************************************************
 FormatDisk

 ******************************************************************************/

void
SyGFileTreeTable::FormatDisk()
{
	if (itsFormatProcess == NULL &&
		(JGetUserNotification())->AskUserNo(JGetString(kWarnEraseDiskID)))
		{
		assert( itsChooseDiskFormatDialog == NULL );

		JPtrArray<JString> choiceList(JPtrArrayT::kDeleteAll);
		for (JIndex i=0; i<kFormatCount; i++)
			{
			choiceList.Append(kFormatName[i]);
			}

		itsChooseDiskFormatDialog =
			new JXRadioGroupDialog(JXGetApplication(), kFormatWindowTitle,
								   kFormatPrompt, choiceList, NULL);
		assert(itsChooseDiskFormatDialog != NULL);
		itsChooseDiskFormatDialog->BeginDialog();
		ListenTo(itsChooseDiskFormatDialog);
		}
}

/******************************************************************************
 ViewManPage

 ******************************************************************************/

void
SyGFileTreeTable::ViewManPage()
{
	if (!EndEditing())
		{
		return;
		}

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

	JPoint cell;
	while (iter.Next(&cell))
		{
		list.Append((itsFileTreeList->GetSyGNode(cell.y))->GetName());
		}

	if (list.IsEmpty())
		{
		(SyGGetManPageDialog())->Activate();
		}
	else
		{
		(SyGGetManPageDialog())->ViewManPages(list);
		}
}

/******************************************************************************
 OpenTerminal

 ******************************************************************************/

void
SyGFileTreeTable::OpenTerminal()
{
	if (EndEditing())
		{
		const JString path = GetCommandPath();
		SyGGetApplication()->OpenTerminal(path);
		}
}

/******************************************************************************
 RunCommand

 ******************************************************************************/

void
SyGFileTreeTable::RunCommand()
{
	if (EndEditing())
		{
		const JString path = GetCommandPath();
		JXFSBindingManager::Exec(path);
		}
}

/******************************************************************************
 GetCommandPath (private)

 ******************************************************************************/

JString
SyGFileTreeTable::GetCommandPath()
	const
{
	JString path;

	JPoint cell1, cell2;
	if ((GetWindow())->IsIconified())
		{
		path = itsFileTree->GetDirectory();
		}
	else if ((GetTableSelection()).GetFirstSelectedCell(&cell1) &&
			 (GetTableSelection()).GetLastSelectedCell(&cell2))
		{
		const JDirEntry* e1 = itsFileTreeList->GetDirEntry(cell1.y);
		const JDirEntry* e2 = itsFileTreeList->GetDirEntry(cell2.y);

		const JString p1 = e1->GetPath();
		const JString p2 = e2->GetPath();

		if (cell1 == cell2 && e1->IsDirectory())
			{
			path = e1->GetFullName();
			}
		else if (p1 == p2)
			{
			path = p1;
			}
		}

	if (path.IsEmpty())
		{
		path = itsFileTree->GetDirectory();
		}

	return path;
}

/******************************************************************************
 ChangeExecPermission (private)

 ******************************************************************************/

void
SyGFileTreeTable::ChangeExecPermission
	(
	const JBoolean canExec
	)
{
	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	JPoint testCell;
	while (iter.Next(&testCell))
		{
		SyGFileTreeNode* node  = itsFileTreeList->GetSyGNode(testCell.y);
		const JString fullName = (node->GetDirEntry())->GetFullName();
		if (node->GetDirEntry()->IsFile())
			{
			JString sysCmd = "chmod ";
			sysCmd += "a";
			sysCmd += (canExec ? "+" : "-");
			sysCmd += "x ";
			sysCmd += JPrepArgForExec(fullName);
			SyGExec(sysCmd);
			node->Update(kJTrue);
			}
		}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
SyGFileTreeTable::UpdateEditMenu()
{
	JXTEBase* te = GetEditMenuHandler();

	JIndex index;
	if (te->EditMenuCmdToIndex(JTextEditor::kSelectAllCmd, &index))
		{
		itsEditMenu->EnableItem(index);
		}

	if ((GetTableSelection()).HasSelection() &&
		te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &index))
		{
		assert( itsCopyPathCmdIndex == index+1 );

		itsEditMenu->EnableItem(index);
		itsEditMenu->EnableItem(itsCopyPathCmdIndex);
		}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
SyGFileTreeTable::HandleEditMenu
	(
	const JIndex index
	)
{
	ClearIncrementalSearchBuffer();

	JTextEditor::CmdIndex cmd;
	if (!(GetEditMenuHandler())->EditMenuIndexToCmd(index, &cmd))
		{
		cmd = JTextEditor::kSeparatorCmd;
		}

	if (cmd == JTextEditor::kCopyCmd)
		{
		CopySelectedFileNames(kJFalse);
		}
	else if (index == itsCopyPathCmdIndex)
		{
		CopySelectedFileNames(kJTrue);
		}
	else if (cmd == JTextEditor::kSelectAllCmd)
		{
		(GetTableSelection()).SelectCol(GetNodeColIndex());
		}
}

/******************************************************************************
 CopySelectedFileNames (private)

 ******************************************************************************/

void
SyGFileTreeTable::CopySelectedFileNames
	(
	const JBoolean useFullPath
	)
	const
{
	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			const JDirEntry* entry = itsFileTreeList->GetDirEntry(cell.y);
			if (useFullPath)
				{
				list.Append(entry->GetFullName());
				}
			else
				{
				list.Append(entry->GetName());
				}
			}

		JXTextSelection* data = new JXTextSelection(GetDisplay(), list);
		assert( data != NULL );

		(GetSelectionManager())->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 UpdateViewMenu

 ******************************************************************************/

void
SyGFileTreeTable::UpdateViewMenu()
{
	if (itsTreeSet->FilterVisible())
		{
		itsViewMenu->CheckItem(kShowFilterCmd);
		}

	if (itsFileTree->HiddenVisible())
		{
		itsViewMenu->CheckItem(kShowHiddenCmd);
		}

	JBoolean prefs[kSyGTreePrefCount];
	GetPreferences(prefs);
	if (prefs[2])
		{
		itsViewMenu->CheckItem(kShowUserCmd);
		}
	if (prefs[3])
		{
		itsViewMenu->CheckItem(kShowGroupCmd);
		}
	if (prefs[4])
		{
		itsViewMenu->CheckItem(kShowSizeCmd);
		}
	if (prefs[5])
		{
		itsViewMenu->CheckItem(kShowModeCmd);
		}
	if (prefs[6])
		{
		itsViewMenu->CheckItem(kShowDateCmd);
		}
}

/******************************************************************************
 HandleViewMenu

 ******************************************************************************/

void
SyGFileTreeTable::HandleViewMenu
	(
	const JIndex index
	)
{
	ClearIncrementalSearchBuffer();

	if (index == kShowFilterCmd)
		{
		itsTreeSet->ToggleFilter();
		}
	else if (index == kShowHiddenCmd)
		{
		TogglePref(0);
		}

	else if (index == kShowUserCmd)
		{
		TogglePref(2);
		}
	else if (index == kShowGroupCmd)
		{
		TogglePref(3);
		}
	else if (index == kShowSizeCmd)
		{
		TogglePref(4);
		}
	else if (index == kShowModeCmd)
		{
		TogglePref(5);
		}
	else if (index == kShowDateCmd)
		{
		TogglePref(6);
		}

	else if (index == kShowAllAttrCmd)
		{
		JBoolean prefs[kSyGTreePrefCount];
		GetPreferences(prefs);
		for (JIndex i = 2; i < kSyGTreePrefCount; i++)
			{
			prefs[i] = kJTrue;
			}
		SetPreferences(prefs);
		}
	else if (index == kHideAllAttrCmd)
		{
		JBoolean prefs[kSyGTreePrefCount];
		GetPreferences(prefs);
		for (JIndex i = 2; i < kSyGTreePrefCount; i++)
			{
			prefs[i] = kJFalse;
			}
		SetPreferences(prefs);
		}

	else if (index == kRefreshCmd)
		{
		UpdateDisplay(kJTrue);
		}
}

/******************************************************************************
 GetPreferences

 ******************************************************************************/

void
SyGFileTreeTable::GetPreferences
	(
	JBoolean prefs[]
	)
{
	prefs[0] = itsFileTree->HiddenVisible();
//	prefs[1] = itsFileTree->FilesVisible();
	prefs[1] = kJTrue;

	for (JIndex i=2; i<kSyGTreePrefCount; i++)
		{
		prefs[i] = itsVisibleCols[i-GetNodeColIndex()];
		}
}

/******************************************************************************
 SetPreferences

 ******************************************************************************/

static const GFMColType kPrefColID[] =
{
	kGFMUID, kGFMGID, kGFMSize, kGFMMode, kGFMDate
};

void
SyGFileTreeTable::SetPreferences
	(
	const JBoolean prefs[]
	)
{
	itsFileTree->ShowHidden(prefs[0]);

	for (JIndex i=2; i<kSyGTreePrefCount; i++)
		{
		const GFMColType type = kPrefColID[i-GetNodeColIndex()];
		if (prefs[i])
			{
			InsertFMTreeCol(type);
			}
		else
			{
			RemoveFMTreeCol(type);
			if (itsCurrentColType == type)
				{
				SetCurrentColIndex(GetNodeColIndex());
				}
			}
		}

	AdjustToTree();
}

/******************************************************************************
 TogglePref (private)

 ******************************************************************************/

void
SyGFileTreeTable::TogglePref
	(
	const JIndex i
	)
{
	JBoolean prefs[kSyGTreePrefCount];
	GetPreferences(prefs);
	prefs[i] = !prefs[i];
	SetPreferences(prefs);
}

/******************************************************************************
 InsertFMTreeCol

 ******************************************************************************/

void
SyGFileTreeTable::InsertFMTreeCol
	(
	const GFMColType type
	)
{
	JIndex		index;
	JCoordinate width;

	if (InsertFMCol(this, itsVisibleCols, GetNodeColIndex(), type, &index, &width))
		{
		InsertCols(index, 1, width);
		}
}

/******************************************************************************
 RemoveFMTreeCol

 ******************************************************************************/

void
SyGFileTreeTable::RemoveFMTreeCol
	(
	const GFMColType type
	)
{
	JIndex index;

	if (RemoveFMCol(this, itsVisibleCols, GetNodeColIndex(), type, &index))
		{
		RemoveCol(index);
		}
}

/******************************************************************************
 LoadPrefs

 ******************************************************************************/

void
SyGFileTreeTable::LoadPrefs()
{
	JBoolean prefs[kSyGTreePrefCount];
	(SyGGetPrefsMgr())->GetTreePreferences(prefs);
	SetPreferences(prefs);
}

/******************************************************************************
 LoadPrefs

 ******************************************************************************/

void
SyGFileTreeTable::LoadPrefs
	(
	istream&			is,
	const JFileVersion	vers
	)
{
	if (vers < 2)
		{
		JFileVersion v;
		is >> v;
		}

	int type;
	is >> type;

	JBoolean prefs[kSyGTreePrefCount];
	if (vers >= 2)
		{
		is >> prefs[0];
		}
	is >> prefs[2];
	is >> prefs[3];
	is >> prefs[4];
	is >> prefs[5];
	is >> prefs[6];
	SetPreferences(prefs);

	SetCurrentColType((GFMColType) type);
}

/******************************************************************************
 SavePrefs

 ******************************************************************************/

void
SyGFileTreeTable::SavePrefs
	(
	ostream& os
	)
{
	os << ' ' << (int)itsCurrentColType << ' ';

	JBoolean prefs[kSyGTreePrefCount];
	GetPreferences(prefs);
	os << prefs[0];
	os << prefs[2];
	os << prefs[3];
	os << prefs[4];
	os << prefs[5];
	os << prefs[6];

	os << ' ';
}

/******************************************************************************
 SavePrefsAsDefault

 ******************************************************************************/

void
SyGFileTreeTable::SavePrefsAsDefault()
{
	JBoolean prefs[kSyGTreePrefCount];
	GetPreferences(prefs);
	SyGGetPrefsMgr()->SetTreePreferences(prefs);
	JXWindow* window = GetWindow();
	SyGGetPrefsMgr()->SaveDefaultWindowSize(window->GetFrameWidth(), window->GetFrameHeight());
	itsTreeSet->SaveFilterPref();
}

/******************************************************************************
 RestoreDirState

 ******************************************************************************/

void
SyGFileTreeTable::RestoreDirState
	(
	istream&			is,
	const JFileVersion	vers
	)
{
	if (vers < 2)
		{
		JFileVersion v;
		is >> v;
		}

	JSize nameCount;
	is >> nameCount;

	JString file, n;
	for (JIndex i=1; i<=nameCount; i++)
		{
		is >> file;

		const JSize count = itsFileTreeList->GetElementCount();	// changes after Open()
		for (JIndex j=1; j<=count; j++)
			{
			n = (itsFileTreeList->GetDirEntry(j))->GetFullName();
			if (JSameDirEntry(n, file))
				{
				itsFileTreeList->Open(j);
				break;
				}
			}
		}
}

/******************************************************************************
 SaveDirState

 ******************************************************************************/

void
SyGFileTreeTable::SaveDirState
	(
	ostream& os
	)
{
	JPtrArray<JString> names(JPtrArrayT::kDeleteAll);
	JSize count = itsFileTreeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (itsFileTreeList->IsOpen(i))
			{
			names.Append((itsFileTreeList->GetDirEntry(i))->GetFullName());
			}
		}

	count = names.GetElementCount();
	os << ' ' << count;

	for (JIndex i=1; i<=count; i++)
		{
		os << ' ' << *(names.NthElement(i));
		}

	os << ' ';
}

/******************************************************************************
 IsCurrentCol

 ******************************************************************************/

JBoolean
SyGFileTreeTable::IsCurrentCol
	(
	const JIndex index
	)
	const
{
	GFMColType type = GetFMColType(index, GetNodeColIndex(), itsVisibleCols);
	return JI2B(type == itsCurrentColType);
}

/******************************************************************************
 SetCurrentColIndex

 ******************************************************************************/

void
SyGFileTreeTable::SetCurrentColIndex
	(
	const JIndex index
	)
{
	SetCurrentColType(GetFMColType(index, GetNodeColIndex(), itsVisibleCols));
}

/******************************************************************************
 SetCurrentColType

 ******************************************************************************/

void
SyGFileTreeTable::SetCurrentColType
	(
	const GFMColType type
	)
{
	ClearIncrementalSearchBuffer();
	if (type != itsCurrentColType && type != kGFMMode)
		{
		itsCurrentColType = type;
		itsFileTree->SetNodeCompareFunction(type);
		}
}

/******************************************************************************
 UpdateGitBranchMenu

 ******************************************************************************/

void
SyGFileTreeTable::UpdateGitBranchMenu()
{
	itsGitBranchMenu->RemoveAllItems();

	pid_t pid;
	int fromFD;
	const JError err = JExecute(itsFileTree->GetDirectory(), "git branch", &pid,
								kJIgnoreConnection, NULL,
								kJCreatePipe, &fromFD);
	if (!err.OK())
		{
		itsGitBranchMenu->AppendItem(JGetString(kNoBranchInfoID));
		itsGitBranchMenu->DisableItem(1);
		return;
		}

	JString line;
	while (1)
		{
		line = JReadUntil(fromFD, '\n');
		if (line.IsEmpty())
			{
			break;
			}

		const JBoolean current = JI2B(line.GetFirstCharacter() == '*');
		line.RemoveSubstring(1,2);

		itsGitBranchMenu->AppendItem(line, kJFalse, kJTrue);
		if (current)
			{
			itsGitBranchMenu->CheckItem(itsGitBranchMenu->GetItemCount());
			}
		}

	if (itsGitBranchMenu->IsEmpty())
		{
		itsGitBranchMenu->AppendItem(JGetString(kNoBranchInfoID));
		itsGitBranchMenu->DisableItem(1);
		}
	else
		{
		itsGitBranchCount = itsGitBranchMenu->GetItemCount();
		itsGitBranchMenu->ShowSeparatorAfter(itsGitBranchCount);

		itsGitBranchMenu->AppendItem(JGetString(kCreateBranchID));
		}
}

/******************************************************************************
 HandleGitBranchMenu

 ******************************************************************************/

void
SyGFileTreeTable::HandleGitBranchMenu
	(
	const JIndex index
	)
{
	if (index <= itsGitBranchCount && !itsGitBranchMenu->IsChecked(index))
		{
		JString cmd = "git checkout ";
		cmd        += JPrepArgForExec(itsGitBranchMenu->GetItemText(index));

		int fromFD;
		const JError err = JExecute(itsFileTree->GetDirectory(), cmd, NULL,
									kJIgnoreConnection, NULL,
									kJCreatePipe, &fromFD,
									kJAttachToFromFD);
		if (err.OK())
			{
			JString msg;
			JReadAll(fromFD, &msg);
			(JGetUserNotification())->DisplayMessage(msg);

			JExecute(itsFileTree->GetDirectory(), (SyGGetApplication())->GetPostCheckoutCommand(), NULL);
			}
		else
			{
			err.ReportIfError();
			}
		}
	else if (index == itsGitBranchCount+1)
		{
		itsCreateBranchDialog =
			new JXGetStringDialog((GetWindow())->GetDirector(), JGetString(kCreateBranchTitleID),
								  JGetString(kCreateBranchPromptID), "");
		assert( itsCreateBranchDialog != NULL );
		itsCreateBranchDialog->Activate();
		ListenTo(itsCreateBranchDialog);
		}
}

/******************************************************************************
 UpdateShortcutMenu

 ******************************************************************************/

void
SyGFileTreeTable::UpdateShortcutMenu()
{
	const JSize count = itsShortcutMenu->GetItemCount();
	for (JIndex i=count; i>kShortcutCmdOffset; i--)
		{
		itsShortcutMenu->RemoveItem(i);
		}

	SyGGetApplication()->UpdateShortcutMenu(itsShortcutMenu);
}

/******************************************************************************
 HandleShortcutMenu

 ******************************************************************************/

void
SyGFileTreeTable::HandleShortcutMenu
	(
	const JIndex index
	)
{
	ClearIncrementalSearchBuffer();

	if (index == kAddShortcutCmd)
		{
		SyGGetApplication()->AddShortcut(itsFileTree->GetDirectory());
		}
	else if (index == kRemoveShortcutCmd)
		{
		SyGGetApplication()->RemoveShortcut(itsFileTree->GetDirectory());
		}
	else if (index == kRemoveAllShortcutsCmd)
		{
		SyGGetApplication()->RemoveAllShortcuts();
		}

	else
		{
		SyGGetApplication()->OpenShortcut(index - kShortcutCmdOffset);
		}
}

/******************************************************************************
 CreateTreeListInput (virtual protected)

	Derived class can override this to instantiate a derived class of
	JXInputField.

 ******************************************************************************/

JXInputField*
SyGFileTreeTable::CreateTreeListInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	JXInputField* obj = new JXSaveFileInput(enclosure, hSizing, vSizing, x,y, w,h);
	assert( obj != NULL );
	return obj;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
SyGFileTreeTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	JXInputField* input;
	const JBoolean ok = GetXInputField(&input);
	assert( ok );

	if (input->IsEmpty())
		{
		return kJTrue;		// treat as cancel
		}

	SyGFileTreeNode* node = itsFileTreeList->GetSyGNode(cell.y);
	const JString newName = input->GetText();	// copy since need after input field gone
	const JBoolean sort   = ((GetDisplay())->GetLatestButtonStates()).AllOff();
	const JError err      = node->Rename(newName, sort);
	input                 = NULL;				// nodes sorted => CancelEditing()
	if (!err.OK())
		{
		(JGetStringManager())->ReportError(kRenameErrorID, err);
		}
	else if (sort)
		{
		ScrollToNode(node);
		}
	else
		{
		itsSortNode = node->GetSyGParent();
		ListenTo(itsSortNode);	// in case it dies
		}
	return err.OK();
}

/******************************************************************************
 AdjustToTree (virtual protected)

 ******************************************************************************/

void
SyGFileTreeTable::AdjustToTree()
{
	const JFontStyle workingLink(kJFalse, kJTrue, 0, kJFalse);
	const JFontStyle brokenLink(kJFalse, kJTrue, 0, kJFalse, (GetColormap())->GetGray60Color());

	const JSize count = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry* entry = itsFileTreeList->GetDirEntry(i);
		if (entry->IsWorkingLink())
			{
			SetCellStyle(JPoint(GetNodeColIndex(), i), workingLink);
			}
		else if (entry->IsBrokenLink())
			{
			SetCellStyle(JPoint(GetNodeColIndex(), i), brokenLink);
			}
		}

	JXNamedTreeListWidget::AdjustToTree();
}

/******************************************************************************
 GetMinCellWidth (virtual protected)

 ******************************************************************************/

JSize
SyGFileTreeTable::GetMinCellWidth
	(
	const JPoint& cell
	)
	const
{
	if (JIndex(cell.x) > GetNodeColIndex())
		{
		const JString str = GetCellString(cell);

		JSize fontSize;
		const JString& fontName = GetFont(&fontSize);

		const GFMColType type       = GetFMColType(cell.x, GetNodeColIndex(), itsVisibleCols);
		const JCoordinate theBuffer = JLAbs(GetFMBufferWidth(itsVisibleCols, GetNodeColIndex(), cell.x));
		JSize w;
		if (type == kGFMMode)
			{
			w = theBuffer + 9 * itsPermCharWidth;
			}
		else
			{
			w = 2 * theBuffer +
				(GetFontManager())->GetStringWidth(fontName, fontSize, JFontStyle(), str);
			}

		const JSize wT = 2 * theBuffer + (GetFontManager())->
			GetStringWidth(fontName, fontSize, JFontStyle(), GetColTitle(cell.x));
		return JMax(wT, w);
		}
	else
		{
		return JXNamedTreeListWidget::GetMinCellWidth(cell);
		}
}

/******************************************************************************
 GetBufferWidth

 ******************************************************************************/

JCoordinate
SyGFileTreeTable::GetBufferWidth
	(
	const JIndex index
	)
	const
{
	if (index == GetNodeColIndex())
		{
		return (GetRowCount() == 0 ? 0 : GetImageWidth(1));
		}
	else
		{
		return GetFMBufferWidth(itsVisibleCols, GetNodeColIndex(), index);
		}
}

/******************************************************************************
 GetColTitle

 ******************************************************************************/

JString
SyGFileTreeTable::GetColTitle
	(
	const JIndex index
	)
	const
{
	JString str;
	if (index == GetNodeColIndex())
		{
		str = "Name";
		}
	else
		{
		GetFMColTitle(itsVisibleCols, GetNodeColIndex(), index, &str);
		}
	return str;
}
