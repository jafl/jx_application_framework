/******************************************************************************
 askGlobals.cc

	Access to global objects and factories.

	Copyright © 2006 by New Planet Software, Inc.. All rights reserved.

 ******************************************************************************/

#include <askStdInc.h>
#include "askGlobals.h"
#include "ASKApp.h"
#include <jAssert.h>

static ASKApp*	theApplication  = NULL;		// owns itself

// string ID's

static const JCharacter* kDescriptionID = "Description::askGlobals";

/******************************************************************************
 ASKCreateGlobals

	Returns kJTrue if this is the first time the program is run.

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
	theApplication = NULL;
}

/******************************************************************************
 ASKGetApplication

 ******************************************************************************/

ASKApp*
ASKGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 ASKGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
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
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}
