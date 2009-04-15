/******************************************************************************
 menu_hello.cpp
 
 Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "MenuHelloDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut2";

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
		new JXApplication(&argc, argv, kAppSignature, kTutorialStringData);
	assert( app != NULL );

	// Create the window director to maintain the Hello World window
	MenuHelloDir* mainDir = new MenuHelloDir(app);
	assert( mainDir != NULL );

	// Show the window and activate it
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
