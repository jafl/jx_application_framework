/******************************************************************************
 jXActions.h

	Shared actions for use as menu item ID's and in keybinding tables.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_jXActions
#define _H_jXActions

#include <jTypes.h>

// File menu

const JUtf8Byte* JXGetPageSetupAction();
const JUtf8Byte* JXGetPrintAction();
const JUtf8Byte* JXGetCloseWindowAction();
const JUtf8Byte* JXGetQuitAction();

// Edit menu

const JUtf8Byte* JXGetUndoAction();
const JUtf8Byte* JXGetRedoAction();
const JUtf8Byte* JXGetCutAction();
const JUtf8Byte* JXGetCopyAction();
const JUtf8Byte* JXGetPasteAction();
const JUtf8Byte* JXGetClearAction();
const JUtf8Byte* JXGetSelectAllAction();

// Help menu

const JUtf8Byte* JXGetHelpTOCAction();
const JUtf8Byte* JXGetHelpSpecificAction();

// Search menu

const JUtf8Byte* JXGetFindDialogAction();
const JUtf8Byte* JXGetFindPreviousAction();
const JUtf8Byte* JXGetFindNextAction();
const JUtf8Byte* JXGetEnterSearchTextAction();
const JUtf8Byte* JXGetEnterReplaceTextAction();
const JUtf8Byte* JXGetFindSelectionBackwardsAction();
const JUtf8Byte* JXGetFindSelectionForwardAction();
const JUtf8Byte* JXGetFindClipboardBackwardsAction();
const JUtf8Byte* JXGetFindClipboardForwardAction();
const JUtf8Byte* JXGetReplaceSelectionAction();
const JUtf8Byte* JXGetReplaceFindPrevAction();
const JUtf8Byte* JXGetReplaceFindNextAction();
const JUtf8Byte* JXGetReplaceAllForwardAction();
const JUtf8Byte* JXGetReplaceAllBackwardsAction();

#endif
