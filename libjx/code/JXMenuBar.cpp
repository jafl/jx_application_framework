/******************************************************************************
 JXMenuBar.cpp

	Maintains a menu bar of pull-down menus.

	*** RemoveMenu() only removes the specified menu from the menu bar.
		It does -not- delete the JXMenu object.  It returns a pointer to
		it so the caller can delete it.  This is useful if one wants to
		dynamically add and remove menus from a menu bar without rebuilding
		the menu each time it is added.  (i.e. Use InsertMenu() to put the
		JXMenu back into the menu bar later.)  To remove a menu from the
		menu bar and throw it away, use DeleteMenu().

	*** Conversely, after calling InsertMenu(), the menu bar owns the
		JXMenu object and is responsible for deleting it.

	BASE CLASS = JXWidget

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXDocktab.h>
#include <JXImage.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <jXGlobals.h>
#include <jAssert.h>

#include <jx_down_chevron.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMenuBar::JXMenuBar
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
	itsOverflowMenu(NULL),
	itsIgnoreWidthChangedCount(0)
{
	itsMenus = new JPtrArray<JXMenu>(JPtrArrayT::kForgetAll);
	assert( itsMenus != NULL );

	SetBorderWidth(kJXDefaultBorderWidth);

	JXDockManager* dockMgr;
	if (JXGetDockManager(&dockMgr))
		{
		Move(JXDocktab::kWidth, 0);
		AdjustSize(-JXDocktab::kWidth, 0);

		JXDocktab* tab =
			new JXDocktab(enclosure,
						  hSizing == kFixedRight ? kFixedRight : kFixedLeft,
						  vSizing, x, y, JXDocktab::kWidth, h);
		assert( tab != NULL );
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMenuBar::~JXMenuBar()
{
	const JSize count = itsMenus->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXMenu* theMenu = itsMenus->NthElement(i);
		if (theMenu == itsOverflowMenu)
			{
			itsOverflowMenu = NULL;
			}

		assert( theMenu->itsOwner == NULL );
		theMenu->itsMenuBar = NULL;		// so they won't talk to us
		delete theMenu;
		}

	delete itsOverflowMenu;
	delete itsMenus;
}

/******************************************************************************
 GetMenuCount

	Returns the number of menus currently installed in the menu bar.

 ******************************************************************************/

JSize
JXMenuBar::GetMenuCount()
	const
{
	return (itsMenus->GetElementCount() +
			(itsOverflowMenu != NULL ? itsOverflowMenu->GetItemCount()-1 : 0));
}

/******************************************************************************
 GetMenu

	Returns the specified menu.

 ******************************************************************************/

JXMenu*
JXMenuBar::GetMenu
	(
	const JIndex index
	)
{
	const JSize count = itsMenus->GetElementCount();
	if (itsOverflowMenu != NULL && index >= count)
		{
		const JXMenu* menu;
		const JBoolean found = itsOverflowMenu->GetSubmenu(index - (count-1), &menu);
		assert( found );
		return const_cast<JXMenu*>(menu);
		}
	else
		{
		return itsMenus->NthElement(index);
		}
}

const JXMenu*
JXMenuBar::GetMenu
	(
	const JIndex index
	)
	const
{
	const JSize count = itsMenus->GetElementCount();
	if (itsOverflowMenu != NULL && index >= count)
		{
		const JXMenu* menu;
		const JBoolean found = itsOverflowMenu->GetSubmenu(index - (count-1), &menu);
		assert( found );
		return menu;
		}
	else
		{
		return itsMenus->NthElement(index);
		}
}

/******************************************************************************
 InsertMenu

 ******************************************************************************/

JXTextMenu*
JXMenuBar::InsertTextMenu
	(
	const JIndex		index,
	const JCharacter*	title
	)
{
	const JRect bounds = GetBounds();
	JXTextMenu* theMenu = new JXTextMenu(title, this, kFixedLeft, kVElastic,
										 bounds.left,bounds.top, 10,bounds.height());
	assert( theMenu != NULL );

	InsertMenu(index, theMenu);
	return theMenu;
}

JXTextMenu*
JXMenuBar::InsertTextMenu
	(
	const JIndex	index,
	JXImage*		image,
	const JBoolean	menuOwnsImage
	)
{
	const JRect bounds = GetBounds();
	JXTextMenu* theMenu =
		new JXTextMenu(image, menuOwnsImage, this, kFixedLeft, kVElastic,
					   bounds.left,bounds.top, 10,bounds.height());
	assert( theMenu != NULL );

	InsertMenu(index, theMenu);
	return theMenu;
}

void
JXMenuBar::InsertMenu
	(
	const JIndex	index,
	JXMenu*			theMenu
	)
{
	assert( theMenu != NULL );

	ClearOverflowMenu();

	JIndex origIndex;
	if (itsMenus->Find(theMenu, &origIndex))
		{
		if (origIndex == index)
			{
			WidthChanged();
			return;
			}
		RemoveMenu(origIndex);
		}

	theMenu->SetSizing(kFixedLeft, kVElastic);

	const JRect bounds = GetBounds();
	JCoordinate left   = bounds.left;
	for (JIndex i=1; i<index; i++)
		{
		JXMenu* aMenu = itsMenus->NthElement(i);
		left += aMenu->GetFrameWidth();
		}

	itsMenus->InsertAtIndex(index, theMenu);

	theMenu->Place(left, bounds.top);
	theMenu->AdjustSize(0, bounds.height() - theMenu->GetFrameHeight());
	MenuWidthChanged(index, theMenu->GetFrameWidth());

	theMenu->SetMenuBar(this);
	theMenu->Show();

	WidthChanged();
}

JBoolean
JXMenuBar::InsertMenuBefore
	(
	JXMenu* existingMenu,
	JXMenu* newMenu
	)
{
	ClearOverflowMenu();

	JIndex menuIndex;
	const JBoolean found = itsMenus->Find(existingMenu, &menuIndex);
	if (found)
		{
		InsertMenu(menuIndex, newMenu);
		}
	else
		{
		WidthChanged();
		}
	return found;
}

JBoolean
JXMenuBar::InsertMenuAfter
	(
	JXMenu* existingMenu,
	JXMenu* newMenu
	)
{
	ClearOverflowMenu();

	JIndex menuIndex;
	const JBoolean found = itsMenus->Find(existingMenu, &menuIndex);
	if (found)
		{
		InsertMenu(menuIndex+1, newMenu);
		}
	else
		{
		WidthChanged();
		}
	return found;
}

/******************************************************************************
 RemoveMenu

	The caller now owns the menu and is responsible for deleting it.

 ******************************************************************************/

JBoolean
JXMenuBar::RemoveMenu
	(
	JXMenu* theMenu
	)
{
	ClearOverflowMenu();

	JIndex menuIndex;
	const JBoolean found = itsMenus->Find(theMenu, &menuIndex);
	if (found)
		{
		RemoveMenu(menuIndex);
		}
	else
		{
		WidthChanged();
		}
	return found;
}

JXMenu*
JXMenuBar::RemoveMenu
	(
	const JIndex index
	)
{
	ClearOverflowMenu();

	JXMenu* theMenu = itsMenus->NthElement(index);
	MenuWidthChanged(index, -theMenu->GetFrameWidth());
	itsMenus->RemoveElement(index);
	assert( theMenu->itsMenuBar == this );
	theMenu->itsMenuBar = NULL;		// SetMenuBar() would cause infinite loop
	theMenu->AdjustAppearance();
	theMenu->Hide();

	WidthChanged();
	return theMenu;
}

/******************************************************************************
 DeleteMenu

 ******************************************************************************/

JBoolean
JXMenuBar::DeleteMenu
	(
	JXMenu* theMenu
	)
{
	ClearOverflowMenu();

	JIndex menuIndex;
	const JBoolean found = itsMenus->Find(theMenu, &menuIndex);
	if (found)
		{
		DeleteMenu(menuIndex);
		}
	else
		{
		WidthChanged();
		}
	return found;
}

void
JXMenuBar::DeleteMenu
	(
	const JIndex index
	)
{
	JXMenu* theMenu = RemoveMenu(index);
	assert( theMenu->itsOwner == NULL );
	theMenu->itsMenuBar = NULL;
	delete theMenu;
}

/******************************************************************************
 MenuWidthChanged (private)

 ******************************************************************************/

void
JXMenuBar::MenuWidthChanged
	(
	JXMenu*				theMenu,
	const JCoordinate	dw
	)
{
	if (dw != 0)
		{
		JIndex menuIndex;
		const JBoolean found = itsMenus->Find(theMenu, &menuIndex);
		if (found)
			{
			MenuWidthChanged(menuIndex, dw);
			WidthChanged();
			}
		}
}

void
JXMenuBar::MenuWidthChanged
	(
	const JIndex		index,
	const JCoordinate	dw
	)
{
	if (dw != 0)
		{
		const JSize menuCount = itsMenus->GetElementCount();
		for (JIndex i=menuCount; i>index; i--)
			{
			JXMenu* aMenu = itsMenus->NthElement(i);
			aMenu->Move(dw,0);
			}
		}
}

/******************************************************************************
 Draw (virtual protected)

	There is nothing to do since we only act as a container.

 ******************************************************************************/

void
JXMenuBar::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXMenuBar::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawUpFrame(p, frame, GetBorderWidth());
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXMenuBar::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidget::ApertureResized(dw, dh);
	WidthChanged();
}

/******************************************************************************
 ClearOverflowMenu (private)

 ******************************************************************************/

void
JXMenuBar::ClearOverflowMenu()
{
	if (itsOverflowMenu != NULL)
		{
		itsMenus->Remove(itsOverflowMenu);
		assert( itsOverflowMenu->itsMenuBar == this );
		itsOverflowMenu->itsMenuBar = NULL;		// SetMenuBar() would cause infinite loop

		JXMenu* overflowMenu = itsOverflowMenu;
		itsOverflowMenu      = NULL;

		itsIgnoreWidthChangedCount++;

		const JSize count = overflowMenu->GetItemCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JXMenu* m;
			const JBoolean found = overflowMenu->GetSubmenu(i, &m);
			assert( found );
			AppendMenu(const_cast<JXMenu*>(m));
			}

		delete overflowMenu;

		assert( itsIgnoreWidthChangedCount > 0 );
		itsIgnoreWidthChangedCount--;
		}
}

/******************************************************************************
 WidthChanged (private)

 ******************************************************************************/

void
JXMenuBar::WidthChanged()
{
	if (itsMenus->IsEmpty() || itsIgnoreWidthChangedCount > 0)
		{
		return;
		}

	itsIgnoreWidthChangedCount++;

	// put all menus back in menubar

	ClearOverflowMenu();

	// check if menus fit on menubar

	const JRect ap = GetAperture();

	JXMenu* m   = itsMenus->LastElement();
	JRect frame = m->GetFrame();
	if (frame.xcenter() < ap.right)
		{
		assert( itsIgnoreWidthChangedCount > 0 );
		itsIgnoreWidthChangedCount--;
		return;
		}

	// create menu to hold overflow

	JXImage* image = new JXImage(GetDisplay(), GetColormap(), jx_down_chevron);
	assert( image != NULL );

	JXTextMenu* overflowMenu =
		new JXTextMenu(image, kJTrue, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( overflowMenu != NULL );
	overflowMenu->SetUpdateAction(JXMenu::kDisableNone);

	overflowMenu->SetMenuBar(this);		// don't show down arrow at right of title
	const JCoordinate extraWidth = overflowMenu->GetFrameWidth();
	overflowMenu->SetMenuBar(NULL);

	// move menus to overflow menu

	while (frame.right > ap.right - extraWidth)
		{
		const JString& title = m->GetTitleText();
		if (title.IsEmpty())
			{
			break;
			}

		overflowMenu->PrependItem(title);
		overflowMenu->AttachSubmenu(1, m);
		if (itsMenus->IsEmpty())
			{
			break;
			}

		m     = itsMenus->LastElement();
		frame = m->GetFrame();
		}

	if (overflowMenu->IsEmpty())
		{
		delete overflowMenu;
		overflowMenu = NULL;
		}
	else
		{
		AppendMenu(overflowMenu);
		itsOverflowMenu = overflowMenu;
		}

	assert( itsIgnoreWidthChangedCount > 0 );
	itsIgnoreWidthChangedCount--;
}
