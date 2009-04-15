/******************************************************************************
 hello_world.cpp
 
 Written by Glenn Bach - 1997. 

 ******************************************************************************/

#include <JXStdInc.h>
#include "HelloWorldDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut1";

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
	// Create the application object - there should only be one of these
	JXApplication* app = 
		new JXApplication(&argc, argv, kAppSignature, kTutorialStringData);
		
	// Make sure that new succeeded
	assert( app != NULL );

	// Create the window director to maintain the Hello World window
	HelloWorldDir* mainDir = new HelloWorldDir(app);
	assert( mainDir != NULL );

	// Show the window and activate it
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
