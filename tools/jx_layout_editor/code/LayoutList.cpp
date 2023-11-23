/******************************************************************************
 LayoutList.cpp

	BASE CLASS = JXStringList

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "LayoutList.h"
#include "MainDocument.h"
#include "actionDefs.h"
#include "globals.h"
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAsciiConstants.h>
#include <jx-af/jcore/jAssert.h>

// Edit menu

static const JUtf8Byte* kEditMenuStr =
	"    Cut        %k Meta-X %i" kJXCutAction
	"  | Copy       %k Meta-C %i" kJXCopyAction
	"  | Paste      %k Meta-V %i" kJXPasteAction
	"  | Delete               %i" kJXClearAction
	"%l| Select all %k Meta-A %i" kJXSelectAllAction;

enum
{
	kCutIndex = 1, kCopyIndex, kPasteIndex, kClearIndex,
	kSelectAllIndex
};

#include <jx-af/image/jx/jx_edit_cut.xpm>
#include <jx-af/image/jx/jx_edit_copy.xpm>
#include <jx-af/image/jx/jx_edit_paste.xpm>
#include <jx-af/image/jx/jx_edit_clear.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

LayoutList::LayoutList
	(
	MainDocument*		doc,
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStringList(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	itsDoc(doc)
{
	itsEditMenu = menuBar->AppendTextMenu(JGetString("EditMenuTitle::JXGlobal"));
	itsEditMenu->SetMenuItems(kEditMenuStr, "MainDocument");
	itsEditMenu->SetUpdateAction(JXMenu::kDisableAll);
	itsEditMenu->AttachHandlers(this,
		&LayoutList::UpdateEditMenu,
		&LayoutList::HandleEditMenu);

	itsEditMenu->SetItemImage(kCutIndex,   jx_edit_cut);
	itsEditMenu->SetItemImage(kCopyIndex,  jx_edit_copy);
	itsEditMenu->SetItemImage(kPasteIndex, jx_edit_paste);
	itsEditMenu->SetItemImage(kClearIndex, jx_edit_clear);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LayoutList::~LayoutList()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
LayoutList::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (button > kJXRightButton)
	{
		ScrollForWheel(button, modifiers);
	}
	else if (!GetCell(pt, &cell))
	{
		s.ClearSelection();
	}
	else if (button == kJXLeftButton && modifiers.shift())
	{
		s.InvertCell(cell);
	}
	else if (button == kJXLeftButton)
	{
		s.ClearSelection();
		s.SelectCell(cell);

		if (clickCount == 2)
		{
			OpenSelectedLayouts();
		}
	}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
LayoutList::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (c == kJReturnKey)
	{
		OpenSelectedLayouts();
	}
	else if (c == kJUpArrow && GetTableSelection().GetFirstSelectedCell(&cell))
	{
		cell.y = JMax(1, cell.y-1);
		SelectSingleCell(cell);
	}
	else if (c == kJUpArrow)
	{
		SelectSingleCell(JPoint(1, GetRowCount()));
	}
	else if (c == kJDownArrow && GetTableSelection().GetFirstSelectedCell(&cell))
	{
		cell.y = JMin(GetRowCount(), cell.y+1);
		SelectSingleCell(cell);
	}
	else if (c == kJDownArrow)
	{
		SelectSingleCell(JPoint(1, 1));
	}
	else
	{
		JXStringList::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 OpenSelectedLayouts (private)

 ******************************************************************************/

void
LayoutList::OpenSelectedLayouts()
	const
{
	const JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);

	const JPtrArray<JString>& names = GetStringList();

	JPoint cell;
	while (iter.Next(&cell))
	{
		itsDoc->OpenLayout(*names.GetItem(cell.y));
	}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
LayoutList::UpdateEditMenu()
{
	if (GetRowCount() == 0)
	{
		return;
	}

	if (GetTableSelection().HasSelection())
	{
		itsEditMenu->EnableItem(kCutIndex);
		itsEditMenu->EnableItem(kCopyIndex);
		itsEditMenu->EnableItem(kClearIndex);
	}

	itsEditMenu->EnableItem(kSelectAllIndex);
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
LayoutList::HandleEditMenu
	(
	const JIndex index
	)
{
	if (index == kCutIndex)
	{
	}
	else if (index == kCopyIndex)
	{
	}
	else if (index == kPasteIndex)
	{
	}
	else if (index == kClearIndex)
	{
	}

	else if (index == kSelectAllIndex)
	{
		GetTableSelection().SelectCol(1);
	}
}
