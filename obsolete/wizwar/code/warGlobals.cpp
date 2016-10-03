/******************************************************************************
 warGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "warGlobals.h"
#include "War2Wiz.h"
#include "wwUtil.h"
#include "wwStringData.h"
#include <JThisProcess.h>
#include <jGlobals.h>
#include <jAssert.h>

static WarSetup*	theSetupManager      = NULL;
static War2Wiz*		theConnectionManager = NULL;

/******************************************************************************
 WarCreateGlobals

 ******************************************************************************/

void
WarCreateGlobals
	(
	const JSize			argc,
	char*				argv[],
	const JCharacter*	versionStr
	)
{
	JInitCore(NULL, WWGetAppSignature(), kWWDefaultStringData);
	JThisProcess::ShouldCatchSignal(SIGTERM, kJFalse);	// allow "kill"

	theSetupManager = new WarSetup(argc, argv, versionStr);
	assert( theSetupManager != NULL );

	theConnectionManager = new War2Wiz;
	assert( theConnectionManager != NULL );
}

/******************************************************************************
 WarDeleteGlobals

 ******************************************************************************/

void
WarDeleteGlobals()
{
	delete theConnectionManager;
	theConnectionManager = NULL;

	delete theSetupManager;
	theSetupManager = NULL;
}

/******************************************************************************
 WarGetSetup

 ******************************************************************************/

WarSetup*
WarGetSetup()
{
	assert( theSetupManager != NULL );
	return theSetupManager;
}

/******************************************************************************
 WarGetConnectionManager

 ******************************************************************************/

War2Wiz*
WarGetConnectionManager()
{
	assert( theConnectionManager != NULL );
	return theConnectionManager;
}
