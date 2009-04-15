/******************************************************************************
 HelloWorldDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "HelloWorldDir.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kWindowTitleID	= "WindowTitle::HelloWorldDir";
static const JCharacter* kWindowTextID	= "WindowText::HelloWorldDir";

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
	JXWindow* window = new JXWindow(this, 200,100, JGetString(kWindowTitleID));

	// Make sure that new succeeded
    assert( window != NULL );

    // Tell the window director that this is its window
    SetWindow(window);

    // Set the min and max size of the window.
    // This particular pair of operations can also be done
    // by calling window->LockCurrentSize().
    window->SetMinSize(200,100);
	window->SetMaxSize(200,100);

	// Create the object to hold our "Hello World" message.
	// We do not need to keep a pointer to it because the
	// framework will delete it automatically when the window
	// is closed.
	JXStaticText* text = 
		new JXStaticText(JGetString(kWindowTextID), window, 
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			20, 40, 160, 20);
	assert( text != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

HelloWorldDir::~HelloWorldDir()
{
}
