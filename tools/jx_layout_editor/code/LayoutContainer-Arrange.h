// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_LayoutContainer_Arrange
#define _H_LayoutContainer_Arrange

static const JUtf8Byte* kArrangeMenuStr =
"* %i AlignLeft::LayoutContainer"
"|* %i AlignCenter::LayoutContainer"
"|* %i AlignRight::LayoutContainer %l"
"|* %i AlignTop::LayoutContainer"
"|* %i AlignMiddle::LayoutContainer"
"|* %i AlignBottom::LayoutContainer %l"
"|* %i DistributeHorizontally::LayoutContainer"
"|* %i DistributeVertically::LayoutContainer"
;

#include "LayoutContainer-Arrange-enum.h"

#ifndef _H_align_left
#define _H_align_left
#include "align_left.xpm"
#endif
#ifndef _H_align_center
#define _H_align_center
#include "align_center.xpm"
#endif
#ifndef _H_align_right
#define _H_align_right
#include "align_right.xpm"
#endif

static void ConfigureArrangeMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#a");
	}
	menu->SetItemImage(kAlignLeftCmd + offset, align_left);
	menu->SetItemImage(kAlignHorizCenterCmd + offset, align_center);
	menu->SetItemImage(kAlignRightCmd + offset, align_right);
};

#endif
