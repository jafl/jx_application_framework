/******************************************************************************
 CBCRMRuleTable.cpp

	BASE CLASS = JXStringTable

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCRMRuleTable.h"
#include "CBEditCRMDialog.h"
#include "CBListCSF.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXRegexInput.h>
#include <JXRegexReplaceInput.h>
#include <JXColHeaderWidget.h>
#include <JStringTableData.h>
#include <JTableSelection.h>
#include <JRegex.h>
#include <jStreamUtil.h>
#include <jAssert.h>

static const JCharacter* kReplaceListStr  = "Replace existing rules";
static const JCharacter* kAppendToListStr = "Merge with existing rules";

static const JCharacter* kInitFirstStr   = "[ \\t]*";
static const JCharacter* kInitRestStr    = "[ \\t]*";
static const JCharacter* kInitReplaceStr = "$0";

const JCoordinate kDefaultColWidth = 200;

static const JCharacter* kColTitle[] =
{
	"First line prefix",
	"Rest of lines prefix",
	"Replace prefix with"
};

const JSize kColCount = sizeof(kColTitle) / sizeof(JCharacter*);

// geometry information

const JFileVersion kCurrentGeometryDataVersion = 0;
const JCharacter kGeometryDataEndDelimiter     = '\1';

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCRMRuleTable::CBCRMRuleTable
	(
	CBEditCRMDialog*	dialog,
	JXTextButton*		addRowButton,
	JXTextButton*		removeRowButton,
	JXTextButton*		loadButton,
	JXTextButton*		saveButton,
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
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsDialog = dialog;

	itsAddRowButton = addRowButton;
	ListenTo(itsAddRowButton);

	itsRemoveRowButton = removeRowButton;
	itsRemoveRowButton->Deactivate();
	ListenTo(itsRemoveRowButton);

	itsLoadButton = loadButton;
	ListenTo(itsLoadButton);

	itsSaveButton = saveButton;
	ListenTo(itsSaveButton);

	itsCSF = jnew CBListCSF(kReplaceListStr, kAppendToListStr);
	assert( itsCSF != NULL );

	itsFirstRegex = JTextEditor::CRMRule::CreateFirst(".", "$0");
	itsRestRegex  = JTextEditor::CRMRule::CreateRest(".");

	JStringTableData* data = GetStringData();
	data->AppendCols(3);	// first prefix pattern, rest prefix pattern, replace prefix

	assert( GetColCount() == kColCount );
	for (JIndex i=1; i<=kColCount; i++)
		{
		SetColWidth(i, kDefaultColWidth);
		}

	JString fontName;
	JSize fontSize;
	CBGetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	SetFont(fontName, fontSize);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCRMRuleTable::~CBCRMRuleTable()
{
	jdelete GetStringData();
	jdelete itsCSF;
	jdelete itsFirstRegex;
	jdelete itsRestRegex;
}

/******************************************************************************
 GetData

 ******************************************************************************/

void
CBCRMRuleTable::GetData
	(
	JTextEditor::CRMRuleList* list
	)
	const
{
	assert( !IsEditing() );

	list->DeleteAll();

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		list->AppendElement(
			JTextEditor::CRMRule(data->GetString(i, kFirstColumn),
								 data->GetString(i, kRestColumn),
								 data->GetString(i, kReplaceColumn)));
		}
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
CBCRMRuleTable::SetData
	(
	const JTextEditor::CRMRuleList& list
	)
{
	JStringTableData* data = GetStringData();
	data->RemoveAllRows();

	const JSize count = list.GetElementCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
		{
		const JTextEditor::CRMRule r = list.GetElement(i);
		data->SetString(i,kFirstColumn,   (r.first)->GetPattern());
		data->SetString(i,kRestColumn,    (r.rest)->GetPattern());
		data->SetString(i,kReplaceColumn, (r.first)->GetReplacePattern());
		}

	Activate();
}

/******************************************************************************
 ClearData

 ******************************************************************************/

void
CBCRMRuleTable::ClearData()
{
	GetStringData()->RemoveAllRows();
	Deactivate();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBCRMRuleTable::Activate()
{
	JXStringTable::Activate();
	if (WouldBeActive())
		{
		itsAddRowButton->Activate();
		itsLoadButton->Activate();
		itsSaveButton->Activate();
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
CBCRMRuleTable::Deactivate()
{
	JXStringTable::Deactivate();
	if (!WouldBeActive())
		{
		itsAddRowButton->Deactivate();
		itsLoadButton->Deactivate();
		itsSaveButton->Deactivate();
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBCRMRuleTable::HandleMouseDown
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
CBCRMRuleTable::Receive
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
		LoadRules();
		}
	else if (sender == itsSaveButton && message.Is(JXButton::kPushed))
		{
		SaveRules();
		}

	else
		{
		JXStringTable::Receive(sender, message);
		}
}

/******************************************************************************
 AddRow (private)

 ******************************************************************************/

void
CBCRMRuleTable::AddRow()
{
	if (GetWindow()->OKToUnfocusCurrentWidget())
		{
		JStringTableData* data = GetStringData();
		data->AppendRows(1);

		const JSize rowIndex = GetRowCount();
		data->SetString(rowIndex, kFirstColumn,   kInitFirstStr);
		data->SetString(rowIndex, kRestColumn,    kInitRestStr);
		data->SetString(rowIndex, kReplaceColumn, kInitReplaceStr);

		BeginEditing(JPoint(1, rowIndex));
		}
}

/******************************************************************************
 RemoveRow (private)

 ******************************************************************************/

void
CBCRMRuleTable::RemoveRow()
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
CBCRMRuleTable::CreateStringTableInput
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
	itsRemoveRowButton->Activate();

	JXInputField* input = NULL;
	if (cell.x == kFirstColumn)
		{
		input = jnew JXRegexInput(itsFirstRegex, kJFalse,
								 enclosure, hSizing, vSizing, x,y, w,h);
		}
	else if (cell.x == kRestColumn)
		{
		input = jnew JXRegexInput(itsRestRegex, kJFalse,
								 enclosure, hSizing, vSizing, x,y, w,h);
		}
	else if (cell.x == kReplaceColumn)
		{
		input = jnew JXRegexReplaceInput(itsFirstRegex, kJFalse,
										enclosure, hSizing, vSizing, x,y, w,h);
		}

	assert( input != NULL );
	return input;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CBCRMRuleTable::PrepareDeleteXInputField()
{
	JXStringTable::PrepareDeleteXInputField();
	itsRemoveRowButton->Deactivate();
}

/******************************************************************************
 LoadRules (private)

 ******************************************************************************/

void
CBCRMRuleTable::LoadRules()
{
	JString fileName;
	if (GetWindow()->OKToUnfocusCurrentWidget() &&
		itsCSF->ChooseFile("", NULL, &fileName))
		{
		ReadData(fileName, itsCSF->ReplaceExisting());
		}
}

/******************************************************************************
 ReadData (private)

	We read the format:  (\n*first\n+rest\n+replace\n)*

 ******************************************************************************/

void
CBCRMRuleTable::ReadData
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
	JString first, rest, replace;
	JIndex state = 1;
	while (!input.eof() && !input.fail())
		{
		if (state == 1)
			{
			first = JReadLine(input);
			if (!first.IsEmpty())
				{
				state = 2;
				}
			}
		else if (state == 2)
			{
			rest = JReadLine(input);
			if (!rest.IsEmpty())
				{
				state = 3;
				}
			}
		else if (state == 3)
			{
			replace = JReadLine(input);
			if (!replace.IsEmpty())
				{
				data->AppendRows(1);
				const JSize rowCount = data->GetRowCount();
				data->SetString(rowCount,kFirstColumn,   first);
				data->SetString(rowCount,kRestColumn,    rest);
				data->SetString(rowCount,kReplaceColumn, replace);
				if (firstNewRow == 0)
					{
					firstNewRow = rowCount;
					}
				state = 1;
				}
			}
		}

	if (firstNewRow != 0)
		{
		ScrollTo((GetBounds()).bottomLeft());
		BeginEditing(JPoint(kFirstColumn, firstNewRow));
		}
}

/******************************************************************************
 SaveRules (private)

 ******************************************************************************/

void
CBCRMRuleTable::SaveRules()
	const
{
	JString origName;
	if (GetWindow()->OKToUnfocusCurrentWidget() &&
		itsDialog->GetCurrentCRMRuleSetName(&origName))
		{
		JString newName;
		if (itsCSF->SaveFile("Save rules as:", NULL, origName, &newName))
			{
			WriteData(newName);
			}
		}
}

/******************************************************************************
 WriteData (private)

	We write the format:  (first\nrest\nreplace\n\n)*

 ******************************************************************************/

void
CBCRMRuleTable::WriteData
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
		(data->GetString(i, kFirstColumn)).Print(output);
		output << '\n';
		(data->GetString(i, kRestColumn)).Print(output);
		output << '\n';
		(data->GetString(i, kReplaceColumn)).Print(output);
		output << "\n\n";
		}
}

/******************************************************************************
 ReadGeometry

 ******************************************************************************/

void
CBCRMRuleTable::ReadGeometry
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentGeometryDataVersion)
		{
		JCoordinate w;
		input >> w;
		SetColWidth(kFirstColumn, w);
		input >> w;
		SetColWidth(kRestColumn, w);
		input >> w;
		SetColWidth(kReplaceColumn, w);
		}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WriteGeometry

 ******************************************************************************/

void
CBCRMRuleTable::WriteGeometry
	(
	ostream& output
	)
	const
{
	output << kCurrentGeometryDataVersion;
	output << ' ' << GetColWidth(kFirstColumn);
	output << ' ' << GetColWidth(kRestColumn);
	output << ' ' << GetColWidth(kReplaceColumn);
	output << kGeometryDataEndDelimiter;
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
CBCRMRuleTable::SetColTitles
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
