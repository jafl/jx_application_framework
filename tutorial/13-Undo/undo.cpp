/******************************************************************************
 undo.cpp
 
 Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "UndoWidgetDir.h"
#include <jx-af/jx/JXApplication.h>
#include "../TutorialStringData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "tut13";

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

	// Create the window director
	UndoWidgetDir* mainDir = jnew UndoWidgetDir(app);

	// Activate the window director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
