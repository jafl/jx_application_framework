/******************************************************************************
 JVMDisplaySourceForMain.cpp

	BASE CLASS = CMDisplaySourceForMain

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "JVMDisplaySourceForMain.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMDisplaySourceForMain::JVMDisplaySourceForMain
	(
	CMSourceDirector* sourceDir
	)
	:
	CMDisplaySourceForMain(sourceDir, "NOP")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMDisplaySourceForMain::~JVMDisplaySourceForMain()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMDisplaySourceForMain::HandleSuccess
	(
	const JString& data
	)
{
}
