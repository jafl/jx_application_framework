/******************************************************************************
 CMGetAssembly.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

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
