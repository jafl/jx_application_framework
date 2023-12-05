// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_App_Help
#define _H_App_Help

static const JUtf8Byte* kHelpMenuStr =
"* %i __About::App %l"
"|* %i HelpTOC::App"
"|* %i __Overview::App"
"|* %i HelpSpecific::App %l"
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

#include <jx-af/image/jx/jx_help_toc.xpm>
#include <jx-af/image/jx/jx_help_specific.xpm>

void SetHelpMenuIcons(JXTextMenu* menu)
{
menu->SetItemImage(kHelpTOCCmd, jx_help_toc);
menu->SetItemImage(kHelpWindowCmd, jx_help_specific);
};

#endif
