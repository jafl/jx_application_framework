/******************************************************************************
 SymcirApp.cpp

	BASE CLASS = JXApplication

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "SymcirApp.h"
#include "SCAboutDialog.h"
#include "scGlobals.h"
#include "scStringData.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SymcirApp::SymcirApp
	(
	int*	argc,
	char*	argv[]
	)
	:
	JXApplication(argc, argv, "symcir", kSCDefaultStringData)
{
	SCCreateGlobals(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SymcirApp::~SymcirApp()
{
	SCDeleteGlobals();
}

/******************************************************************************
 DisplayAbout

 ******************************************************************************/

void
SymcirApp::DisplayAbout
	(
	JXDisplay* display
	)
{
	SCAboutDialog* dlog = new SCAboutDialog(this);
	assert( dlog != NULL );
	dlog->BeginDialog();
}
