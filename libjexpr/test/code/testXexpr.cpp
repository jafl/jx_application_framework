/******************************************************************************
 testXexpr.cpp

	This provides a way to test the parsing system and JXExprEditor.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

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
	JXApplication* app = jnew JXApplication(&argc, argv, "testjexpr", kDefaultStringData);
	assert( app != nullptr );

	TestExprDirector* mainDir = jnew TestExprDirector(app);
	assert( mainDir != nullptr );

	mainDir->Activate();
	app->Run();
	return 0;
}
