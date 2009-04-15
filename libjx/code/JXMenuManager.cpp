/******************************************************************************
 JXMenuManager.cpp

	Global object to help manage pull-down menus.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXMenuManager.h>
#include <JXMenu.h>
#include <JXWindow.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMenuManager::JXMenuManager()
{
	itsOpenMenuList = new JPtrArray<JXMenu>(JPtrArrayT::kForgetAll);
	assert( itsOpenMenuList != NULL );

	itsOpenWindowList = new JPtrArray<JXWindow>(JPtrArrayT::kForgetAll);
	assert( itsOpenWindowList != NULL );
}

/******************************************************************************
 Destructor

	We should not be deleted until all JXMenus have been deleted.

 ******************************************************************************/

JXMenuManager::~JXMenuManager()
{
	assert( itsOpenMenuList->IsEmpty() && itsOpenWindowList->IsEmpty() );

	delete itsOpenMenuList;
	delete itsOpenWindowList;
}

/******************************************************************************
 CloseCurrentMenus

 ******************************************************************************/

void
JXMenuManager::CloseCurrentMenus()
{
	if (!itsOpenMenuList->IsEmpty())
		{
		(itsOpenMenuList->FirstElement())->Close();
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
		JXMenu* menu = itsOpenMenuList->NthElement(index);
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
