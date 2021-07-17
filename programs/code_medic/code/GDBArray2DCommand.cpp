/******************************************************************************
 GDBArray2DCommand.cpp

	BASE CLASS = CMArray2DCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBArray2DCommand.h"
#include "CMArray2DDir.h"
#include "CMVarNode.h"
#include <JXStringTable.h>
#include <JStringTableData.h>
#include <JStyleTableData.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBArray2DCommand::GDBArray2DCommand
	(
	CMArray2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	CMArray2DCommand(dir, table, data)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBArray2DCommand::~GDBArray2DCommand()
{
}

/******************************************************************************
 PrepareToSend (virtual)

 ******************************************************************************/

void
GDBArray2DCommand::PrepareToSend
	(
	const UpdateType	type,
	const JIndex		index,
	const JInteger		arrayIndex
	)
{
	CMArray2DCommand::PrepareToSend(type, index, arrayIndex);

	JString cmd("set print pretty off\nset print array off\n"
				"set print repeats 0\nset width 0\n");

	const JIndex max =
		(GetType() == kRow ? GetData()->GetColCount() : GetData()->GetRowCount());
	assert( max > 0 );

	for (JIndex i=1; i<=max; i++)
		{
		cmd += "print ";
		cmd += GetDirector()->GetExpression(GetCell(i));
		cmd += "\n";
		}

	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex prefixPattern = "^\\$[[:digit:]]+[[:space:]]+=[[:space:]]+(.*)$";

void
GDBArray2DCommand::HandleSuccess
	(
	const JString& data
	)
{
	const JIndex max =
		(GetType() == kRow ? GetData()->GetColCount() : GetData()->GetRowCount());
	if (max == 0 || !ItsIndexValid())
		{
		GetDirector()->UpdateNext();
		return;
		}

	JIndex i = 1;

	JStringIterator iter(data);
	JString value;
	while (i <= max && iter.Next(prefixPattern))
		{
		value = iter.GetLastMatch().GetSubstring(1);
		value.TrimWhitespace();

		const JPoint cell        = GetCell(i);
		const JString& origValue = GetData()->GetString(cell);
		const bool isNew     = !origValue.IsEmpty() && origValue != value;

		GetData()->SetString(cell, value);
		GetTable()->SetCellStyle(cell, CMVarNode::GetFontStyle(true, isNew));

		i++;
		}
	iter.Invalidate();

	if (i == 1)
		{
		JString s = data;
		s.TrimWhitespace();

		JStringIterator iter2(&s);
		if (iter2.Next("\n"))
			{
			iter2.SkipPrev();
			iter2.RemoveAllNext();
			s.TrimWhitespace();
			}
		iter2.Invalidate();
		HandleFailure(1, s);
		}
	else if (i <= max)
		{
		HandleFailure(i, JString::empty);
		}

	GetDirector()->UpdateNext();
}
