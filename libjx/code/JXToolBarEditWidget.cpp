/******************************************************************************
 JXToolBarEditWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include "jx-af/jx/JXToolBarEditWidget.h"
#include "jx-af/jx/JXToolBarNode.h"
#include "jx-af/jx/JXDirector.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXMenuBar.h"
#include "jx-af/jx/JXTextMenu.h"
#include "jx-af/jx/JXWindowPainter.h"
#include "jx-af/jx/JXSelectionManager.h"
#include "jx-af/jx/JXDNDManager.h"
#include "jx-af/jx/JXChooseSaveFile.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXColorManager.h"
#include "jx-af/jx/JXImage.h"
#include "jx-af/jx/JXImageCache.h"
#include <jx-af/jcore/JTreeList.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

#include <jx-af/image/jx/jx_toolbar_menu_node.xpm>
#include <jx-af/image/jx/jx_toolbar_checked.xpm>
#include <jx-af/image/jx/jx_toolbar_unchecked.xpm>

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

	const auto* tbnode = dynamic_cast<const JXToolBarNode*>(node);
	assert( tbnode != nullptr );

	*image = tbnode->IsChecked() ? itsCheckedItemImage : itsUncheckedItemImage;
	return true;
}

/******************************************************************************
 HandleMouseDown (protected)

 ******************************************************************************/

void
JXToolBarEditWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton	button,
	const JSize			clickCount,
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
			auto* tbnode = dynamic_cast<JXToolBarNode*>(node);
			assert(tbnode != nullptr);
			tbnode->ToggleChecked();
		}
		else
		{
			JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
	}
}
