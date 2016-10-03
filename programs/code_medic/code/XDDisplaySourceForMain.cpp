/******************************************************************************
 XDDisplaySourceForMain.cpp

	BASE CLASS = CMDisplaySourceForMain

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XDDisplaySourceForMain.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDDisplaySourceForMain::XDDisplaySourceForMain
	(
	CMSourceDirector* sourceDir
	)
	:
	CMDisplaySourceForMain(sourceDir, "status")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDDisplaySourceForMain::~XDDisplaySourceForMain()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDDisplaySourceForMain::HandleSuccess
	(
	const JString& data
	)
{
}
