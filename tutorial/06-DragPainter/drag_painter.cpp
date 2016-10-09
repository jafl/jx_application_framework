/******************************************************************************
 drag_painter.cpp
 
 Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "DragWidgetDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut6";

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
	assert( app != NULL );

	// Create the window director
	DragWidgetDir* mainDir = jnew DragWidgetDir(app);
	assert( mainDir != NULL );

	// Activate the window director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
