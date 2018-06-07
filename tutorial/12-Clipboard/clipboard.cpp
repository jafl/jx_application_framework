/******************************************************************************
 clipboard.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "ClipboardDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut12";

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

	// Create the window director to maintain the Clipboard window
	ClipboardDir* mainDir = jnew ClipboardDir(app);
	assert( mainDir != nullptr );

	// Show the window and activate it
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
