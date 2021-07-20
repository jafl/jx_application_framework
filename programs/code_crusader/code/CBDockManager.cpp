/******************************************************************************
 CBDockManager.cpp

	BASE CLASS = JXDockManager

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#include "CBDockManager.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "CBFileListDirector.h"
#include "CBCTreeDirector.h"
#include "CBJavaTreeDirector.h"
#include "CBPHPTreeDirector.h"
#include "CBTextDocument.h"
#include "cbGlobals.h"
#include <JXDockDirector.h>
#include <JXWindow.h>
#include <JXHorizDockPartition.h>
#include <JXVertDockPartition.h>
#include <JXDockWidget.h>
#include <JXTabGroup.h>
#include <JXImage.h>
#include "jcc_project_window.xpm"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDockManager::CBDockManager()
	:
	JXDockManager(JXGetApplication()->GetCurrentDisplay(), JGetString("CBName"),
				  CBGetPrefsManager(), kCBDockPrefID)
{
	JXDisplay* display = JXGetApplication()->GetCurrentDisplay();
	auto* icon      = jnew JXImage(display, jcc_project_window);
	assert( icon != nullptr );
	SetIcon(icon);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDockManager::~CBDockManager()
{
}

/******************************************************************************
 CanDockAll (virtual)

 ******************************************************************************/

bool
CBDockManager::CanDockAll()
	const
{
	return GetDockList()->IsEmpty();
}

/******************************************************************************
 DockAll (virtual)

	Dock all the windows and set docking positions for all window types.

 ******************************************************************************/

void
CBDockManager::DockAll()
{
	if (!GetDockList()->IsEmpty())
		{
		return;
		}

	JXDockDirector* dir = CreateDock(false);

	JXVertDockPartition* v;
	bool ok = dir->GetVertPartition(&v);
	assert( ok );

	JXHorizDockPartition* h;
	ok = v->SplitHoriz(1, &h);
	assert( ok );

	v->SetElasticIndex(1);
	h->SetElasticIndex(2);

	(dir->GetWindow())->AdjustSize(200, 0);
	v->SetCompartmentSize(2, 2*v->GetCompartmentSize(2)/3);

	JXDockWidget* dock1 = h->GetDockWidget(1);
	assert( dock1 != nullptr );
	SetDefaultDock(CBGetProjectWindowClass(),  dock1);
	SetDefaultDock(CBGetSymbolWindowClass(),   dock1);
	SetDefaultDock(CBGetFileListWindowClass(), dock1);

	JXDockWidget* dock2 = h->GetDockWidget(2);
	assert( dock2 != nullptr );
	SetDefaultDock(CBGetEditorWindowClass(), dock2);
	SetDefaultDock(CBGetTreeWindowClass(),   dock2);

	JXDockWidget* dock3 = v->GetDockWidget(2);
	assert( dock3 != nullptr );
	SetDefaultDock(CBGetExecOutputWindowClass(),    dock3);
	SetDefaultDock(CBGetCompileOutputWindowClass(), dock3);
	SetDefaultDock(CBGetSearchOutputWindowClass(),  dock3);

	(dock3->GetTabGroup())->SetTabEdge(JXTabGroup::kBottom);

	JPtrArray<CBProjectDocument>* projList =
		CBGetDocumentManager()->GetProjectDocList();

	JSize count = projList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBProjectDocument* projDoc = projList->GetElement(i);

		dock1->Dock(projDoc);
		dock1->Dock(projDoc->GetSymbolDirector());
		dock1->Dock(projDoc->GetFileListDirector());

		dock2->Dock(projDoc->GetCTreeDirector());
		dock2->Dock(projDoc->GetJavaTreeDirector());
		dock2->Dock(projDoc->GetPHPTreeDirector());
		}

	JPtrArray<CBTextDocument>* textList =
		CBGetDocumentManager()->GetTextDocList();

	count = textList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBTextDocument* textDoc = textList->GetElement(i);

		const CBTextFileType type = textDoc->GetFileType();
		if (type == kCBExecOutputFT   ||
			type == kCBSearchOutputFT ||
			type == kCBShellOutputFT)
			{
			dock3->Dock(textDoc);
			}
		else
			{
			dock2->Dock(textDoc);
			}
		}
}
