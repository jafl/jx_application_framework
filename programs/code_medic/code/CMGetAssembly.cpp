/******************************************************************************
 CMGetAssembly.cpp

	BASE CLASS = CMCommand

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMGetAssembly.h"
#include "CMSourceDirector.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetAssembly::CMGetAssembly
	(
	CMSourceDirector*	dir,
	const JCharacter*	cmd
	)
	:
	CMCommand(cmd, kJFalse, kJTrue),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetAssembly::~CMGetAssembly()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMGetAssembly::HandleFailure()
{
	JPtrArray<JString> addrList(JPtrArrayT::kDeleteAll);
	itsDir->DisplayDisassembly(&addrList, "");
}
