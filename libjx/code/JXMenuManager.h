/******************************************************************************
 JXMenuManager.h

	Interface for JXMenuManager class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMenuManager
#define _H_JXMenuManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>

class JXWindow;
class JXMenu;

class JXMenuManager
{
	friend class JXMenu;

public:

	JXMenuManager();

	virtual ~JXMenuManager();

	void	CloseCurrentMenus();

	JBoolean	IsMenuForWindow(JXWindow* menuWindow, JXWindow* mainWindow) const;

private:

	JPtrArray<JXMenu>*		itsOpenMenuList;	// we don't own the menus
	JPtrArray<JXWindow>*	itsOpenWindowList;	// menus own their windows

private:

	// called by JXMenu

	void	MenuOpened(JXMenu* menu, JXWindow* window);
	void	MenuClosed(JXMenu* menu);

	// not allowed

	JXMenuManager(const JXMenuManager& source);
	const JXMenuManager& operator=(const JXMenuManager& source);
};

#endif
