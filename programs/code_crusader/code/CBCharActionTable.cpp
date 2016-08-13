/******************************************************************************
 CBCharActionTable.cpp

	BASE CLASS = CBKeyScriptTableBase

	Copyright © 1998-2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCharActionTable.h"
#include "CBCharActionManager.h"
#include "CBEditMacroDialog.h"
#include "CBListCSF.h"
#include <JXCharInput.h>
#include <JXTextButton.h>
#include <JXChooseSaveFile.h>
#include <JStringTableData.h>
#include <jStreamUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kReplaceListID  = "ReplaceCharActionList::CBCharActionTable";
static const JCharacter* kAppendToListID = "AppendToCharActionList::CBCharActionTable";
static const JCharacter* kSavePromptID   = "SaveCharActionListPrompt::CBCharActionTable";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCharActionTable::CBCharActionTable
	(
	CBEditMacroDialog*	dialog,
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
	CBKeyScriptTableBase(dialog, addRowButton, removeRowButton,
						 scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsLoadButton = loadButton;
	ListenTo(itsLoadButton);

	itsSaveButton = saveButton;
	ListenTo(itsSaveButton);

	itsCSF = new CBListCSF(JGetString(kReplaceListID), JGetString(kAppendToListID));
	assert( itsCSF != NULL );

	SetColWidth(kMacroColumn, 40);
	// script column set automatically
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCharActionTable::~CBCharActionTable()
{
	delete itsCSF;
}

/******************************************************************************
 GetData

 ******************************************************************************/

void
CBCharActionTable::GetData
	(
	CBCharActionManager* mgr
	)
	const
{
	assert( !IsEditing() );

	mgr->ClearAllActions();

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		mgr->SetAction((data->GetString(i, kMacroColumn)).GetCharacter(1),
					   data->GetString(i, kScriptColumn));
		}
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
CBCharActionTable::SetData
	(
	const CBCharActionManager& mgr
	)
{
	JStringTableData* data = GetStringData();
	data->RemoveAllRows();

	JCharacter key[] = { '\0', '\0' };

	const JPtrArray<JString>& actionList = mgr.GetActionList();
	const JSize count                    = actionList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* script = actionList.NthElement(i);
		if (script != NULL)
			{
			data->AppendRows(1);

			key[0] = (JCharacter) i-1;
			data->SetString(GetRowCount(), kMacroColumn,  key);
			data->SetString(GetRowCount(), kScriptColumn, *script);
			}
		}

	Activate();
}

/******************************************************************************
 ClearData

 ******************************************************************************/

void
CBCharActionTable::ClearData()
{
	GetStringData()->RemoveAllRows();
	Deactivate();
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

 ******************************************************************************/

JXInputField*
CBCharActionTable::CreateStringTableInput
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
	JXInputField* input =
		CBKeyScriptTableBase::CreateStringTableInput(
									cell, enclosure, hSizing, vSizing, x,y, w,h);

	if (cell.x == kMacroColumn)
		{
		delete input;
		input = new JXCharInput(enclosure, hSizing, vSizing, x,y, w,h);
		assert( input != NULL );
		}

	return input;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBCharActionTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLoadButton && message.Is(JXButton::kPushed))
		{
		LoadMacros();
		}
	else if (sender == itsSaveButton && message.Is(JXButton::kPushed))
		{
		SaveMacros();
		}

	else
		{
		CBKeyScriptTableBase::Receive(sender, message);
		}
}

/******************************************************************************
 LoadMacros (private)

 ******************************************************************************/

void
CBCharActionTable::LoadMacros()
{
	JString fileName;
	if (GetDialog()->ContentsValid() &&
		itsCSF->ChooseFile("", NULL, &fileName))
		{
		ReadData(fileName, itsCSF->ReplaceExisting());
		}
}

/******************************************************************************
 ReadData (private)

	We read the format:  (\n*action\n+script\n)*

 ******************************************************************************/

void
CBCharActionTable::ReadData
	(
	const JCharacter*	fileName,
	const JBoolean		replace
	)
{
	JStringTableData* data = GetStringData();
	if (replace)
		{
		data->RemoveAllRows();
		}

	JIndex firstNewRow = 0;

	ifstream input(fileName);
	JString action, script;
	JIndex state = 1;
	while (!input.eof() && !input.fail())
		{
		if (state == 1)
			{
			action = JReadLine(input);
			if (!action.IsEmpty())
				{
				state = 2;
				}
			}
		else if (state == 2)
			{
			script = JReadLine(input);
			if (!script.IsEmpty())
				{
				data->AppendRows(1);
				const JSize rowCount = data->GetRowCount();
				data->SetString(rowCount,kMacroColumn,  action.GetSubstring(1,1));
				data->SetString(rowCount,kScriptColumn, script);
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
		BeginEditing(JPoint(kMacroColumn, firstNewRow));
		}
}

/******************************************************************************
 SaveMacros (private)

 ******************************************************************************/

void
CBCharActionTable::SaveMacros()
	const
{
	JString origName;
	if (GetDialog()->ContentsValid() &&
		GetDialog()->GetCurrentMacroSetName(&origName))
		{
		JString newName;
		if (itsCSF->SaveFile(JGetString(kSavePromptID), NULL, origName, &newName))
			{
			WriteData(newName);
			}
		}
}

/******************************************************************************
 WriteData (private)

	We write the format:  (action\nscript\n\n)*

 ******************************************************************************/

void
CBCharActionTable::WriteData
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
		(data->GetString(i, kMacroColumn)).Print(output);
		output << '\n';
		(data->GetString(i, kScriptColumn)).Print(output);
		output << "\n\n";
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBCharActionTable::Activate()
{
	CBKeyScriptTableBase::Activate();
	if (WouldBeActive())
		{
		itsLoadButton->Activate();
		itsSaveButton->Activate();
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
CBCharActionTable::Deactivate()
{
	CBKeyScriptTableBase::Deactivate();
	if (!WouldBeActive())
		{
		itsLoadButton->Deactivate();
		itsSaveButton->Deactivate();
		}
}
