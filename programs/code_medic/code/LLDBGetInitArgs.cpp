/******************************************************************************
 LLDBGetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .lldbinit

	BASE CLASS = CMGetInitArgs

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetInitArgs.h"
#include "cmGlobals.h"
#include <JXInputField.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetInitArgs::LLDBGetInitArgs
	(
	JXInputField* argInput
	)
	:
	CMGetInitArgs("NOP"),
	itsArgInput(argInput)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetInitArgs::~LLDBGetInitArgs()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetInitArgs::HandleSuccess
	(
	const JString& data
	)
{
}
