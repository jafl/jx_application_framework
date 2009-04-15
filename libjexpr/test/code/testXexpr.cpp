/******************************************************************************
 testXexpr.cpp

	This provides a way to test the parsing system and JXExprEditor.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestExprDirector.h"
#include "testjexprStringData.h"
#include <JXApplication.h>
#include <jAssert.h>

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
	JXApplication* app = new JXApplication(&argc, argv, "testjexpr", kDefaultStringData);
	assert( app != NULL );

	TestExprDirector* mainDir = new TestExprDirector(app);
	assert( mainDir != NULL );

	mainDir->Activate();
	app->Run();
	return 0;
}
