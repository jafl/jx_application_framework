/******************************************************************************
 CBCommandSelection.cpp

	text/x-CBCommand with DELETE support for DND.

	BASE CLASS = JXSelectionData

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#include "CBCommandSelection.h"
#include "CBCommandTable.h"
#include <JXDisplay.h>
#include <JTableSelection.h>
#include <jAssert.h>

static const JCharacter* kCommandXAtomName = "text/x-CBCommand";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCommandSelection::CBCommandSelection
	(
	JXDisplay*							display,
	CBCommandTable*						table,
	const CBCommandManager::CmdInfo&	cmdInfo
	)
	:
	JXSelectionData(display),
	itsTable(table)
{
	assert( itsTable != nullptr );

	JPoint cell;
	const JBoolean hasSelection =
		(itsTable->GetTableSelection()).GetSingleSelectedCell(&cell);
	assert( hasSelection );
	itsSrcRowIndex = cell.y;

	std::ostringstream data;
	CBCommandManager::WriteCmdInfo(data, cmdInfo);
	data << std::ends;
	itsData = data.str();

	ClearWhenGoingAway(itsTable, &itsTable);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCommandSelection::~CBCommandSelection()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
CBCommandSelection::AddTypes
	(
	const Atom selectionName
	)
{
	itsCBCommandXAtom = AddType(kCommandXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
CBCommandSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	*bitsPerBlock = 8;

	JXSelectionManager* selMgr = GetSelectionManager();
	JPoint cell;

	if (requestType == itsCBCommandXAtom)
		{
		*returnType = itsCBCommandXAtom;
		*dataLength = itsData.GetLength();
		*data       = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
			{
			memcpy(*data, itsData.GetCString(), *dataLength);
			return kJTrue;
			}
		}

	else if (requestType == selMgr->GetDeleteSelectionXAtom() &&
			 itsTable != nullptr && IsCurrent() &&
			 (itsTable->GetTableSelection()).GetSingleSelectedCell(&cell) &&
			 JIndex(cell.y) == itsSrcRowIndex)
		{
		itsTable->RemoveCommand();

		*data       = jnew unsigned char[1];
		*dataLength = 0;
		*returnType = selMgr->GetNULLXAtom();
		return kJTrue;
		}

	*data       = nullptr;
	*dataLength = 0;
	*returnType = None;
	return kJFalse;
}

/******************************************************************************
 GetCommandXAtomName (static)

 ******************************************************************************/

const JCharacter*
CBCommandSelection::GetCommandXAtomName()
{
	return kCommandXAtomName;
}
