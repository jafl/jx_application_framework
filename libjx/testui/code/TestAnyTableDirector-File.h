// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TestAnyTableDirector_File
#define _H_TestAnyTableDirector_File

static const JUtf8Byte* kFileMenuStr =
"* %i __PageSetup::TestAnyTableDirector"
"|* %i __Print::TestAnyTableDirector %l"
"|* %i __Close::TestAnyTableDirector"
;

enum {
	kPageSetupCmd=1,
	kPrintCmd,
	kCloseCmd,
};

#ifndef _H_jx_af_image_jx_jx_file_print
#define _H_jx_af_image_jx_jx_file_print
#include <jx-af/image/jx/jx_file_print.xpm>
#endif

static void ConfigureFileMenu(JXTextMenu* menu, const int offset = 0) {
	if (menu->IsEmpty() && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#f", JString::kNoCopy));
	}
	menu->SetItemImage(kPrintCmd + offset, jx_file_print);
};

#endif