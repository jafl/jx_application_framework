// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_MenuDocument_Preferences
#define _H_MenuDocument_Preferences

static const JUtf8Byte* kPreferencesMenuStr =
"* %i __EditToolBar::MenuDocument"
"|* %i __FileManager::MenuDocument"
"|* %i __MacWinXEmulation::MenuDocument %l"
"|* %i __SaveTableLayoutAsDefault::MenuDocument"
;

enum {
	kEditToolBarCmd=1,
	kFilePrefsCmd,
	kEditMacWinPrefsCmd,
	kSaveLayoutPrefsCmd,
};


static void ConfigurePreferencesMenu(JXTextMenu* menu) {
	if (JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#p", JString::kNoCopy));
	}
};

#endif
