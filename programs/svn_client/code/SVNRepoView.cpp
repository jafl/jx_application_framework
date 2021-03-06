/******************************************************************************
 SVNRepoView.cc

	BASE CLASS = JXNamedTreeListWidget, SVNTabBase

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include "SVNRepoView.h"
#include "SVNRepoTree.h"
#include "SVNRepoTreeNode.h"
#include "SVNRepoTreeList.h"
#include "SVNRepoDragData.h"
#include "SVNMainDirector.h"
#include "SVNBeginEditingTask.h"
#include "SVNCreateRepoDirectoryDialog.h"
#include "SVNDuplicateRepoItemDialog.h"
#include "svnMenus.h"
#include <JXWindow.h>
#include <JXInputField.h>
#include <JXTextMenu.h>
#include <JXDNDManager.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JXWebBrowser.h>
#include <JXImageCache.h>
#include <JXDragPainter.h>
#include <JXColormap.h>
#include <JXTimerTask.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <X11/keysym.h>
#include <JTreeList.h>
#include <JTreeNode.h>
#include <JTableSelection.h>
#include <JFontManager.h>
#include <JSubstitute.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jProcessUtil.h>
#include <jMouseUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

#include <jx_folder_small.xpm>
#include <jx_folder_selected_small.xpm>
#include <jx_plain_file_small.xpm>
#include <jx_plain_file_selected_small.xpm>
#include <svn_repo_error_small.xpm>
#include <svn_repo_error_selected_small.xpm>
#include <svn_repo_busy.xpm>
#include <svn_repo_busy_selected.xpm>

static const JCharacter* kDNDAtomName = "Item::SVNRepoDragData";

const Time kRefreshInterval    = 60000;		// 1 min (milliseconds)
const JCoordinate kMarginWidth = 6;

enum
{
	kRevColIndex = 3,
	kAgeColIndex,
	kAuthorColIndex,
	kSizeColIndex
};

const JSize kExtraColCount = 4;

// Context menu

static const JCharacter* kContextMenuStr =
	"    Compare with edited"
	"  | Compare with current"
	"  | Compare with previous"
	"%l| Info & Log"
	"  | Properties"
	"%l| Check out..."
	"%l| Show in file manager";

enum
{
	kDiffEditedSelectedFilesCtxCmd = 1,
	kDiffCurrentSelectedFilesCtxCmd,
	kDiffPrevSelectedFilesCtxCmd,
	kInfoLogSelectedFilesCtxCmd,
	kPropSelectedFilesCtxCmd,
	kCheckOutSelectedDirCtxCmd,
	kShowSelectedFilesCtxCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNRepoView::SVNRepoView
	(
	SVNMainDirector*	director,
	const JCharacter*	repoPath,
	const JCharacter*	repoRevision,
	JXTextMenu*			editMenu,
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
	JXNamedTreeListWidget(BuildTreeList(repoPath, repoRevision), scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	SVNTabBase(director),
	itsEditMenu(editMenu),
	itsContextMenu(NULL),
	itsCreateDirectoryDialog(NULL),
	itsDuplicateItemDialog(NULL),
	itsCopyItemDialog(NULL),
	itsCopyItemDestNode(NULL),
	itsEditTask(NULL),
	itsSortNode(NULL)
{
	itsRepoTreeList = dynamic_cast<SVNRepoTreeList*>(GetTreeList());
	assert( itsRepoTreeList != NULL );

	itsRepoTree = itsRepoTreeList->GetRepoTree();

	itsRefreshTask = jnew JXTimerTask(kRefreshInterval);
	assert( itsRefreshTask != NULL );
	itsRefreshTask->Start();
	ListenTo(itsRefreshTask);

	AppendCols(kExtraColCount);
	SetSelectionBehavior(kJTrue, kJTrue);
	ShouldHilightTextOnly(kJTrue);
	WantInput(kJTrue);

	itsAltRowColor = GetColormap()->GetGrayColor(95);
	itsDNDDataType = GetDisplay()->RegisterXAtom(kDNDAtomName);

	FitToEnclosure();

	ListenTo(itsEditMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNRepoView::~SVNRepoView()
{
	jdelete itsRepoTree;
	jdelete itsRefreshTask;
	jdelete itsEditTask;
}

/******************************************************************************
 BuildTreeList (static private)

 ******************************************************************************/

SVNRepoTreeList*
SVNRepoView::BuildTreeList
	(
	const JCharacter*	repoPath,
	const JCharacter*	repoRevision
	)
{
	SVNRepoTreeNode* root =
		jnew SVNRepoTreeNode(NULL, repoPath, repoRevision, "",
							SVNRepoTreeNode::kDirectory, 0, 0, "", 0);
	assert( root != NULL );

	SVNRepoTree* tree = jnew SVNRepoTree(root);
	assert( tree != NULL );

	SVNRepoTreeList* list = jnew SVNRepoTreeList(tree);
	assert( list != NULL );

	return list;
}

/******************************************************************************
 RefreshContent (virtual)

 ******************************************************************************/

void
SVNRepoView::RefreshContent()
{
	GetDNDManager()->CancelDND();
	CancelEditing();
	GetWindow()->EndDrag(this, JPoint(0,0),
						   GetDisplay()->GetLatestButtonStates(),
						   GetDisplay()->GetLatestKeyModifiers());

	itsRepoTree->Update(itsRepoTreeList);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
SVNRepoView::TableDrawCell
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

	const JString str = GetCellString(cell);
	if (!str.IsEmpty())
		{
		p.SetFont(GetFont());

		JRect r = rect;
		if (cell.x == kRevColIndex)
			{
			r.right -= kMarginWidth;
			}
		else
			{
			r.left += kMarginWidth;
			}

		p.String(r, str,
				 cell.x == kRevColIndex ? JPainter::kHAlignRight : JPainter::kHAlignLeft,
				 JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 GetImage (virtual protected)

 ******************************************************************************/

JBoolean
SVNRepoView::GetImage
	(
	const JIndex	index,
	const JXImage** image
	)
	const
{
	JXImageCache* c         = JXGetApplication()->GetDisplay(1)->GetImageCache();
	const JBoolean selected = (GetTableSelection()).IsSelected(index, GetNodeColIndex());

	const SVNRepoTreeNode* node      = itsRepoTreeList->GetRepoNode(index);
	const SVNRepoTreeNode::Type type = node->GetType();
	if (type == SVNRepoTreeNode::kDirectory && selected)
		{
		*image = c->GetImage(jx_folder_selected_small);
		}
	else if (type == SVNRepoTreeNode::kDirectory)
		{
		*image = c->GetImage(jx_folder_small);
		}
	else if (type == SVNRepoTreeNode::kError && selected)
		{
		*image = c->GetImage(svn_repo_error_selected_small);
		}
	else if (type == SVNRepoTreeNode::kError)
		{
		*image = c->GetImage(svn_repo_error_small);
		}
	else if (type == SVNRepoTreeNode::kBusy && selected)
		{
		*image = c->GetImage(svn_repo_busy_selected);
		}
	else if (type == SVNRepoTreeNode::kBusy)
		{
		*image = c->GetImage(svn_repo_busy);
		}
	else if (selected)
		{
		*image = c->GetImage(jx_plain_file_selected_small);
		}
	else
		{
		*image = c->GetImage(jx_plain_file_small);
		}
	return kJTrue;
}

/******************************************************************************
 GetCellString (private)

 ******************************************************************************/

JString
SVNRepoView::GetCellString
	(
	const JPoint& cell
	)
	const
{
	const SVNRepoTreeNode* node      = itsRepoTreeList->GetRepoNode(cell.y);
	const SVNRepoTreeNode::Type type = node->GetType();
	if (type == SVNRepoTreeNode::kError || type == SVNRepoTreeNode::kBusy)
		{
		return JString();
		}
	else if (cell.x == kRevColIndex)
		{
		return JString(node->GetRevision(), JString::kBase10);
		}
	else if (cell.x == kAgeColIndex)
		{
		return node->GetAgeString();
		}
	else if (cell.x == kAuthorColIndex)
		{
		return node->GetAuthor();
		}
	else if (cell.x == kSizeColIndex && type == SVNRepoTreeNode::kFile)
		{
		return JPrintFileSize(node->GetFileSize());
		}
	else if (cell.x == kSizeColIndex && type == SVNRepoTreeNode::kDirectory)
		{
		return "-";
		}
	else
		{
		return JString();
		}
}

/******************************************************************************
 GetMinCellWidth (virtual protected)

 ******************************************************************************/

JSize
SVNRepoView::GetMinCellWidth
	(
	const JPoint& cell
	)
	const
{
	if (JIndex(cell.x) > GetNodeColIndex())
		{
		return 2 * kMarginWidth + GetFont().GetStringWidth(GetCellString(cell));
		}
	else
		{
		return JXNamedTreeListWidget::GetMinCellWidth(cell);
		}
}

/******************************************************************************
 AdjustToTree (virtual protected)

 ******************************************************************************/

void
SVNRepoView::AdjustToTree()
{
	const JFontStyle error(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetRedColor());
	const JFontStyle busy(GetColormap()->GetGrayColor(60));

	const JSize count = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		const SVNRepoTreeNode* node      = itsRepoTreeList->GetRepoNode(i);
		const SVNRepoTreeNode::Type type = node->GetType();
		if (type == SVNRepoTreeNode::kError)
			{
			SetCellStyle(JPoint(GetNodeColIndex(), i), error);
			}
		else if (type == SVNRepoTreeNode::kBusy)
			{
			SetCellStyle(JPoint(GetNodeColIndex(), i), busy);
			}
		}

	JXNamedTreeListWidget::AdjustToTree();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNRepoView::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
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
		UpdateContextMenu();
		}
	else if (sender == itsContextMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleContextMenu(selection->GetIndex());
		}

	else if (sender == itsRefreshTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		Refresh();
		}

	else if (sender == itsCreateDirectoryDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );

		if (info->Successful())
			{
			CreateDirectory1();
			}

		itsCreateDirectoryDialog = NULL;
		}

	else if (sender == itsDuplicateItemDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );

		if (info->Successful())
			{
			DuplicateItem1();
			}

		itsDuplicateItemDialog = NULL;
		}

	else if (sender == itsCopyItemDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );

		if (info->Successful())
			{
			CopyItem();
			}

		itsCopyItemDialog   = NULL;
		itsCopyItemDestNode = NULL;
		}

	else
		{
		JXNamedTreeListWidget::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
SVNRepoView::UpdateEditMenu()
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
SVNRepoView::HandleEditMenu
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
		CopySelectedFiles(kJFalse);
		}
	else if (*id == kSVNCopyFullPathAction)
		{
		CopySelectedFiles(kJTrue);
		}
	else if (*id == kJXSelectAllAction)
		{
		(GetTableSelection()).SelectAll();
		}
}

/******************************************************************************
 CopySelectedFiles (private)

 ******************************************************************************/

void
SVNRepoView::CopySelectedFiles
	(
	const JBoolean fullPath
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list);
	if (list.IsEmpty())
		{
		return;
		}

	if (!fullPath)
		{
		const JSize count = list.GetElementCount();
		JString path, name;
		for (JIndex i=1; i<=count; i++)
			{
			JString* s = list.NthElement(i);
			if (JSplitPathAndName(*s, &path, &name))
				{
				*s = name;
				}
			}
		}

	JXTextSelection* data = jnew JXTextSelection(GetDisplay(), list);
	assert( data != NULL );

	GetSelectionManager()->SetData(kJXClipboardName, data);
}

/******************************************************************************
 IsEditable (virtual)

	Derived classes should override this if there are some cells that
	cannot be edited.

 ******************************************************************************/

JBoolean
SVNRepoView::IsEditable
	(
	const JPoint& cell
	)
	const
{
	if (JIndex(cell.x) != GetNodeColIndex())
		{
		return kJFalse;
		}

	SVNRepoTreeNode* node      = itsRepoTreeList->GetRepoNode(cell.y);
	SVNRepoTreeNode::Type type = node->GetType();
	return JI2B( type == SVNRepoTreeNode::kFile ||
				 type == SVNRepoTreeNode::kDirectory );
}

/******************************************************************************
 CreateTreeListInput (virtual protected)

 ******************************************************************************/

JXInputField*
SVNRepoView::CreateTreeListInput
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

	return JXNamedTreeListWidget::CreateTreeListInput(cell, enclosure, hSizing, vSizing, x,y, w,h);
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
SVNRepoView::ExtractInputData
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

	SVNRepoTreeNode* node = itsRepoTreeList->GetRepoNode(cell.y);
	const JString newName = input->GetText();	// copy since need after input field gone
	const JBoolean sort   = (GetDisplay()->GetLatestButtonStates()).AllOff();
	const JError err      = node->Rename(newName, sort);
	input                 = NULL;				// nodes sorted => CancelEditing()
	if (!err.OK())
		{
		(JGetStringManager())->ReportError("RenameError::SVNRepoView", err);
		}
	else if (sort)
		{
		ScrollToNode(node);
		}
	else
		{
		itsSortNode = node->GetRepoParent();
		ListenTo(itsSortNode);	// in case it dies
		}
	return err.OK();
}

/******************************************************************************
 HandleMouseHere (virtual protected)

 ******************************************************************************/

void
SVNRepoView::HandleMouseHere
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
SVNRepoView::HandleMouseDown
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

	jdelete itsEditTask;
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
	else if (part == kBeforeImage || part == kAfterText ||
			 (part == kOtherColumn && (GetCellString(cell)).IsEmpty()))
		{
		StartDragRect(pt, button, modifiers);
		}
	else if (button == kJXRightButton)
		{
		if (!s.IsSelected(cell))
			{
			s.ClearSelection();
			s.SetBoat(cell);
			s.SetAnchor(cell);
			s.SelectCell(cell);
			}

		CreateContextMenu();
		itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
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
		itsWaitingToEditFlag = JI2B(part == kInText);
		itsEditCell          = cell;

		if (s.IsSelected(cell.y, GetNodeColIndex()))
			{
			itsClearIfNotDNDFlag = kJTrue;
			}
		else
			{
			s.ClearSelection();
			s.SelectCell(cell.y, GetNodeColIndex());
			}

		itsWaitingForDragFlag = s.GetSingleSelectedCell(&cell);
		}
}

/******************************************************************************
 StartDragRect (private)

 ******************************************************************************/

void
SVNRepoView::StartDragRect
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
SVNRepoView::HandleMouseDrag
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

			JPoint cell;
			(GetTableSelection()).GetSingleSelectedCell(&cell);

			SVNRepoTreeNode* node      = itsRepoTreeList->GetRepoNode(cell.y);
			SVNRepoTreeNode::Type type = node->GetType();
			if (type == SVNRepoTreeNode::kFile ||
				type == SVNRepoTreeNode::kDirectory)
				{
				JString uri = node->GetRepoPath();

				JString rev;
				if (node->GetRepoRevision(&rev))
					{
					uri += "@";
					uri += rev;
					}

				SVNRepoDragData* data = jnew SVNRepoDragData(GetDisplay(), itsDNDDataType, uri);
				assert(data != NULL);

				itsDNDCursorType = (type == SVNRepoTreeNode::kDirectory ? kDNDDirCursor : kDNDFileCursor);
				BeginDND(pt, buttonStates, modifiers, data);
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
SVNRepoView::HandleMouseUp
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
		OpenFiles();
		}
	else if (itsWaitingToEditFlag)
		{
		JPoint cell;
		if (GetCell(itsStartPt, &cell))		// might update dir contents
			{
			s.ClearSelection();
			s.SelectCell(itsEditCell);

			assert( itsEditTask == NULL );
			itsEditTask = jnew SVNBeginEditingTask(this, itsEditCell);
			assert( itsEditTask != NULL );
			itsEditTask->Start();
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
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
SVNRepoView::WillAcceptDrop
	(
	const JArray<Atom>& typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (!EndEditing())
		{
		return kJFalse;
		}

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		const Atom type = typeList.GetElement(i);
		if (type == itsDNDDataType)
			{
			HandleDNDHere(pt, source);
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

 ******************************************************************************/

void
SVNRepoView::HandleDNDEnter()
{
	ClearDNDTargetIndex();
}

/******************************************************************************
 HandleDNDHere (virtual protected)

 ******************************************************************************/

void
SVNRepoView::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget* source
	)
{
	JPoint cell;
	NodePart part;
	if (!GetNode(pt, &cell, &part) ||
		part == kToggleColumn || part == kBeforeImage || part == kAfterText)
		{
		ClearDNDTargetIndex();
		}
	else
		{
		const JTreeList* treeList = GetTreeList();

		JIndex dirIndex = cell.y;

		const JTreeNode* node = treeList->GetNode(dirIndex);
		if (!node->IsOpenable())
			{
			const JTreeNode* parent = node->GetParent();
			if (!treeList->FindNode(parent, &dirIndex))
				{
				dirIndex = 0;	// if file not in subdirectory
				}
			}

		SetDNDTargetIndex(dirIndex);
		}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

 ******************************************************************************/

void
SVNRepoView::HandleDNDLeave()
{
	ClearDNDTargetIndex();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

 ******************************************************************************/

void
SVNRepoView::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>& typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXDNDManager* dndMgr           = GetDNDManager();
	const Atom dndName             = dndMgr->GetDNDSelectionName();
	JXSelectionManager* selManager = GetSelectionManager();
	JIndex dndIndex                = 0;
	const Window dragWindow        = dndMgr->GetDraggerWindow();

	unsigned char* data = NULL;
	JSize dataLength;
	Atom returnType;
	JXSelectionManager::DeleteMethod delMethod;
	if (selManager->GetData(dndName, time, itsDNDDataType,
							&returnType, &data, &dataLength, &delMethod))
		{
		if (returnType == XA_STRING)
			{
			itsCopyItemSrcURI.Set((char*) data, dataLength);

			JString path, initialName;
			JStripTrailingDirSeparator(&itsCopyItemSrcURI);
			JSplitPathAndName(itsCopyItemSrcURI, &path, &initialName);

			JIndex i;
			if (initialName.LocateSubstring("@", &i))
				{
				initialName.RemoveSubstring(i, initialName.GetLength());
				JStripTrailingDirSeparator(&initialName);
				}

			assert( itsCopyItemDestNode == NULL );

			itsCopyItemDestNode =
				GetDNDTargetIndex(&dndIndex) ?
				itsRepoTreeList->GetRepoNode(dndIndex) :
				itsRepoTree->GetRepoRoot();

			assert( itsCopyItemDialog == NULL );

			itsCopyItemDialog =
				jnew JXGetStringDialog(
					GetDirector(), JGetString("CopyItemWindowTitle::SVNRepoView"),
					JGetString("CopyItemPrompt::SVNRepoView"), initialName);
			assert( itsCopyItemDialog != NULL );
			(itsCopyItemDialog->GetInputField())->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
			ListenTo(itsCopyItemDialog);
			itsCopyItemDialog->BeginDialog();
			}

		selManager->DeleteData(&data, delMethod);
		}

	ClearDNDTargetIndex();
	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly during the drag so the drop action can be
	changed based on the current target, buttons, and modifier keys.

 ******************************************************************************/

Atom
SVNRepoView::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionCopyXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

	This is called when the target indicates whether or not it will accept
	the drop.  If !dropAccepted, the action is undefined.  If the drop target
	is not within the same application, target is NULL.

 ******************************************************************************/

void
SVNRepoView::HandleDNDResponse
	(
	const JXContainer*	target,
	const JBoolean		dropAccepted,
	const Atom			action
	)
{
	JXDNDManager* dndMgr = GetDNDManager();
	JCursorIndex cursor;
	if (itsDNDCursorType == kDNDDirCursor)
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
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
SVNRepoView::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (!(GetDisplay()->GetLatestButtonStates()).AllOff())
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

	else
		{
		JXNamedTreeListWidget::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 CreateContextMenu (private)

 ******************************************************************************/

#include "svn_info_log.xpm"

void
SVNRepoView::CreateContextMenu()
{
	if (itsContextMenu == NULL)
		{
		itsContextMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsContextMenu != NULL );
		itsContextMenu->SetMenuItems(kContextMenuStr, "SVNRepoView");
		itsContextMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsContextMenu->SetToHiddenPopupMenu();

		itsContextMenu->SetItemImage(kInfoLogSelectedFilesCtxCmd, svn_info_log);

		ListenTo(itsContextMenu);
		}
}

/******************************************************************************
 UpdateContextMenu (private)
 
 ******************************************************************************/

void
SVNRepoView::UpdateContextMenu()
{
	JString rev;
	itsContextMenu->SetItemEnable(kDiffEditedSelectedFilesCtxCmd,
		JI2B(GetDirector()->HasPath() && GetBaseRevision(&rev)));

	itsContextMenu->SetItemEnable(kDiffCurrentSelectedFilesCtxCmd,
		(itsRepoTree->GetRepoRoot())->GetRepoRevision(&rev));

	itsContextMenu->SetItemEnable(kCheckOutSelectedDirCtxCmd, CanCheckOutSelection());
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
SVNRepoView::HandleContextMenu
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
		if ((itsRepoTree->GetRepoRoot())->GetRepoRevision(&rev))
			{
			CompareCurrent(rev);
			}
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

	else if (index == kCheckOutSelectedDirCtxCmd)
		{
		CheckOutSelection();
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
SVNRepoView::UpdateActionsMenu
	(
	JXTextMenu* menu
	)
{
	menu->EnableItem(kRefreshCmd);

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list);
	if (list.IsEmpty())
		{
		menu->EnableItem(kCreateDirectoryCmd);
		}
	else
		{
		menu->EnableItem(kRemoveSelectedFilesCmd);

		if (list.GetElementCount() == 1)
			{
			menu->EnableItem(kCreateDirectoryCmd);
			menu->EnableItem(kDuplicateSelectedItemCmd);
			}
		}
}

/******************************************************************************
 UpdateInfoMenu (virtual)

 ******************************************************************************/

void
SVNRepoView::UpdateInfoMenu
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
		menu->EnableItem(kDiffPrevSelectedFilesCmd);

		JString rev;
		if (GetDirector()->HasPath() && GetBaseRevision(&rev))
			{
			menu->EnableItem(kDiffEditedSelectedFilesCmd);
			}

		if ((itsRepoTree->GetRepoRoot())->GetRepoRevision(&rev))
			{
			menu->EnableItem(kDiffCurrentSelectedFilesCmd);
			}
		}
}

/******************************************************************************
 GetSelectedFiles (virtual)

 ******************************************************************************/

void
SVNRepoView::GetSelectedFiles
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
	while (iter.Next(&cell))
		{
		SVNRepoTreeNode* node      = itsRepoTreeList->GetRepoNode(cell.y);
		SVNRepoTreeNode::Type type = node->GetType();
		if (type == SVNRepoTreeNode::kFile ||
			type == SVNRepoTreeNode::kDirectory)
			{
			fullNameList->Append(node->GetRepoPath());
			}
		}
}

/******************************************************************************
 GetSelectedFilesForDiff (virtual)

 ******************************************************************************/

void
SVNRepoView::GetSelectedFilesForDiff
	(
	JPtrArray<JString>* fullNameList,
	JArray<JIndex>*		revList
	)
{
	fullNameList->CleanOut();
	fullNameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);
	revList->RemoveAll();

	JString basePath;
	const JBoolean hasCheckout = GetDirector()->GetPath(&basePath);

	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	const JString& baseURL = itsRepoTree->GetRepoPath();
	JString path, fullName;
	JPoint cell;
	while (iter.Next(&cell))
		{
		SVNRepoTreeNode* node      = itsRepoTreeList->GetRepoNode(cell.y);
		SVNRepoTreeNode::Type type = node->GetType();
		if (type == SVNRepoTreeNode::kFile ||
			type == SVNRepoTreeNode::kDirectory)
			{
			const JString& url = node->GetRepoPath();
			if (hasCheckout && url.BeginsWith(baseURL))
				{
				path     = url.GetSubstring(baseURL.GetLength()+1, url.GetLength());
				fullName = JCombinePathAndName(basePath, path);
				fullNameList->Append(fullName);
				}
			else if (!hasCheckout)
				{
				fullNameList->Append(url);
				revList->AppendElement(node->GetRevision());
				}
			}
		}
}

/******************************************************************************
 OpenFiles (virtual)

 ******************************************************************************/

void
SVNRepoView::OpenFiles()
{
	ShowFiles();
}

/******************************************************************************
 ShowFiles (virtual)

 ******************************************************************************/

void
SVNRepoView::ShowFiles()
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	JArray<JIndex> revList;
	GetSelectedFilesForDiff(&list, &revList);

	(JXGetWebBrowser())->ShowFileLocations(list);
}

/******************************************************************************
 GetBaseRevision (virtual)

 ******************************************************************************/

JBoolean
SVNRepoView::GetBaseRevision
	(
	JString* rev
	)
{
	if ((itsRepoTree->GetRepoRoot())->GetRepoRevision(rev))
		{
		return kJTrue;
		}
	else if (GetDirector()->HasPath())
		{
		*rev = "BASE";
		return kJTrue;
		}
	else
		{
		rev->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 ScheduleForRemove (virtual)

 ******************************************************************************/

JBoolean
SVNRepoView::ScheduleForRemove()
{
	if (SVNTabBase::ScheduleForRemove())
		{
		GetDirector()->RefreshRepo();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 CreateDirectory (virtual)

 ******************************************************************************/

JBoolean
SVNRepoView::CreateDirectory()
{
	SVNRepoTreeNode* parentNode;
	JPoint cell;
	if ((GetTableSelection()).GetSingleSelectedCell(&cell))
		{
		parentNode = itsRepoTreeList->GetRepoNode(cell.y);

		SVNRepoTreeNode::Type type = parentNode->GetType();
		if (type == SVNRepoTreeNode::kFile)
			{
			parentNode = parentNode->GetRepoParent();
			}
		else if (type != SVNRepoTreeNode::kDirectory)
			{
			return kJFalse;
			}
		}
	else
		{
		parentNode = itsRepoTree->GetRepoRoot();
		}

	assert( itsCreateDirectoryDialog == NULL );

	itsCreateDirectoryDialog =
		jnew SVNCreateRepoDirectoryDialog(
			GetDirector(), JGetString("CreateDirectoryWindowTitle::SVNRepoView"),
			JGetString("CreateDirectoryPrompt::SVNRepoView"), "", parentNode);
	assert( itsCreateDirectoryDialog != NULL );
	ListenTo(itsCreateDirectoryDialog);
	itsCreateDirectoryDialog->BeginDialog();
	return kJTrue;
}

/******************************************************************************
 CreateDirectory1 (private)

 ******************************************************************************/

static const JCharacter* kSVNCreateDirectoryCmd = "svn mkdir $path";

JBoolean
SVNRepoView::CreateDirectory1()
{
	assert( itsCreateDirectoryDialog != NULL );

	SVNRepoTreeNode* parentNode = itsCreateDirectoryDialog->GetParentNode();
	const JString& newName      = itsCreateDirectoryDialog->GetString();

	const JString path = JPrepArgForExec(
		JCombinePathAndName(parentNode->GetRepoPath(), newName));

	JSubstitute subst;
	subst.DefineVariable("path", path);

	JString cmd = kSVNCreateDirectoryCmd;
	subst.Substitute(&cmd);

	GetDirector()->Execute("CreateDirectoryTab::SVNMainDirector", cmd,
							 kJTrue, kJFalse, kJFalse);
	return kJTrue;
}

/******************************************************************************
 DuplicateItem (virtual)

 ******************************************************************************/

JBoolean
SVNRepoView::DuplicateItem()
{
	JPoint cell;
	if (!(GetTableSelection()).GetSingleSelectedCell(&cell))
		{
		return kJFalse;
		}

	SVNRepoTreeNode* srcNode  = itsRepoTreeList->GetRepoNode(cell.y);

	JString initialName, root, suffix;
	if (JSplitRootAndSuffix(srcNode->GetName(), &root, &suffix))
		{
		root       += JGetString("DuplicateItemSuffix::SVNRepoView");
		initialName = JCombineRootAndSuffix(root, suffix);
		}
	else
		{
		initialName = srcNode->GetName() + JGetString("DuplicateItemSuffix::SVNRepoView");
		}

	assert( itsDuplicateItemDialog == NULL );

	itsDuplicateItemDialog =
		jnew SVNDuplicateRepoItemDialog(
			GetDirector(), JGetString("DuplicateItemWindowTitle::SVNRepoView"),
			JGetString("DuplicateItemPrompt::SVNRepoView"), initialName, srcNode);
	assert( itsDuplicateItemDialog != NULL );
	ListenTo(itsDuplicateItemDialog);
	itsDuplicateItemDialog->BeginDialog();
	return kJTrue;
}

/******************************************************************************
 DuplicateItem1 (private)

 ******************************************************************************/

static const JCharacter* kSVNDuplicateItemCmd = "svn copy $rev $src $dst";

JBoolean
SVNRepoView::DuplicateItem1()
{
	SVNRepoTreeNode* srcNode = itsDuplicateItemDialog->GetSrcNode();
	const JString& newName   = itsDuplicateItemDialog->GetString();

	JString rev;
	if (srcNode->GetRepoRevision(&rev))
		{
		rev.Prepend("-r ");
		}

	const JString src = JPrepArgForExec(srcNode->GetRepoPath());

	JString path, name;
	JSplitPathAndName(srcNode->GetRepoPath(), &path, &name);
	name = JCombinePathAndName(path, newName);

	const JString dst = JPrepArgForExec(name);

	JSubstitute subst;
	subst.DefineVariable("rev", rev);
	subst.DefineVariable("src", src);
	subst.DefineVariable("dst", dst);

	JString cmd = kSVNDuplicateItemCmd;
	subst.Substitute(&cmd);

	GetDirector()->Execute("DuplicateItemTab::SVNMainDirector", cmd,
							 kJTrue, kJFalse, kJFalse);
	return kJTrue;
}

/******************************************************************************
 CopyItem (private)

 ******************************************************************************/

static const JCharacter* kSVNCopyItemCmd = "svn copy $src $dst";

JBoolean
SVNRepoView::CopyItem()
{
	const JString src = JPrepArgForExec(itsCopyItemSrcURI);

	JString dst = JCombinePathAndName(itsCopyItemDestNode->GetRepoPath(), itsCopyItemDialog->GetString());
	dst         = JPrepArgForExec(dst);

	JSubstitute subst;
	subst.DefineVariable("src", src);
	subst.DefineVariable("dst", dst);

	JString cmd = kSVNCopyItemCmd;
	subst.Substitute(&cmd);

	GetDirector()->Execute("CopyItemTab::SVNMainDirector", cmd,
							 kJTrue, kJFalse, kJFalse);
	return kJTrue;
}

/******************************************************************************
 CanCheckOutSelection (virtual)

 ******************************************************************************/

JBoolean
SVNRepoView::CanCheckOutSelection()
	const
{
	JPoint cell;
	if ((GetTableSelection()).GetSingleSelectedCell(&cell))
		{
		SVNRepoTreeNode* node = itsRepoTreeList->GetRepoNode(cell.y);
		if (node->GetType() == SVNRepoTreeNode::kDirectory)
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 CheckOutSelection (virtual)

 ******************************************************************************/

void
SVNRepoView::CheckOutSelection()
{
	JPoint cell;
	if ((GetTableSelection()).GetSingleSelectedCell(&cell))
		{
		SVNRepoTreeNode* node = itsRepoTreeList->GetRepoNode(cell.y);
		if (node->GetType() == SVNRepoTreeNode::kDirectory)
			{
			SVNMainDirector::CheckOut(node->GetRepoPath());
			}
		}
}

/******************************************************************************
 SkipSetup (static)

 ******************************************************************************/

void
SVNRepoView::SkipSetup
	(
	std::istream&		input,
	JFileVersion	vers
	)
{
	SVNRepoTree::SkipSetup(input, vers);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
SVNRepoView::ReadSetup
	(
	const JBoolean	hadSetup,
	std::istream&		input,
	JFileVersion	vers
	)
{
	if (hadSetup)
		{
		itsRepoTree->ReadSetup(input, vers);
		}

	itsRepoTree->Update(itsRepoTreeList, kJFalse);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
SVNRepoView::WriteSetup
	(
	std::ostream& output
	)
	const
{
	itsRepoTree->WriteSetup(output, itsRepoTreeList);
}
