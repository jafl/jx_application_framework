/******************************************************************************
 datatable_main.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include <JXStdInc.h>
#include "DataTableDir.h"
#include <JXApplication.h>
#include "../TutorialStringData.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "tut9";

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

	// Create our window director
	DataTableDir* mainDir = new DataTableDir(app);
	assert( mainDir != NULL );

	// Activate the director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
