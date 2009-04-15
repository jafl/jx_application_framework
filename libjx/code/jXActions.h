/******************************************************************************
 jXActions.h

	Shared actions for use as menu item ID's and in keybinding tables.

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jXActions
#define _H_jXActions

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

// File menu

const JCharacter* JXGetPageSetupAction();
const JCharacter* JXGetPrintAction();
const JCharacter* JXGetCloseWindowAction();
const JCharacter* JXGetQuitAction();

// Edit menu

const JCharacter* JXGetUndoAction();
const JCharacter* JXGetRedoAction();
const JCharacter* JXGetCutAction();
const JCharacter* JXGetCopyAction();
const JCharacter* JXGetPasteAction();
const JCharacter* JXGetClearAction();
const JCharacter* JXGetSelectAllAction();

// Help menu

const JCharacter* JXGetHelpTOCAction();
const JCharacter* JXGetHelpSpecificAction();

// Search menu

const JCharacter* JXGetFindDialogAction();
const JCharacter* JXGetFindPreviousAction();
const JCharacter* JXGetFindNextAction();
const JCharacter* JXGetEnterSearchTextAction();
const JCharacter* JXGetEnterReplaceTextAction();
const JCharacter* JXGetFindSelectionBackwardsAction();
const JCharacter* JXGetFindSelectionForwardAction();
const JCharacter* JXGetFindClipboardBackwardsAction();
const JCharacter* JXGetFindClipboardForwardAction();
const JCharacter* JXGetReplaceSelectionAction();
const JCharacter* JXGetReplaceFindPrevAction();
const JCharacter* JXGetReplaceFindNextAction();
const JCharacter* JXGetReplaceAllForwardAction();
const JCharacter* JXGetReplaceAllBackwardsAction();

#endif
