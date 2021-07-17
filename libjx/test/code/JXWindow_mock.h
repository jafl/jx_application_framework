/******************************************************************************
 JXWindow.h

	Mock for the JXWindow class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWindow
#define _H_JXWindow

#include <JXKeyModifiers.h>

class JString;
class JXTextMenu;

class JXWindow
{
public:

	JXWindow();

	bool	InstallMenuShortcut(JXTextMenu* menu, const JIndex menuItem,
									const int key, const JXKeyModifiers& origModifiers);
	void		ClearMenuShortcut(JXTextMenu* menu, const JIndex menuItem);
	void		ClearAllMenuShortcuts(JXTextMenu* menu);
	void		MenuItemInserted(JXTextMenu* menu, const JIndex newItem);
	void		MenuItemRemoved(JXTextMenu* menu, const JIndex oldItem);

	static JIndex	GetULShortcutIndex(const JString& label, const JString* list);
};

#endif
