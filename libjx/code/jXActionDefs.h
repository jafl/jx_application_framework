/******************************************************************************
 jXActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_jXActionDefs
#define _H_jXActionDefs

// File menu

#define kJXPageSetupAction		"PageSetup::JX"
#define kJXPrintAction			"Print::JX"
#define kJXCloseWindowAction	"CloseWindow::JX"
#define kJXQuitAction			"Quit::JX"

// Edit menu

#define kJXUndoAction		"Undo::JX"
#define kJXRedoAction		"Redo::JX"
#define kJXCutAction		"Cut::JX"
#define kJXCopyAction		"Copy::JX"
#define kJXPasteAction		"Paste::JX"
#define kJXClearAction		"Clear::JX"
#define kJXSelectAllAction	"SelectAll::JX"

// Windows menu

#define kJXRaiseAllWindowsAction		"RaiseAllWindows::JX"
#define kJXCloseAllOtherWindowsAction	"CloseAllOtherWindows::JX"

// Help menu

#define kJXHelpTOCAction		"HelpTOC::JX"
#define kJXHelpSpecificAction	"HelpSpecific::JX"

#endif
