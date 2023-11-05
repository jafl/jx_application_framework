/******************************************************************************
 drag_painter.cpp
 
 Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "DragWidgetDir.h"
#include <jx-af/jx/JXApplication.h>
#include "../TutorialStringData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "tut6";

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

	// Create the window director
	DragWidgetDir* mainDir = jnew DragWidgetDir(app);

	// Activate the window director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
