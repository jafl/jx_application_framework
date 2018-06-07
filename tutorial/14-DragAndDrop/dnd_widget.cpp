/******************************************************************************
 dnd_widget.cpp
 
 Written by Glenn Bach - 2000.

 ******************************************************************************/

#include "DNDWidgetDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut14";

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

	// Create the window director
	DNDWidgetDir* mainDir = jnew DNDWidgetDir(app);
	assert( mainDir != nullptr );

	// Activate the window director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
