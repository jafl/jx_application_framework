/******************************************************************************
 JVMDisplaySourceForMain.cpp

	BASE CLASS = CMDisplaySourceForMain

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

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
