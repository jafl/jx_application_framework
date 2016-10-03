/******************************************************************************
 CMArray2DCommand.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMArray2DCommand.h"
#include "CMArray2DDir.h"
#include "CMVarNode.h"
#include <JXStringTable.h>
#include <JXColormap.h>
#include <JStringTableData.h>
#include <JStyleTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMArray2DCommand::CMArray2DCommand
	(
	CMArray2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	CMCommand("", kJFalse, kJTrue),
	itsDirector(dir),
	itsTable(table),
	itsData(data)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMArray2DCommand::~CMArray2DCommand()
{
}

/******************************************************************************
 PrepareToSend (virtual)

 ******************************************************************************/

void
CMArray2DCommand::PrepareToSend
	(
	const UpdateType	type,
	const JIndex		index,
	const JInteger		arrayIndex
	)
{
	itsType       = type;
	itsIndex      = index;
	itsArrayIndex = arrayIndex;
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMArray2DCommand::HandleFailure()
{
	HandleFailure(1, "");
	itsDirector->UpdateNext();
}

/******************************************************************************
 HandleFailure (protected)

 *****************************************************************************/

void
CMArray2DCommand::HandleFailure
	(
	const JIndex		startIndex,
	const JCharacter*	value
	)
{
	const JIndex max =
		(itsType == kRow ? itsData->GetColCount() : itsData->GetRowCount());
	if (max == 0 || !ItsIndexValid())
		{
		itsDirector->UpdateNext();
		return;
		}

	// ugly, but much faster
	JStyleTableData* styleData =
		const_cast<JStyleTableData*>(&(itsTable->GetStyleData()));
	const JFontStyle style =
		CMVarNode::GetFontStyle(kJFalse, kJFalse, itsTable->GetColormap());

	if (startIndex == 1)
		{
		JString s(value);

		JPtrArray<JString> data(JPtrArrayT::kForgetAll, max);
		for (JIndex i=1; i<=max; i++)
			{
			data.Append(&s);
			}

		if (itsType == kRow)
			{
			itsData->SetRow(itsIndex, data);
			styleData->SetRow(itsIndex, style);
			}
		else
			{
			itsData->SetCol(itsIndex, data);
			styleData->SetCol(itsIndex, style);
			}
		}
	else
		{
		for (JIndex i=startIndex; i<=max; i++)
			{
			itsData->SetString(GetCell(i), value);
			}

		if (!JStringEmpty(value))
			{
			if (itsType == kRow)
				{
				styleData->SetPartialRow(itsIndex, startIndex, max, style);
				}
			else
				{
				styleData->SetPartialCol(itsIndex, startIndex, max, style);
				}
			}
		}

	itsDirector->UpdateNext();
}

/******************************************************************************
 GetCell (protected)

 *****************************************************************************/

JPoint
CMArray2DCommand::GetCell
	(
	const JIndex i
	)
	const
{
	JPoint cell;
	if (itsType == kRow)
		{
		cell.Set(i, itsIndex);
		}
	else
		{
		cell.Set(itsIndex, i);
		}
	return cell;
}

/******************************************************************************
 ItsIndexValid (protected)

 *****************************************************************************/

JBoolean
CMArray2DCommand::ItsIndexValid()
	const
{
	if (itsType == kRow)
		{
		return JI2B(itsData->RowIndexValid(itsIndex) &&
					itsDirector->ArrayRowIndexValid(itsArrayIndex));
		}
	else
		{
		return JI2B(itsData->ColIndexValid(itsIndex) &&
					itsDirector->ArrayColIndexValid(itsArrayIndex));
		}
}
