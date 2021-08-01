/******************************************************************************
 JXTreeListWidget.cpp

	Displays contents of JTreeList.

	Derived classes must override:

		TLWDrawNode
			Draw the specified node inside the given rectangle.

		GetMinCellWidth
			Return the minimum width for the specified cell.  This is used
			to adjust the column widths.

	Derived classes can also override:

		AdjustToTree
			Adjust the table to match the new state of the tree list.
			(The base class adjusts the width of the table.)

	BASE CLASS = JXStyleTable

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JXTreeListWidget.h"
#include "JXTLWAdjustToTreeTask.h"
#include "jXGlobals.h"
#include <JTree.h>
#include <JTreeList.h>
#include <JTreeNode.h>
#include <JPainter.h>
#include "JXColorManager.h"
#include <JTableSelection.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <sstream>
#include <jAssert.h>

const JSize kDefColWidth          = 100;
const JSize kMinCellWidth         = 10;
const JSize kDefaultIndent        = 10;
const JCoordinate kToggleColWidth = 20;
const JSize kDefaultMaxOpenDepth  = 5;

// JBroadcaster messages

const JUtf8Byte* JXTreeListWidget::kNodeDblClicked = "NodeDblClicked::JXTreeListWidget";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTreeListWidget::JXTreeListWidget
	(
	JTreeList*			treeList,
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
	JXStyleTable(scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsToggleOpenColIndex(0),
	itsNodeColIndex(0),
	itsElasticColIndex(0)
{
	itsTreeList = treeList;
	ListenTo(itsTreeList);

	itsMinColWidths = jnew JArray<JSize>;
	assert( itsMinColWidths != nullptr );

	itsReselectNodeList = jnew JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);
	assert( itsReselectNodeList != nullptr );
	ListenTo(itsTreeList->GetTree());

	itsIndentWidth       = kDefaultIndent;
	itsDrawSelectionFlag = true;
	itsAdjustToTreeTask  = nullptr;
	itsToggleDragIndex   = 0;
	itsDNDTargetIndex    = 0;
	itsMaxOpenDepth      = kDefaultMaxOpenDepth;
	itsSavedScrollSetup  = nullptr;

	SetRowBorderInfo(0, GetBackColor());
	SetColBorderInfo(0, GetBackColor());

	ShouldActLike1DList();

	NeedsAdjustToTree();

	AppendRows(itsTreeList->GetElementCount());
	AppendCols(2, kToggleColWidth);		// second width adjusts automatically

	itsToggleOpenColIndex = 1;
	itsNodeColIndex       = 2;
	itsElasticColIndex    = itsNodeColIndex;
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTreeListWidget::~JXTreeListWidget()
{
	jdelete itsTreeList;
	jdelete itsMinColWidths;
	jdelete itsReselectNodeList;
	jdelete itsAdjustToTreeTask;
	jdelete itsSavedScrollSetup;
}

/******************************************************************************
 ScrollToNode

 ******************************************************************************/

void
JXTreeListWidget::ScrollToNode
	(
	const JTreeNode* node
	)
{
	JIndex index;
	if (itsTreeList->FindNode(node, &index))
		{
		TableScrollToCell(JPoint(1, index));
		}
}

/******************************************************************************
 ScrollToShowChildren

	Scroll down as little as necessary to make the children of the
	specified node visible.  Don't scroll the specified node beyond the top
	of the aperture.

 ******************************************************************************/

void
JXTreeListWidget::ScrollToShowChildren
	(
	const JIndex index
	)
{
	const JTreeNode* node = itsTreeList->GetNode(index);
	if (!node->HasChildren())
		{
		return;
		}

	// scroll minimum to show last child

	const JTreeNode* lastChild = node->GetChild(node->GetChildCount());
	ScrollToNode(lastChild);

	// if original node not visible, force it to be

	const JCoordinate y = GetRowTop(index);
	if (y < (GetAperture()).top)
		{
		ScrollTo(0, y);
		}
}

/******************************************************************************
 GetSelectedNodes

 ******************************************************************************/

void
JXTreeListWidget::GetSelectedNodes
	(
	JPtrArray<JTreeNode>* list
	)
{
	JTableSelectionIterator iter(&(GetTableSelection()));

	JPoint cell;
	while (iter.Next(&cell))
		{
		JTreeNode* node = itsTreeList->GetNode(cell.y);
		if (!list->Includes(node))
			{
			list->Append(node);
			}
		}
}

/******************************************************************************
 SelectNodes

	This does not clear the selection before selecting the specified nodes.

 ******************************************************************************/

void
JXTreeListWidget::SelectNodes
	(
	const JPtrArray<JTreeNode>& list
	)
{
	JTableSelection& s = GetTableSelection();

	for (const auto* node : list)
		{
		JIndex index;
		if (itsTreeList->FindNode(node, &index))
			{
			const JPoint cell(itsNodeColIndex, index);
			s.SelectCell(cell);

			if (!s.HasAnchor())
				{
				s.SetAnchor(cell);
				}
			s.SetBoat(cell);
			}
		}
}

/******************************************************************************
 OpenSelectedNodes

 ******************************************************************************/

void
JXTreeListWidget::OpenSelectedNodes
	(
	const bool openSiblings,
	const bool openDescendants
	)
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	iter.MoveTo(kJIteratorStartAtEnd, 0,0);
	JPoint cell, lastCell;
	while (iter.Prev(&cell))
		{
		if (openSiblings)
			{
			itsTreeList->OpenSiblings(cell.y);
			}
		if (openDescendants)
			{
			itsTreeList->OpenDescendants(cell.y, itsMaxOpenDepth);
			}
		if (!openSiblings && !openDescendants)
			{
			itsTreeList->Open(cell.y);
			}

		lastCell = cell;
		}

	if (RowIndexValid(lastCell.y) && !openSiblings)
		{
		ScrollToShowChildren(lastCell.y);
		}
}

/******************************************************************************
 CloseSelectedNodes

 ******************************************************************************/

void
JXTreeListWidget::CloseSelectedNodes
	(
	const bool closeSiblings,
	const bool closeDescendants
	)
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
		{
		if (closeSiblings)
			{
			itsTreeList->CloseSiblings(cell.y);
			}
		if (closeDescendants)
			{
			itsTreeList->CloseDescendants(cell.y);
			}
		if (!closeSiblings && !closeDescendants)
			{
			itsTreeList->Close(cell.y);
			}
		}
}

/******************************************************************************
 GetNodeIndent

 ******************************************************************************/

JSize
JXTreeListWidget::GetNodeIndent
	(
	const JIndex index
	)
	const
{
	return ((itsTreeList->GetNode(index))->GetDepth() - 1) * itsIndentWidth;
}

/******************************************************************************
 GetDefaultIndentWidth (static)

 ******************************************************************************/

JSize
JXTreeListWidget::GetDefaultIndentWidth()
{
	return kDefaultIndent;
}

/******************************************************************************
 IsSelectable (virtual)

	The default is for all cells other than the ToggleOpen column to be
	selectable.  forExtend is true if the cell will be selected as part
	of an "extend selection" operation.

 ******************************************************************************/

bool
JXTreeListWidget::IsSelectable
	(
	const JPoint&	cell,
	const bool	forExtend
	)
	const
{
	return JIndex(cell.x) == itsNodeColIndex;
}

/******************************************************************************
 IsEditable (virtual)

	Derived classes should override this if there are some cells that
	cannot be edited.

 ******************************************************************************/

bool
JXTreeListWidget::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return JIndex(cell.x) != itsToggleOpenColIndex;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

static JPolygon kOpenTriangle, kClosedTriangle;

void
JXTreeListWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if (JIndex(cell.x) == itsNodeColIndex && itsDrawSelectionFlag)
		{
		HilightIfSelected(p, cell, rect);
		}

	const JTreeNode* node = itsTreeList->GetNode(cell.y);
	if (JIndex(cell.x) == itsToggleOpenColIndex && node->IsOpenable())
		{
		p.ShiftOrigin(rect.center());

		const JPolygon* triangle = (itsTreeList->IsOpen(cell.y) ?
									&kOpenTriangle : &kClosedTriangle);
		if (kOpenTriangle.IsEmpty())
			{
			kOpenTriangle.AppendElement(JPoint(-5,  -2));
			kOpenTriangle.AppendElement(JPoint(5, -2));
			kOpenTriangle.AppendElement(JPoint(0, 3));

			kClosedTriangle.AppendElement(JPoint(0, -5));
			kClosedTriangle.AppendElement(JPoint(5, 0));
			kClosedTriangle.AppendElement(JPoint(0, 5));
			}

		if ((itsToggleDragIndex == cell.y && itsMouseInToggleFlag) ||
			itsDNDTargetIndex == JIndex(cell.y))
			{
			p.SetFilling(true);
			p.SetPenColor(JColorManager::GetBlackColor());
			p.Polygon(*triangle);
			}
		else
			{
			p.SetFilling(true);
			p.SetPenColor(JColorManager::GetGrayColor(90));
			p.Polygon(*triangle);

			p.SetFilling(false);
			p.SetPenColor(JColorManager::GetBlackColor());
			p.Polygon(*triangle);
			}

		p.ShiftOrigin(-(rect.center()));
		}

	else if (JIndex(cell.x) == itsNodeColIndex)
		{
		JRect r = rect;
		r.left += GetNodeIndent(cell.y);
		p.SetClipRect(r);
		TLWDrawNode(p, cell, r);
		// Table controls clip rect, so we don't have to reset it.
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXTreeListWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsToggleDragIndex = 0;

	JPoint cell;
	const bool inCell = GetCell(pt, &cell);

	if (button == kJXLeftButton &&
		inCell && JIndex(cell.x) == itsToggleOpenColIndex &&
		(itsTreeList->GetNode(cell.y))->IsOpenable())
		{
		itsMouseInToggleFlag = true;
		itsToggleDragIndex   = cell.y;
		TableRefreshCell(cell);
		}
	else if (button == kJXLeftButton && clickCount == 2 &&
			 inCell && JIndex(cell.x) != itsToggleOpenColIndex)
		{
		Broadcast(NodeDblClicked(itsTreeList->GetNode(cell.y), cell));
		}
	else
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXTreeListWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (!itsTreeList->IndexValid(itsToggleDragIndex))
		{
		itsToggleDragIndex = 0;
		}

	JPoint cell;
	if (itsToggleDragIndex > 0 &&
		GetCell(pt, &cell) &&
		JIndex(cell.x) == itsToggleOpenColIndex &&
		cell.y == itsToggleDragIndex)
		{
		if (!itsMouseInToggleFlag)
			{
			itsMouseInToggleFlag = true;
			TableRefreshCell(itsToggleDragIndex, itsToggleOpenColIndex);
			}
		}
	else if (itsToggleDragIndex > 0 && itsMouseInToggleFlag)
		{
		itsMouseInToggleFlag = false;
		TableRefreshCell(itsToggleDragIndex, itsToggleOpenColIndex);
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXTreeListWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (!itsTreeList->IndexValid(itsToggleDragIndex))
		{
		itsToggleDragIndex = 0;
		}

	if (itsToggleDragIndex > 0 && itsMouseInToggleFlag)
		{
		ToggleNode(itsToggleDragIndex, modifiers.shift(), modifiers.meta());
		Refresh();
		}

	itsMouseInToggleFlag = false;
	itsToggleDragIndex   = 0;
}

/******************************************************************************
 ToggleNode

 ******************************************************************************/

void
JXTreeListWidget::ToggleNode
	(
	const JIndex	index,
	const bool	siblings,
	const bool	descendants
	)
{
	const bool wasOpen = itsTreeList->IsOpen(index);

	if (siblings && wasOpen)
		{
		JXGetApplication()->DisplayBusyCursor();
		itsTreeList->CloseSiblings(index);
		}
	else if (siblings)
		{
		JXGetApplication()->DisplayBusyCursor();
		itsTreeList->OpenSiblings(index);
		}

	if (descendants && wasOpen)
		{
		JXGetApplication()->DisplayBusyCursor();
		itsTreeList->CloseDescendants(index);
		}
	else if (descendants)
		{
		JXGetApplication()->DisplayBusyCursor();
		itsTreeList->OpenDescendants(index, itsMaxOpenDepth);
		}

	if (!siblings && !descendants)
		{
		itsTreeList->Toggle(index);
		}

	if (!siblings && !wasOpen)
		{
		ScrollToShowChildren(index);
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXTreeListWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJLeftArrow && !IsEditing())
		{
		CloseSelectedNodes(modifiers.shift(), modifiers.meta());
		}
	else if (c == kJRightArrow && !IsEditing())
		{
		OpenSelectedNodes(modifiers.shift(), modifiers.meta());
		}
	else
		{
		JXStyleTable::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXTreeListWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTreeList && message.Is(JTreeList::kNodeInserted))
		{
		const auto* info =
			dynamic_cast<const JTreeList::NodeInserted*>(&message);
		assert( info != nullptr );
		InsertRows(info->GetIndex(), 1);
		NeedsAdjustToTree();
		}

	else if (sender == itsTreeList && message.Is(JTreeList::kNodeRemoved))
		{
		const auto* info =
			dynamic_cast<const JTreeList::NodeRemoved*>(&message);
		assert( info != nullptr );
		RemoveRow(info->GetIndex());
		NeedsAdjustToTree();
		}

	else if (sender == itsTreeList && message.Is(JTreeList::kNodeChanged))
		{
		const auto* info =
			dynamic_cast<const JTreeList::NodeChanged*>(&message);
		assert( info != nullptr );
		TableRefreshRow(info->GetIndex());
		NeedsAdjustToTree();
		}

	else if (sender == itsTreeList &&
			 (message.Is(JTreeList::kNodeOpened) ||
			  message.Is(JTreeList::kNodeClosed)))
		{
		const auto* info =
			dynamic_cast<const JTreeList::NodeMessage*>(&message);
		assert( info != nullptr );
		TableRefreshRow(info->GetIndex());
		}

	else if (sender == itsTreeList->GetTree() &&
			 message.Is(JTree::kPrepareForNodeMove))
		{
		HandlePrepareForNodeMove();
		}

	else if (sender == itsTreeList->GetTree() &&
			 message.Is(JTree::kNodeMoveFinished))
		{
		HandleNodeMoveFinished();
		}

	else
		{
		if (sender == this && message.Is(kColsInserted))
			{
			const auto* info = dynamic_cast<const ColsInserted*>(&message);
			assert( info != nullptr );
			info->AdjustIndex(&itsToggleOpenColIndex);
			info->AdjustIndex(&itsNodeColIndex);
			info->AdjustIndex(&itsElasticColIndex);
			NeedsAdjustToTree();
			}

		else if (sender == this && message.Is(kColsRemoved))
			{
			const auto* info = dynamic_cast<const ColsRemoved*>(&message);
			assert( info != nullptr );
			bool ok = info->AdjustIndex(&itsToggleOpenColIndex);
			assert( ok );
			ok = info->AdjustIndex(&itsNodeColIndex);
			assert( ok );
			info->AdjustIndex(&itsElasticColIndex);		// ok to let it go to zero
			NeedsAdjustToTree();
			}

		else if (sender == this && message.Is(kColMoved))
			{
			const auto* info =
				dynamic_cast<const ColMoved*>(&message);
			assert( info != nullptr );
			info->AdjustIndex(&itsToggleOpenColIndex);
			info->AdjustIndex(&itsNodeColIndex);
			info->AdjustIndex(&itsElasticColIndex);
			itsMinColWidths->MoveElementToIndex(info->GetOrigIndex(), info->GetNewIndex());
			}

		JXStyleTable::Receive(sender, message);
		}
}

/******************************************************************************
 HandlePrepareForNodeMove (private)

 ******************************************************************************/

void
JXTreeListWidget::HandlePrepareForNodeMove()
{
	// save selected nodes

	itsReselectNodeList->RemoveAll();

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
		{
		JTreeNode* node = itsTreeList->GetNode(cell.y);
		if (!itsReselectNodeList->Includes(node))
			{
			itsReselectNodeList->Append(node);
			}
		}

	// save scroll position

	std::ostringstream data;
	WriteScrollSetup(data);

	assert( itsSavedScrollSetup == nullptr );
	itsSavedScrollSetup = jnew JString(data.str(), JUtf8ByteRange());
	assert( itsSavedScrollSetup != nullptr );
}

/******************************************************************************
 HandleNodeMoveFinished (private)

 ******************************************************************************/

void
JXTreeListWidget::HandleNodeMoveFinished()
{
	// restore selection

	(GetTableSelection()).ClearSelection();
	SelectNodes(*itsReselectNodeList);
	itsReselectNodeList->RemoveAll();

	// restore scroll position

	assert( itsSavedScrollSetup != nullptr );
	const std::string s(itsSavedScrollSetup->GetBytes(), itsSavedScrollSetup->GetByteCount());
	std::istringstream input(s);
	ReadScrollSetup(input);

	jdelete itsSavedScrollSetup;
	itsSavedScrollSetup = nullptr;
}

/******************************************************************************
 NeedsAdjustToTree (protected)

 ******************************************************************************/

void
JXTreeListWidget::NeedsAdjustToTree()
{
	if (itsAdjustToTreeTask == nullptr)
		{
		itsAdjustToTreeTask = jnew JXTLWAdjustToTreeTask(this);
		assert( itsAdjustToTreeTask != nullptr );
		itsAdjustToTreeTask->Go();
		}
}

/******************************************************************************
 ForceAdjustToTree

	This function is provided so that you can update the aperture
	immediately instead of waiting until the urgent task does it.

	We need a while loop because AdjustToTree() is virtual so derived
	classes may trigger additional adjustments.

 ******************************************************************************/

void
JXTreeListWidget::ForceAdjustToTree()
{
	while (itsAdjustToTreeTask != nullptr)
		{
		jdelete itsAdjustToTreeTask;
		itsAdjustToTreeTask = nullptr;
		AdjustToTree();
		}
}

/******************************************************************************
 AdjustToTree (virtual protected)

 ******************************************************************************/

void
JXTreeListWidget::AdjustToTree()
{
	itsMinColWidths->RemoveAll();

	JPoint cell;
	const JSize rowCount = GetRowCount();
	const JSize colCount = GetColCount();
	for (JIndex j=1; j<=colCount; j++)
		{
		if (j == itsToggleOpenColIndex)
			{
			itsMinColWidths->AppendElement(kToggleColWidth);
			}
		else
			{
			cell.x         = j;
			JSize maxWidth = 0;
			for (JIndex i=1; i<=rowCount; i++)
				{
				cell.y            = i;
				const JSize width = JMax(kMinCellWidth, GetMinCellWidth(cell));
				if (width > maxWidth)
					{
					maxWidth = width;
					}
				}

			if (maxWidth == 0)
				{
				maxWidth = kDefColWidth;
				}
			itsMinColWidths->AppendElement(maxWidth);
			}
		}

	AdjustColWidths();
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXTreeListWidget::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXStyleTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
JXTreeListWidget::AdjustColWidths()
{
	if (itsAdjustToTreeTask != nullptr)
		{
		return;
		}

	const JSize colCount = GetColCount();
	if (itsMinColWidths->GetElementCount() != colCount)
		{
		NeedsAdjustToTree();
		return;
		}

	for (JIndex i=1; i<=colCount; i++)
		{
		SetColWidth(i, itsMinColWidths->GetElement(i));
		}

	if (ColIndexValid(itsElasticColIndex))
		{
		const JCoordinate minReqWidth = itsMinColWidths->GetElement(itsElasticColIndex);
		if (minReqWidth > 0)
			{
			const JCoordinate availWidth =
				GetApertureWidth() - (GetBoundsWidth() - GetColWidth(itsElasticColIndex));
			SetColWidth(itsElasticColIndex, JMax(minReqWidth, availWidth));
			}
		}
}
