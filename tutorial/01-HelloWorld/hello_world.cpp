/******************************************************************************
 hello_world.cpp
 
 Written by Glenn Bach - 1997. 

 ******************************************************************************/

#include "HelloWorldDir.h"
#include <jx-af/jx/JXApplication.h>
#include "../TutorialStringData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "tut1";

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
		jnew JXApplication(&argc, argv, kAppSignature, kAppSignature, kTutorialStringData);

	// Create the window director to maintain the Hello World window
	HelloWorldDir* mainDir = jnew HelloWorldDir(app);

	// Show the window and activate it
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
