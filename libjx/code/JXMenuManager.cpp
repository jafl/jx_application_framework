/******************************************************************************
 JXMenuManager.cpp

	Global object to help manage pull-down menus.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXMenuManager.h"
#include "JXMenu.h"
#include "JXWindow.h"
#include <jx-af/jcore/jAssert.h>

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
