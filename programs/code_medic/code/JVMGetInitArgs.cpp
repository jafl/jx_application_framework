/******************************************************************************
 JVMGetInitArgs.cpp

	BASE CLASS = CMGetInitArgs

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetInitArgs.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetInitArgs::JVMGetInitArgs
	(
	JXInputField* argInput
	)
	:
	CMGetInitArgs(JString("NOP", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetInitArgs::~JVMGetInitArgs()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetInitArgs::HandleSuccess
	(
	const JString& data
	)
{
}
