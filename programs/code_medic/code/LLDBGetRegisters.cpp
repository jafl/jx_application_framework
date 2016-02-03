/******************************************************************************
 LLDBGetRegisters.cpp

	BASE CLASS = CMGetRegisters

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetRegisters.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "CMRegistersDir.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetRegisters::LLDBGetRegisters
	(
	CMRegistersDir* dir
	)
	:
	CMGetRegisters("", dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetRegisters::~LLDBGetRegisters()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetRegisters::HandleSuccess
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

	lldb::SBCommandReturnObject result;
	interp.HandleCommand("register read", result);

	// https://llvm.org/bugs/show_bug.cgi?id=26421
	if (result.IsValid() && result.Succeeded() /* && result.HasResult() */)
		{
		(GetDirector())->Update(result.GetOutput());
		}
}
