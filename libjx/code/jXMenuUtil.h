/******************************************************************************
 jXMenuUtil.h

   Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#ifndef _H_jXMenuUtil
#define _H_jXMenuUtil

#include "JXMenu.h"		// need ItemType

class JString;

void	JXParseMenuItemStr(JString* text, bool* isActive,
						   bool* hasSeparator, JXMenu::ItemType* type,
						   JString* shortcuts, JString* nmShortcut,
						   JString* id);

bool	JXParseNMShortcut(JString* str, int* key,
						  JXKeyModifiers* modifiers,
						  const bool useMacOSSymbols);

void	JXAdjustNMShortcutString(JString* str, const JUtf8Byte* origStr,
								 const JXModifierKey newKey);

JXModifierKey	JXAdjustNMShortcutModifier(const JXModifierKey key);

#endif
