/******************************************************************************
 CBCPPMacroTable.cpp

	BASE CLASS = JXStringTable

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

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

static const JUtf8Byte* kDefaultFileName = "cpp.macros";

const JSize kColCount = 2;

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
	JXStringTable(jnew JStringTableData,
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

	itsCSF = jnew CBListCSF(JGetString("ReplaceList::CBCPPMacroTable"), JGetString("AppendToList::CBCPPMacroTable"));
	assert( itsCSF != nullptr );

	JString fontName;
	JSize fontSize;
	CBGetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	SetFont(fontName, fontSize);

	JStringTableData* data = GetStringData();
	data->AppendCols(kColCount);	// name, value
	FitToEnclosure();				// make sure SetColWidth() won't fail
	ListenTo(this);					// adjust value col width
	SetColWidth(kNameColumn, kDefNameColWidth);

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
	jdelete GetStringData();
	jdelete itsCSF;
}

/******************************************************************************
 ContentsValid

	Check that the macro names are unique.

 ******************************************************************************/

bool
CBCPPMacroTable::ContentsValid()
	const
{
	auto* me = const_cast<CBCPPMacroTable*>(this);
	if (!me->EndEditing())
		{
		return false;
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
				JGetUserNotification()->ReportError(
					JGetString("UniqueName::CBCPPMacroTable"));
				return false;
				}
			}
		}

	return true;
}

/******************************************************************************
 UpdateMacros

	Returns true if anything changed.

 ******************************************************************************/

bool
CBCPPMacroTable::UpdateMacros
	(
	CBCPreprocessor* cpp
	)
	const
{
	assert( !IsEditing() );

	bool changed = false;

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
			if (!list.SearchSorted(info, JListT::kAnyMatch, &j))
				{
				changed = true;
				break;
				}
			const CBCPreprocessor::MacroInfo info2 = list.GetElement(j);
			if (*(info.value) != *(info2.value))
				{
				changed = true;
				break;
				}
			}
		}
	else
		{
		changed = true;
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
		GetStringData()->RemoveRow(editCell.y);
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
	input->GetText()->SetCharacterInWordFunction(CBMIsCharacterInWord);
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
		itsCSF->ChooseFile(JString::empty, JString::empty, &fileName))
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
	const JString&	fileName,
	const bool	replaceExisting
	)
{
	JStringTableData* data = GetStringData();
	if (replaceExisting)
		{
		data->RemoveAllRows();
		}

	JIndex firstNewRow = 0;

	std::ifstream input(fileName.GetBytes());
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
		itsCSF->SaveFile(JGetString("SavePrompt::CBCPPMacroTable"), JString::empty, itsFileName, &newName))
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
	const JString& fileName
	)
	const
{
	std::ofstream output(fileName.GetBytes());

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		data->GetString(i, kNameColumn).Print(output);
		output << '\n';
		data->GetString(i, kValueColumn).Print(output);
		output << "\n\n";
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBCPPMacroTable::ReadSetup
	(
	std::istream& input
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
	std::ostream& output
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
	widget->SetColumnTitles("CBCPPMacroTable", kColCount);
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
	JColorID color;
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
