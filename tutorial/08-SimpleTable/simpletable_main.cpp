/******************************************************************************
 simpletable_main.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "SimpleTableDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut8";

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
	assert( app != nullptr );

	// Create our window director
	SimpleTableDir* mainDir = jnew SimpleTableDir(app);
	assert( mainDir != nullptr );

	// Activate the director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
