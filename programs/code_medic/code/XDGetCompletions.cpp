/******************************************************************************
 XDGetCompletions.cpp

	BASE CLASS = CMGetCompletions

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetCompletions.h"
#include "CMCommandInput.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetCompletions::XDGetCompletions
	(
	CMCommandInput*			input,
	CMCommandOutputDisplay*	history
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
