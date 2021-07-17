/******************************************************************************
 LLDBArray2DCommand.cpp

	BASE CLASS = CMArray2DCommand

	Copyright (C) 2016 by John Lindal.

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
#include <JXColorManager.h>
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
		lldb::SBValue v   = f.EvaluateExpression(expr.GetBytes());
		if (!v.IsValid())
			{
			HandleFailure(i, JString(v.GetError().GetCString(), JString::kNoCopy));
			continue;
			}

		const JUtf8Byte* value = v.GetValue();
		if (value == nullptr)
			{
			HandleFailure(i, JString(v.GetError().GetCString(), JString::kNoCopy));
			continue;
			}

		const JString& origValue = GetData()->GetString(cell);
		const bool isNew     = !origValue.IsEmpty() && origValue != value;

		GetData()->SetString(cell, JString(value, JString::kNoCopy));
		GetTable()->SetCellStyle(cell,
			CMVarNode::GetFontStyle(true, isNew));
		}

	GetDirector()->UpdateNext();
}
