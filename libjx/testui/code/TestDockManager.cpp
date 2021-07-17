/******************************************************************************
 TestDockManager.cpp

	BASE CLASS = JXDockManager

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#include "TestDockManager.h"
#include <JXDockDirector.h>
#include <JXHorizDockPartition.h>
#include <JXVertDockPartition.h>
#include <JXDockWidget.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestDockManager::TestDockManager
	(
	JXDisplay* display
	)
	:
	JXDockManager(display, JGetString("DockTitle::TestDockManager"))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestDockManager::~TestDockManager()
{
}

/******************************************************************************
 CanDockAll (virtual)

	Derived class should override this to return true if it implements
	DockAll().

 ******************************************************************************/

bool
TestDockManager::CanDockAll()
	const
{
	return GetDockList()->IsEmpty();
}

/******************************************************************************
 DockAll (virtual)

	Derived class should override this to dock all the windows and set
	docking positions for all window types.

 ******************************************************************************/

void
TestDockManager::DockAll()
{
	if (!GetDockList()->IsEmpty())
		{
		return;
		}

	JXDockDirector* dir = CreateDock(true);

	JXHorizDockPartition* h;
	bool ok = dir->GetHorizPartition(&h);
	assert( ok );

	JXVertDockPartition* v1;
	ok = h->SplitVert(1, &v1);
	assert( ok );

	JXDockWidget* dock = v1->GetDockWidget(1);
	assert( dock != nullptr );
	SetDefaultDock("TestDirector", dock);

	dock = v1->GetDockWidget(2);
	assert( dock != nullptr );
	SetDefaultDock("TestTextEditDocument", dock);

	JXVertDockPartition* v2;
	ok = h->SplitVert(2, &v2);
	assert( ok );

	dock = v2->GetDockWidget(1);
	assert( dock != nullptr );
	SetDefaultDock("TestFloatTableDirector", dock);

	dock = v2->GetDockWidget(2);
	assert( dock != nullptr );
	SetDefaultDock("TestImageDirector", dock);

	h->SetElasticIndex(1);
	v1->SetElasticIndex(2);
	v2->SetElasticIndex(1);
}
