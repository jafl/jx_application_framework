// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TestTextEditDocument_Emulator
#define _H_TestTextEditDocument_Emulator

static const JUtf8Byte* kEmulatorMenuStr =
"* %r %i __None::TestTextEditDocument"
"|* %r %i __vi::TestTextEditDocument"
;

enum {
	kNoEmulatorCmd=1,
	kVIEmulatorCmd,
};


static void ConfigureEmulatorMenu(JXTextMenu* menu, const int offset = 0) {
	if (menu->IsEmpty() && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#e", JString::kNoCopy));
	}
};

#endif
