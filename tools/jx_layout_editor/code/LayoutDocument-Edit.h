// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_LayoutDocument_Edit
#define _H_LayoutDocument_Edit

static const JUtf8Byte* kEditMenuStr =
"* %i Undo::JX"
"|* %i Redo::JX %l"
"|* %i Cut::JX"
"|* %i Copy::JX"
"|* %i Paste::JX"
"|* %i Clear::JX %l"
"|* %i SelectAll::JX"
;

enum {
	kUndoCmd=1,
	kRedoCmd,
	kCutCmd,
	kCopyCmd,
	kPasteCmd,
	kClearCmd,
	kSelectAllCmd,
};

#ifndef _H_jx_af_image_jx_jx_edit_undo
#define _H_jx_af_image_jx_jx_edit_undo
#include <jx-af/image/jx/jx_edit_undo.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_redo
#define _H_jx_af_image_jx_jx_edit_redo
#include <jx-af/image/jx/jx_edit_redo.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_cut
#define _H_jx_af_image_jx_jx_edit_cut
#include <jx-af/image/jx/jx_edit_cut.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_copy
#define _H_jx_af_image_jx_jx_edit_copy
#include <jx-af/image/jx/jx_edit_copy.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_paste
#define _H_jx_af_image_jx_jx_edit_paste
#include <jx-af/image/jx/jx_edit_paste.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_edit_clear
#define _H_jx_af_image_jx_jx_edit_clear
#include <jx-af/image/jx/jx_edit_clear.xpm>
#endif

static void ConfigureEditMenu(JXTextMenu* menu, const int offset = 0) {
	if (menu->IsEmpty() && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#e", JString::kNoCopy));
	}
	menu->SetItemImage(kUndoCmd + offset, jx_edit_undo);
	menu->SetItemImage(kRedoCmd + offset, jx_edit_redo);
	menu->SetItemImage(kCutCmd + offset, jx_edit_cut);
	menu->SetItemImage(kCopyCmd + offset, jx_edit_copy);
	menu->SetItemImage(kPasteCmd + offset, jx_edit_paste);
	menu->SetItemImage(kClearCmd + offset, jx_edit_clear);
};

#endif
