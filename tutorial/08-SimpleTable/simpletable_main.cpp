/******************************************************************************
 simpletable_main.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "SimpleTableDir.h"
#include <jx-af/jx/JXApplication.h>
#include "../TutorialStringData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "tut8";

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
		jnew JXApplication(&argc, argv, kAppSignature, kTutorialStringData);

	// Create our window director
	SimpleTableDir* mainDir = jnew SimpleTableDir(app);

	// Activate the director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
