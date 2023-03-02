/******************************************************************************
 JXMenuManager.h

	Interface for JXMenuManager class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMenuManager
#define _H_JXMenuManager

#include <jx-af/jcore/JPtrArray.h>

class JXWindow;
class JXMenu;

class JXMenuManager
{
	friend class JXMenu;

public:

	JXMenuManager();

	virtual ~JXMenuManager();

	void	CloseCurrentMenus();

private:

	JPtrArray<JXMenu>*		itsOpenMenuList;	// we don't own the menus
	JPtrArray<JXWindow>*	itsOpenWindowList;	// menus own their windows

private:

	// called by JXMenu

	void	MenuOpened(JXMenu* menu, JXWindow* window);
	void	MenuClosed(JXMenu* menu);

	// not allowed

	JXMenuManager(const JXMenuManager&) = delete;
	JXMenuManager& operator=(const JXMenuManager&) = delete;
};

#endif
