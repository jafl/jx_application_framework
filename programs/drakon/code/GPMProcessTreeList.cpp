/******************************************************************************
 GPMProcessTreeList.cpp

	BASE CLASS = JXNamedTreeListWidget

	Copyright (C) 2006 by John Lindal.

 *****************************************************************************/

#include "GPMProcessTreeList.h"
#include "GPMProcessTable.h"
#include "GPMProcessList.h"
#include "gpmGlobals.h"

#include <JXTextMenu.h>
#include <JXTEBase.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <JXColorManager.h>

#include <JTableSelection.h>
#include <JTreeList.h>
#include <JPainter.h>
#include <jAssert.h>

#include <jx_edit_clear.xpm>

const JCoordinate kHMarginWidth	= 5;

/******************************************************************************
 Constructor

 *****************************************************************************/

GPMProcessTreeList::GPMProcessTreeList
	(
	GPMProcessList*		list,
	JNamedTreeList*		treeList,
	JXTEBase*			fullCmdDisplay,
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
	JXNamedTreeListWidget(treeList, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsList(list),
	itsContextMenu(nullptr),
	itsSelectedEntry(nullptr),
	itsFullCmdDisplay(fullCmdDisplay),
	itsZombieImage(nullptr)
{
	AppendCols(GPMProcessList::kTreeCount - 2);
	SetColWidth(GPMProcessList::kTreeState,  20);
	SetColWidth(GPMProcessList::kTreePID,    50);
	SetColWidth(GPMProcessList::kTreeUser,   75);
	SetColWidth(GPMProcessList::kTreeNice,   40);
	SetColWidth(GPMProcessList::kTreeSize,   60);
	SetColWidth(GPMProcessList::kTreeCPU,    50);
	SetColWidth(GPMProcessList::kTreeMemory, 60);
	SetColWidth(GPMProcessList::kTreeTime,   60);

	itsZombieImage = GetDisplay()->GetImageCache()->GetImage(jx_edit_clear);

	itsContextMenu = GPMProcessTable::CreateContextMenu(this);
	ListenTo(itsContextMenu);

	ListenTo(itsList);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMProcessTreeList::~GPMProcessTreeList()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GPMProcessTreeList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsList && message.Is(GPMProcessList::kPrepareForUpdate))
		{
		if (itsSelectedEntry != nullptr)
			{
			StopListening(itsSelectedEntry);
			itsSelectedEntry = nullptr;
			}

		if (GetSelectedProcess(&itsSelectedEntry))
			{
			ClearWhenGoingAway(itsSelectedEntry, &itsSelectedEntry);
			}
		}

	else if (sender == itsList && message.Is(GPMProcessList::kListChanged))
		{
		JTableSelection& s = GetTableSelection();
		s.ClearSelection();

		JIndex index;
		if (itsSelectedEntry != nullptr &&
			GetTreeList()->FindNode(itsSelectedEntry, &index))
			{
			s.SelectRow(index);

			StopListening(itsSelectedEntry);
			itsSelectedEntry = nullptr;
			}

		Refresh();
		}

	else if (sender == itsContextMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateContextMenu();
		}
	else if (sender == itsContextMenu && message.Is(JXMenu::kItemSelected))
		{
		 const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleContextMenu(selection->GetIndex());
		}

	else
		{
		if (sender == &(GetTableSelection()) && message.Is(JTableData::kRectChanged))
			{
			const GPMProcessEntry* entry;
			if (IsVisible() && GetSelectedProcess(&entry))
				{
				itsFullCmdDisplay->GetText()->SetText(entry->GetFullCommand());
				}
			}

		JXNamedTreeListWidget::Receive(sender, message);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GPMProcessTreeList::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	GPMProcessTable::DrawRowBackground(p, cell, rect, JColorManager::GetGrayColor(95));

	if (cell.x == GPMProcessList::kTreeOpenClose ||
		cell.x == GPMProcessList::kTreeCommand)
		{
		JXNamedTreeListWidget::TableDrawCell(p, cell, rect);
		return;
		}

	HilightIfSelected(p, cell, rect);

	const JTreeNode* node        = GetTreeList()->GetNode(cell.y);
	const GPMProcessEntry& entry = * dynamic_cast<const GPMProcessEntry*>(node);

	JString str;
	JPainter::HAlignment halign = JPainter::kHAlignRight;
	if (cell.x == GPMProcessList::kTreeState)
		{
		GPMProcessTable::DrawProcessState(entry, p, rect, *itsZombieImage);
		}
	else if (cell.x == GPMProcessList::kTreePID)
		{
		str	= JString((JUInt64) entry.GetPID());
		}
	else if (cell.x == GPMProcessList::kTreeUser)
		{
		str    = entry.GetUser();
		halign = JPainter::kHAlignLeft;
		}
/*	else if (cell.x == GPMProcessList::kTreePPID)
		{
		str	= JString((JUInt64) entry.GetPPID());
		}
	else if (cell.x == GPMProcessList::kTreePriority)
		{
		str	= JString((JUInt64) entry.GetPriority());
		}
*/	else if (cell.x == GPMProcessList::kTreeNice)
		{
		str	= JString((JUInt64) entry.GetNice());
		}
	else if (cell.x == GPMProcessList::kTreeSize)
		{
		str	= JString((JUInt64) entry.GetSize());
		}
/*	else if (cell.x == GPMProcessList::kTreeResident)
		{
		str	= JString((JUInt64) entry.GetResident());
		}
	else if (cell.x == GPMProcessList::kTreeShare)
		{
		str	= JString((JUInt64) entry.GetShare());
		}
*/	else if (cell.x == GPMProcessList::kTreeCPU)
		{
		str	= JString(entry.GetPercentCPU(), 1);
		}
	else if (cell.x == GPMProcessList::kTreeMemory)
		{
		str	= JString(entry.GetPercentMemory(), 1);
		}
	else if (cell.x == GPMProcessList::kTreeTime)
		{
		str	= JString((JUInt64) entry.GetTime());
		}

	JRect r  = rect;
	r.left  += kHMarginWidth;
	r.right -= kHMarginWidth;
	p.JPainter::String(r, str, halign, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GPMProcessTreeList::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (ScrollForWheel(button, modifiers))
		{
		return;
		}

	JTableSelection& s	= GetTableSelection();
	s.ClearSelection();

	JPoint cell;
	if (!GetCell(pt, &cell))
		{
		return;
		}

	s.SelectRow(cell.y);

	if (cell.x == GPMProcessList::kTreeState)
		{
		const GPMProcessEntry* entry;
		const bool ok = GetSelectedProcess(&entry);
		assert( ok );

		GPMProcessTable::ToggleProcessState(*entry);
		itsList->Update();
		}
	else if (cell.x != GPMProcessList::kTreeOpenClose &&
			 button == kJXRightButton)
		{
		itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else
		{
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
GPMProcessTreeList::UpdateContextMenu()
{
	const GPMProcessEntry* entry;
	if (GetSelectedProcess(&entry))
		{
		GPMProcessTable::UpdateContextMenu(itsContextMenu, *entry);
		}
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
GPMProcessTreeList::HandleContextMenu
	(
	const JIndex index
	)
{
	const GPMProcessEntry* entry;
	if (GetSelectedProcess(&entry))
		{
		GPMProcessTable::HandleContextMenu(index, *entry, itsList);
		}
}

/******************************************************************************
 GetSelectedProcess

 ******************************************************************************/

bool
GPMProcessTreeList::GetSelectedProcess
	(
	const GPMProcessEntry** entry
	)
	const
{
	JPtrArray<JTreeNode> list(JPtrArrayT::kForgetAll);
	const_cast<GPMProcessTreeList*>(this)->GetSelectedNodes(&list);

	if (!list.IsEmpty())
		{
		*entry = dynamic_cast<GPMProcessEntry*>(list.GetFirstElement());
		return true;
		}
	else
		{
		*entry = nullptr;
		return false;
		}
}

/******************************************************************************
 SelectProcess

 ******************************************************************************/

void
GPMProcessTreeList::SelectProcess
	(
	const GPMProcessEntry& entry
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();

	GetTreeList()->MakeVisible(&entry);

	JIndex index;
	if (GetTreeList()->FindNode(&entry, &index))
		{
		s.SelectRow(index);
		ScrollToNode(&entry);
		}
}
