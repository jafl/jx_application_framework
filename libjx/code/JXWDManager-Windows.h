// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_JXWDManager_Windows
#define _H_JXWDManager_Windows

static const JUtf8Byte* kWindowsMenuStr =
"* %i RaiseAllWindows::JX"
"|* %i CloseAllOtherWindows::JX %l"
;

#include "JXWDManager-Windows-enum.h"


static void ConfigureWindowsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#w");
	}
};

#endif
