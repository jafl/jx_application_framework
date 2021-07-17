/******************************************************************************
 CBCommandSelection.cpp

	text/x-CBCommand with DELETE support for DND.

	BASE CLASS = JXSelectionData

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#include "CBCommandSelection.h"
#include "CBCommandTable.h"
#include <JXDisplay.h>
#include <JTableSelection.h>
#include <jAssert.h>

static const JUtf8Byte* kCommandXAtomName = "text/x-CBCommand";

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
	const bool hasSelection =
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

bool
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
		*dataLength = itsData.GetByteCount();
		*data       = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
			{
			memcpy(*data, itsData.GetRawBytes(), *dataLength);
			return true;
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
		return true;
		}

	*data       = nullptr;
	*dataLength = 0;
	*returnType = None;
	return false;
}

/******************************************************************************
 GetCommandXAtomName (static)

 ******************************************************************************/

const JUtf8Byte*
CBCommandSelection::GetCommandXAtomName()
{
	return kCommandXAtomName;
}
