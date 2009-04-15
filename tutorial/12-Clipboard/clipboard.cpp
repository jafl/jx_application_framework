/******************************************************************************
 clipboard.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include <JXStdInc.h>
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
		new JXApplication(&argc, argv, kAppSignature, kTutorialStringData);
	assert( app != NULL );

	// Create the window director to maintain the Clipboard window
	ClipboardDir* mainDir = new ClipboardDir(app);
	assert( mainDir != NULL );

	// Show the window and activate it
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
