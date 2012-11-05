/******************************************************************************
 GDBGetRegisters.cpp

	BASE CLASS = CMGetRegisters

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "GDBGetRegisters.h"
#include "CMRegistersDir.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetRegisters::GDBGetRegisters
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

GDBGetRegisters::~GDBGetRegisters()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBGetRegisters::HandleSuccess
	(
	const JString& data
	)
{
	(GetDirector())->Update(data);
}
