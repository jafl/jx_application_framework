/******************************************************************************
 MenuTable.cpp

	BASE CLASS = JXEditTable, JPrefObject

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#include "MenuTable.h"
#include "MenuItemSelection.h"
#include "MenuDocument.h"
#include "ImportDialog.h"
#include "ImageCache.h"
#include "globals.h"
#include "fileVersions.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXImageMenu.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/jXMenuUtil.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jMouseUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kMaxIconSize = 20;

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

enum
{
	kTypeColumn = 1,
	kIconColumn,
	kTextColumn,
	kWindowsKeyColumn,
	kShortcutColumn,
	kIDColumn,
	kEnumColumn,
	kSeparatorColumn
};

const JCoordinate kInitColWidth[] =
{
	40, 40, 200, 40, 100, 200, 200, 75
};

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

static const JUtf8Byte* kNamespaceOperator = "::";

#include "MenuTable-Table.h"
#include "MenuTable-Type.h"

// prefs

const JFileVersion kCurrentPrefsVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

MenuTable::MenuTable
	(
	MenuDocument*		doc,
	JXMenuBar*			menuBar,
	JXTEBase*			editMenuProvider,
	JXTextMenu*			editMenu,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	JPrefObject(GetPrefsManager(), kMenuTableID),
	itsDoc(doc),
	itsEditMenuProvider(editMenuProvider),
	itsEditMenu(editMenu),
	itsTextInput(nullptr),
	itsCharInput(nullptr)
{
	itsDNDRowIndex = 0;

	itsMenuItemXAtom =
		GetDisplay()->RegisterXAtom(MenuItemSelection::GetMenuItemXAtomName());

	const JSize rowHeight = JMax((JSize)kMaxIconSize, 2*kVMarginWidth +
		JFontManager::GetDefaultFont().GetLineHeight(GetFontManager()));
	SetDefaultRowHeight(rowHeight);

	itsIconPathList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	// menus

	itsTableMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::MenuTable_Table"));
	itsTableMenu->SetMenuItems(kTableMenuStr);
	itsTableMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTableMenu->AttachHandler(this, &MenuTable::HandleTableMenu);
	ConfigureTableMenu(itsTableMenu);

	menuBar->InsertMenuAfter(itsEditMenu, itsTableMenu);

	itsTypeMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	itsTypeMenu->SetMenuItems(kTypeMenuStr);
	itsTypeMenu->SetToHiddenPopupMenu();
	itsTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTypeMenu->AttachHandlers(this,
		&MenuTable::UpdateTypeMenu,
		&MenuTable::HandleTypeMenu);
	ConfigureTypeMenu(itsTypeMenu);

	itsIconMenu = jnew JXImageMenu(JString::empty, 10, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	itsIconMenu->SetToHiddenPopupMenu();
	itsIconMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsIconMenu->AttachHandler(this, &MenuTable::HandleIconMenu);
	BuildIconMenu();

	ListenTo(itsEditMenu);
	ListenTo(this);

	// data

	itsItemList = jnew ItemList;

	for (const auto w : kInitColWidth)
	{
		AppendCols(1, w);
	}

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MenuTable::~MenuTable()
{
	itsItemList->DeleteAll();
	jdelete itsItemList;

	jdelete itsIconPathList;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
MenuTable::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXEditTable::Draw(p, rect);

	if (itsDNDRowIndex > 0)
	{
		p.ResetClipRect();

		const JSize origLineWidth = p.GetLineWidth();
		p.SetLineWidth(3);

		const JRect b = GetBounds();
		if (RowIndexValid(itsDNDRowIndex))
		{
			const JRect r = GetCellRect(JPoint(1, itsDNDRowIndex));
			p.Line(b.left, r.top, b.right, r.top);
		}
		else
		{
			const JRect r = GetCellRect(JPoint(1, GetRowCount()));
			p.Line(b.left, r.bottom, b.right, r.bottom);
		}

		p.SetLineWidth(origLineWidth);
	}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
MenuTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
	{
		return;
	}

	HilightIfSelected(p, cell, rect);

	const ItemInfo info = itsItemList->GetItem(cell.y);
	if (info.separator)
	{
		JPoint pt1 = rect.bottomLeft(), pt2 = rect.bottomRight();
		pt1.y--;
		pt2.y--;
		p.Line(pt1, pt2);
	}

	if (!GetTableSelection().IsSelected(cell) &&
		(cell.x == kTypeColumn || cell.x == kIconColumn))
	{
		p.SetFilling(true);
		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.Rect(rect);
		p.ResetAllButClipping();
	}

	if (cell.x == kTypeColumn)
	{
		const JPoint pt = rect.center();
		JRect r(pt.y, pt.x, pt.y, pt.x);
		if (info.type == JXMenu::kCheckboxType)
		{
			r.Expand(5, 5);
			JXDrawUpFrame(p, r, kJXDefaultBorderWidth, true, JColorManager::GetDefaultBackColor());
		}
		else if (info.type == JXMenu::kRadioType)
		{
			r.Expand(6, 6);
			JXDrawUpDiamond(p, r, kJXDefaultBorderWidth, true, JColorManager::GetDefaultBackColor());
		}
	}
	else if (cell.x == kIconColumn)
	{
		if (info.iconIndex > 0)
		{
			const JXImage* image = itsIconMenu->GetItemImage(info.iconIndex+1);
			const JRect srcRect  = image->GetBounds();

			JRect destRect(rect.center(), rect.center());
			destRect.Expand(srcRect.width()/2, srcRect.height()/2);
			p.Image(*image, srcRect, destRect.topLeft());
		}
	}
	else if (cell.x == kSeparatorColumn)
	{
		const JPoint pt = rect.center();
		JRect r(pt.y, pt.x, pt.y, pt.x);
		if (info.separator)
		{
			r.Expand(3, 3);
			p.SetFilling(true);
			p.Ellipse(r);
		}		
	}
	else if (cell.x == kWindowsKeyColumn)
	{
		p.String(rect, info.windowsKey, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	}
	else
	{
		const JString* s = nullptr;
		if (cell.x == kTextColumn)
		{
			s = info.text;
		}
		else if (cell.x == kShortcutColumn)
		{
			s = info.shortcut;
		}
		else if (cell.x == kIDColumn)
		{
			s = info.id;
		}
		else if (cell.x == kEnumColumn)
		{
			s = info.enumName;
		}
		assert( s != nullptr );

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, *s, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
MenuTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsStartPt = pt;

	JPoint cell;
	if (ScrollForWheel(button, modifiers) ||
		!GetCell(pt, &cell))
	{
		return;
	}

	if (cell.x == kTypeColumn)
	{
		SelectSingleCell(cell);
		itsTypeMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else if (cell.x == kIconColumn)
	{
		SelectSingleCell(cell);
		itsIconMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else if (cell.x == kSeparatorColumn)
	{
		SelectSingleCell(cell);
		ItemInfo info  = itsItemList->GetItem(cell.y);
		info.separator = ! info.separator;
		itsItemList->SetItem(cell.y, info);
		Refresh();
		itsDoc->DataModified();
	}
	else if (button == kJXLeftButton)
	{
		SelectSingleCell(cell);
		if (clickCount == 2)
		{
			BeginEditing(cell);
		}
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
MenuTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (JMouseMoved(itsStartPt, pt))
	{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetSingleSelectedCell(&cell) && GetCellRect(cell).Contains(itsStartPt))
		{
			auto* data =
				jnew MenuItemSelection(GetDisplay(), this,
									   itsItemList->GetItem(cell.y));

			BeginDND(pt, buttonStates, modifiers, data);
		}
	}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
MenuTable::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const bool move =
		modifiers.GetState(JXAdjustNMShortcutModifier(kJXControlKeyIndex));

	if ((target == this && !move) ||
		(target != this &&  move))
	{
		return GetDNDManager()->GetDNDActionMoveXAtom();
	}
	else
	{
		return GetDNDManager()->GetDNDActionCopyXAtom();
	}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept any drops from ourselves and Command from anybody else.

 ******************************************************************************/

bool
MenuTable::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (source == this)
	{
		return true;
	}
	else if (source == nullptr)
	{
		return false;
	}

	for (auto type : typeList)
	{
		if (type == itsMenuItemXAtom)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
MenuTable::HandleDNDEnter()
{
	itsDNDRowIndex = 0;
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
MenuTable::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	JIndex newRowIndex = itsDNDRowIndex;

	JPoint cell;
	if (GetCell(JPinInRect(pt, GetBounds()), &cell))
	{
		const JRect r = GetCellRect(cell);
		if (pt.y <= r.ycenter())
		{
			newRowIndex = cell.y;
		}
		else
		{
			newRowIndex = cell.y + 1;
		}
	}

	if (newRowIndex != itsDNDRowIndex)
	{
		itsDNDRowIndex = newRowIndex;
		Refresh();
	}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
MenuTable::HandleDNDLeave()
{
	itsDNDRowIndex = 0;
	Refresh();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept Command, we don't bother to check typeList.

 ******************************************************************************/

void
MenuTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();
	JXDNDManager* dndMgr       = GetDNDManager();
	const Atom selName         = dndMgr->GetDNDSelectionName();

	if (source == this && action == dndMgr->GetDNDActionMoveXAtom())
	{
		JPoint cell;
		if (GetTableSelection().GetSingleSelectedCell(&cell) &&
			itsDNDRowIndex != JIndex(cell.y) && itsDNDRowIndex != JIndex(cell.y)+1)
		{
			JIndex newIndex = itsDNDRowIndex;
			if (newIndex > JIndex(cell.y))
			{
				newIndex--;
			}
			newIndex = JMin(newIndex, GetRowCount());

			itsItemList->MoveItemToIndex(cell.y, newIndex);
			MoveRow(cell.y, newIndex);
			SelectSingleCell(JPoint(1, newIndex));
			itsDoc->DataModified();
		}
	}
	else if (source == this)
	{
		JPoint cell;
		if (GetTableSelection().GetSingleSelectedCell(&cell))
		{
			itsItemList->InsertItemAtIndex(
				itsDNDRowIndex, itsItemList->GetItem(cell.y).Copy());
			InsertRows(itsDNDRowIndex, 1);
			SelectSingleCell(JPoint(1, itsDNDRowIndex));
			itsDoc->DataModified();
		}
	}
	else
	{
		Atom returnType;
		unsigned char* data;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;
		if (selMgr->GetData(selName, time, itsMenuItemXAtom,
							&returnType, &data, &dataLength, &delMethod))
		{
			if (returnType == itsMenuItemXAtom)
			{
				const std::string s((char*) data, dataLength);
				std::istringstream input(s);

				ItemInfo info = ReadMenuItem(input, kCurrentMenuFileVersion);
				if (!input.fail())
				{
					const JIndex newIndex = JMax((JIndex) 1, itsDNDRowIndex);
					itsItemList->InsertItemAtIndex(newIndex, info);
					InsertRows(newIndex, 1);
					SelectSingleCell(JPoint(1, newIndex));
					itsDoc->DataModified();

					if (action == dndMgr->GetDNDActionMoveXAtom())
					{
						selMgr->SendDeleteRequest(selName, time);
					}
				}
			}

			selMgr->DeleteData(&data, delMethod);
		}
	}

	HandleDNDLeave();
}

/******************************************************************************
 HandleKeyPress (virtual)

	Backspace/Delete:  clear menu text, menu shortcut, name

 ******************************************************************************/

void
MenuTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	bool cleared = false;
	if (c == kJDeleteKey || c == kJForwardDeleteKey)
	{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetSingleSelectedCell(&cell))
		{
			ItemInfo info = itsItemList->GetItem(cell.y);
			if (cell.x == kTextColumn)
			{
				info.text->Clear();
				cleared = true;
			}
			else if (cell.x == kShortcutColumn)
			{
				info.shortcut->Clear();
				cleared = true;
			}
			else if (cell.x == kWindowsKeyColumn)
			{
				info.windowsKey = ' ';
				itsItemList->SetItem(cell.y, info);
				cleared = true;
			}
			else if (cell.x == kIDColumn)
			{
				info.id->Clear();
				cleared = true;
			}
			else if (cell.x == kEnumColumn)
			{
				info.enumName->Clear();
				cleared = true;
			}
		}
	}

	if (cleared)
	{
		Refresh();
		itsDoc->DataModified();
	}
	else if (IsEditing())
	{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
	}
	else
	{
		HandleSelectionKeyPress(c, modifiers);
	}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

bool
MenuTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return (cell.x == kTextColumn || cell.x == kShortcutColumn ||
			cell.x == kWindowsKeyColumn || cell.x == kIDColumn ||
			cell.x == kEnumColumn);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
MenuTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == nullptr && itsCharInput == nullptr );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	const ItemInfo info = itsItemList->GetItem(cell.y);

	if (cell.x == kWindowsKeyColumn)
	{
		itsCharInput = jnew JXCharInput(this, kFixedLeft, kFixedTop, x,y, w,h);
		itsCharInput->SetCharacter(info.windowsKey);
		itsCharInput->ShareEditMenu(itsEditMenu);
		return itsCharInput;
	}

	itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);
	const JString* text = nullptr;
	if (cell.x == kTextColumn)
	{
		text = info.text;
		itsTextInput->SetIsRequired();
	}
	else if (cell.x == kShortcutColumn)
	{
		text = info.shortcut;
	}
	else if (cell.x == kIDColumn)
	{
		text = info.id;
	}
	else if (cell.x == kEnumColumn)
	{
		text = info.enumName;
		itsTextInput->SetIsRequired();
	}
	assert( text != nullptr );

	itsTextInput->GetText()->SetText(*text);
	itsTextInput->ShareEditMenu(itsEditMenu);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

static const JRegex enumPattern("^[_a-z][_a-z0-9]+$", "i");

bool
MenuTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	ItemInfo info = itsItemList->GetItem(cell.y);

	if (cell.x == kWindowsKeyColumn)
	{
		assert( itsCharInput != nullptr );
		if (itsCharInput->GetCharacter() != info.windowsKey)
		{
			info.windowsKey = itsCharInput->GetCharacter();
			itsItemList->SetItem(cell.y, info);
			itsDoc->DataModified();
		}
		return true;
	}

	assert( itsTextInput != nullptr );

	if (!itsTextInput->InputValid())
	{
		return false;
	}

	JString* s = nullptr;
	if (cell.x == kTextColumn)
	{
		s = info.text;
	}
	else if (cell.x == kShortcutColumn)
	{
		s = info.shortcut;

		JString tmp = itsTextInput->GetText()->GetText();
		int key;
		JXKeyModifiers mod(GetDisplay());
		if (!tmp.IsEmpty() && tmp != *s &&
			!JXParseNMShortcut(&tmp, &key, &mod, false) &&
			!JGetUserNotification()->AskUserNo(JGetString("WarnInvalidShortcut::MenuTable")))
		{
			return false;
		}
	}
	else if (cell.x == kIDColumn)
	{
		s = info.id;

		if (itsTextInput->GetText()->GetText().Contains(" "))
		{
			JGetUserNotification()->ReportError(JGetString("IDCannotIncludeSpace::MenuTable"));
			return false;
		}
	}
	else if (cell.x == kEnumColumn)
	{
		s = info.enumName;

		if (!enumPattern.Match(itsTextInput->GetText()->GetText()))
		{
			JGetUserNotification()->ReportError(JGetString("EnumMustBeValidIdentifier::MenuTable"));
			return false;
		}
	}
	assert( s != nullptr );

	if (itsTextInput->GetText()->GetText() != *s)
	{
		*s = itsTextInput->GetText()->GetText();
		s->TrimWhitespace();
		itsDoc->DataModified();
	}
	return true;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
MenuTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;
	itsCharInput = nullptr;
}

/******************************************************************************
 ValidateWindowsKeys

 ******************************************************************************/

bool
MenuTable::ValidateWindowsKeys()
{
	const JSize count = itsItemList->GetItemCount();
	for (JIndex i=2; i<=count; i++)
	{
		const auto key1 = itsItemList->GetItem(i).windowsKey;
		if (key1.IsSpace())
		{
			continue;
		}

		for (JIndex j=1; j<i; j++)
		{
			const auto key2 = itsItemList->GetItem(j).windowsKey;
			if (key2.IsSpace())
			{
				continue;
			}

			if (key2 == key1)
			{
				SelectSingleCell(JPoint(kWindowsKeyColumn, i));
				GetTableSelection().SelectCell(JPoint(kWindowsKeyColumn, j));
				JGetUserNotification()->ReportError(
					JGetString("DuplicateWindowsKey::MenuTable"));
				return false;
			}
		}
	}

	return true;
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
MenuTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		if (HasFocus())
		{
			UpdateEditMenu();
		}
	}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
	{
		if (HasFocus())
		{
			const auto* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			HandleEditMenu(selection->GetIndex());
		}
	}

	else
	{
		if (message.Is(JTable::kColWidthChanged) ||
			message.Is(JTable::kAllColWidthsChanged))
		{
			itsDoc->DataModified();
		}

		JXEditTable::Receive(sender, message);
	}
}

/******************************************************************************
 EnclosingBoundsResized (virtual protected)

 ******************************************************************************/

void
MenuTable::EnclosingBoundsResized
	(
	const JCoordinate dwb,
	const JCoordinate dhb
	)
{
	itsDoc->DataModified();
	JXEditTable::EnclosingBoundsResized(dwb,dhb);
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
MenuTable::UpdateEditMenu()
{
	JIndex i;
	bool ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kUndoCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kRedoCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kCutCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kPasteCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kDeleteSelCmd, &i);
	assert( ok );
	itsEditMenu->SetItemEnabled(i, GetTableSelection().HasSelection());

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kSelectAllCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
MenuTable::HandleEditMenu
	(
	const JIndex index
	)
{
	JTextEditor::CmdIndex cmd;
	if (!itsEditMenuProvider->EditMenuIndexToCmd(index, &cmd))
	{
		return;
	}

	if (cmd == JTextEditor::kDeleteSelCmd)
	{
		CancelEditing();

		JTableSelectionIterator iter(&GetTableSelection());
		JPoint cell;
		while (iter.Next(&cell))
		{
			ItemInfo info = itsItemList->GetItem(cell.y);
			info.Free();

			itsItemList->RemoveItem(cell.y);
			RemoveRow(cell.y);
		}
		itsDoc->DataModified();
	}
}

/******************************************************************************
 HandleTableMenu (private)

 ******************************************************************************/

void
MenuTable::HandleTableMenu
	(
	const JIndex index
	)
{
	if (index == kAppendCmd)
	{
		AddItem();
	}
	else if (index == kDuplicateCmd)
	{
		DuplicateSelectedItems();
	}

	else if (index == kImportCmd)
	{
		auto* dlog = jnew ImportDialog;
		if (dlog->DoDialog())
		{
			Import(dlog);
		}
	}
}

/******************************************************************************
 AddItem (private)

 ******************************************************************************/

void
MenuTable::AddItem()
{
	if (!EndEditing())
	{
		return;
	}

	JIndex row = GetRowCount()+1;

	JPoint cell;
	if (GetTableSelection().GetSingleSelectedCell(&cell))
	{
		row = cell.y+1;
	}

	ItemInfo info(JXMenu::kPlainType, 0,
				  jnew JString, jnew JString,
				  JUtf8Character(' '), jnew JString, jnew JString, false);
	itsItemList->InsertItemAtIndex(row, info);
	InsertRows(row, 1);
	BeginEditing(JPoint(kEnumColumn, row));
}

/******************************************************************************
 DuplicateSelectedItems (private)

 ******************************************************************************/

void
MenuTable::DuplicateSelectedItems()
{
	JPoint cell;
	if (GetTableSelection().GetFirstSelectedCell(&cell) && EndEditing())
	{
		itsItemList->AppendItem(itsItemList->GetItem(cell.y).Copy());
		AppendRows(1);
		BeginEditing(JPoint(kTextColumn, itsItemList->GetItemCount()));
		itsDoc->DataModified();
	}
}

/******************************************************************************
 RemoveSelectedItem

 ******************************************************************************/

void
MenuTable::RemoveSelectedItem()
{
	JPoint cell;
	if (GetTableSelection().GetSingleSelectedCell(&cell))
	{
		CancelEditing();

		ItemInfo info = itsItemList->GetItem(cell.y);
		info.Free();

		itsItemList->RemoveItem(cell.y);
		RemoveRow(cell.y);
		itsDoc->DataModified();
	}
}

/******************************************************************************
 UpdateTypeMenu (private)

 ******************************************************************************/

void
MenuTable::UpdateTypeMenu()
{
	JPoint cell;
	const bool ok = GetTableSelection().GetSingleSelectedCell(&cell);
	assert( ok );

	const ItemInfo info = itsItemList->GetItem(cell.y);

	itsTypeMenu->CheckItem(info.type+1);
}

/******************************************************************************
 HandleTypeMenu (private)

 ******************************************************************************/

void
MenuTable::HandleTypeMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	const bool ok = GetTableSelection().GetSingleSelectedCell(&cell);
	assert( ok );

	ItemInfo info = itsItemList->GetItem(cell.y);
	info.type     = (JXMenu::ItemType) (index-1);
	itsItemList->SetItem(cell.y, info);
	TableRefreshCell(cell);
	itsDoc->DataModified();
}

/******************************************************************************
 BuildIconMenu (private)

 ******************************************************************************/

static const JString coreIconPath(JX_INCLUDE_PATH "/image/jx", JString::kNoCopy);
static const JString iconPattern("*.png *.gif *.xpm", JString::kNoCopy);

static const char * empty_icon[] = {
"1 1 1 1",
" 	c None",
" "};

void
MenuTable::BuildIconMenu()
{
	itsIconPathList->CleanOut();

	JDirInfo* info;

	JString iconPath;
	if (itsDoc->ExistsOnDisk() &&
		MenuDocument::FindProjectRoot(itsDoc->GetFilePath(), &iconPath))
	{
		iconPath = JCombinePathAndName(iconPath, "image");
		if (JDirInfo::Create(iconPath, &info))
		{
			info->ShowDirs(false);
			info->SetWildcardFilter(iconPattern);
			LoadIcons(*info);
			jdelete info;
		}
	}

	if (JDirInfo::Create(coreIconPath, &info))
	{
		info->ShowDirs(false);
		info->SetWildcardFilter(iconPattern);
		LoadIcons(*info);
		jdelete info;
	}

	auto* image = jnew JXImage(GetDisplay(), empty_icon);
	itsIconMenu->PrependItem(image, true);
}

/******************************************************************************
 LoadIcons (private)

 ******************************************************************************/

static const JRegex excludeIconPattern("_busy|_large|_selected|_pushed|new_planet_software");

void
MenuTable::LoadIcons
	(
	const JDirInfo& info
	)
{
	for (const auto* e : info)
	{
		if (excludeIconPattern.Match(e->GetName()))
		{
			continue;
		}

		JXImage* image;
		if (GetImageCache()->GetImage(e->GetFullName(), &image) &&
			image->GetWidth() <= kMaxIconSize && image->GetHeight() <= kMaxIconSize)
		{
			itsIconMenu->AppendItem(image, false);
			itsIconPathList->Append(e->GetFullName());
		}
	}
}

/******************************************************************************
 RebuildIconMenu

 ******************************************************************************/

void
MenuTable::RebuildIconMenu()
{
	std::stringstream data;
	WriteMenuItems(data);

	itsIconMenu->RemoveAllItems();
	BuildIconMenu();

	data.seekg(0);
	ReadMenuItems(data);
}

/******************************************************************************
 HandleIconMenu (private)

 ******************************************************************************/

void
MenuTable::HandleIconMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	if (GetTableSelection().GetSingleSelectedCell(&cell))
	{
		ItemInfo info  = itsItemList->GetItem(cell.y);
		info.iconIndex = index-1;
		itsItemList->SetItem(cell.y, info);

		TableRefreshCell(cell);
		itsDoc->DataModified();
	}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
MenuTable::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentPrefsVersion)
	{
		ReadGeometry(input);
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
MenuTable::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ';
	WriteGeometry(output);
}

/******************************************************************************
 ReadGeometry

 ******************************************************************************/

void
MenuTable::ReadGeometry
	(
	std::istream& input
	)
{
	JCoordinate w;
	input >> w;
	SetColWidth(kTypeColumn, w);
	input >> w;
	SetColWidth(kIconColumn, w);
	input >> w;
	SetColWidth(kTextColumn, w);
	input >> w;
	SetColWidth(kShortcutColumn, w);
	input >> w;
	SetColWidth(kWindowsKeyColumn, w);
	input >> w;
	SetColWidth(kIDColumn, w);
	input >> w;
	SetColWidth(kEnumColumn, w);
	input >> w;
	SetColWidth(kSeparatorColumn, w);
}

/******************************************************************************
 WriteGeometry

 ******************************************************************************/

void
MenuTable::WriteGeometry
	(
	std::ostream& output
	)
	const
{
	output << GetColWidth(kTypeColumn) << std::endl;
	output << GetColWidth(kIconColumn) << std::endl;
	output << GetColWidth(kTextColumn) << std::endl;
	output << GetColWidth(kShortcutColumn) << std::endl;
	output << GetColWidth(kWindowsKeyColumn) << std::endl;
	output << GetColWidth(kIDColumn) << std::endl;
	output << GetColWidth(kEnumColumn) << std::endl;
	output << GetColWidth(kSeparatorColumn) << std::endl;
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
MenuTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	widget->SetColumnTitles("MenuTable", kColCount);
}

/******************************************************************************
 ReadMenuItems

 ******************************************************************************/

void
MenuTable::ReadMenuItems
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (input.fail() || vers > kCurrentMenuFileVersion)
	{
		return;
	}

	itsItemList->DeleteAll();
	RemoveAllRows();

	while (true)
	{
		bool keepGoing;
		input >> keepGoing;
		if (input.fail() || !keepGoing)
		{
			break;
		}

		ItemInfo info = ReadMenuItem(input, vers);
		itsItemList->AppendItem(info);
	}

	AppendRows(itsItemList->GetItemCount());
}

/******************************************************************************
 ReadMenuItem

 ******************************************************************************/

MenuTable::ItemInfo
MenuTable::ReadMenuItem
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	assert( vers <= kCurrentMenuFileVersion );

	int type;
	input >> type;

	bool isCoreIcon;
	JString iconName;
	input >> isCoreIcon >> iconName;

	JIndex iconIndex = 0;
	if (!iconName.IsEmpty())
	{
		iconName.Prepend(ACE_DIRECTORY_SEPARATOR_STR);
		JIndex i = 0;
		for (const auto* path : *itsIconPathList)
		{
			i++;
			if (path->EndsWith(iconName) &&
				((isCoreIcon && path->StartsWith(JX_INCLUDE_PATH)) || !isCoreIcon))
			{
				iconIndex = i;
				break;
			}
		}
	}

	auto* text = jnew JString;
	input >> *text;

	auto* shortcut = jnew JString;
	input >> *shortcut;

	JUtf8Character key;
	JReadUntil(input, kCharacterMarker);
	input >> key;

	auto* id = jnew JString;
	input >> *id;

	auto* enumName = jnew JString;
	input >> *enumName;

	bool separator;
	input >> separator;

	return ItemInfo((JXMenu::ItemType) type, iconIndex, text, shortcut, key, id, enumName, separator);
}

/******************************************************************************
 WriteMenuItems

 ******************************************************************************/

void
MenuTable::WriteMenuItems
	(
	std::ostream& output
	)
	const
{
	output << kCurrentMenuFileVersion << std::endl;

	for (const auto& item : *itsItemList)
	{
		output << true << std::endl;
		WriteMenuItem(output, item);
		output << std::endl;
	}

	output << false;
}

/******************************************************************************
 WriteMenuItem

 ******************************************************************************/

void
MenuTable::WriteMenuItem
	(
	std::ostream&	output,
	const ItemInfo&	item
	)
	const
{
	output << item.type << std::endl;

	JString iconPath, iconName;
	if (item.iconIndex > 0)
	{
		JSplitPathAndName(*itsIconPathList->GetItem(item.iconIndex), &iconPath, &iconName);
	}
	output << iconPath.StartsWith(JX_INCLUDE_PATH) << ' ' << iconName << std::endl;

	output << *item.text << std::endl;
	output << *item.shortcut << std::endl;
	output << kCharacterMarker << item.windowsKey << std::endl;
	output << *item.id << std::endl;
	output << *item.enumName << std::endl;
	output << item.separator << std::endl;
}

/******************************************************************************
 Import (private)

 ******************************************************************************/

static const JRegex actionUsePattern("\"\\s*([^\\s\";]+)\\s*\"?;?");
static const JRegex emptyActionPattern("\"(\\s*\"|;)");

static const JString coreActionDefsFile = JString(JX_INCLUDE_PATH "/jx/jXActionDefs.h", JString::kNoCopy);

void
MenuTable::Import
	(
	ImportDialog* dlog
	)
{
	// action map

	JStringPtrMap<JString> actionMap(JPtrArrayT::kDeleteAll);
	ImportActionDefs(coreActionDefsFile, &actionMap);

	JString actionDefsFile;
	if (dlog->GetActionDefsFile(&actionDefsFile))
	{
		ImportActionDefs(actionDefsFile, &actionMap);
	}

	// definition

	JString text = dlog->GetMenuText();
	text.TrimWhitespace();

	JStringIterator textIter(&text);
	if (!textIter.Next("\""))
	{
		JGetUserNotification()->ReportError(JGetString("MissingOpenQuote:MenuTable"));
		return;
	}
	textIter.RemoveAllPrev();

	while (textIter.Next(emptyActionPattern))
	{
		textIter.RemoveLastMatch();
	}

	textIter.MoveTo(JStringIterator::kStartAtBeginning, 0);

	JString* action;
	while (textIter.Next(actionUsePattern))
	{
		const JStringMatch m = textIter.GetLastMatch();
		const JString macro  = m.GetSubstring(1);
		if (!actionMap.GetItem(macro, &action))
		{
			const JUtf8Byte* map[] =
			{
				"m", macro.GetBytes()
			};
			const JString msg = JGetString("UnknownActionMacro:MenuTable", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			return;
		}

		textIter.ReplaceLastMatch(*action);
	}
	textIter.Invalidate();

	itsItemList->DeleteAll();
	RemoveAllRows();

	JPtrArray<JString> itemList(JPtrArrayT::kDeleteAll);
	text.Split("|", &itemList);

	JString shortcuts;
	JString defSuffix = "::" + itsDoc->GetClassName();
	for (auto* text : itemList)
	{
		ItemInfo info(jnew JString, jnew JString, jnew JString, jnew JString);

		bool isActive;
		JXParseMenuItemStr(text, &isActive, &info.separator, &info.type,
						   &shortcuts, info.shortcut, info.id);

		info.text->Set(*text);
		if (!shortcuts.IsEmpty())
		{
			info.windowsKey = shortcuts.GetFirstCharacter();
		}

		if (info.id->IsEmpty())
		{
			GenerateUniqueID(&info);
		}
		else if (info.id->EndsWith(defSuffix))
		{
			JStringIterator iter(info.id, JStringIterator::kStartAtEnd);
			const bool found = iter.Prev("::");
			assert( found );
			iter.RemoveAllNext();
		}

		itsItemList->AppendItem(info);
	}

	// enum

	const JString& enumText = dlog->GetEnumText();
	if (!enumText.IsEmpty())
	{
		std::istringstream enumInput(enumText.GetBytes());
		if (enumText.Contains("{"))
		{
			JIgnoreUntil(enumInput, '{');
		}

		for (auto item : *itsItemList)
		{
			JReadUntil(enumInput, 2, ",}", item.enumName);

			JStringIterator iter(item.enumName, JStringIterator::kStartAtEnd);
			if (iter.Prev("="))
			{
				iter.RemoveAllNext();
			}
			item.enumName->TrimWhitespace();
		}
	}

	// clean up

	AppendRows(itsItemList->GetItemCount());
	Refresh();
	itsDoc->DataModified();
}

/******************************************************************************
 ImportActionDefs (private)

 ******************************************************************************/

static const JRegex actionDefPattern("#define\\s+([^\\s]+)\\s+\"([^\"]+)\"");

void
MenuTable::ImportActionDefs
	(
	const JString&			fullName,
	JStringPtrMap<JString>*	actionMap
	)
	const
{
	std::ifstream actionInput(fullName.GetBytes());

	JString line;
	while (actionInput.good())
	{
		line = JReadLine(actionInput);

		const JStringMatch m = actionDefPattern.Match(line, JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
		{
			actionMap->SetNewItem(m.GetSubstring(1), m.GetSubstring(2));
		}
	}
}

/******************************************************************************
 FillInItemIDs

 ******************************************************************************/

void
MenuTable::FillInItemIDs
	(
	const JString& className
	)
{
	for (auto info : *itsItemList)
	{
		if (info.id->IsEmpty())
		{
			GenerateUniqueID(&info);
		}

		if (info.enumName->IsEmpty())
		{
			JString s(*info.id);
			if (!s.StartsWith("__"))
			{
				s.Prepend("__");
			}

			JStringIterator iter(&s);
			if (iter.Next(kNamespaceOperator))
			{
				iter.SkipPrev(strlen(kNamespaceOperator));
				iter.RemoveAllNext();
			}
			iter.Invalidate();

			info.enumName->Set(s);
		}
	}

	Refresh();
}

/******************************************************************************
 GenerateUniqueID (private)

 ******************************************************************************/

static const JRegex invalidCharPattern("[^_a-z0-9]+", "i");

void
MenuTable::GenerateUniqueID
	(
	ItemInfo* info
	)
{
	info->id->Set(*info->text);

	JStringIterator iter(info->id);
	JUtf8Character c;
	while (iter.Next(invalidCharPattern))
	{
		iter.RemoveLastMatch();
		if (iter.Next(&c, JStringIterator::kStay))
		{
			iter.SetNext(c.ToUpper());
		}
	}
	iter.Invalidate();

	info->id->Prepend(JXToolBar::kIgnorePrefix);
}

/******************************************************************************
 GenerateCode

 ******************************************************************************/

void
MenuTable::GenerateCode
	(
	std::ostream&	output,
	const JString&	className,
	const JString&	menuTitle,
	const JString&	menuTitleShortcut,
	std::ostream&	enumOutput,
	const JString&	enumFileName
	)
	const
{
	// definition

	bool first = true;
	for (const auto& item : *itsItemList)
	{
		output << '"';
		if (!first)
		{
			output << '|';
		}
		output << '*';		// actual text stored in strings file

		if (item.type == JXMenu::kCheckboxType)
		{
			output << " %b";
		}
		else if (item.type == JXMenu::kRadioType)
		{
			output << " %r";
		}

		if (!item.id->IsEmpty())
		{
			output << " %i ";
			item.id->Print(output);
			if (!item.id->Contains(kNamespaceOperator))
			{
				output << kNamespaceOperator;
				className.Print(output);
			}
		}

		if (item.separator)
		{
			output << " %l";
		}

		output << '"';
		output << std::endl;

		first = false;
	}

	output << ';' << std::endl << std::endl;

	// enum

	output << "#include " << enumFileName << std::endl << std::endl;

	enumOutput << "enum {" << std::endl;

	first = true;
	for (const auto& item : *itsItemList)
	{
		enumOutput << '\t';
		item.enumName->Print(enumOutput);
		if (first)
		{
			enumOutput << "=1";
			first = false;
		}
		enumOutput << ',' << std::endl;
	}

	enumOutput << "};" << std::endl;

	// attach icons

	bool hasIcons = false;
	JString p, n, r;
	for (const auto& item : *itsItemList)
	{
		if (item.iconIndex > 0)
		{
			const auto* path      = itsIconPathList->GetItem(item.iconIndex);
			const bool isCoreIcon = path->StartsWith(JX_INCLUDE_PATH);
			JSplitPathAndName(*path, &p, &n);
			JSplitRootAndSuffix(n, &r, &p);

			p.Set(isCoreIcon ? "jx_af_image_jx_" : "");

			output << "#ifndef _H_";
			p.Print(output);
			r.Print(output);
			output << std::endl;

			output << "#define _H_";
			p.Print(output);
			r.Print(output);
			output << std::endl;

			output << "#include ";
			output << (isCoreIcon ? "<jx-af/image/jx/" : "\"");
			r.Print(output);
			output << ".xpm";
			output << (isCoreIcon ? '>' : '"') << std::endl;

			output << "#endif" << std::endl;

			hasIcons = true;
		}
	}
	output << std::endl;

	const JUtf8Byte* map[] =
	{
		"title", menuTitle.GetBytes()
	};
	JGetString("ConfigureMenuHeader::MenuTable", map, sizeof(map)).Print(output);

	if (menuTitleShortcut != " ")
	{
		const JUtf8Byte* map[] =
		{
			"key", menuTitleShortcut.GetBytes()
		};
		JGetString("ConfigureShortcut::MenuTable", map, sizeof(map)).Print(output);
	}

	if (hasIcons)
	{
		JString icon;
		for (const auto& item : *itsItemList)
		{
			if (item.iconIndex > 0)
			{
				JSplitPathAndName(*itsIconPathList->GetItem(item.iconIndex), &p, &n);
				JSplitRootAndSuffix(n, &icon, &p);

				const JUtf8Byte* map[] =
				{
					"enum", item.enumName->GetBytes(),
					"icon", icon.GetBytes()
				};
				JGetString("SetIconsLine::MenuTable", map, sizeof(map)).Print(output);
				output << std::endl;
			}
		}
	}

	output << "};" << std::endl;
}

/******************************************************************************
 GenerateStrings

 ******************************************************************************/

void
MenuTable::GenerateStrings
	(
	std::ostream&	output,
	const JString&	className
	)
	const
{
	JString s;
	for (const auto& item : *itsItemList)
	{
		if (item.id->IsEmpty())
		{
			continue;
		}

		s = *item.text;

		if (!item.shortcut->IsEmpty())
		{
			s += " %k ";
			s += *item.shortcut;
		}

		if (item.windowsKey != ' ')
		{
			s += " %h ";
			s += item.windowsKey;
		}

		item.id->Print(output);
		if (!item.id->Contains(kNamespaceOperator))
		{
			output << kNamespaceOperator;
			className.Print(output);
		}
		output << ' ' << s << std::endl;
	}
}

/******************************************************************************
 ItemList functions

 ******************************************************************************/

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
MenuTable::ItemList::DeleteAll()
{
	for (auto info : *this)
	{
		info.Free();
	}
	RemoveAll();
}

/******************************************************************************
 ItemInfo functions

 ******************************************************************************/

/******************************************************************************
 Copy

 ******************************************************************************/

MenuTable::ItemInfo
MenuTable::ItemInfo::Copy()
	const
{
	ItemInfo info = *this;
	info.text     = jnew JString(*this->text);
	info.shortcut = jnew JString(*this->shortcut);
	info.id       = jnew JString(*this->id);
	return info;
}

/******************************************************************************
 Free

 ******************************************************************************/

void
MenuTable::ItemInfo::Free()
{
	jdelete text;
	text = nullptr;

	jdelete shortcut;
	shortcut = nullptr;

	jdelete id;
	id = nullptr;
}
