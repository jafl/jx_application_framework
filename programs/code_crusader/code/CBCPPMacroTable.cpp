/******************************************************************************
 CBCPPMacroTable.cpp

	BASE CLASS = JXStringTable

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBCPPMacroTable.h"
#include "CBCPreprocessor.h"
#include "CBListCSF.h"
#include "cbGlobals.h"
#include "cbmUtil.h"
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXColHeaderWidget.h>
#include <JStringTableData.h>
#include <JTableSelection.h>
#include <jStreamUtil.h>
#include <jAssert.h>

const JCoordinate kNameColumn  = 1;
const JCoordinate kValueColumn = 2;

const JCoordinate kDefNameColWidth = 100;

static const JCharacter* kDefaultFileName = "cpp.macros";

static const JCharacter* kReplaceListStr  = "Replace existing macros";
static const JCharacter* kAppendToListStr = "Merge with existing macros";

static const JCharacter* kColTitle[] =
{
	"Name",
	"Replace with"
};

const JSize kColCount = sizeof(kColTitle) / sizeof(JCharacter*);

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCPPMacroTable::CBCPPMacroTable
	(
	const CBCPreprocessor&	cpp,
	JXTextButton*			addRowButton,
	JXTextButton*			removeRowButton,
	JXTextButton*			loadButton,
	JXTextButton*			saveButton,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXStringTable(new JStringTableData,
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsFileName(kDefaultFileName)
{
	itsAddRowButton = addRowButton;
	ListenTo(itsAddRowButton);

	itsRemoveRowButton = removeRowButton;
	itsRemoveRowButton->Deactivate();
	ListenTo(itsRemoveRowButton);

	itsLoadButton = loadButton;
	ListenTo(itsLoadButton);

	itsSaveButton = saveButton;
	ListenTo(itsSaveButton);

	itsCSF = new CBListCSF(kReplaceListStr, kAppendToListStr);
	assert( itsCSF != NULL );

	JString fontName;
	JSize fontSize;
	(CBGetPrefsManager())->GetDefaultFont(&fontName, &fontSize);
	SetFont(fontName, fontSize);

	JStringTableData* data = GetStringData();
	data->AppendCols(2);	// name, value
	FitToEnclosure();		// make sure SetColWidth() won't fail
	ListenTo(this);			// adjust value col width
	SetColWidth(kNameColumn, kDefNameColWidth);

	assert( GetColCount() == kColCount );

	const CBPPMacroList& list = cpp.GetMacroList();
	const JSize count         = list.GetElementCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
		{
		const CBCPreprocessor::MacroInfo info = list.GetElement(i);
		data->SetString(i,kNameColumn,  *(info.name));
		data->SetString(i,kValueColumn, *(info.value));
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCPPMacroTable::~CBCPPMacroTable()
{
	delete GetStringData();
	delete itsCSF;
}

/******************************************************************************
 ContentsValid

	Check that the macro names are unique.

 ******************************************************************************/

JBoolean
CBCPPMacroTable::ContentsValid()
	const
{
	CBCPPMacroTable* me = const_cast<CBCPPMacroTable*>(this);
	if (!me->EndEditing())
		{
		return kJFalse;
		}

	const JStringTableData* data = GetStringData();
	const JSize rowCount         = GetRowCount();
	for (JIndex i=1; i<rowCount; i++)
		{
		const JString& s1 = data->GetElement(i, kNameColumn);
		for (JIndex j=i+1; j<=rowCount; j++)
			{
			const JString& s2 = data->GetElement(j, kNameColumn);
			if (s1 == s2)
				{
				JTableSelection& s = me->GetTableSelection();
				s.ClearSelection();
				s.SelectRow(i);
				s.SelectRow(j);
				me->TableScrollToCell(JPoint(1,i));
				(JGetUserNotification())->ReportError(
					"The macro names must be unique.");
				return kJFalse;
				}
			}
		}

	return kJTrue;
}

/******************************************************************************
 UpdateMacros

	Returns kJTrue if anything changed.

 ******************************************************************************/

JBoolean
CBCPPMacroTable::UpdateMacros
	(
	CBCPreprocessor* cpp
	)
	const
{
	assert( !IsEditing() );

	JBoolean changed = kJFalse;

	const CBPPMacroList& list    = cpp->GetMacroList();
	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	if (count == list.GetElementCount())
		{
		// We know there are the same number of
		// items and no duplicates.

		for (JIndex i=1; i<=count; i++)
			{
			JIndex j;
			const CBCPreprocessor::MacroInfo info
				(const_cast<JString*>(&(data->GetString(i, kNameColumn))),
				 const_cast<JString*>(&(data->GetString(i, kValueColumn))));
			if (!list.SearchSorted(info, JOrderedSetT::kAnyMatch, &j))
				{
				changed = kJTrue;
				break;
				}
			const CBCPreprocessor::MacroInfo info2 = list.GetElement(j);
			if (*(info.value) != *(info2.value))
				{
				changed = kJTrue;
				break;
				}
			}
		}
	else
		{
		changed = kJTrue;
		}

	if (changed)
		{
		cpp->UndefineAllMacros();

		for (JIndex i=1; i<=count; i++)
			{
			cpp->DefineMacro(data->GetString(i, kNameColumn),
							 data->GetString(i, kValueColumn));
			}
		}

	return changed;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBCPPMacroTable::HandleMouseDown
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
CBCPPMacroTable::Receive
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

	else if (sender == itsLoadButton && message.Is(JXButton::kPushed))
		{
		LoadMacros();
		}
	else if (sender == itsSaveButton && message.Is(JXButton::kPushed))
		{
		SaveMacros();
		}

	else
		{
		if (sender == this && message.Is(kColWidthChanged))
			{
			// do it regardless of which column changed so they can't shrink value column
			AdjustColWidths();
			}

		JXStringTable::Receive(sender, message);
		}
}

/******************************************************************************
 AddRow (private)

 ******************************************************************************/

void
CBCPPMacroTable::AddRow()
{
	if (EndEditing())
		{
		JStringTableData* data = GetStringData();
		data->AppendRows(1);
		BeginEditing(JPoint(kNameColumn, GetRowCount()));
		}
}

/******************************************************************************
 RemoveRow (private)

 ******************************************************************************/

void
CBCPPMacroTable::RemoveRow()
{
	JPoint editCell;
	if (GetEditedCell(&editCell))
		{
		CancelEditing();
		(GetStringData())->RemoveRow(editCell.y);
		}
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

 ******************************************************************************/

JXInputField*
CBCPPMacroTable::CreateStringTableInput
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
	(GetTableSelection()).ClearSelection();		// cancel ContentsValid() display
	itsRemoveRowButton->Activate();

	JXInputField* input =
		JXStringTable::CreateStringTableInput(cell, enclosure,
											  hSizing, vSizing, x,y, w,h);
	if (cell.x == kNameColumn)
		{
		input->SetIsRequired();
		}
	input->SetCharacterInWordFunction(CBMIsCharacterInWord);
	return input;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CBCPPMacroTable::PrepareDeleteXInputField()
{
	JXStringTable::PrepareDeleteXInputField();
	itsRemoveRowButton->Deactivate();
}

/******************************************************************************
 LoadMacros (private)

 ******************************************************************************/

void
CBCPPMacroTable::LoadMacros()
{
	JString fileName;
	if (EndEditing() &&
		itsCSF->ChooseFile("", NULL, &fileName))
		{
		ReadData(fileName, itsCSF->ReplaceExisting());
		}
}

/******************************************************************************
 ReadData (private)

	We read the format:  (\n*name\nvalue\n)*

 ******************************************************************************/

void
CBCPPMacroTable::ReadData
	(
	const JCharacter*	fileName,
	const JBoolean		replaceExisting
	)
{
	JStringTableData* data = GetStringData();
	if (replaceExisting)
		{
		data->RemoveAllRows();
		}

	JIndex firstNewRow = 0;

	ifstream input(fileName);
	JString name, value;
	while (!input.eof() && !input.fail())
		{
		name = JReadLine(input);
		if (!name.IsEmpty())
			{
			value = JReadLine(input);
			data->AppendRows(1);
			const JSize rowCount = data->GetRowCount();
			data->SetString(rowCount,kNameColumn,  name);
			data->SetString(rowCount,kValueColumn, value);
			if (firstNewRow == 0)
				{
				firstNewRow = rowCount;
				}
			}
		}

	if (firstNewRow != 0)
		{
		ScrollTo((GetBounds()).bottomLeft());
		BeginEditing(JPoint(kNameColumn, firstNewRow));
		}
}

/******************************************************************************
 SaveMacros (private)

 ******************************************************************************/

void
CBCPPMacroTable::SaveMacros()
	const
{
	JString newName;
	if (const_cast<CBCPPMacroTable*>(this)->EndEditing() &&
		itsCSF->SaveFile("Save macros as:", NULL, itsFileName, &newName))
		{
		itsFileName = newName;
		WriteData(newName);
		}
}

/******************************************************************************
 WriteData (private)

	We write the format:  (name\nvalue\n\n)*

 ******************************************************************************/

void
CBCPPMacroTable::WriteData
	(
	const JCharacter* fileName
	)
	const
{
	ofstream output(fileName);

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		(data->GetString(i, kNameColumn)).Print(output);
		output << '\n';
		(data->GetString(i, kValueColumn)).Print(output);
		output << "\n\n";
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBCPPMacroTable::ReadSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentSetupVersion );

	input >> itsFileName;

	JCoordinate w;
	input >> w;
	SetColWidth(kNameColumn, w);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBCPPMacroTable::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsFileName;
	output << ' ' << GetColWidth(kNameColumn);
	output << ' ';
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
CBCPPMacroTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	for (JIndex i=1; i<=kColCount; i++)
		{
		widget->SetColTitle(i, kColTitle[i-1]);
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
CBCPPMacroTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXStringTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
CBCPPMacroTable::AdjustColWidths()
{
	const JSize apWidth = GetApertureWidth();

	JCoordinate lineWidth;
	JColorIndex color;
	GetColBorderInfo(&lineWidth, &color);
	const JSize usedWidth = GetColWidth(kNameColumn) + lineWidth;

	if (apWidth > usedWidth)
		{
		SetColWidth(kValueColumn, apWidth - usedWidth);
		}
	else
		{
		const JSize nameWidth = apWidth/3;
		SetColWidth(kNameColumn, nameWidth);
		SetColWidth(kValueColumn, apWidth - nameWidth - lineWidth);
		}
}
