// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_JXTEBase_ReadOnly
#define _H_JXTEBase_ReadOnly

static const JUtf8Byte* kReadOnlyMenuStr =
"* %b %i ToggleReadOnlyCmd::JXTEBase"
;

#include "JXTEBase-ReadOnly-enum.h"

#ifndef _H_jx_af_image_jx_jx_edit_read_only
#define _H_jx_af_image_jx_jx_edit_read_only
#include <jx-af/image/jx/jx_edit_read_only.xpm>
#endif

static void ConfigureReadOnlyMenu(JXTextMenu* menu, const int offset = 0) {
	menu->SetItemImage(__ToggleReadOnlyCmd + offset, jx_edit_read_only);
};

#endif
