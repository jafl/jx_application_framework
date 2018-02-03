/******************************************************************************
 GDBGetRegisters.cpp

	BASE CLASS = CMGetRegisters

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

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
	GetDirector()->Update(data);
}
