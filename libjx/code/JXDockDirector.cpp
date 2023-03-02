/******************************************************************************
 JXDockDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 2002-03 by John Lindal.

 ******************************************************************************/

#include "JXDockDirector.h"
#include "JXDockManager.h"
#include "JXWindow.h"
#include "JXHorizDockPartition.h"
#include "JXVertDockPartition.h"
#include "JXDockWidget.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockDirector::JXDockDirector
	(
	const JString&	title,
	const bool	splitHoriz
	)
	:
	JXWindowDirector(JXGetDockManager()),
	itsHorizPartition(nullptr),
	itsVertPartition(nullptr),
	itsFocusWindow(nullptr)
{
	BuildWindow(title, splitHoriz);
}

JXDockDirector::JXDockDirector
	(
	std::istream&		input,
	const JFileVersion	vers,
	const JString&		title
	)
	:
	JXWindowDirector(JXGetDockManager()),
	itsHorizPartition(nullptr),
	itsVertPartition(nullptr),
	itsFocusWindow(nullptr)
{
	bool splitHoriz;
	input >> JBoolFromString(splitHoriz);

	BuildWindow(title, splitHoriz);
	GetWindow()->ReadGeometry(input);

	if (itsHorizPartition != nullptr)
	{
		itsHorizPartition->ReadSetup(input, vers);
	}
	else
	{
		itsVertPartition->ReadSetup(input, vers);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockDirector::~JXDockDirector()
{
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
JXDockDirector::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << ' ' << JBoolToString(itsHorizPartition != nullptr);

	GetWindow()->WriteGeometry(output);

	if (itsHorizPartition != nullptr)
	{
		itsHorizPartition->WriteSetup(output);
	}
	else
	{
		itsVertPartition->WriteSetup(output);
	}
}

/******************************************************************************
 Close (virtual)

	Close all windows that are docked within us.  If we are the last dock,
	then we quit the application to avoid losing the dock's configuration.

 ******************************************************************************/

bool
JXDockDirector::Close()
{
	JXApplication* app = JXGetApplication();
	JXDockManager* mgr = JXGetDockManager();
	if (mgr->IsLastDock(this) && !app->IsQuitting() && HasWindows())
	{
		app->Quit();
		return false;
	}
	else
	{
		// We must close all windows before closing ourselves because
		// otherwise, we will get a Draw event after partially destructing
		// if a document needs to ask the user whether or not to close.

		return CloseAllWindows() && JXWindowDirector::Close();
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXDockDirector::BuildWindow
	(
	const JString&	title,
	const bool	splitHoriz
	)
{
	const JCoordinate kSize = 300;

	JArray<JCoordinate> sizes;
	sizes.AppendElement(kSize);
	sizes.AppendElement(kSize);

	JArray<JCoordinate> minSizes;
	minSizes.AppendElement(JXDockWidget::kDefaultMinSize);
	minSizes.AppendElement(JXDockWidget::kDefaultMinSize);

	const JCoordinate w = 2*kSize + (splitHoriz ? JPartition::kDragRegionSize : 0);
	const JCoordinate h = 2*kSize + (splitHoriz ? 0 : JPartition::kDragRegionSize);

	auto* window = jnew JXWindow(this, w, h, title);
	assert( window != nullptr );

	JXImage* icon;
	if ((JXGetDockManager())->CreateIcon(&icon))
	{
		window->SetIcon(icon);
	}

	if (splitHoriz)
	{
		itsHorizPartition =
			jnew JXHorizDockPartition(this, nullptr, sizes, 0, minSizes,
									 window, JXWidget::kHElastic, JXWidget::kVElastic,
									 0,0, w,h);
		assert( itsHorizPartition != nullptr );
	}
	else
	{
		itsVertPartition =
			jnew JXVertDockPartition(this, nullptr, sizes, 0, minSizes,
									window, JXWidget::kHElastic, JXWidget::kVElastic,
									0,0, w,h);
		assert( itsVertPartition != nullptr );
	}

	window->SetMinSize(100,100);
	window->SetWMClass("Dock", JXGetDockWindowClass());
}

/******************************************************************************
 FindDock

 ******************************************************************************/

bool
JXDockDirector::FindDock
	(
	const JIndex	id,
	JXDockWidget**	dock
	)
{
	if (itsHorizPartition != nullptr)
	{
		return itsHorizPartition->FindDock(id, dock);
	}
	else
	{
		return itsVertPartition->FindDock(id, dock);
	}
}

/******************************************************************************
 HasWindows

 ******************************************************************************/

bool
JXDockDirector::HasWindows()
	const
{
	if (itsHorizPartition != nullptr)
	{
		return itsHorizPartition->HasWindows();
	}
	else
	{
		return itsVertPartition->HasWindows();
	}
}

/******************************************************************************
 CloseAllWindows

 ******************************************************************************/

bool
JXDockDirector::CloseAllWindows()
{
	if (itsHorizPartition != nullptr)
	{
		return itsHorizPartition->CloseAllWindows();
	}
	else
	{
		return itsVertPartition->CloseAllWindows();
	}
}

/******************************************************************************
 UpdateMinSize

 ******************************************************************************/

void
JXDockDirector::UpdateMinSize()
{
	JPoint minSize;
	if (itsHorizPartition != nullptr)
	{
		minSize = itsHorizPartition->UpdateMinSize();
	}
	else
	{
		minSize = itsVertPartition->UpdateMinSize();
	}
	GetWindow()->SetMinSize(minSize.x, minSize.y);
}

/******************************************************************************
 GetHorizPartition

 ******************************************************************************/

bool
JXDockDirector::GetHorizPartition
	(
	JXHorizDockPartition** p
	)
	const
{
	if (itsHorizPartition != nullptr)
	{
		*p = itsHorizPartition;
		assert( *p != nullptr );
		return true;
	}
	else
	{
		*p = nullptr;
		return false;
	}
}

/******************************************************************************
 GetVertPartition

 ******************************************************************************/

bool
JXDockDirector::GetVertPartition
	(
	JXVertDockPartition** p
	)
	const
{
	if (itsVertPartition != nullptr)
	{
		*p = itsVertPartition;
		assert( *p != nullptr );
		return true;
	}
	else
	{
		*p = nullptr;
		return false;
	}
}

/******************************************************************************
 Focus window

 ******************************************************************************/

bool
JXDockDirector::GetFocusWindow
	(
	JXWindow** window
	)
{
	*window = itsFocusWindow;
	return itsFocusWindow != nullptr;
}

void
JXDockDirector::SetFocusWindow
	(
	JXWindow* window
	)
{
	if (window != itsFocusWindow)
	{
		if (itsFocusWindow != nullptr)
		{
			itsFocusWindow->Refresh();
		}

		itsFocusWindow = window;

		if (itsFocusWindow != nullptr)
		{
			itsFocusWindow->Refresh();
		}
	}
}

void
JXDockDirector::ClearFocusWindow
	(
	JXWindow* window
	)
{
	if (itsFocusWindow == window)
	{
		if (itsFocusWindow != nullptr)
		{
			itsFocusWindow->Refresh();
		}

		itsFocusWindow = nullptr;
	}
}
