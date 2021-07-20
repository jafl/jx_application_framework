/******************************************************************************
 CMDockManager.cpp

	BASE CLASS = JXDockManager

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#include "CMDockManager.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include <JXDockDirector.h>
#include <JXWindow.h>
#include <JXHorizDockPartition.h>
#include <JXVertDockPartition.h>
#include <JXDockWidget.h>
#include <JXImage.h>
#include "medic_command_window.xpm"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMDockManager::CMDockManager()
	:
	JXDockManager(JXGetApplication()->GetCurrentDisplay(), JGetString("CMName"),
				  CMGetPrefsManager(), kCMDockPrefID)
{
	JXDisplay* display = JXGetApplication()->GetCurrentDisplay();
	auto* icon      = jnew JXImage(display, medic_command_window);
	assert( icon != nullptr );
	SetIcon(icon);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMDockManager::~CMDockManager()
{
}

/******************************************************************************
 CanDockAll (virtual)

 ******************************************************************************/

bool
CMDockManager::CanDockAll()
	const
{
	return GetDockList()->IsEmpty();
}

/******************************************************************************
 DockAll (virtual)

	Dock all the windows and set docking positions for all window types.

 ******************************************************************************/

void
CMDockManager::DockAll()
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

	JXVertDockPartition* v2;
	ok = h->SplitVert(2, &v2);
	assert( ok );

	h->SetElasticIndex(1);
	v1->SetElasticIndex(1);
	v2->SetElasticIndex(2);

	(dir->GetWindow())->AdjustSize(200, 0);

	JXDockWidget* dock1 = v1->GetDockWidget(1);
	assert( dock1 != nullptr );
	SetDefaultDock(CMGetMainSourceWindowClass(), dock1);
	SetDefaultDock(CMGetMainAsmWindowClass(),    dock1);
	SetDefaultDock(CMGetSourceViewWindowClass(), dock1);
	SetDefaultDock(CMGetPlot2DWindowClass(),     dock1);

	JXDockWidget* dock2 = v1->GetDockWidget(2);
	assert( dock2 != nullptr );
	SetDefaultDock(CMGetCommandWindowClass(), dock2);

	JXDockWidget* dock3 = v2->GetDockWidget(1);
	assert( dock3 != nullptr );
	SetDefaultDock(CMGetThreadWindowClass(),   dock3);
	SetDefaultDock(CMGetStackWindowClass(),    dock3);
	SetDefaultDock(CMGetFileListWindowClass(), dock3);

	JXDockWidget* dock4 = v2->GetDockWidget(2);
	assert( dock4 != nullptr );
	SetDefaultDock(CMGetVariableWindowClass(),      dock4);
	SetDefaultDock(CMGetLocalVariableWindowClass(), dock4);
	SetDefaultDock(CMGetArray1DWindowClass(),       dock4);
	SetDefaultDock(CMGetArray2DWindowClass(),       dock4);
	SetDefaultDock(CMGetMemoryWindowClass(),        dock4);

	CMCommandDirector* cmdDir = CMGetCommandDirector();
	dock2->Dock(cmdDir->GetWindow());
	cmdDir->DockAll(dock1, dock3, dock4);
}
