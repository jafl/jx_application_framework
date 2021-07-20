/******************************************************************************
 CBMacroSetTable.cpp

	BASE CLASS = JXStringTable

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBMacroSetTable.h"
#include "CBCharActionManager.h"
#include "CBCharActionTable.h"
#include "CBMacroManager.h"
#include "CBMacroTable.h"
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JStringTableData.h>
#include <JTableSelection.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBMacroSetTable::CBMacroSetTable
	(
	JArray<CBPrefsManager::MacroSetInfo>*	macroList,

	const JIndex		initialSelection,
	const JIndex		firstUnusedID,
	CBCharActionTable*	actionTable,
	CBMacroTable*		macroTable,
	JXTextButton*		addRowButton,
	JXTextButton*		removeRowButton,
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
	JXStringTable(jnew JStringTableData,
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsFirstNewID(firstUnusedID)
{
	itsMacroList         = macroList;
	itsOwnsMacroListFlag = true;
	itsLastNewID         = itsFirstNewID - 1;
	itsMacroIndex        = 0;

	itsActionTable = actionTable;
	itsMacroTable  = macroTable;

	itsAddRowButton = addRowButton;
	ListenTo(itsAddRowButton);

	itsRemoveRowButton = removeRowButton;
	itsRemoveRowButton->Deactivate();
	ListenTo(itsRemoveRowButton);

	JStringTableData* data = GetStringData();
	data->AppendCols(1);

	const JSize count = itsMacroList->GetElementCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
		{
		const CBPrefsManager::MacroSetInfo info = itsMacroList->GetElement(i);
		data->SetString(i,1, *(info.name));
		}

	JTableSelection& s = GetTableSelection();
	ListenTo(&s);

	if (data->RowIndexValid(initialSelection))
		{
		s.SelectCell(initialSelection,1);
		}
	else if (data->GetRowCount() > 0)
		{
		s.SelectCell(1,1);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBMacroSetTable::~CBMacroSetTable()
{
	if (itsOwnsMacroListFlag)
		{
		const JSize count = itsMacroList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			CBPrefsManager::MacroSetInfo info = itsMacroList->GetElement(i);
			jdelete info.name;
			jdelete info.action;
			jdelete info.macro;
			}
		jdelete itsMacroList;
		}

	jdelete GetStringData();
}

/******************************************************************************
 ContentsValid

 ******************************************************************************/

bool
CBMacroSetTable::ContentsValid()
	const
{
	auto* me = const_cast<CBMacroSetTable*>(this);

	return me->EndEditing() &&
				itsActionTable->ContentsValid() &&
				itsMacroTable->ContentsValid();
}

/******************************************************************************
 GetCurrentMacroSetName

 ******************************************************************************/

bool
CBMacroSetTable::GetCurrentMacroSetName
	(
	JString* name
	)
	const
{
	if (itsMacroIndex > 0 &&
		const_cast<CBMacroSetTable*>(this)->EndEditing())
		{
		*name = GetStringData()->GetString(itsMacroIndex, 1);
		return true;
		}
	else
		{
		name->Clear();
		return false;
		}
}

/******************************************************************************
 GetMacroList

	The client receives ownership of itsMacroList.  If no macro sets were
	created, *lastNewID < *firstNewID.  Otherwise, all id's between
	*firstNewID and *lastNewID inclusive have been allocated.

 ******************************************************************************/

JArray<CBPrefsManager::MacroSetInfo>*
CBMacroSetTable::GetMacroList
	(
	JIndex* firstNewID,
	JIndex* lastNewID
	)
	const
{
	assert( !IsEditing() &&
			!itsActionTable->IsEditing() &&
			!itsMacroTable->IsEditing() );

	JStringTableData* data = GetStringData();
	const JSize count      = data->GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBPrefsManager::MacroSetInfo info = itsMacroList->GetElement(i);
		*(info.name) = data->GetString(i,1);
		}

	if (itsMacroIndex > 0)
		{
		CBPrefsManager::MacroSetInfo info = itsMacroList->GetElement(itsMacroIndex);
		itsActionTable->GetData(info.action);
		itsMacroTable->GetData(info.macro);
		}

	*firstNewID = itsFirstNewID;
	*lastNewID  = itsLastNewID;

	itsOwnsMacroListFlag = false;
	return itsMacroList;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBMacroSetTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
		{
		BeginEditing(cell);
		}
	else
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBMacroSetTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddRowButton && message.Is(JXButton::kPushed))
		{
		AddRow();
		}
	else if (sender == itsRemoveRowButton && message.Is(JXButton::kPushed))
		{
		RemoveRow();
		}

	else
		{
		if (sender == &(GetTableSelection()))
			{
			SwitchDisplay();
			}

		JXStringTable::Receive(sender, message);
		}
}

/******************************************************************************
 AddRow (private)

 ******************************************************************************/

void
CBMacroSetTable::AddRow()
{
	if (ContentsValid())
		{
		itsLastNewID++;
		CBPrefsManager::MacroSetInfo info(itsLastNewID, jnew JString("New"),
										  jnew CBCharActionManager,
										  jnew CBMacroManager);
		assert( info.name   != nullptr &&
				info.action != nullptr &&
				info.macro  != nullptr );
		itsMacroList->AppendElement(info);

		JStringTableData* data = GetStringData();
		data->AppendRows(1);

		const JPoint newCell(1, GetRowCount());
		data->SetString(newCell, *(info.name));
		BeginEditing(newCell);
		}
}

/******************************************************************************
 RemoveRow (private)

 ******************************************************************************/

void
CBMacroSetTable::RemoveRow()
{
	assert( itsMacroIndex > 0 );

	CancelEditing();
	itsActionTable->CancelEditing();
	itsMacroTable->CancelEditing();

	CBPrefsManager::MacroSetInfo info = itsMacroList->GetElement(itsMacroIndex);
	jdelete info.name;
	jdelete info.action;
	jdelete info.macro;

	if (itsFirstNewID <= info.id && info.id == itsLastNewID)
		{
		itsLastNewID--;
		}
	else if (itsFirstNewID <= info.id && info.id < itsLastNewID)
		{
		JIndex index;
		const bool found =
			CBPrefsManager::FindMacroID(*itsMacroList, itsLastNewID, &index);
		assert( found );

		CBPrefsManager::MacroSetInfo info2 = itsMacroList->GetElement(index);
		info2.id = info.id;
		itsMacroList->SetElement(index, info2);

		itsLastNewID--;
		}

	// itsMacroIndex must be 0 when SwitchDisplay() is called

	const JIndex macroIndex = itsMacroIndex;
	itsMacroIndex = 0;
	itsMacroList->RemoveElement(macroIndex);
	GetStringData()->RemoveRow(macroIndex);
}

/******************************************************************************
 SwitchDisplay (private)

	We cannot validate the outgoing data because we cannot cancel editing.

 ******************************************************************************/

void
CBMacroSetTable::SwitchDisplay()
{
	if (itsMacroIndex > 0)
		{
		const CBPrefsManager::MacroSetInfo info = itsMacroList->GetElement(itsMacroIndex);
		itsActionTable->GetData(info.action);
		itsMacroTable->GetData(info.macro);
		}

	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
		{
		itsMacroIndex = cell.y;

		const CBPrefsManager::MacroSetInfo info = itsMacroList->GetElement(itsMacroIndex);
		itsActionTable->SetData(*(info.action));
		itsMacroTable->SetData(*(info.macro));

		itsRemoveRowButton->Activate();
		}
	else
		{
		itsMacroIndex = 0;

		itsActionTable->ClearData();
		itsMacroTable->ClearData();

		itsRemoveRowButton->Deactivate();
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
CBMacroSetTable::ApertureResized
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
CBMacroSetTable::CreateStringTableInput
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
	SelectSingleCell(cell);

	JXInputField* input =
		JXStringTable::CreateStringTableInput(
							cell, enclosure, hSizing, vSizing, x,y, w,h);
	input->SetIsRequired();
	return input;
}
