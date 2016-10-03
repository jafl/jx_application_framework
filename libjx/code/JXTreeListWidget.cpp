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

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXTreeListWidget.h>
#include <JXTLWAdjustToTreeTask.h>
#include <jXGlobals.h>
#include <JTree.h>
#include <JTreeList.h>
#include <JTreeNode.h>
#include <JPainter.h>
#include <JColormap.h>
#include <JTableSelection.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <sstream>
#include <jAssert.h>

const JSize kDefColWidth          = 100;
const JSize kDefaultIndent        = 10;
const JCoordinate kToggleColWidth = 20;
const JSize kDefaultMaxOpenDepth  = 5;

// JBroadcaster messages

const JCharacter* JXTreeListWidget::kNodeDblClicked = "NodeDblClicked::JXTreeListWidget";

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

	itsMinColWidths = new JArray<JSize>;
	assert( itsMinColWidths != NULL );

	itsReselectNodeList = new JPtrArray<JTreeNode>(JPtrArrayT::kForgetAll);
	assert( itsReselectNodeList != NULL );
	ListenTo(itsTreeList->GetTree());

	itsIndentWidth       = kDefaultIndent;
	itsDrawSelectionFlag = kJTrue;
	itsAdjustToTreeTask  = NULL;
	itsToggleDragIndex   = 0;
	itsDNDTargetIndex    = 0;
	itsMaxOpenDepth      = kDefaultMaxOpenDepth;
	itsSavedScrollSetup  = NULL;

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
	delete itsTreeList;
	delete itsMinColWidths;
	delete itsReselectNodeList;
	delete itsAdjustToTreeTask;
	delete itsSavedScrollSetup;
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

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JTreeNode* node = list.NthElement(i);
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
	const JBoolean openSiblings,
	const JBoolean openDescendants
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
	const JBoolean closeSiblings,
	const JBoolean closeDescendants
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
	selectable.  forExtend is kJTrue if the cell will be selected as part
	of an "extend selection" operation.

 ******************************************************************************/

JBoolean
JXTreeListWidget::IsSelectable
	(
	const JPoint&	cell,
	const JBoolean	forExtend
	)
	const
{
	return JI2B( JIndex(cell.x) == itsNodeColIndex );
}

/******************************************************************************
 IsEditable (virtual)

	Derived classes should override this if there are some cells that
	cannot be edited.

 ******************************************************************************/

JBoolean
JXTreeListWidget::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return JI2B( JIndex(cell.x) != itsToggleOpenColIndex );
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
		p.ShiftOrigin(rect.topLeft());

		const JPolygon* triangle = (itsTreeList->IsOpen(cell.y) ?
									&kOpenTriangle : &kClosedTriangle);
		if (kOpenTriangle.IsEmpty())
			{
			kOpenTriangle.AppendElement(JPoint(5,  6));
			kOpenTriangle.AppendElement(JPoint(15, 6));
			kOpenTriangle.AppendElement(JPoint(10, 11));

			kClosedTriangle.AppendElement(JPoint(10, 3));
			kClosedTriangle.AppendElement(JPoint(15, 8));
			kClosedTriangle.AppendElement(JPoint(10, 13));
			}

		const JColormap* colormap = p.GetColormap();

		if ((itsToggleDragIndex == cell.y && itsMouseInToggleFlag) ||
			itsDNDTargetIndex == JIndex(cell.y))
			{
			p.SetFilling(kJTrue);
			p.SetPenColor(colormap->GetBlackColor());
			p.Polygon(*triangle);
			}
		else
			{
			p.SetFilling(kJTrue);
			p.SetPenColor(colormap->GetGrayColor(90));
			p.Polygon(*triangle);

			p.SetFilling(kJFalse);
			p.SetPenColor(colormap->GetBlackColor());
			p.Polygon(*triangle);
			}

		p.ShiftOrigin(-(rect.topLeft()));
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
	const JBoolean inCell = GetCell(pt, &cell);

	if (button == kJXLeftButton &&
		inCell && JIndex(cell.x) == itsToggleOpenColIndex &&
		(itsTreeList->GetNode(cell.y))->IsOpenable())
		{
		itsMouseInToggleFlag = kJTrue;
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
			itsMouseInToggleFlag = kJTrue;
			TableRefreshCell(itsToggleDragIndex, itsToggleOpenColIndex);
			}
		}
	else if (itsToggleDragIndex > 0 && itsMouseInToggleFlag)
		{
		itsMouseInToggleFlag = kJFalse;
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

	itsMouseInToggleFlag = kJFalse;
	itsToggleDragIndex   = 0;
}

/******************************************************************************
 ToggleNode

 ******************************************************************************/

void
JXTreeListWidget::ToggleNode
	(
	const JIndex	index,
	const JBoolean	siblings,
	const JBoolean	descendants
	)
{
	const JBoolean wasOpen = itsTreeList->IsOpen(index);

	if (siblings && wasOpen)
		{
		(JXGetApplication())->DisplayBusyCursor();
		itsTreeList->CloseSiblings(index);
		}
	else if (siblings)
		{
		(JXGetApplication())->DisplayBusyCursor();
		itsTreeList->OpenSiblings(index);
		}

	if (descendants && wasOpen)
		{
		(JXGetApplication())->DisplayBusyCursor();
		itsTreeList->CloseDescendants(index);
		}
	else if (descendants)
		{
		(JXGetApplication())->DisplayBusyCursor();
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
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJLeftArrow && !IsEditing())
		{
		CloseSelectedNodes(modifiers.shift(), modifiers.meta());
		}
	else if (key == kJRightArrow && !IsEditing())
		{
		OpenSelectedNodes(modifiers.shift(), modifiers.meta());
		}
	else
		{
		JXStyleTable::HandleKeyPress(key, modifiers);
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
		const JTreeList::NodeInserted* info =
			dynamic_cast<const JTreeList::NodeInserted*>(&message);
		assert( info != NULL );
		InsertRows(info->GetIndex(), 1);
		NeedsAdjustToTree();
		}

	else if (sender == itsTreeList && message.Is(JTreeList::kNodeRemoved))
		{
		const JTreeList::NodeRemoved* info =
			dynamic_cast<const JTreeList::NodeRemoved*>(&message);
		assert( info != NULL );
		RemoveRow(info->GetIndex());
		NeedsAdjustToTree();
		}

	else if (sender == itsTreeList && message.Is(JTreeList::kNodeChanged))
		{
		const JTreeList::NodeChanged* info =
			dynamic_cast<const JTreeList::NodeChanged*>(&message);
		assert( info != NULL );
		TableRefreshRow(info->GetIndex());
		NeedsAdjustToTree();
		}

	else if (sender == itsTreeList &&
			 (message.Is(JTreeList::kNodeOpened) ||
			  message.Is(JTreeList::kNodeClosed)))
		{
		const JTreeList::NodeMessage* info =
			dynamic_cast<const JTreeList::NodeMessage*>(&message);
		assert( info != NULL );
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
			const ColsInserted* info = dynamic_cast<const ColsInserted*>(&message);
			assert( info != NULL );
			info->AdjustIndex(&itsToggleOpenColIndex);
			info->AdjustIndex(&itsNodeColIndex);
			info->AdjustIndex(&itsElasticColIndex);
			NeedsAdjustToTree();
			}

		else if (sender == this && message.Is(kColsRemoved))
			{
			const ColsRemoved* info = dynamic_cast<const ColsRemoved*>(&message);
			assert( info != NULL );
			JBoolean ok = info->AdjustIndex(&itsToggleOpenColIndex);
			assert( ok );
			ok = info->AdjustIndex(&itsNodeColIndex);
			assert( ok );
			info->AdjustIndex(&itsElasticColIndex);		// ok to let it go to zero
			NeedsAdjustToTree();
			}

		else if (sender == this && message.Is(kColMoved))
			{
			const ColMoved* info =
				dynamic_cast<const ColMoved*>(&message);
			assert( info != NULL );
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

	assert( itsSavedScrollSetup == NULL );
	itsSavedScrollSetup = new JString(data.str());
	assert( itsSavedScrollSetup != NULL );
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

	assert( itsSavedScrollSetup != NULL );
	const std::string s(itsSavedScrollSetup->GetCString(), itsSavedScrollSetup->GetLength());
	std::istringstream input(s);
	ReadScrollSetup(input);

	delete itsSavedScrollSetup;
	itsSavedScrollSetup = NULL;
}

/******************************************************************************
 NeedsAdjustToTree (protected)

 ******************************************************************************/

void
JXTreeListWidget::NeedsAdjustToTree()
{
	if (itsAdjustToTreeTask == NULL)
		{
		itsAdjustToTreeTask = new JXTLWAdjustToTreeTask(this);
		assert( itsAdjustToTreeTask != NULL );
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
	while (itsAdjustToTreeTask != NULL)
		{
		delete itsAdjustToTreeTask;
		itsAdjustToTreeTask = NULL;
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
				const JSize width = GetMinCellWidth(cell);
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
	if (itsAdjustToTreeTask != NULL)
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
