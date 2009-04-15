/******************************************************************************
 jXActions.cpp

	Shared actions for use as menu item ID's and in keybinding tables.

	Copyright © 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <jXActionDefs.h>

// File menu

const JCharacter*
JXGetPageSetupAction()
{
	return kJXPageSetupAction;
}

const JCharacter*
JXGetPrintAction()
{
	return kJXPrintAction;
}

const JCharacter*
JXGetCloseWindowAction()
{
	return kJXCloseWindowAction;
}

const JCharacter*
JXGetQuitAction()
{
	return kJXQuitAction;
}

// Edit menu

const JCharacter*
JXGetUndoAction()
{
	return kJXUndoAction;
}

const JCharacter*
JXGetRedoAction()
{
	return kJXRedoAction;
}

const JCharacter*
JXGetCutAction()
{
	return kJXCutAction;
}

const JCharacter*
JXGetCopyAction()
{
	return kJXCopyAction;
}

const JCharacter*
JXGetPasteAction()
{
	return kJXPasteAction;
}

const JCharacter*
JXGetClearAction()
{
	return kJXClearAction;
}

const JCharacter*
JXGetSelectAllAction()
{
	return kJXSelectAllAction;
}

// Help menu

const JCharacter*
JXGetHelpTOCAction()
{
	return kJXHelpTOCAction;
}

const JCharacter*
JXGetHelpSpecificAction()
{
	return kJXHelpSpecificAction;
}

// Search menu

const JCharacter*
JXGetFindDialogAction()
{
	return kJXFindDialogAction;
}

const JCharacter*
JXGetFindPreviousAction()
{
	return kJXFindPreviousAction;
}

const JCharacter*
JXGetFindNextAction()
{
	return kJXFindNextAction;
}

const JCharacter*
JXGetEnterSearchTextAction()
{
	return kJXEnterSearchTextAction;
}

const JCharacter*
JXGetEnterReplaceTextAction()
{
	return kJXEnterReplaceTextAction;
}

const JCharacter*
JXGetFindSelectionBackwardsAction()
{
	return kJXFindSelectionBackwardsAction;
}

const JCharacter*
JXGetFindSelectionForwardAction()
{
	return kJXFindSelectionForwardAction;
}

const JCharacter*
JXGetFindClipboardBackwardsAction()
{
	return kJXFindClipboardBackwardsAction;
}

const JCharacter*
JXGetFindClipboardForwardAction()
{
	return kJXFindClipboardForwardAction;
}

const JCharacter*
JXGetReplaceSelectionAction()
{
	return kJXReplaceSelectionAction;
}

const JCharacter*
JXGetReplaceFindPrevAction()
{
	return kJXReplaceFindPrevAction;
}

const JCharacter*
JXGetReplaceFindNextAction()
{
	return kJXReplaceFindNextAction;
}

const JCharacter*
JXGetReplaceAllForwardAction()
{
	return kJXReplaceAllForwardAction;
}

const JCharacter*
JXGetReplaceAllBackwardsAction()
{
	return kJXReplaceAllBackwardsAction;
}
