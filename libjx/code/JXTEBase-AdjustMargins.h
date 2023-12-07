// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_JXTEBase_AdjustMargins
#define _H_JXTEBase_AdjustMargins

static const JUtf8Byte* kAdjustMarginsMenuStr =
"* %i CleanRightMarginCmd::JXTEBase"
"|* %i CoerceRightMarginCmd::JXTEBase %l"
"|* %i ShiftSelLeftCmd::JXTEBase"
"|* %i ShiftSelRightCmd::JXTEBase"
"|* %i ForceShiftSelLeftCmd::JXTEBase"
;

enum {
	__CleanRightMarginCmd=1,
	__CoerceRightMarginCmd,
	__ShiftSelLeftCmd,
	__ShiftSelRightCmd,
	__ForceShiftSelLeftCmd,
};

#ifndef _H_jx_af_image_jx_jx_edit_clean_right_margin
#define _H_jx_af_image_jx_jx_edit_clean_right_margin
#include <jx-af/image/jx/jx_edit_clean_right_margin.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_coerce_right_margin
#define _H_jx_af_image_jx_jx_edit_coerce_right_margin
#include <jx-af/image/jx/jx_edit_coerce_right_margin.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_shift_left
#define _H_jx_af_image_jx_jx_edit_shift_left
#include <jx-af/image/jx/jx_edit_shift_left.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_shift_right
#define _H_jx_af_image_jx_jx_edit_shift_right
#include <jx-af/image/jx/jx_edit_shift_right.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_force_shift_left
#define _H_jx_af_image_jx_jx_edit_force_shift_left
#include <jx-af/image/jx/jx_edit_force_shift_left.xpm>
#endif

static void ConfigureAdjustMarginsMenu(JXTextMenu* menu, const int offset = 0) {
	menu->SetItemImage(__CleanRightMarginCmd + offset, jx_edit_clean_right_margin);
	menu->SetItemImage(__CoerceRightMarginCmd + offset, jx_edit_coerce_right_margin);
	menu->SetItemImage(__ShiftSelLeftCmd + offset, jx_edit_shift_left);
	menu->SetItemImage(__ShiftSelRightCmd + offset, jx_edit_shift_right);
	menu->SetItemImage(__ForceShiftSelLeftCmd + offset, jx_edit_force_shift_left);
};

#endif
