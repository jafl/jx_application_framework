// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_ClipboardWidget_Edit
#define _H_ClipboardWidget_Edit

static const JUtf8Byte* kEditMenuStr =
"* %i __Copy::ClipboardWidget"
"|* %i __Paste::ClipboardWidget"
;

#include "ClipboardWidget-Edit-enum.h"


static void ConfigureEditMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#e");
	}
};

#endif
