/******************************************************************************
 LLDBArray2DCommand.cpp

	BASE CLASS = CMArray2DCommand

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "LLDBArray2DCommand.h"
#include "CMArray2DDir.h"
#include "CMVarNode.h"
#include "LLDBLink.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBValue.h"
#include "cmGlobals.h"
#include <JXStringTable.h>
#include <JXColormap.h>
#include <JStringTableData.h>
#include <JStyleTableData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBArray2DCommand::LLDBArray2DCommand
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

LLDBArray2DCommand::~LLDBArray2DCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBArray2DCommand::HandleSuccess
	(
	const JString& data
	)
{
	lldb::SBFrame f =
		dynamic_cast<LLDBLink*>(CMGetLink())->GetDebugger()->
			GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (!f.IsValid())
		{
		GetDirector()->UpdateNext();
		return;
		}

	const JIndex max =
		(GetType() == kRow ? GetData()->GetColCount() : GetData()->GetRowCount());
	if (max == 0 || !ItsIndexValid())
		{
		GetDirector()->UpdateNext();
		return;
		}

	JString expr;
	for (JIndex i=1; i<=max; i++)
		{
		const JPoint cell = GetCell(i);
		expr              = GetDirector()->GetExpression(cell);
		lldb::SBValue v   = f.EvaluateExpression(expr);
		if (!v.IsValid())
			{
			HandleFailure(i, v.GetError().GetCString());
			continue;
			}

		const JCharacter* value = v.GetValue();
		if (value == NULL)
			{
			HandleFailure(i, v.GetError().GetCString());
			continue;
			}

		const JString& origValue = GetData()->GetString(cell);
		const JBoolean isNew     = JI2B(!origValue.IsEmpty() && origValue != value);

		GetData()->SetString(cell, value);
		GetTable()->SetCellStyle(cell,
			CMVarNode::GetFontStyle(kJTrue, isNew, GetTable()->GetColormap()));
		}

	GetDirector()->UpdateNext();
}
