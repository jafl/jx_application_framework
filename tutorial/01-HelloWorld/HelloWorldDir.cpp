/******************************************************************************
 HelloWorldDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "HelloWorldDir.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

HelloWorldDir::HelloWorldDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	// Create the Hello World window. The function JGetString, retrieves
	// the appropriate title string from the strings database. This allows
	// for translations into other languages.
	JXWindow* window = jnew JXWindow(this, 200,100, JGetString("WindowTitle::HelloWorldDir"));

	// Set the min and max size of the window.
	// This particular pair of operations can also be done
	// by calling window->LockCurrentSize().
	window->SetMinSize(200,100);
	window->SetMaxSize(200,100);

	// Create the object to hold our "Hello World" message.
	// We do not need to keep a pointer to it because the
	// framework will delete it automatically when the window
	// is closed.
	jnew JXStaticText(JGetString("WindowText::HelloWorldDir"), window,
		JXWidget::kFixedLeft, JXWidget::kFixedTop,
		20, 40, 160, 20);
}
