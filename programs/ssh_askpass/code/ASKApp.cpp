/******************************************************************************
 ASKApp.cpp

	BASE CLASS = public JXApplication

	Copyright (C) 2006 by John Lindal.

 *****************************************************************************/

#include "ASKApp.h"
#include "askStringData.h"
#include "askGlobals.h"
#include <jAssert.h>

static const JUtf8Byte* kAppSignature = "nps-ssh-askpass";

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
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
ASKApp::InitStrings()
{
	JGetStringManager()->Register(kAppSignature, kASKDefaultStringData);
}
