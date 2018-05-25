/******************************************************************************
 JXWindow.cpp

	Written by John Lindal.

 ******************************************************************************/

#include <JXWindow_mock.h>
#include <JXDisplay_mock.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWindow::JXWindow()
{
}

/******************************************************************************
 MenuItemInserted

	Update the item indices of the shortcuts.

 ******************************************************************************/

void
JXWindow::MenuItemInserted
	(
	JXTextMenu*		menu,
	const JIndex	newItem
	)
{
}

/******************************************************************************
 MenuItemRemoved

	Remove the shortcuts for the item and update the item indices of the
	other shortcuts.

 ******************************************************************************/

void
JXWindow::MenuItemRemoved
	(
	JXTextMenu*		menu,
	const JIndex	oldItem
	)
{
}

/******************************************************************************
 InstallMenuShortcut

 ******************************************************************************/

JBoolean
JXWindow::InstallMenuShortcut
	(
	JXTextMenu*				menu,
	const JIndex			menuItem,
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	return kJTrue;
}

/******************************************************************************
 ClearMenuShortcut

 ******************************************************************************/

void
JXWindow::ClearMenuShortcut
	(
	JXTextMenu*		menu,
	const JIndex	menuItem
	)
{
}

/******************************************************************************
 ClearAllMenuShortcuts

 ******************************************************************************/

void
JXWindow::ClearAllMenuShortcuts
	(
	JXTextMenu* menu
	)
{
}

/******************************************************************************
 GetULShortcutIndex (static)

 ******************************************************************************/

JIndex
JXWindow::GetULShortcutIndex
	(
	const JString&	label,
	const JString*	list
	)
{
	return 1;
}
