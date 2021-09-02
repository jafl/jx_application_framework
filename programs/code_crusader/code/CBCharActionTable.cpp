/******************************************************************************
 CBCharActionTable.cpp

	BASE CLASS = CBKeyScriptTableBase

	Copyright © 1998-2008 by John Lindal.

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

	itsCSF = jnew CBListCSF(JGetString("ReplaceCharActionList::CBCharActionTable"),
							JGetString("AppendToCharActionList::CBCharActionTable"));
	assert( itsCSF != nullptr );

	SetColWidth(kMacroColumn, 40);
	// script column set automatically
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCharActionTable::~CBCharActionTable()
{
	jdelete itsCSF;
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
		mgr->SetAction(data->GetString(i, kMacroColumn).GetFirstCharacter(),
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

	const JStringPtrMap<JString>& actionMap = mgr.GetActionMap();
	JStringMapCursor cursor(&actionMap);
	while (cursor.Next())
		{
		data->AppendRows(1);
		data->SetString(GetRowCount(), kMacroColumn,  cursor.GetKey());
		data->SetString(GetRowCount(), kScriptColumn, *cursor.GetValue());
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
		jdelete input;
		input = jnew JXCharInput(enclosure, hSizing, vSizing, x,y, w,h);
		assert( input != nullptr );
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
		itsCSF->ChooseFile(JString::empty, JString::empty, &fileName))
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
	const JString&	fileName,
	const bool		replace
	)
{
	JStringTableData* data = GetStringData();
	if (replace)
		{
		data->RemoveAllRows();
		}

	JIndex firstNewRow = 0;

	std::ifstream input(fileName.GetBytes());
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
				data->SetString(rowCount,kMacroColumn,  JString(action.GetFirstCharacter()));
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
		if (itsCSF->SaveFile(JGetString("SaveCharActionListPrompt::CBCharActionTable"), JString::empty, origName, &newName))
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
	const JString& fileName
	)
	const
{
	std::ofstream output(fileName.GetBytes());

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		data->GetString(i, kMacroColumn).Print(output);
		output << '\n';
		data->GetString(i, kScriptColumn).Print(output);
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
