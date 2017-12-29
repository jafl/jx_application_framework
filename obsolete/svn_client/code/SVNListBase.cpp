/******************************************************************************
 SVNListBase.cc

	BASE CLASS = JXStringList, SVNTabBase

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include "SVNListBase.h"
#include "SVNMainDirector.h"
#include "svnMenus.h"
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXColormap.h>
#include <JXDeleteObjectTask.h>
#include <JXTextSelection.h>
#include <jXGlobals.h>
#include <JTableSelection.h>
#include <JProcess.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

// Context menu

static const JCharacter* kContextMenuStr =
	"    Compare with edited"
	"  | Compare with current"
	"  | Compare with previous"
	"%l| Info & Log"
	"  | Properties"
	"%l| Ignore..."
	"%l| Show in file manager";

	/*
	 * Remember to keep enum in SVNListBase.h sync
	 */

/******************************************************************************
 Constructor

	Derived class must call RefreshContent()

 ******************************************************************************/

SVNListBase::SVNListBase
	(
	SVNMainDirector*	director,
	JXTextMenu*			editMenu,
	const JString&		cmd,
	const JBoolean		refreshRepo,
	const JBoolean		refreshStatus,
	const JBoolean		reload,
	const JBoolean		enableContextMenu,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStringList(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	SVNTabBase(director),
	itsCmd(cmd),
	itsRefreshRepoFlag(refreshRepo),
	itsRefreshStatusFlag(refreshStatus),
	itsReloadOpenFilesFlag(reload),
	itsEditMenu(editMenu),
	itsEnableContextMenuFlag(enableContextMenu),
	itsContextMenu(NULL),
	itsProcess(NULL),
	itsMessageLink(NULL),
	itsErrorLink(NULL)
{
	SetFont(JGetMonospaceFontName(), JGetDefaultMonoFontSize());
	SetSelectionBehavior(kJTrue, kJTrue);

	itsLineList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsLineList != NULL );
	itsLineList->SetCompareObject(CompareLines(this));
	SetStringList(itsLineList);

	itsErrorList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsErrorList != NULL );

	itsSavedSelection = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSavedSelection != NULL );
	itsSavedSelection->SetCompareFunction(JCompareStringsCaseSensitive);

	FitToEnclosure();

	ListenTo(itsEditMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNListBase::~SVNListBase()
{
	if (itsProcess != NULL)
		{
		StopListening(itsProcess);
		itsProcess->Kill();
		}
	jdelete itsProcess;

	DeleteLinks();

	JXDeleteObjectTask<JBroadcaster>::Delete(itsLineList);
	jdelete itsErrorList;
	jdelete itsSavedSelection;
}

/******************************************************************************
 RefreshContent (virtual)

 ******************************************************************************/

void
SVNListBase::RefreshContent()
{
	if (itsProcess != NULL)
		{
		JProcess* p = itsProcess;
		itsProcess  = NULL;

		p->Kill();
		jdelete p;

		DeleteLinks();
		}
	else
		{
		itsSavedSelection->CleanOut();
		JTableSelection& s = GetTableSelection();
		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			const JString* line = itsLineList->NthElement(cell.y);
			itsSavedSelection->InsertSorted(jnew JString(ExtractRelativePath(*line)));
			}
		}

	itsDisplayState = SaveDisplayState();
	itsLineList->CleanOut();

	int outFD, errFD;
	JError err = JNoError();
	if (GetDirector()->HasPath())
		{
		err = JProcess::Create(&itsProcess, GetPath(), itsCmd,
							   kJIgnoreConnection, NULL,
							   kJCreatePipe, &outFD,
							   kJCreatePipe, &errFD);
		}
	else	// working with URL
		{
		err = JProcess::Create(&itsProcess, itsCmd,
							   kJIgnoreConnection, NULL,
							   kJCreatePipe, &outFD,
							   kJCreatePipe, &errFD);
		}

	if (err.OK())
		{
		itsProcess->ShouldDeleteWhenFinished();
		ListenTo(itsProcess);
		GetDirector()->RegisterActionProcess(this, itsProcess, itsRefreshRepoFlag,
											   itsRefreshStatusFlag, itsReloadOpenFilesFlag);

		SetConnection(outFD, errFD);
		}
	else
		{
		err.ReportIfError();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNListBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsMessageLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		ReceiveMessageLine();
		}
	else if (sender == itsErrorLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		ReceiveErrorLine();
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
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != NULL );
			HandleEditMenu(selection->GetIndex());
			}
		}

	else if (sender == itsContextMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateContextMenu(itsContextMenu);
		}
	else if (sender == itsContextMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleContextMenu(selection->GetIndex());
		}

	else
		{
		JXStringList::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
SVNListBase::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsProcess)
		{
		itsProcess = NULL;
		DeleteLinks();

		DisplayErrors();
		if (itsDisplayState.vMax > 0)
			{
			RestoreDisplayState(itsDisplayState);
			}
		}
	else
		{
		JXStringList::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 ReceiveMessageLine (private)

 ******************************************************************************/

void
SVNListBase::ReceiveMessageLine()
{
	assert( itsMessageLink != NULL );

	JString line;
	const JBoolean ok = itsMessageLink->GetNextMessage(&line);
	assert( ok );

	if (!ShouldDisplayLine(&line))
		{
		return;
		}

	const JFontStyle red(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetRedColor());
	const JFontStyle blue = GetColormap()->GetBlueColor();
	const JFontStyle strike(kJFalse, kJFalse, 0, kJTrue);

	JString* temp = jnew JString(line);
	assert( temp != NULL );

	JIndex i;
	itsLineList->InsertSorted(temp, kJTrue, &i);
	StyleLine(i, line, red, blue, strike);

	JString relPath = ExtractRelativePath(line);
	JIndex j;
	if (itsSavedSelection->SearchSorted(&relPath, JOrderedSetT::kAnyMatch, &j))
		{
		(GetTableSelection()).SelectRow(i);
		}
}

/******************************************************************************
 ShouldDisplayLine (virtual protected)

	Return kJFalse if the line should not be displayed.

 ******************************************************************************/

JBoolean
SVNListBase::ShouldDisplayLine
	(
	JString* line
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 ReceiveErrorLine (private)

 ******************************************************************************/

void
SVNListBase::ReceiveErrorLine()
{
	assert( itsErrorLink != NULL );

	JString line;
	const JBoolean ok = itsErrorLink->GetNextMessage(&line);
	assert( ok );

	itsErrorList->Append(line);
}

/******************************************************************************
 DisplayErrors (private)

 ******************************************************************************/

void
SVNListBase::DisplayErrors()
{
	const JFontStyle red(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetRedColor());

	const JSize count = itsErrorList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsLineList->InsertAtIndex(i, itsErrorList->NthElement(i));
		SetStyle(i, red);
		}

	itsErrorList->RemoveAll();
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
SVNListBase::UpdateEditMenu()
{
	const JSize count = itsEditMenu->GetItemCount();
	const JString* id;
	for (JIndex i=1; i<=count; i++)
		{
		if (itsEditMenu->GetItemID(i, &id) &&
			(((*id == kJXCopyAction || *id == kSVNCopyFullPathAction) &&
			  (GetTableSelection()).HasSelection()) ||
			 *id == kJXSelectAllAction))
			{
			itsEditMenu->EnableItem(i);
			}
		}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
SVNListBase::HandleEditMenu
	(
	const JIndex index
	)
{
	const JString* id;
	if (!itsEditMenu->GetItemID(index, &id))
		{
		return;
		}

	if (*id == kJXCopyAction)
		{
		CopySelectedItems(kJFalse);
		}
	else if (*id == kSVNCopyFullPathAction)
		{
		CopySelectedItems(kJTrue);
		}
	else if (*id == kJXSelectAllAction)
		{
		(GetTableSelection()).SelectAll();
		}
}

/******************************************************************************
 CopySelectedItems (virtual protected)

 ******************************************************************************/

void
SVNListBase::CopySelectedItems
	(
	const JBoolean fullPath
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list, kJTrue);
	if (list.IsEmpty())
		{
		return;
		}

	if (!fullPath)
		{
		const JSize count       = list.GetElementCount();
		const JString& basePath = GetPath();
		JString fullPath;
		for (JIndex i=1; i<=count; i++)
			{
			JString* path = list.NthElement(i);
			*path         = JConvertToRelativePath(*path, basePath);
			}
		}

	JXTextSelection* data = jnew JXTextSelection(GetDisplay(), list);
	assert( data != NULL );

	GetSelectionManager()->SetData(kJXClipboardName, data);
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
SVNListBase::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsProcess != NULL)
		{
		DisplayCursor(kJXBusyCursor);
		}
	else
		{
		JXWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
SVNListBase::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();
	itsIsDraggingFlag  = kJFalse;
	itsSavedSelection->CleanOut();

	JPoint cell;
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		}
	else if (!GetCell(pt, &cell))
		{
		s.ClearSelection();
		}
	else if (button == kJXLeftButton && modifiers.shift())
		{
		itsIsDraggingFlag = s.ExtendSelection(cell);
		}
	else if (button == kJXLeftButton && modifiers.control())
		{
		s.InvertCell(cell);
		if (s.IsSelected(cell))
			{
			s.SetBoat(cell);
			s.SetAnchor(cell);
			}
		else
			{
			s.ClearBoat();
			s.ClearAnchor();
			}
		}
	else if (button == kJXLeftButton)
		{
		s.ClearSelection();
		s.SetBoat(cell);
		s.SetAnchor(cell);
		s.SelectCell(cell);
		itsIsDraggingFlag = kJTrue;

		if (clickCount == 2)
			{
			OpenSelectedItems();
			}
		}
	else if (button == kJXRightButton && clickCount == 1)
		{
		if (CreateContextMenu())
			{
			if (!s.IsSelected(cell))
				{
				s.ClearSelection();
				s.SetBoat(cell);
				s.SetAnchor(cell);
				s.SelectCell(cell);
				}

			itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
SVNListBase::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsIsDraggingFlag)
		{
		ScrollForDrag(pt);

		JPoint cell;
		const JBoolean ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
		assert( ok );
		(GetTableSelection()).ExtendSelection(cell);
		}
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
SVNListBase::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s          = GetTableSelection();
	const JBoolean hadSelection = s.HasSelection();
	itsSavedSelection->CleanOut();

	if (key == kJReturnKey)
		{
		OpenSelectedItems();
		}

	// space->clear is handled by JXStringList

	else if (key == kJUpArrow || key == kJDownArrow)
		{
		ClearIncrementalSearchBuffer();
		if (!hadSelection && key == kJUpArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(1, GetRowCount()));
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			}
		}

	else
		{
		JXStringList::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 CreateContextMenu (private)

 ******************************************************************************/

#include "svn_info_log.xpm"

JBoolean
SVNListBase::CreateContextMenu()
{
	if (itsContextMenu == NULL && itsEnableContextMenuFlag)
		{
		itsContextMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsContextMenu != NULL );
		itsContextMenu->SetMenuItems(kContextMenuStr, "SVNListBase");
		itsContextMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsContextMenu->SetToHiddenPopupMenu();

		itsContextMenu->SetItemImage(kInfoLogSelectedFilesCtxCmd, svn_info_log);

		ListenTo(itsContextMenu);
		}

	return JI2B(itsEnableContextMenuFlag && itsContextMenu != NULL);
}

/******************************************************************************
 UpdateContextMenu (virtual protected)

 ******************************************************************************/

void
SVNListBase::UpdateContextMenu
	(
	JXTextMenu* menu
	)
{
	JString rev;
	menu->SetItemEnable(kDiffEditedSelectedFilesCtxCmd, GetBaseRevision(&rev));
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
SVNListBase::HandleContextMenu
	(
	const JIndex index
	)
{
	if (index == kDiffEditedSelectedFilesCtxCmd)
		{
		JString rev;
		if (GetBaseRevision(&rev))
			{
			CompareEdited(rev);
			}
		}
	else if (index == kDiffCurrentSelectedFilesCtxCmd)
		{
		JString rev;
		GetBaseRevision(&rev);
		CompareCurrent(rev);
		}
	else if (index == kDiffPrevSelectedFilesCtxCmd)
		{
		JString rev;
		GetBaseRevision(&rev);
		ComparePrev(rev);
		}

	else if (index == kInfoLogSelectedFilesCtxCmd)
		{
		GetDirector()->ShowInfoLog(this);
		}
	else if (index == kPropSelectedFilesCtxCmd)
		{
		GetDirector()->ShowProperties(this);
		}

	else if (index == kIgnoreSelectionCtxCmd)
		{
		Ignore();
		}

	else if (index == kShowSelectedFilesCtxCmd)
		{
		ShowFiles();
		}
}

/******************************************************************************
 UpdateActionsMenu (virtual)

 ******************************************************************************/

void
SVNListBase::UpdateActionsMenu
	(
	JXTextMenu* menu
	)
{
	menu->EnableItem(kRefreshCmd);

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list);
	if (!list.IsEmpty())
		{
		menu->EnableItem(kAddSelectedFilesCmd);
		menu->EnableItem(kResolveSelectedConflictsCmd);
		}

	GetSelectedFiles(&list, kJTrue);
	if (!list.IsEmpty())
		{
		menu->EnableItem(kCommitSelectedChangesCmd);
		menu->EnableItem(kRemoveSelectedFilesCmd);
		menu->EnableItem(kForceRemoveSelectedFilesCmd);
		menu->EnableItem(kRevertSelectedChangesCmd);
		}
}

/******************************************************************************
 UpdateInfoMenu (virtual)

 ******************************************************************************/

void
SVNListBase::UpdateInfoMenu
	(
	JXTextMenu* menu
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list, kJTrue);
	if (!list.IsEmpty())
		{
		menu->EnableItem(kInfoLogSelectedFilesCmd);
		menu->EnableItem(kPropSelectedFilesCmd);
		menu->EnableItem(kDiffCurrentSelectedFilesCmd);
		menu->EnableItem(kDiffPrevSelectedFilesCmd);

		JString rev;
		if (GetBaseRevision(&rev))
			{
			menu->EnableItem(kDiffEditedSelectedFilesCmd);
			}
		}
}

/******************************************************************************
 OpenSelectedItems (virtual)

 ******************************************************************************/

void
SVNListBase::OpenSelectedItems()
{
	OpenFiles();
}

/******************************************************************************
 GetSelectedFiles (virtual)

 ******************************************************************************/

void
SVNListBase::GetSelectedFiles
	(
	JPtrArray<JString>*	fullNameList,
	const JBoolean		includeDeleted
	)
{
	fullNameList->CleanOut();
	fullNameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);

	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	JString name, fullName;
	JPoint cell;
	const JString& basePath = GetPath();
	while (iter.Next(&cell))
		{
		const JString* line   = itsLineList->NthElement(cell.y);
		name                  = ExtractRelativePath(*line);
		const JBoolean exists = JConvertToAbsolutePath(name, basePath, &fullName);
		if (exists || includeDeleted)
			{
			fullNameList->Append(fullName);
			}
		}
}

/******************************************************************************
 CompareLines class

 ******************************************************************************/

SVNListBase::CompareLines::CompareLines
	(
	SVNListBase* widget
	)
	:
	itsWidget(widget)
{
}

SVNListBase::CompareLines::~CompareLines()
{
}

JOrderedSetT::CompareResult
SVNListBase::CompareLines::Compare
	(
	JString* const & s1,
	JString* const & s2
	)
	const
{
	const JString p1 = itsWidget->ExtractRelativePath(*s1);
	const JString p2 = itsWidget->ExtractRelativePath(*s2);

	const int r = JStringCompare(p1, p2, kJFalse);
	if (r > 0)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

JElementComparison<JString*>*
SVNListBase::CompareLines::Copy()
	const
{
	CompareLines* copy = jnew CompareLines(itsWidget);
	assert( copy != NULL );
	return copy;
}

/******************************************************************************
 SetConnection (private)

 ******************************************************************************/

void
SVNListBase::SetConnection
	(
	const int outFD,
	const int errFD
	)
{
	itsMessageLink = new RecordLink(outFD);
	assert( itsMessageLink != NULL );
	ListenTo(itsMessageLink);

	itsErrorLink = new RecordLink(errFD);
	assert( itsErrorLink != NULL );
	ListenTo(itsErrorLink);
}

/******************************************************************************
 DeleteLinks (private)

 ******************************************************************************/

void
SVNListBase::DeleteLinks()
{
	delete itsMessageLink;
	itsMessageLink = NULL;

	delete itsErrorLink;
	itsErrorLink = NULL;
}
