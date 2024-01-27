/******************************************************************************
 menu_hello.cpp
 
 Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "MenuHelloDir.h"
#include <jx-af/jx/JXApplication.h>
#include "../TutorialStringData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "tut2";

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

	// Create the window director to maintain the Hello World window
	MenuHelloDir* mainDir = jnew MenuHelloDir(app);

	// Show the window and activate it
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
