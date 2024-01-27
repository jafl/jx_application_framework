/******************************************************************************
 selectiontable_main.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "SelectionTableDir.h"
#include <jx-af/jx/JXApplication.h>
#include "../TutorialStringData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "tut10";

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	// Create the application object - one per program
	JXApplication* app = 
		jnew JXApplication(&argc, argv, kAppSignature, kAppSignature, kTutorialStringData);

	// Create our window director
	SelectionTableDir* mainDir = jnew SelectionTableDir(app);

	// Activate the director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
