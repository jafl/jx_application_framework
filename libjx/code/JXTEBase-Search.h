// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_JXTEBase_Search
#define _H_JXTEBase_Search

static const JUtf8Byte* kSearchMenuStr =
"* %i FindDialog::JX"
"|* %i FindPrevious::JX"
"|* %i FindNext::JX %l"
"|* %i EnterSearchText::JX"
"|* %i EnterReplaceText::JX %l"
"|* %i FindSelectionBackwards::JX"
"|* %i FindSelectionForward::JX %l"
"|* %i FindClipboardBackwards::JX"
"|* %i FindClipboardForward::JX"
;

enum {
	__FindDialog_Search=1,
	__FindPrevious_Search,
	__FindNext_Search,
	__EnterSearchText_Search,
	__EnterReplaceText_Search,
	__FindSelectionBackwards_Search,
	__FindSelectionForward_Search,
	__FindClipboardBackwards_Search,
	__FindClipboardForward_Search,
};

#ifndef _H_jx_af_image_jx_jx_find
#define _H_jx_af_image_jx_jx_find
#include <jx-af/image/jx/jx_find.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_find_previous
#define _H_jx_af_image_jx_jx_find_previous
#include <jx-af/image/jx/jx_find_previous.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_find_next
#define _H_jx_af_image_jx_jx_find_next
#include <jx-af/image/jx/jx_find_next.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_find_selection_backwards
#define _H_jx_af_image_jx_jx_find_selection_backwards
#include <jx-af/image/jx/jx_find_selection_backwards.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_find_selection_forward
#define _H_jx_af_image_jx_jx_find_selection_forward
#include <jx-af/image/jx/jx_find_selection_forward.xpm>
#endif

static void ConfigureSearchMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#s", JString::kNoCopy));
	}
	menu->SetItemImage(__FindDialog_Search + offset, jx_find);
	menu->SetItemImage(__FindPrevious_Search + offset, jx_find_previous);
	menu->SetItemImage(__FindNext_Search + offset, jx_find_next);
	menu->SetItemImage(__FindSelectionBackwards_Search + offset, jx_find_selection_backwards);
	menu->SetItemImage(__FindSelectionForward_Search + offset, jx_find_selection_forward);
};

#endif
