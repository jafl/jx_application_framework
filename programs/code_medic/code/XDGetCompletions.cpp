/******************************************************************************
 XDGetCompletions.cpp

	BASE CLASS = CMGetCompletions

	Copyright � 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "XDGetCompletions.h"
#include "CMCommandInput.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetCompletions::XDGetCompletions
	(
	CMCommandInput*	input,
	CMHistoryText*	history
	)
	:
	CMGetCompletions("status"),
	itsPrefix(input->GetText()),
	itsInput(input),
	itsHistory(history)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetCompletions::~XDGetCompletions()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetCompletions::HandleSuccess
	(
	const JString& data
	)
{
}
