/******************************************************************************
 ClipDir.cc

	BASE CLASS = JXWindowDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "ClipDir.h"
#include <JXWindow.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ClipDir::ClipDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	JXWindow* window = new JXWindow(this, 20,20, "");
    assert( window != NULL );
    SetWindow(window);
    window->SetCloseAction(JXWindow::kDeactivateDirector);

}

/******************************************************************************
 Destructor

 ******************************************************************************/

ClipDir::~ClipDir()
{
}
