/******************************************************************************
 JXToolBarEditWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include "JXToolBarEditWidget.h"
#include "JXToolBarNode.h"
#include "JXDirector.h"
#include "JXWindow.h"
#include "JXMenuBar.h"
#include "JXTextMenu.h"
#include "JXWindowPainter.h"
#include "JXSelectionManager.h"
#include "JXDNDManager.h"
#include "JXDisplay.h"
#include "JXColorManager.h"
#include "JXImage.h"
#include "JXImageCache.h"
#include <jx-af/jcore/JTreeList.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

#include "jx_toolbar_menu_node.xpm"
#include "jx_toolbar_checked.xpm"
#include "jx_toolbar_unchecked.xpm"

/******************************************************************************
 Constructor

 *****************************************************************************/

JXToolBarEditWidget::JXToolBarEditWidget
	(
	JNamedTreeList*		treeList,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXNamedTreeListWidget(treeList, scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXImageCache* cache   = GetDisplay()->GetImageCache();
	itsMenuImage          = cache->GetImage(jx_toolbar_menu_node);
	itsCheckedItemImage   = cache->GetImage(jx_toolbar_checked);
	itsUncheckedItemImage = cache->GetImage(jx_toolbar_unchecked);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXToolBarEditWidget::~JXToolBarEditWidget()
{
}

/******************************************************************************
 GetImage (protected)

 ******************************************************************************/

bool
JXToolBarEditWidget::GetImage
	(
	const JIndex	index,
	const JXImage**	image
	)
	const
{
	const JTreeNode* node = GetTreeList()->GetNode(index);
	if (node->IsOpenable())
	{
		*image = itsMenuImage;
		return true;
	}

	auto& tbnode = dynamic_cast<const JXToolBarNode&>(*node);
	*image       = tbnode.IsChecked() ? itsCheckedItemImage : itsUncheckedItemImage;
	return true;
}

/******************************************************************************
 HandleMouseDown (protected)

 ******************************************************************************/

void
JXToolBarEditWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (GetCell(pt, &cell))
	{
		JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (cell.x == 2 && button == kJXLeftButton && !node->IsOpenable())
		{
			auto& tbnode = dynamic_cast<JXToolBarNode&>(*node);
			tbnode.ToggleChecked();
		}
		else
		{
			JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
	}
}
