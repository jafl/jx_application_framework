/******************************************************************************
 LLDBGetMemory.cpp

	BASE CLASS = CMGetMemory

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "LLDBGetMemory.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "CMMemoryDir.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetMemory::LLDBGetMemory
	(
	CMMemoryDir* dir
	)
	:
	CMGetMemory(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetMemory::~LLDBGetMemory()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JCharacter* kCommandName[] =
{
	"xb", "xh", "xw", "xg", "cb", "i"
};

void
LLDBGetMemory::HandleSuccess
	(
	const JString& data
	)
{
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == NULL)
		{
		return;
		}

	lldb::SBCommandInterpreter interp = link->GetDebugger()->GetCommandInterpreter();
	if (!interp.IsValid())
		{
		return;
		}

	CMMemoryDir::DisplayType type;
	JSize count;
	const JString& expr = GetDirector()->GetExpression(&type, &count);

	JString cmd = "memory read -g ";
	cmd        += JString(count, JString::kBase10);
	cmd        += kCommandName[ type-1 ];
	cmd        += " ";
	cmd        += expr;

	lldb::SBCommandReturnObject result;
	interp.HandleCommand(cmd, result);

	if (result.IsValid() && result.Succeeded() && result.HasResult())
		{
		GetDirector()->Update(result.GetOutput());
		}
}
