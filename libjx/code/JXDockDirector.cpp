/******************************************************************************
 JXDockDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 2002-03 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXDockDirector.h>
#include <JXDockManager.h>
#include <JXWindow.h>
#include <JXHorizDockPartition.h>
#include <JXVertDockPartition.h>
#include <JXDockWidget.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockDirector::JXDockDirector
	(
	const JCharacter*	title,
	const JBoolean		splitHoriz
	)
	:
	JXWindowDirector(JXGetDockManager()),
	itsHorizPartition(NULL),
	itsVertPartition(NULL),
	itsFocusWindow(NULL)
{
	BuildWindow(title, splitHoriz);
}

JXDockDirector::JXDockDirector
	(
	istream&			input,
	const JFileVersion	vers,
	const JCharacter*	title
	)
	:
	JXWindowDirector(JXGetDockManager()),
	itsHorizPartition(NULL),
	itsVertPartition(NULL),
	itsFocusWindow(NULL)
{
	JBoolean splitHoriz;
	input >> splitHoriz;

	BuildWindow(title, splitHoriz);
	GetWindow()->ReadGeometry(input);

	if (itsHorizPartition != NULL)
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
	ostream& output
	)
	const
{
	output << ' ' << JI2B(itsHorizPartition != NULL);

	GetWindow()->WriteGeometry(output);

	if (itsHorizPartition != NULL)
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

JBoolean
JXDockDirector::Close()
{
	JXApplication* app = JXGetApplication();
	JXDockManager* mgr = JXGetDockManager();
	if (mgr->IsLastDock(this) && !app->IsQuitting() && HasWindows())
		{
		app->Quit();
		return kJFalse;
		}
	else
		{
		// We must close all windows before closing ourselves because
		// otherwise, we will get a Draw event after partially destructing
		// if a document needs to ask the user whether or not to close.

		return JI2B( CloseAllWindows() && JXWindowDirector::Close() );
		}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXDockDirector::BuildWindow
	(
	const JCharacter*	title,
	const JBoolean		splitHoriz
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

	JXWindow* window = new JXWindow(this, w, h, title);
	assert( window != NULL );

	JXImage* icon;
	if ((JXGetDockManager())->CreateIcon(&icon))
		{
		window->SetIcon(icon);
		}

	if (splitHoriz)
		{
		itsHorizPartition =
			new JXHorizDockPartition(this, NULL, sizes, 0, minSizes,
									 window, JXWidget::kHElastic, JXWidget::kVElastic,
									 0,0, w,h);
		assert( itsHorizPartition != NULL );
		}
	else
		{
		itsVertPartition =
			new JXVertDockPartition(this, NULL, sizes, 0, minSizes,
									window, JXWidget::kHElastic, JXWidget::kVElastic,
									0,0, w,h);
		assert( itsVertPartition != NULL );
		}

	window->SetMinSize(100,100);
	window->SetWMClass("Dock", JXGetDockWindowClass());
}

/******************************************************************************
 FindDock

 ******************************************************************************/

JBoolean
JXDockDirector::FindDock
	(
	const JIndex	id,
	JXDockWidget**	dock
	)
{
	if (itsHorizPartition != NULL)
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

JBoolean
JXDockDirector::HasWindows()
	const
{
	if (itsHorizPartition != NULL)
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

JBoolean
JXDockDirector::CloseAllWindows()
{
	if (itsHorizPartition != NULL)
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
	if (itsHorizPartition != NULL)
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

JBoolean
JXDockDirector::GetHorizPartition
	(
	JXHorizDockPartition** p
	)
	const
{
	if (itsHorizPartition != NULL)
		{
		*p = itsHorizPartition;
		assert( *p != NULL );
		return kJTrue;
		}
	else
		{
		*p = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 GetVertPartition

 ******************************************************************************/

JBoolean
JXDockDirector::GetVertPartition
	(
	JXVertDockPartition** p
	)
	const
{
	if (itsVertPartition != NULL)
		{
		*p = itsVertPartition;
		assert( *p != NULL );
		return kJTrue;
		}
	else
		{
		*p = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 Focus window

 ******************************************************************************/

JBoolean
JXDockDirector::GetFocusWindow
	(
	JXWindow** window
	)
{
	*window = itsFocusWindow;
	return JI2B( itsFocusWindow != NULL );
}

void
JXDockDirector::SetFocusWindow
	(
	JXWindow* window
	)
{
	if (window != itsFocusWindow)
		{
		if (itsFocusWindow != NULL)
			{
			itsFocusWindow->Refresh();
			}

		itsFocusWindow = window;

		if (itsFocusWindow != NULL)
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
		if (itsFocusWindow != NULL)
			{
			itsFocusWindow->Refresh();
			}

		itsFocusWindow = NULL;
		}
}
