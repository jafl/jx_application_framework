// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_MenuHelloDir_Text
#define _H_MenuHelloDir_Text

static const JUtf8Byte* kTextMenuStr =
"* %i __HelloWorld::MenuHelloDir"
"|* %i __GoodbyeCruelWorld::MenuHelloDir"
;

#include "MenuHelloDir-Text-enum.h"


static void ConfigureTextMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#t");
	}
};

#endif
