/******************************************************************************
 LLDBGetRegisters.cpp

	BASE CLASS = CMGetRegisters

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetRegisters.h"
#include "CMRegistersDir.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetRegisters::LLDBGetRegisters
	(
	CMRegistersDir* dir
	)
	:
	CMGetRegisters("info all-registers", dir)
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
	(GetDirector())->Update(data);
}
