/******************************************************************************
 GMDirectorMenu.cc

	BASE CLASS = public JXTextMenu

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMDirectorMenu.h>
#include <GMDirectorManager.h>
#include <GMGlobals.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GMDirectorMenu::GMDirectorMenu
	(
	const JCharacter*	title,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	ListenTo(this);
	ListenTo(GGetDirMgr());
	GGetDirMgr()->UpdateDirectorMenu(this);
	SetUpdateAction(JXMenu::kDisableNone);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMDirectorMenu::~GMDirectorMenu()
{
}

/******************************************************************************


 ******************************************************************************/

void
GMDirectorMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GGetDirMgr() && message.Is(GMDirectorManager::kDirectorListChanged))
		{
		GGetDirMgr()->UpdateDirectorMenu(this);
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		GGetDirMgr()->ActivateDirector(selection->GetIndex());
		}
	JXTextMenu::Receive(sender, message);
}
