/******************************************************************************
 jXActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_jXActionDefs
#define _H_jXActionDefs

#include "jXActions.h"	// for convenience

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

#define kCheckAllSpellingAction	"CheckAllSpellingCmd::JX"
#define kCheckSpellingSelAction	"CheckSpellingSelCmd::JX"

// Search menu

#define kJXFindDialogAction				"FindDialog::JX"
#define kJXFindPreviousAction			"FindPrevious::JX"
#define kJXFindNextAction				"FindNext::JX"
#define kJXEnterSearchTextAction		"EnterSearchText::JX"
#define kJXEnterReplaceTextAction		"EnterReplaceText::JX"
#define kJXFindSelectionBackwardsAction	"FindSelectionBackwards::JX"
#define kJXFindSelectionForwardAction	"FindSelectionForward::JX"
#define kJXFindClipboardBackwardsAction	"FindClipboardBackwards::JX"
#define kJXFindClipboardForwardAction	"FindClipboardForward::JX"
#define kJXReplaceSelectionAction		"ReplaceSelection::JX"
#define kJXReplaceFindPrevAction		"ReplaceFindPrev::JX"
#define kJXReplaceFindNextAction		"ReplaceFindNext::JX"
#define kJXReplaceAllAction				"ReplaceAll::JX"
#define kJXReplaceAllInSelectionAction	"ReplaceAllInSelection::JX"

// Font size menu

// [0-9]+::JX
#define kJXOtherFontSizeAction	"OtherFontSize::JX"

// Font style menu

#define kJXPlainStyleAction				"PlainStyle::JX"
#define kJXBoldStyleAction				"BoldStyle::JX"
#define kJXItalicStyleAction			"ItalicStyle::JX"
#define kJXUnderlineStyleAction			"UnderlineStyle::JX"
#define kJXDoubleUnderlineStyleAction	"DoubleUnderlineStyle::JX"
#define kJXStrikeStyleAction			"StrikeStyle::JX"
#define kJXBlackStyleAction				"BlackStyle::JX"
#define kJXGrayStyleAction				"GrayStyle::JX"
#define kJXBrownStyleAction				"BrownStyle::JX"
#define kJXOrangeStyleAction			"OrangeStyle::JX"
#define kJXRedStyleAction				"RedStyle::JX"
#define kJXDarkRedStyleAction			"DarkRedStyle::JX"
#define kJXGreenStyleAction				"GreenStyle::JX"
#define kJXBlueStyleAction				"BlueStyle::JX"
#define kJXLightBlueStyleAction			"LightBlueStyle::JX"
#define kJXPinkStyleAction				"PinkStyle::JX"
#define kJXOtherColorStyleAction		"OtherColorStyle::JX"

// Windows menu

#define kJXRaiseAllWindowsAction		"RaiseAllWindows::JX"
#define kJXCloseAllOtherWindowsAction	"CloseAllOtherWindows::JX"

// Help menu

#define kJXHelpTOCAction		"HelpTOC::JX"
#define kJXHelpSpecificAction	"HelpSpecific::JX"

#endif
