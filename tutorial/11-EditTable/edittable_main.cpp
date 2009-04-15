/******************************************************************************
 edittable_main.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include <JXStdInc.h>
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
		new JXApplication(&argc, argv, kAppSignature, kTutorialStringData);
	assert( app != NULL );

	// Create our window director
	EditTableDir* mainDir = new EditTableDir(app);
	assert( mainDir != NULL );

	// Activate the director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
