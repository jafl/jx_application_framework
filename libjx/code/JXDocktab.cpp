/******************************************************************************
 JXDocktab.cpp

	Sits to the left of a menu bar and allows the window to be docked by
	dragging to a dock.  Also provides a menu of other, useful options.

	BASE CLASS = JXWidget

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include <JXDocktab.h>
#include <JXDockManager.h>
#include <JXDockDirector.h>
#include <JXDockWidget.h>
#include <JXDockDragData.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>
#include <JColorManager.h>
#include <jAssert.h>

// Action menu -- remember to update JXDockTabGroup

static const JUtf8Byte* kActionMenuStr =
	"    Undock this window"
	"  | Undock all windows in this compartment"
	"  | Undock all windows in this dock"
	"  | Undock all windows"
	"%l| UpdateWindowTypeMap"
	"%l| Create new dock (split horizontally)"
	"  | Create new dock (split vertically)"
	"%l| Dock all windows in default configuration"
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
	kShowFirstDockCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDocktab::JXDocktab
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
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsActionMenu(nullptr),
	itsDockFinder(nullptr)
{
	SetHint(JGetString("Hint::JXDocktab"));

	itsFocusColor = JColorManager::GetColorID(JRGB(0, 0, kJMaxRGBValue/2));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDocktab::~JXDocktab()
{
	jdelete itsDockFinder;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXDocktab::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	// drag region

	JXWindow* w = GetWindow();
	JXWindow* focusWindow;

	JXDockWidget* dock;
	if (w->GetDockWidget(&dock) &&
		(dock->GetDockDirector())->GetFocusWindow(&focusWindow) &&
		focusWindow == w)
		{
		p.SetPenColor(itsFocusColor);
		p.SetFilling(kJTrue);
		p.JPainter::Rect(rect);
		p.SetPenColor(JColorManager::GetBlackColor());
		p.SetFilling(kJFalse);

		p.SetPenColor(JColorManager::GetWhiteColor());
		}
	else
		{
		p.SetPenColor(JColorManager::GetGrayColor(60));
		}

	const JRect ap       = GetAperture();
	const JCoordinate y1 = ap.top + 1;
	const JCoordinate y2 = ap.bottom - 2;

	p.Line(3, y1, 3, y2);
	p.Line(5, y1, 5, y2);
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXDocktab::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXDocktab::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton)
		{
		if (itsDockFinder == nullptr)
			{
			itsDockFinder = jnew DockFinder(GetDisplay());
			assert( itsDockFinder != nullptr );
			}

		JXDockDragData* data = jnew JXDockDragData(GetWindow());
		assert( data != nullptr );

		BeginDND(pt, buttonStates, modifiers, data, itsDockFinder);
		}
	else if (button == kJXRightButton)
		{
		OpenActionMenu(pt, buttonStates, modifiers);
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
JXDocktab::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 OpenActionMenu (private)

 ******************************************************************************/

void
JXDocktab::OpenActionMenu
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsActionMenu == nullptr)
		{
		itsActionMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsActionMenu != nullptr );
		itsActionMenu->SetToHiddenPopupMenu();
		itsActionMenu->SetMenuItems(kActionMenuStr);
		itsActionMenu->SetUpdateAction(JXMenu::kDisableNone);
		ListenTo(itsActionMenu);
		}

	itsActionMenu->PopUp(this, pt, buttonStates, modifiers);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXDocktab::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsActionMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionMenu();
		}
	else if (sender == itsActionMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionMenu(selection->GetIndex());
		}

	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateActionMenu (private)

 ******************************************************************************/

void
JXDocktab::UpdateActionMenu()
{
	JXDockManager* dockMgr;
	if (!JXGetDockManager(&dockMgr))
		{
		itsActionMenu->RemoveAllItems();
		return;
		}

	while (itsActionMenu->GetItemCount() >= kShowFirstDockCmd)
		{
		itsActionMenu->RemoveItem(itsActionMenu->GetItemCount());
		}

	JPtrArray<JXDockDirector>* dockList = dockMgr->GetDockList();

	for (JXDockDirector* dir : *dockList)
		{
		JString itemText = dir->GetWindow()->GetTitle();
		itemText.Prepend(JGetString("ShowDockPrefix::JXDocktab"));
		itsActionMenu->AppendItem(itemText);
		}

	const JBoolean isDocked = GetWindow()->IsDocked();
	itsActionMenu->SetItemEnable(kUndockCmd, isDocked);
	itsActionMenu->SetItemEnable(kUndockAllCompartmentCmd, isDocked);
	itsActionMenu->SetItemEnable(kUndockAllDockCmd, isDocked);
	itsActionMenu->SetItemEnable(kUndockAllCmd, !dockList->IsEmpty());
	itsActionMenu->SetItemEnable(kDockAllDefConfigCmd, dockMgr->CanDockAll());

	itsActionMenu->SetItemEnable(kUpdateWindowTypeMapCmd, GetWindow()->HasWindowType());
	if (isDocked)
		{
		itsActionMenu->SetItemText(kUpdateWindowTypeMapCmd, JGetString("AutoDockNewWindows::JXDocktab"));
		}
	else
		{
		itsActionMenu->SetItemText(kUpdateWindowTypeMapCmd, JGetString("NoAutoDockNewWindows::JXDocktab"));
		}
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
JXDocktab::HandleActionMenu
	(
	const JIndex index
	)
{
	JXDockManager* dockMgr;
	if (!JXGetDockManager(&dockMgr))
		{
		return;
		}

	if (index == kUndockCmd)
		{
		GetWindow()->Undock();
		}
	else if (index == kUndockAllCompartmentCmd)
		{
		JXDockWidget* dock;
		if (GetWindow()->GetDockWidget(&dock))
			{
			dock->UndockAll();
			}
		}
	else if (index == kUndockAllDockCmd)
		{
		JXWindow* w;
		if (GetWindow()->GetDockWindow(&w))
			{
			w->UndockAllChildWindows();
			}
		}
	else if (index == kUndockAllCmd)
		{
		GetDisplay()->UndockAllWindows();
		}

	else if (index == kUpdateWindowTypeMapCmd)
		{
		JXWindow* w = GetWindow();
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

/******************************************************************************
 JXDocktab::DockFinder

 ******************************************************************************/

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDocktab::DockFinder::DockFinder
	(
	JXDisplay* display
	)
	:
	TargetFinder(display)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDocktab::DockFinder::~DockFinder()
{
}

/******************************************************************************
 FindTarget (virtual)

 ******************************************************************************/

JBoolean
JXDocktab::DockFinder::FindTarget
	(
	const JXContainer*	coordOwner,
	const JPoint&		pt,
	Window*				xWindow,
	Window*				msgWindow,
	JXContainer**		target,
	Atom*				vers
	)
{
	if (GetDisplay()->FindMouseContainer(coordOwner, pt, target, xWindow))
		{
		JXDockWidget* dock;
		if (((**target).GetWindow())->GetDockWidget(&dock))
			{
			*target  = dock;
			*xWindow = (dock->JXContainer::GetWindow())->GetXWindow();
			}

		return GetDNDManager()->IsDNDAware(*xWindow, msgWindow, vers);
		}
	else
		{
		*target    = nullptr;
		*msgWindow = *xWindow;
		*vers      = 0;
		return kJFalse;
		}
}
