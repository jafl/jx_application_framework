// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_ClipboardDir_Text
#define _H_ClipboardDir_Text

static const JUtf8Byte* kTextMenuStr =
"* %i __ChangeText::ClipboardDir %l"
"|* %i __Quit::ClipboardDir"
;

#include "ClipboardDir-Text-enum.h"


static void ConfigureTextMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#t");
	}
};

#endif
