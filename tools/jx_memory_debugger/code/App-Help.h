// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_App_Help
#define _H_App_Help

static const JUtf8Byte* kHelpMenuStr =
"* %i __About::App %l"
"|* %i HelpTOC::JX"
"|* %i __Overview::App"
"|* %i HelpSpecific::JX %l"
"|* %i __Changes::App"
"|* %i __Credits::App"
;

enum {
	kHelpAboutCmd=1,
	kHelpTOCCmd,
	kHelpOverviewCmd,
	kHelpWindowCmd,
	kHelpChangeLogCmd,
	kHelpCreditsCmd,
};

#ifndef _H_jx_af_image_jx_jx_help_toc
#define _H_jx_af_image_jx_jx_help_toc
#include <jx-af/image/jx/jx_help_toc.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_help_specific
#define _H_jx_af_image_jx_jx_help_specific
#include <jx-af/image/jx/jx_help_specific.xpm>
#endif

static void ConfigureHelpMenu(JXTextMenu* menu, const int offset = 0) {
	if (menu->IsEmpty() && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#h", JString::kNoCopy));
	}
	menu->SetItemImage(kHelpTOCCmd + offset, jx_help_toc);
	menu->SetItemImage(kHelpWindowCmd + offset, jx_help_specific);
};

#endif
