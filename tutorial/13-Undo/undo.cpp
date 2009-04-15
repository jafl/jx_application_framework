/******************************************************************************
 undo.cpp
 
 Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "UndoWidgetDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut13";

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

	// Create the window director
	UndoWidgetDir* mainDir = new UndoWidgetDir(app);
	assert( mainDir != NULL );

	// Activate the window director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
