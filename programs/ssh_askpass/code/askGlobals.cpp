/******************************************************************************
 askGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 2006 by John Lindal.

 ******************************************************************************/

#include "askGlobals.h"
#include "ASKApp.h"
#include <jAssert.h>

static ASKApp*	theApplication  = nullptr;		// owns itself

/******************************************************************************
 ASKCreateGlobals

	Returns true if this is the first time the program is run.

 ******************************************************************************/

void
ASKCreateGlobals
	(
	ASKApp* app
	)
{
	theApplication = app;
}

/******************************************************************************
 ASKDeleteGlobals

 ******************************************************************************/

void
ASKDeleteGlobals()
{
	theApplication = nullptr;
}

/******************************************************************************
 ASKGetApplication

 ******************************************************************************/

ASKApp*
ASKGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 ASKGetVersionNumberStr

 ******************************************************************************/

const JString&
ASKGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 ASKGetVersionStr

 ******************************************************************************/

JString
ASKGetVersionStr()
{
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("Description::askGlobals", map, sizeof(map));
}
