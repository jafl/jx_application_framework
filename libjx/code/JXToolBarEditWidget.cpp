/******************************************************************************
 JXToolBarEditWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include <JXToolBarEditWidget.h>
#include <JXToolBarNode.h>
#include <JXDirector.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXWindowPainter.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXChooseSaveFile.h>
#include <JXDisplay.h>
#include <JXColorManager.h>
#include <JXImage.h>
#include <JTreeList.h>
#include <JTableSelection.h>
#include <jAssert.h>

#include <jx_toolbar_menu_node.xpm>
#include <jx_toolbar_checked.xpm>
#include <jx_toolbar_unchecked.xpm>

const JSize kIdentWidth	= 20;

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
	itsMenuImage = jnew JXImage(GetDisplay(), jx_toolbar_menu_node);
	assert(itsMenuImage != nullptr);

	itsCheckedItemImage = jnew JXImage(GetDisplay(), jx_toolbar_checked);
	assert(itsCheckedItemImage != nullptr);

	itsUncheckedItemImage = jnew JXImage(GetDisplay(), jx_toolbar_unchecked);
	assert(itsUncheckedItemImage != nullptr);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXToolBarEditWidget::~JXToolBarEditWidget()
{
	jdelete itsMenuImage;
	jdelete itsCheckedItemImage;
	jdelete itsUncheckedItemImage;
}

/******************************************************************************
 GetImage (protected)

 ******************************************************************************/

JBoolean
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
		return kJTrue;
		}

	const JXToolBarNode* tbnode = dynamic_cast<const JXToolBarNode*>(node);
	assert( tbnode != nullptr );

	*image = tbnode->IsChecked() ? itsCheckedItemImage : itsUncheckedItemImage;
	return kJTrue;
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
			JXToolBarNode* tbnode = dynamic_cast<JXToolBarNode*>(node);
			assert(tbnode != nullptr);
			tbnode->ToggleChecked();
			}
		else
			{
			JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
			}
		}
}
