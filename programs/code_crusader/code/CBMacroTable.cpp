/******************************************************************************
 CBMacroTable.cpp

	BASE CLASS = CBKeyScriptTableBase

	Copyright © 1998-2008 by John Lindal.

 ******************************************************************************/

#include "CBMacroTable.h"
#include "CBMacroManager.h"
#include "CBEditMacroDialog.h"
#include "CBListCSF.h"
#include <JXTextButton.h>
#include <JXChooseSaveFile.h>
#include <JStringTableData.h>
#include <jStreamUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBMacroTable::CBMacroTable
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

	itsCSF = jnew CBListCSF(JGetString("ReplaceMacroList::CBMacroTable"),
						   JGetString("AppendToMacroList::CBMacroTable"));
	assert( itsCSF != nullptr );

	SetColWidth(kMacroColumn, 60);
	// script column set automatically
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBMacroTable::~CBMacroTable()
{
	jdelete itsCSF;
}

/******************************************************************************
 GetData

 ******************************************************************************/

void
CBMacroTable::GetData
	(
	CBMacroManager* mgr
	)
	const
{
	assert( !IsEditing() );

	mgr->RemoveAllMacros();

	const JStringTableData* data = GetStringData();
	const JSize count            = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		mgr->AddMacro(data->GetString(i, kMacroColumn).GetBytes(),
					  data->GetString(i, kScriptColumn).GetBytes());
		}
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
CBMacroTable::SetData
	(
	const CBMacroManager& mgr
	)
{
	JStringTableData* data = GetStringData();
	data->RemoveAllRows();

	const CBMacroList& macroList = mgr.GetMacroList();
	const JSize count            = macroList.GetElementCount();
	data->AppendRows(count);
	for (JIndex i=1; i<=count; i++)
		{
		const CBMacroManager::MacroInfo info = macroList.GetElement(i);
		data->SetString(i,kMacroColumn,  *(info.macro));
		data->SetString(i,kScriptColumn, *(info.script));
		}

	Activate();
}

/******************************************************************************
 ClearData

 ******************************************************************************/

void
CBMacroTable::ClearData()
{
	GetStringData()->RemoveAllRows();
	Deactivate();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBMacroTable::Receive
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
CBMacroTable::LoadMacros()
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

	We read the format:  (\n*macro\n+script\n)*

 ******************************************************************************/

void
CBMacroTable::ReadData
	(
	const JString&	fileName,
	const bool	replace
	)
{
	JStringTableData* data = GetStringData();
	if (replace)
		{
		data->RemoveAllRows();
		}

	JIndex firstNewRow = 0;

	std::ifstream input(fileName.GetBytes());
	JString macro, script;
	JIndex state = 1;
	while (!input.eof() && !input.fail())
		{
		if (state == 1)
			{
			macro = JReadLine(input);
			if (!macro.IsEmpty())
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
				data->SetString(rowCount,kMacroColumn,  macro);
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
CBMacroTable::SaveMacros()
	const
{
	JString origName;
	if (GetDialog()->ContentsValid() &&
		GetDialog()->GetCurrentMacroSetName(&origName))
		{
		JString newName;
		if (itsCSF->SaveFile(JGetString("SaveMacroListPrompt::CBMacroTable"), JString::empty, origName, &newName))
			{
			WriteData(newName);
			}
		}
}

/******************************************************************************
 WriteData (private)

	We write the format:  (macro\nscript\n\n)*

 ******************************************************************************/

void
CBMacroTable::WriteData
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
CBMacroTable::Activate()
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
CBMacroTable::Deactivate()
{
	CBKeyScriptTableBase::Deactivate();
	if (!WouldBeActive())
		{
		itsLoadButton->Deactivate();
		itsSaveButton->Deactivate();
		}
}
