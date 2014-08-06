/******************************************************************************
 gfm.cc

	This provides a way to test the JX library and provides sample source
	code for everyone to study.

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <TreeTestDir3.h>
#include <JXApplication.h>
#include <JString.h>
#include <stdlib.h>
#include <jAssert.h>

// Prototypes


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
	JXApplication* app =
		new JXApplication(&argc, argv);
	assert( app != NULL );

//	TreeTestDir* dir = new TreeTestDir(app);
//	dir->Activate();
//
//	TreeTestDir2* dir2 = new TreeTestDir2(app);
//	dir2->Activate();
//
	TreeTestDir3* dir3 = new TreeTestDir3(app);
	dir3->Activate();

	app->Run();				// never actually returns
	return 0;
}
