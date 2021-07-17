/******************************************************************************
 XDDisplaySourceForMain.cpp

	BASE CLASS = CMDisplaySourceForMain

	Copyright (C) 2001 by John Lindal.

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
	CMDisplaySourceForMain(sourceDir, JString("status", JString::kNoCopy))
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
