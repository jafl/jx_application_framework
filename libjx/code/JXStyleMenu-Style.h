// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_JXStyleMenu_Style
#define _H_JXStyleMenu_Style

static const JUtf8Byte* kStyleMenuStr =
"* %b %i PlainStyle::JX %l"
"|* %b %i BoldStyle::JX"
"|* %b %i ItalicStyle::JX"
"|* %b %i UnderlineStyle::JX"
"|* %b %i DoubleUnderlineStyle::JX"
"|* %b %i StrikeStyle::JX %l"
"|* %r %i BlackStyle::JX"
"|* %r %i GrayStyle::JX"
"|* %r %i BrownStyle::JX"
"|* %r %i OrangeStyle::JX"
"|* %r %i RedStyle::JX"
"|* %r %i DarkRedStyle::JX"
"|* %r %i GreenStyle::JX"
"|* %r %i BlueStyle::JX"
"|* %r %i LightBlueStyle::JX"
"|* %r %i PinkStyle::JX %l"
"|* %r %i OtherColorStyle::JX"
;

#include "JXStyleMenu-Style-enum.h"


static void ConfigureStyleMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#y");
	}
};

#endif
