/******************************************************************************
 clipboard.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "ClipboardDir.h"
#include <jx-af/jx/JXApplication.h>
#include "../TutorialStringData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "tut12";

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

	// Create the window director to maintain the Clipboard window
	ClipboardDir* mainDir = jnew ClipboardDir(app);

	// Show the window and activate it
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
