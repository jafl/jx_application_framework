// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_MenuTable_Table
#define _H_MenuTable_Table

static const JUtf8Byte* kTableMenuStr =
"* %i New::MenuTable"
"|* %i Duplicate::MenuTable %l"
"|* %i Import::MenuTable"
;

enum {
	kAppendCmd=1,
	kDuplicateCmd,
	kImportCmd,
};


static void ConfigureTableMenu(JXTextMenu* menu) {
	if (JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#t", JString::kNoCopy));
	}
};

#endif
