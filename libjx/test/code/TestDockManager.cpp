/******************************************************************************
 TestDockManager.cpp

	BASE CLASS = JXDockManager

	Copyright © 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestDockManager.h"
#include <JXDockDirector.h>
#include <JXHorizDockPartition.h>
#include <JXVertDockPartition.h>
#include <JXDockWidget.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestDockManager::TestDockManager
	(
	JXDisplay* display
	)
	:
	JXDockManager(display, "testjx")
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

	Derived class should override this to return kJTrue if it implements
	DockAll().

 ******************************************************************************/

JBoolean
TestDockManager::CanDockAll()
	const
{
	return (GetDockList())->IsEmpty();
}

/******************************************************************************
 DockAll (virtual)

	Derived class should override this to dock all the windows and set
	docking positions for all window types.

 ******************************************************************************/

void
TestDockManager::DockAll()
{
	if (!(GetDockList())->IsEmpty())
		{
		return;
		}

	JXDockDirector* dir = CreateDock(kJTrue);

	JXHorizDockPartition* h;
	JBoolean ok = dir->GetHorizPartition(&h);
	assert( ok );

	JXVertDockPartition* v1;
	ok = h->SplitVert(1, &v1);
	assert( ok );

	JXDockWidget* dock = dynamic_cast(JXDockWidget*, v1->GetCompartment(1));
	assert( dock != NULL );
	SetDefaultDock("TestDirector", dock);

	dock = dynamic_cast(JXDockWidget*, v1->GetCompartment(2));
	assert( dock != NULL );
	SetDefaultDock("TestTextEditDocument", dock);

	JXVertDockPartition* v2;
	ok = h->SplitVert(2, &v2);
	assert( ok );

	dock = dynamic_cast(JXDockWidget*, v2->GetCompartment(1));
	assert( dock != NULL );
	SetDefaultDock("TestFloatTableDirector", dock);

	dock = dynamic_cast(JXDockWidget*, v2->GetCompartment(2));
	assert( dock != NULL );
	SetDefaultDock("TestImageDirector", dock);

	h->SetElasticIndex(1);
	v1->SetElasticIndex(2);
	v2->SetElasticIndex(1);
}
