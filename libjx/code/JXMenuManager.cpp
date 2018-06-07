/******************************************************************************
 JXMenuManager.cpp

	Global object to help manage pull-down menus.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXMenuManager.h>
#include <JXMenu.h>
#include <JXWindow.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMenuManager::JXMenuManager()
{
	itsOpenMenuList = jnew JPtrArray<JXMenu>(JPtrArrayT::kForgetAll);
	assert( itsOpenMenuList != nullptr );

	itsOpenWindowList = jnew JPtrArray<JXWindow>(JPtrArrayT::kForgetAll);
	assert( itsOpenWindowList != nullptr );
}

/******************************************************************************
 Destructor

	We should not be deleted until all JXMenus have been deleted.

 ******************************************************************************/

JXMenuManager::~JXMenuManager()
{
	assert( itsOpenMenuList->IsEmpty() && itsOpenWindowList->IsEmpty() );

	jdelete itsOpenMenuList;
	jdelete itsOpenWindowList;
}

/******************************************************************************
 CloseCurrentMenus

 ******************************************************************************/

void
JXMenuManager::CloseCurrentMenus()
{
	if (!itsOpenMenuList->IsEmpty())
		{
		(itsOpenMenuList->GetFirstElement())->Close();
		assert( itsOpenMenuList->IsEmpty() && itsOpenWindowList->IsEmpty() );
		}
}

/******************************************************************************
 IsMenuForWindow

	For use by JXApplication::HandleOneEventForWindow().

 ******************************************************************************/

JBoolean
JXMenuManager::IsMenuForWindow
	(
	JXWindow* menuWindow,
	JXWindow* mainWindow
	)
	const
{
	JIndex index;
	if (itsOpenWindowList->Find(menuWindow, &index))
		{
		JXMenu* menu = itsOpenMenuList->GetElement(index);
		return JI2B(mainWindow == menu->GetWindow());
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 MenuOpened (private)

 ******************************************************************************/

void
JXMenuManager::MenuOpened
	(
	JXMenu*		menu,
	JXWindow*	window
	)
{
	if (!itsOpenMenuList->Includes(menu))
		{
		itsOpenMenuList->Append(menu);
		itsOpenWindowList->Append(window);
		}
}

/******************************************************************************
 MenuClosed (private)

 ******************************************************************************/

void
JXMenuManager::MenuClosed
	(
	JXMenu* menu
	)
{
	JIndex index;
	if (itsOpenMenuList->Find(menu, &index))
		{
		itsOpenMenuList->RemoveElement(index);
		itsOpenWindowList->RemoveElement(index);
		}
}
