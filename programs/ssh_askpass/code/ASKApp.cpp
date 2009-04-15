/******************************************************************************
 ASKApp.cc

	<Description>

	BASE CLASS = public JXApplication

	Copyright © 2006 by New Planet Software, Inc.. All rights reserved.

 *****************************************************************************/

#include <askStdInc.h>
#include "ASKApp.h"
#include "askStringData.h"
#include "askGlobals.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "nps-ssh-askpass";

/******************************************************************************
 Constructor

 *****************************************************************************/

ASKApp::ASKApp
	(
	int*	argc,
	char*	argv[]
	)
	:
	JXApplication(argc, argv, kAppSignature, kASKDefaultStringData)
{
	ASKCreateGlobals(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

ASKApp::~ASKApp()
{
	ASKDeleteGlobals();
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
ASKApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
ASKApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kASKDefaultStringData);
}
