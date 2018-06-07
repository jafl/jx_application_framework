/******************************************************************************
 edittable_main.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "EditTableDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut11";

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
	// Create the application
	JXApplication* app = 
		jnew JXApplication(&argc, argv, kAppSignature, kTutorialStringData);
	assert( app != nullptr );

	// Create our window director
	EditTableDir* mainDir = jnew EditTableDir(app);
	assert( mainDir != nullptr );

	// Activate the director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
