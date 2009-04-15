/******************************************************************************
 JXDockTabGroup.cpp

	Displays a list of tabs with Docktab drag source, right-click menu,
	and close button.

	BASE CLASS = JXTabGroup

	Copyright © 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDockTabGroup.h>
#include <JXDockManager.h>
#include <JXDockDirector.h>
#include <JXDockWidget.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <jXGlobals.h>
#include <jAssert.h>

// Context menu -- remember to update JXDocktab

static const JCharacter* kContextMenuStr =
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

// string ID's

static const JCharacter* kShowDockPrefixID     = "ShowDockPrefix::JXDocktab";
static const JCharacter* kAddToWindowTypeMapID = "AutoDockNewWindows::JXDocktab";

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
	itsDockWidget(NULL),
	itsDockContextMenu(NULL),
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
	SetTabCanClose(index, JI2B(itsDockWidget->GetWindow(index, &w) &&
							   w->GetCloseAction() != JXWindow::kDeactivateDirector));
}

/******************************************************************************
 OKToDeleteTab (virtual protected)

	Returns kJFalse if base class should not remove the tab.

 ******************************************************************************/

JBoolean
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

	return kJFalse;
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
	const JBoolean hasMouseTab = GetMouseTabIndex(&mouseIndex);
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
	if (sender == itsDockContextMenu && message.Is(JXTextMenu::kNeedsUpdate))
		{
		UpdateDockContextMenu();
		}
	else if (sender == itsDockContextMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
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
	if (itsDockContextMenu == NULL)
		{
		itsDockContextMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsDockContextMenu != NULL );
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
	JBoolean found = JXGetDockManager(&dockMgr);
	assert( found );

	while (itsDockContextMenu->GetItemCount() >= kShowFirstDockCmd)
		{
		itsDockContextMenu->RemoveItem(itsDockContextMenu->GetItemCount());
		}

	JPtrArray<JXDockDirector>* dockList = dockMgr->GetDockList();

	const JSize dockCount = dockList->GetElementCount();
	for (JIndex i=1; i<=dockCount; i++)
		{
		JString itemText = ((dockList->NthElement(i))->GetWindow())->GetTitle();
		itemText.Prepend(JGetString(kShowDockPrefixID));
		itsDockContextMenu->AppendItem(itemText);
		}

	itsDockContextMenu->SetItemEnable(kUndockAllCmd, JI2B(dockCount > 0));
	itsDockContextMenu->SetItemEnable(kDockAllDefConfigCmd, dockMgr->CanDockAll());

	JXWindow* w;
	found = itsDockWidget->GetWindow(itsDockContextIndex, &w);
	assert( found );

	itsDockContextMenu->SetItemEnable(kUpdateWindowTypeMapCmd, w->HasWindowType());
	itsDockContextMenu->SetItemText(kUpdateWindowTypeMapCmd, JGetString(kAddToWindowTypeMapID));
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
	JBoolean found = JXGetDockManager(&dockMgr);
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
		(GetDisplay())->UndockAllWindows();
		}

	else if (index == kUpdateWindowTypeMapCmd)
		{
		const JCharacter* type;
		if (w->GetWindowType(&type))
			{
			JXDockWidget* dock;
			w->GetDockWidget(&dock);
			(JXGetDockManager())->SetDefaultDock(type, w->IsDocked() ? dock : NULL);
			}
		}

	else if (index == kCreateDockHorizCmd)
		{
		dockMgr->CreateDock(kJTrue);
		}
	else if (index == kCreateDockVertCmd)
		{
		dockMgr->CreateDock(kJFalse);
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
			(dockList->NthElement(i))->Activate();
			}
		}
}
