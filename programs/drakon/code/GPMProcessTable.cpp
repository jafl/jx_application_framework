/******************************************************************************
 GPMProcessTable.cpp

	BASE CLASS = JXTable

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#include "GPMProcessTable.h"
#include "GPMProcessList.h"

#include "gpmGlobals.h"

#include <JXColorManager.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <JXTEBase.h>
#include <JXImage.h>

#include <JPainter.h>
#include <JTableSelection.h>

#include <JSimpleProcess.h>
#include <jASCIIConstants.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <jAssert.h>

#include <jx_edit_clear.xpm>
#include "pause.xpm"
#include "cont.xpm"
#include "slow.xpm"
#include "gpm_stop.xpm"

const JCoordinate kDefColWidth	= 100;
const JCoordinate kDefRowHeight	= 20;
const JCoordinate kHMarginWidth	= 5;

static const JCharacter* kContextMenuStr =
	"    End process"
	"  | Kill process"
	"%l| Pause process"
	"  | Continue process"
	"%l| Re-nice process";
//	"  | Send signal to process";

enum
{
	kContextEndCmd = 1,
	kContextKillCmd,
	kContextPauseCmd,
	kContextContinueCmd,
	kContextReNiceCmd
};

/******************************************************************************
 Constructor

 *****************************************************************************/

GPMProcessTable::GPMProcessTable
	(
	GPMProcessList*		list,
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
	JXTable(kDefRowHeight, kDefColWidth, scrollbarSet,
			enclosure, hSizing, vSizing, x,y, w,h),
	itsList(list),
	itsContextMenu(nullptr),
	itsSelectedEntry(nullptr),
	itsFullCmdDisplay(fullCmdDisplay),
	itsZombieImage(nullptr)
{
	AppendCols(GPMProcessList::kListCount);
	SetColWidth(GPMProcessList::kListState,  20);
	SetColWidth(GPMProcessList::kListPID,    50);
	SetColWidth(GPMProcessList::kListUser,   75);
	SetColWidth(GPMProcessList::kListNice,   40);
	SetColWidth(GPMProcessList::kListSize,   60);
	SetColWidth(GPMProcessList::kListCPU,    50);
	SetColWidth(GPMProcessList::kListMemory, 70);
	SetColWidth(GPMProcessList::kListTime,   60);

	SetRowBorderInfo(0, GetColormap()->GetBlackColor());
	SetColBorderInfo(0, GetColormap()->GetBlackColor());

	itsZombieImage = jnew JXImage(GetDisplay(), jx_edit_clear);
	assert( itsZombieImage != nullptr );

	itsContextMenu = CreateContextMenu(this);
	ListenTo(itsContextMenu);

	ListenTo(itsList);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMProcessTable::~GPMProcessTable()
{
	jdelete itsZombieImage;
}

/******************************************************************************
 CreateContextMenu (static)

 ******************************************************************************/

JXTextMenu*
GPMProcessTable::CreateContextMenu
	(
	JXContainer* enclosure
	)
{
	JXTextMenu* menu = jnew JXTextMenu("", enclosure, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( menu != nullptr );
	menu->SetMenuItems(kContextMenuStr);
	menu->SetToHiddenPopupMenu(kJTrue);

	menu->SetItemImage(kContextEndCmd,      JXPM(gpm_stop));
	menu->SetItemImage(kContextKillCmd,     JXPM(jx_edit_clear));
	menu->SetItemImage(kContextPauseCmd,    JXPM(gpm_pause));
	menu->SetItemImage(kContextContinueCmd, JXPM(gpm_cont));
	menu->SetItemImage(kContextReNiceCmd,   JXPM(gpm_slow));

	return menu;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GPMProcessTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsList && message.Is(GPMProcessList::kPrepareForUpdate))
		{
		StopListening(itsSelectedEntry);
		if (GetSelectedProcess(&itsSelectedEntry))
			{
			ClearWhenGoingAway(itsSelectedEntry, &itsSelectedEntry);
			}
		}

	else if (sender == itsList && message.Is(GPMProcessList::kListChanged))
		{
		JTableSelection& s = GetTableSelection();
		s.ClearSelection();

		const JSize count	= GetRowCount();
		const JSize lCount	= itsList->GetElementCount();
		if (lCount > count)
			{
			AppendRows(lCount - count);
			}
		else if (count > lCount)
			{
			RemoveNextRows(1, count - lCount);
			}

		JIndex index;
		if (itsSelectedEntry != nullptr)
			{
			if (itsList->GetEntryIndex(itsSelectedEntry, &index))
				{
				s.SelectRow(index);
				}

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
		 const JXMenu::ItemSelected* selection =
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
				itsFullCmdDisplay->SetText(entry->GetFullCommand());
				}
			}

		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GPMProcessTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	DrawRowBackground(p, cell, rect, (p.GetColormap())->GetGrayColor(95));

	HilightIfSelected(p, cell, rect);

	const GPMProcessEntry& entry = *(itsList->GetProcessEntry(cell.y));

	JString str;
	JPainter::HAlignment halign = JPainter::kHAlignRight;
	if (cell.x == GPMProcessList::kListState)
		{
		DrawProcessState(entry, p, rect, *itsZombieImage);
		return;
		}
	else if (cell.x == GPMProcessList::kListPID)
		{
		str	= JString((JUInt64) entry.GetPID());
		}
	else if (cell.x == GPMProcessList::kListUser)
		{
		str		= entry.GetUser();
		halign	= JPainter::kHAlignLeft;
		}
/*	else if (cell.x == GPMProcessList::kListPPID)
		{
		str	= JString((JUInt64) entry.GetPPID());
		}
	else if (cell.x == GPMProcessList::kListPriority)
		{
		str	= JString((JUInt64) entry.GetPriority());
		}
*/	else if (cell.x == GPMProcessList::kListNice)
		{
		str	= JString((JUInt64) entry.GetNice());
		}
	else if (cell.x == GPMProcessList::kListSize)
		{
		str	= JString((JUInt64) entry.GetSize());
		}
/*	else if (cell.x == GPMProcessList::kListResident)
		{
		str	= JString((JUInt64) entry.GetResident());
		}
	else if (cell.x == GPMProcessList::kListShare)
		{
		str	= JString((JUInt64) entry.GetShare());
		}
*/	else if (cell.x == GPMProcessList::kListCPU)
		{
		str	= JString(entry.GetPercentCPU(), 1);
		}
	else if (cell.x == GPMProcessList::kListMemory)
		{
		str	= JString(entry.GetPercentMemory(), 1);
		}
	else if (cell.x == GPMProcessList::kListTime)
		{
		str	= JString((JUInt64) entry.GetTime());
		}
	else if (cell.x == GPMProcessList::kListCommand)
		{
		str	= entry.GetCommand();
		halign	= JPainter::kHAlignLeft;
		}

	JRect r = rect;
	r.Shrink(kHMarginWidth, 0);
	p.JPainter::String(r, str, halign, JPainter::kVAlignCenter);
}

/******************************************************************************
 DrawRowBackground (static)

 ******************************************************************************/

void
GPMProcessTable::DrawRowBackground
	(
	JPainter&			p,
	const JPoint&		cell,
	const JRect&		rect,
	const JColorID	color
	)
{
	if (cell.y % 2 == 1)
		{
		p.SetPenColor(color);
		p.SetFilling(kJTrue);
		p.Rect(rect);
		p.SetFilling(kJFalse);
		}
}

/******************************************************************************
 DrawProcessState (static)

 ******************************************************************************/

void
GPMProcessTable::DrawProcessState
	(
	const GPMProcessEntry&	entry,
	JPainter&				p,
	const JRect&			rect,
	const JXImage&			zombieImage
	)
{
	if (entry.GetState() == GPMProcessEntry::kZombie)
		{
		p.Image(zombieImage, zombieImage.GetBounds(), rect);
		}
	else
		{
		JRect r(rect.ycenter()-3, rect.xcenter()-3,
				rect.ycenter()+4, rect.xcenter()+4);
		p.SetPenColor(entry.GetState() == GPMProcessEntry::kStopped ?
					  JColorManager::GetRedColor() : JColorManager::GetGreenColor());
		p.SetFilling(kJTrue);
		p.Ellipse(r);
		p.SetPenColor(JColorManager::GetBlackColor());
		p.SetFilling(kJFalse);
		p.Ellipse(r);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GPMProcessTable::HandleMouseDown
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
	itsKeyBuffer.Clear();

	JPoint cell;
	if (!GetCell(pt, &cell))
		{
		return;
		}

	s.SelectRow(cell.y);

	if (cell.x == GPMProcessList::kListState)
		{
		const GPMProcessEntry* entry = itsList->GetProcessEntry(cell.y);
		ToggleProcessState(*entry);
		itsList->Update();
		}
	else if (button == kJXRightButton)
		{
		itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
		}
}

/******************************************************************************
 ToggleProcessState (static)

 ******************************************************************************/

void
GPMProcessTable::ToggleProcessState
	(
	const GPMProcessEntry& entry
	)
{
	const pid_t pid = entry.GetPID();
	const uid_t uid = getuid();
	if (uid == 0 || entry.GetUID() != uid)
		{
		// too dangerous to allow toggle
		}
	else if (pid == getpid() || pid == 0)
		{
		// do not allow pause
		}
	else if (entry.GetState() == GPMProcessEntry::kZombie)
		{
		// cannot do anything
		}
	else if (entry.GetState() == GPMProcessEntry::kStopped)
		{
		JSendSignalToProcess(pid, SIGCONT);
		}
	else
		{
		JSendSignalToProcess(pid, SIGSTOP);
		}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
GPMProcessTable::HandleFocusEvent()
{
	JXTable::HandleFocusEvent();
	itsKeyBuffer.Clear();
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
GPMProcessTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == ' ' || key == kJEscapeKey)
		{
		itsKeyBuffer.Clear();
		GetTableSelection().ClearSelection();
		}

	// incremental search

	else if (JXIsPrint(key) && !modifiers.control() && !modifiers.meta())
		{
		itsKeyBuffer.AppendCharacter(key);

		GPMProcessEntry* entry;
		JIndex index;
		if (itsList->ClosestMatch(itsKeyBuffer, &entry) &&
			itsList->GetEntryIndex(entry, &index))
			{
			(GetTableSelection()).ClearSelection();
			(GetTableSelection()).SelectRow(index);
			TableScrollToCell(JPoint(1, index));
			}
		else
			{
			(GetTableSelection()).ClearSelection();
			}
		}

	else
		{
		if (JXIsPrint(key))
			{
			itsKeyBuffer.Clear();
			}
		JXTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
GPMProcessTable::UpdateContextMenu()
{
	const GPMProcessEntry* entry;
	if (GetSelectedProcess(&entry))
		{
		UpdateContextMenu(itsContextMenu, *entry);
		}
}

// static

void
GPMProcessTable::UpdateContextMenu
	(
	JXTextMenu*				menu,
	const GPMProcessEntry&	entry
	)
{
	if (entry.GetState() != GPMProcessEntry::kZombie)
		{
		const JBoolean notSelf = JI2B(entry.GetPID() != getpid());
		menu->EnableItem(kContextEndCmd);
		menu->EnableItem(kContextKillCmd);
		menu->SetItemEnable(kContextPauseCmd, notSelf);
		menu->SetItemEnable(kContextContinueCmd, notSelf);
		menu->EnableItem(kContextReNiceCmd);
		}
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
GPMProcessTable::HandleContextMenu
	(
	const JIndex index
	)
{
	const GPMProcessEntry* entry;
	if (GetSelectedProcess(&entry))
		{
		HandleContextMenu(index, *entry, itsList);
		}
}

// static

void
GPMProcessTable::HandleContextMenu
	(
	const JIndex			menuIndex,
	const GPMProcessEntry&	entry,
	GPMProcessList*			list
	)
{
	if (entry.GetState() == GPMProcessEntry::kZombie)
		{
		return;
		}

	if (menuIndex == kContextReNiceCmd)
		{
		JSetProcessPriority(entry.GetPID(), 19);
		list->Update();
		return;
		}

	JIndex sigValue = 0;
	if (menuIndex == kContextEndCmd)
		{
		sigValue = SIGTERM;
		}
	else if (menuIndex == kContextKillCmd)
		{
		sigValue = SIGKILL;
		}
	else if (menuIndex == kContextPauseCmd)
		{
		sigValue = SIGSTOP;
		}
	else if (menuIndex == kContextContinueCmd)
		{
		sigValue = SIGCONT;
		}

	const pid_t pid = entry.GetPID();
	if (sigValue == 0 || pid == 0)
		{
		return;
		}

	const uid_t uid = getuid();
	if (uid == 0 || entry.GetUID() == uid)
		{
		JSendSignalToProcess(pid, sigValue);
		list->Update();
		}
	else
		{
		JString cmd = "xterm -title 'Drakon sudo' -e /bin/sh -c 'sudo -k ; sudo kill -";
		cmd        += JString((JUInt64) sigValue);
		cmd        += " ";
		cmd        += JString((JUInt64) pid);
		cmd        += "'";
		JSimpleProcess::Create(cmd, kJTrue);
		}
}

/******************************************************************************
 GetSelectedProcess

 ******************************************************************************/

JBoolean
GPMProcessTable::GetSelectedProcess
	(
	const GPMProcessEntry** entry
	)
	const
{
	JPoint cell;
	if (GetTableSelection().GetFirstSelectedCell(&cell))
		{
		*entry = itsList->GetProcessEntry(cell.y);
		return kJTrue;
		}
	else
		{
		*entry = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 SelectProcess

 ******************************************************************************/

void
GPMProcessTable::SelectProcess
	(
	const GPMProcessEntry& entry
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	itsKeyBuffer.Clear();

	JIndex index;
	if (itsList->GetEntryIndex(&entry, &index))
		{
		s.SelectRow(index);
		TableScrollToCell(JPoint(1, index));
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GPMProcessTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
GPMProcessTable::AdjustColWidths()
{
	const JCoordinate availWidth =
		GetApertureWidth() - (GetBoundsWidth() - GetColWidth(GPMProcessList::kListCommand));
	SetColWidth(GPMProcessList::kListCommand, JMax(kDefColWidth, availWidth));
}
