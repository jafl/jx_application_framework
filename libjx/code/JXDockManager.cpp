/******************************************************************************
 JXDockManager.cpp

	This class is designed to be used as a global object.  It maintains the
	list of active docking windows.

	Derived classes are responsible for calling Read/WritePrefs().

	To do:  support multiple displays

	BASE CLASS = JXDirector, JPrefObject

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include "JXDockManager.h"
#include "JXDockDirector.h"
#include "JXDockWidget.h"
#include "JXDockWindowTask.h"
#include "JXDisplay.h"
#include "JXImage.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentSetupVersion = 3;

	// version  3 saved elastic index for each partition
	// version  2 saves JXTabGroup data
	// version  1 saves itsWindowTypeMap

static const JUtf8Byte* kAtomNames[ JXDockManager::kAtomCount ] =
{
	"MinSize::JXDockDragData",
	"Window::JXDockDragData"
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockManager::JXDockManager
	(
	JXDisplay*		display,
	const JString&	title,
	JPrefsManager*	prefsMgr,
	const JPrefID&	id
	)
	:
	JXDirector(nullptr),
	JPrefObject(prefsMgr, id),
	itsDisplay(display),
	itsTitle(title),
	itsWindowIcon(nullptr),
	itsNextDockIndex(1),
	itsNextDockID(1),
	itsIsReadingSetupFlag(false),
	itsCloseDockMode(kUndockWindows)
{
	itsDockList = jnew JPtrArray<JXDockDirector>(JPtrArrayT::kForgetAll);
	assert( itsDockList != nullptr );

	itsWindowTypeMap = jnew JStringMap<JIndex>;
	assert( itsWindowTypeMap != nullptr );

	display->RegisterXAtoms(kAtomCount, kAtomNames, itsAtoms);

	JXSetDockManager(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockManager::~JXDockManager()
{
	jdelete itsWindowIcon;
	jdelete itsDockList;		// JXDockDirectors deleted by JXDirector
	jdelete itsWindowTypeMap;
}

/******************************************************************************
 CreateDock

 ******************************************************************************/

JXDockDirector*
JXDockManager::CreateDock
	(
	const bool splitHoriz
	)
{
	const JString title = GetNewDockTitle();

	JXGetApplication()->SetCurrentDisplay(itsDisplay);
	JXDockWindowTask::PrepareForDockAll();

	auto* dock = jnew JXDockDirector(title, splitHoriz);
	assert( dock != nullptr );
	itsDockList->Append(dock);
	dock->Activate();
	return dock;
}

/******************************************************************************
 GetNewDockTitle (private)

 ******************************************************************************/

JString
JXDockManager::GetNewDockTitle()
{
	JString title(itsNextDockIndex, 0);
	title.Prepend(JGetString("WindowTitleSuffix::JXDockManager"));
	title.Prepend(itsTitle);

	itsNextDockIndex++;

	return title;
}

/******************************************************************************
 SetIcon

	We take ownership of the icon.

 ******************************************************************************/

void
JXDockManager::SetIcon
	(
	JXImage* icon
	)
{
	jdelete itsWindowIcon;
	itsWindowIcon = icon;
}

/******************************************************************************
 CreateIcon

 ******************************************************************************/

bool
JXDockManager::CreateIcon
	(
	JXImage** icon
	)
	const
{
	if (itsWindowIcon != nullptr)
	{
		*icon = jnew JXImage(*itsWindowIcon);
		assert( *icon != nullptr );
		return true;
	}
	else
	{
		*icon = nullptr;
		return false;
	}
}

/******************************************************************************
 FindDock

 ******************************************************************************/

bool
JXDockManager::FindDock
	(
	const JIndex	id,
	JXDockWidget**	dock
	)
{
	for (auto* dir : *itsDockList)
	{
		if (dir->FindDock(id, dock))
		{
			return true;
		}
	}

	*dock = nullptr;
	return false;
}

/*****************************************************************************
 CloseAll

 ******************************************************************************/

void
JXDockManager::CloseAll()
{
	// make copy of list so DirectorClosed() doesn't interfere

	JPtrArray<JXDockDirector> list(*itsDockList, JPtrArrayT::kForgetAll);
	itsDockList->RemoveAll();
	itsNextDockIndex = 1;
	itsNextDockID    = 1;

	for (auto* dir : list)
	{
		dir->Close();
	}
}

/******************************************************************************
 CanDockAll (virtual)

	Derived class should override this to return true if it implements
	DockAll().

 ******************************************************************************/

bool
JXDockManager::CanDockAll()
	const
{
	return false;
}

/******************************************************************************
 DockAll (virtual)

	Derived class should override this to dock all the windows and set
	docking positions for all window types.

 ******************************************************************************/

void
JXDockManager::DockAll()
{
}

/******************************************************************************
 GetDefaultDock

 ******************************************************************************/

bool
JXDockManager::GetDefaultDock
	(
	const JUtf8Byte*	windowType,
	JXDockWidget**		dock
	)
{
	JIndex id;
	if (itsWindowTypeMap->GetElement(windowType, &id))
	{
		if (FindDock(id, dock))
		{
			return dock != nullptr;
		}
		else
		{
			SetDefaultDock(windowType, nullptr);
			return false;
		}
	}
	else
	{
		*dock = nullptr;
		return false;
	}
}

/******************************************************************************
 SetDefaultDock

	If dock==nullptr, clears the mapping.

 ******************************************************************************/

void
JXDockManager::SetDefaultDock
	(
	const JUtf8Byte*	windowType,
	const JXDockWidget*	dock
	)
{
	if (dock == nullptr || windowType == JXGetDockWindowClass())
	{
		itsWindowTypeMap->RemoveElement(windowType);
	}
	else
	{
		itsWindowTypeMap->SetElement(windowType, dock->GetID());
	}
}

/*****************************************************************************
 DirectorClosed (virtual protected)

	Listen for docks that are closed.

 ******************************************************************************/

void
JXDockManager::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	const JSize count = itsDockList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		JXDockDirector* dir = itsDockList->GetElement(i);
		if (theDirector == dir)
		{
			itsDockList->RemoveElement(i);
			break;
		}
	}

	JXDirector::DirectorClosed(theDirector);
}

/******************************************************************************
 ReadSetup

	We assert that we can read the data that is provided because there
	is no way to skip over it.

 ******************************************************************************/

void
JXDockManager::ReadSetup
	(
	std::istream& input
	)
{
	CloseAll();

	itsIsReadingSetupFlag = true;

	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentSetupVersion );

	JSize count;
	input >> count;

	JXGetApplication()->SetCurrentDisplay(itsDisplay);

	JString title;
	for (JIndex i=1; i<=count; i++)
	{
		title = GetNewDockTitle();

		auto* dir = jnew JXDockDirector(input, vers, title);
		assert( dir != nullptr );
		itsDockList->Append(dir);
		dir->Activate();
	}

	itsWindowTypeMap->RemoveAll();
	if (vers >= 1)
	{
		JString windowType;
		while (true)
		{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
			if (input.fail() || !keepGoing)
			{
				break;
			}

			JIndex id;
			input >> windowType >> id;

			JXDockWidget* dock;
			if (FindDock(id, &dock))
			{
				itsWindowTypeMap->SetElement(windowType, id);
			}
		}
	}

	itsIsReadingSetupFlag = false;
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXDockManager::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	output << ' ' << itsDockList->GetElementCount();

	for (auto* dir : *itsDockList)
	{
		dir->StreamOut(output);
	}

	JStringMapCursor<JIndex> cursor(itsWindowTypeMap);
	JString windowType;
	while (cursor.Next())
	{
		windowType = cursor.GetKey();
		output << ' ' << JBoolToString(true);
		output << ' ' << windowType;
		output << ' ' << cursor.GetValue();
	}
	output << ' ' << JBoolToString(false);

	output << ' ';
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
JXDockManager::ReadPrefs
	(
	std::istream& input
	)
{
	ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
JXDockManager::WritePrefs
	(
	std::ostream& output
	)
	const
{
	WriteSetup(output);
}
