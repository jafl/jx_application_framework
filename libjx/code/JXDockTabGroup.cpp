/******************************************************************************
 JXDockTabGroup.cpp

	Displays a list of tabs with Docktab drag source, right-click menu,
	and close button.

	BASE CLASS = JXTabGroup

	Copyright (C) 2006 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXDockTabGroup.h"
#include "jx-af/jx/JXDockManager.h"
#include "jx-af/jx/JXDockDirector.h"
#include "jx-af/jx/JXDockWidget.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXTextMenu.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

// Context menu -- remember to update JXDocktab

static const JUtf8Byte* kContextMenuStr =
	"    Undock this window"
	"  | Undock all windows in this compartment"
	"  | Undock all windows in this dock"
	"  | Undock all windows"
	"%l| UpdateWindowTypeMap"
	"%l| Create new dock (split horizontally)"
	"  | Create new dock (split vertically)"
	"%l| Dock all windows in default configuration"
	"%l| Show tabs on top"
	"  | Show tabs on bottom"
	"%l| Show tabs at left"
	"  | Show tabs at right"
	"%l";

enum
{
	kUndockCmd = 1,
	kUndockAllCompartmentCmd,
	kUndockAllDockCmd,
	kUndockAllCmd,
	kUpdateWindowTypeMapCmd,
	kCreateDockHorizCmd,
	kCreateDockVertCmd,
	kDockAllDefConfigCmd,

	kContextTopCmd,
	kContextBottomCmd,
	kContextLeftCmd,
	kContextRightCmd,

	kShowFirstDockCmd,
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockTabGroup::JXDockTabGroup
	(
	
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTabGroup(enclosure, hSizing, vSizing, x,y, w,h),
	itsDockWidget(nullptr),
	itsDockContextMenu(nullptr),
	itsDockContextIndex(0)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockTabGroup::~JXDockTabGroup()
{
}

/******************************************************************************
 DrawTab (virtual protected)

 ******************************************************************************/

void
JXDockTabGroup::DrawTab
	(
	const JIndex		index,
	JXWindowPainter&	p,
	const JRect&		rect,
	const Edge			edge
	)
{
	JXWindow* w;
	SetTabCanClose(index, itsDockWidget->GetWindow(index, &w) &&
							   w->GetCloseAction() != JXWindow::kDeactivateDirector);
}

/******************************************************************************
 OKToDeleteTab (virtual protected)

	Returns false if base class should not remove the tab.

 ******************************************************************************/

bool
JXDockTabGroup::OKToDeleteTab
	(
	const JIndex index
	)
{
	JXWindow* w;
	if (itsDockWidget->GetWindow(index, &w))
	{
		w->Close();
	}

	return false;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXDockTabGroup::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JIndex mouseIndex;
	const bool hasMouseTab = GetMouseTabIndex(&mouseIndex);
	if (button == kJXRightButton && hasMouseTab)
	{
		itsDockContextIndex = mouseIndex;
		CreateDockContextMenu();
		itsDockContextMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else
	{
		JXTabGroup::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXDockTabGroup::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDockContextMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateDockContextMenu();
	}
	else if (sender == itsDockContextMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleDockContextMenu(selection->GetIndex());
	}

	else
	{
		JXTabGroup::Receive(sender, message);
	}
}

/******************************************************************************
 CreateDockContextMenu (private)

 ******************************************************************************/

void
JXDockTabGroup::CreateDockContextMenu()
{
	if (itsDockContextMenu == nullptr)
	{
		itsDockContextMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsDockContextMenu != nullptr );
		itsDockContextMenu->SetMenuItems(kContextMenuStr, "JXDockTabGroup");
		itsDockContextMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsDockContextMenu->SetToHiddenPopupMenu();
		ListenTo(itsDockContextMenu);
	}
}

/******************************************************************************
 UpdateDockContextMenu (private)

 ******************************************************************************/

void
JXDockTabGroup::UpdateDockContextMenu()
{
	JXDockManager* dockMgr;
	bool found = JXGetDockManager(&dockMgr);
	assert( found );

	while (itsDockContextMenu->GetItemCount() >= kShowFirstDockCmd)
	{
		itsDockContextMenu->RemoveItem(itsDockContextMenu->GetItemCount());
	}

	JPtrArray<JXDockDirector>* dockList = dockMgr->GetDockList();

	for (auto* dir : *dockList)
	{
		JString itemText = dir->GetWindow()->GetTitle();
		itemText.Prepend(JGetString("ShowDockPrefix::JXDocktab"));
		itsDockContextMenu->AppendItem(itemText);
	}

	itsDockContextMenu->SetItemEnabled(kUndockAllCmd, !dockList->IsEmpty());
	itsDockContextMenu->SetItemEnabled(kDockAllDefConfigCmd, dockMgr->CanDockAll());

	JXWindow* w;
	found = itsDockWidget->GetWindow(itsDockContextIndex, &w);
	assert( found );

	itsDockContextMenu->SetItemEnabled(kUpdateWindowTypeMapCmd, w->HasWindowType());
	itsDockContextMenu->SetItemText(kUpdateWindowTypeMapCmd, JGetString("AutoDockNewWindows::JXDocktab"));
}

/******************************************************************************
 HandleDockContextMenu (private)

 ******************************************************************************/

void
JXDockTabGroup::HandleDockContextMenu
	(
	const JIndex index
	)
{
	JXDockManager* dockMgr;
	bool found = JXGetDockManager(&dockMgr);
	assert( found );

	JXWindow* w;
	found = itsDockWidget->GetWindow(itsDockContextIndex, &w);
	assert( found );

	if (index == kUndockCmd)
	{
		w->Undock();
	}
	else if (index == kUndockAllCompartmentCmd)
	{
		JXDockWidget* dock;
		if (w->GetDockWidget(&dock))
		{
			dock->UndockAll();
		}
	}
	else if (index == kUndockAllDockCmd)
	{
		JXWindow* w1;
		if (w->GetDockWindow(&w1))
		{
			w1->UndockAllChildWindows();
		}
	}
	else if (index == kUndockAllCmd)
	{
		GetDisplay()->UndockAllWindows();
	}

	else if (index == kUpdateWindowTypeMapCmd)
	{
		JString type;
		if (w->GetWindowType(&type))
		{
			JXDockWidget* dock;
			w->GetDockWidget(&dock);
			(JXGetDockManager())->SetDefaultDock(type.GetBytes(), w->IsDocked() ? dock : nullptr);
		}
	}

	else if (index == kCreateDockHorizCmd)
	{
		dockMgr->CreateDock(true);
	}
	else if (index == kCreateDockVertCmd)
	{
		dockMgr->CreateDock(false);
	}
	else if (index == kDockAllDefConfigCmd)
	{
		dockMgr->DockAll();
	}

	else if (index == kContextTopCmd)
	{
		SetTabEdge(kTop);
	}
	else if (index == kContextBottomCmd)
	{
		SetTabEdge(kBottom);
	}
	else if (index == kContextLeftCmd)
	{
		SetTabEdge(kLeft);
	}
	else if (index == kContextRightCmd)
	{
		SetTabEdge(kRight);
	}

	else if (index >= kShowFirstDockCmd)
	{
		JPtrArray<JXDockDirector>* dockList = dockMgr->GetDockList();
		const JIndex i                      = index - kShowFirstDockCmd + 1;
		if (dockList->IndexValid(i))		// paranoia
		{
			(dockList->GetElement(i))->Activate();
		}
	}
}
