// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_LayoutContainer_Layout
#define _H_LayoutContainer_Layout

static const JUtf8Byte* kLayoutMenuStr =
"* %i EditConfiguration::LayoutContainer %l"
"|* %i SelectParent::LayoutContainer"
;

#include "LayoutContainer-Layout-enum.h"

#ifndef _H_select_parent
#define _H_select_parent
#include "select_parent.xpm"
#endif

static void ConfigureLayoutMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#l");
	}
	menu->SetItemImage(kSelectParentCmd + offset, select_parent);
};

#endif