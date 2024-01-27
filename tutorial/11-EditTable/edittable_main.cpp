/******************************************************************************
 edittable_main.cpp
 
 Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "EditTableDir.h"
#include <jx-af/jx/JXApplication.h>
#include "../TutorialStringData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "tut11";

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
	// Create the application
	JXApplication* app = 
		jnew JXApplication(&argc, argv, kAppSignature, kAppSignature, kTutorialStringData);

	// Create our window director
	EditTableDir* mainDir = jnew EditTableDir(app);

	// Activate the director
	mainDir->Activate();
	
	// Start the event loop
	app->Run();				
	return 0;
}
