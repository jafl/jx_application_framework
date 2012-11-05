/******************************************************************************
 CBStylerTable.cpp

	BASE CLASS = JXStringTable

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBStylerTable.h"
#include "CBStylerTableInput.h"
#include "CBStylerTableMenu.h"
#include "cbmUtil.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JStringTableData.h>
#include <JTableSelection.h>
#include <JString.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBStylerTable::CBStylerTable
	(
	const JCharacter**			typeNames,
	const JArray<JFontStyle>&	typeStyles,
	JXScrollbarSet*				scrollbarSet,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXStringTable(new JStringTableData,
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	CBStylerTableX(kCBUnknownFT, kJFalse, NULL, NULL);

	JStringTableData* data = GetStringData();
	data->AppendCols(1);

	JString s;
	const JSize count = typeStyles.GetElementCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
		{
		s = typeNames[i-1];
		data->SetString(i,1, s);

		SetCellStyle(JPoint(1,i), typeStyles.GetElement(i));
		}
}

CBStylerTable::CBStylerTable
	(
	const CBTextFileType					fileType,
	const JArray<CBStylerBase::WordStyle>&	wordList,
	JXTextButton*							addRowButton,
	JXTextButton*							removeButton,
	JXScrollbarSet*							scrollbarSet,
	JXContainer*							enclosure,
	const HSizingOption						hSizing,
	const VSizingOption						vSizing,
	const JCoordinate						x,
	const JCoordinate						y,
	const JCoordinate						w,
	const JCoordinate						h
	)
	:
	JXStringTable(new JStringTableData,
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	CBStylerTableX(fileType, kJTrue, addRowButton, removeButton);

	JStringTableData* data = GetStringData();
	data->AppendCols(1);

	JString s;
	const JSize count = wordList.GetElementCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
		{
		const CBStylerBase::WordStyle info = wordList.GetElement(i);

		s = info.key;
		data->SetString(i,1, s);

		SetCellStyle(JPoint(1,i), info.value);
		}
}

// private

void
CBStylerTable::CBStylerTableX
	(
	const CBTextFileType	fileType,
	const JBoolean			allowEdit,
	JXTextButton*			addRowButton,
	JXTextButton*			removeButton
	)
{
	itsFileType      = fileType;
	itsAllowEditFlag = allowEdit;

	itsAddRowButton = addRowButton;
	if (itsAddRowButton != NULL)
		{
		ListenTo(itsAddRowButton);
		}

	itsRemoveButton = removeButton;
	if (itsRemoveButton != NULL)
		{
		ListenTo(itsRemoveButton);
		}

	SetSelectionBehavior(kJTrue, kJTrue);
	ListenTo(&(GetTableSelection()));

	CBMPrefsManager* prefsMgr = CBMGetPrefsManager();

	JString fontName;
	JSize fontSize;
	prefsMgr->GetDefaultFont(&fontName, &fontSize);
	SetFont(fontName, fontSize);

	const JColorIndex backColor = prefsMgr->GetColor(CBMPrefsManager::kBackColorIndex);
	SetBackColor(backColor);
	SetFocusColor(backColor);
	SetSelectionColor(prefsMgr->GetColor(CBMPrefsManager::kSelColorIndex));

	itsStyleMenu = new CBStylerTableMenu(this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsStyleMenu != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBStylerTable::~CBStylerTable()
{
	delete GetStringData();
}

/******************************************************************************
 GetData

	colorList can be NULL.

 ******************************************************************************/

void
CBStylerTable::GetData
	(
	JArray<JFontStyle>* typeStyles
	)
	const
{
	const JSize count = GetRowCount();
	assert( count == typeStyles->GetElementCount() );

	for (JIndex i=1; i<=count; i++)
		{
		const JFontStyle style = GetCellStyle(JPoint(1,i));
		typeStyles->SetElement(i, style);
		}
}

void
CBStylerTable::GetData
	(
	JStringMap<JFontStyle>* wordStyles
	)
	const
{
	wordStyles->RemoveAll();

	const JStringTableData* data = GetStringData();

	const JSize count = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JPoint cell(1,i);
		const JString& word = data->GetString(cell);
		if (!word.IsEmpty())
			{
			const JFontStyle style = GetCellStyle(cell);
			wordStyles->SetElement(word, style);
			}
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBStylerTable::HandleMouseDown
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
	if (ScrollForWheel(button, modifiers))
		{
		// work has been done
		}
	else if (!GetCell(pt, &cell))
		{
		s.ClearSelection();
		}
	else if ((button == kJXLeftButton && modifiers.meta()) ||
			 button == kJXRightButton)
		{
		if (!s.IsSelected(cell))
			{
			SelectSingleCell(cell, kJFalse);
			}
		DisplayFontMenu(cell, this, pt, buttonStates, modifiers);
		}
	else if (button == kJXLeftButton && clickCount == 2 && itsAllowEditFlag)
		{
		BeginEditing(cell);
		}
	else if (clickCount == 1)
		{
		BeginSelectionDrag(cell, button, modifiers);
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
CBStylerTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ContinueSelectionDrag(pt, modifiers);
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
CBStylerTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	FinishSelectionDrag();
}

/******************************************************************************
 DisplayFontMenu

 ******************************************************************************/

void
CBStylerTable::DisplayFontMenu
	(
	const JPoint&			cell,
	JXContainer*			mouseOwner,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsStyleMenu->PrepareForPopup(cell);
	itsStyleMenu->PopUp(mouseOwner, pt, buttonStates, modifiers);
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
CBStylerTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s          = GetTableSelection();
	const JBoolean hadSelection = s.GetFirstSelectedCell(&topSelCell);

	if (key == kJReturnKey && !IsEditing())
		{
		if (itsAllowEditFlag &&
			hadSelection && s.GetSelectedCellCount() == 1)
			{
			BeginEditing(topSelCell);
			}
		}

	else if ((key == kJUpArrow || key == kJDownArrow) && !IsEditing())
		{
		if (!hadSelection && key == kJUpArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(1, GetRowCount()));
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			}
		}

	else if ((key == 'a' || key == 'A') && modifiers.meta() && !modifiers.shift())
		{
		s.SelectAll();
		}

	else
		{
		JXStringTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBStylerTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddRowButton && message.Is(JXButton::kPushed))
		{
		AddRow();
		}
	else if (sender == itsRemoveButton && message.Is(JXButton::kPushed))
		{
		RemoveSelection();
		}
	else
		{
		if (sender == &(GetTableSelection()))
			{
			UpdateButtons();
			}

		JXStringTable::Receive(sender, message);
		}
}

/******************************************************************************
 AddRow (private)

 ******************************************************************************/

void
CBStylerTable::AddRow()
{
	if (EndEditing() && itsAllowEditFlag)
		{
		(GetStringData())->AppendRows(1);
		BeginEditing(JPoint(1, GetRowCount()));
		}
}

/******************************************************************************
 RemoveSelection (private)

 ******************************************************************************/

void
CBStylerTable::RemoveSelection()
{
	JStringTableData* data = GetStringData();

	JPoint editCell;
	if (GetEditedCell(&editCell))
		{
		CancelEditing();
		data->RemoveRow(editCell.y);
		}
	else
		{
		JTableSelectionIterator iter(&(GetTableSelection()));

		JPoint cell;
		while (iter.Next(&cell))
			{
			data->RemoveRow(cell.y);
			}
		}
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
CBStylerTable::UpdateButtons()
{
	if (itsRemoveButton != NULL && (GetTableSelection()).HasSelection())
		{
		itsRemoveButton->Activate();
		}
	else if (itsRemoveButton != NULL)
		{
		itsRemoveButton->Deactivate();
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
CBStylerTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXStringTable::ApertureResized(dw,dh);
	SetColWidth(1, GetApertureWidth());
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

 ******************************************************************************/

JXInputField*
CBStylerTable::CreateStringTableInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( enclosure == this );

	SelectSingleCell(cell, kJFalse);

	CBStylerTableInput* obj =
		new CBStylerTableInput(itsFileType, this, hSizing, vSizing, x,y, w,h);
	assert( obj != NULL );
	return obj;
}
