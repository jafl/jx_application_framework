/******************************************************************************
 ClipboardDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "ClipboardDir.h"
#include "ClipboardWidget.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXGetStringDialog.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

// This defines the menu items
static const JUtf8Byte* kTextMenuStr =
	"Change Text %k Meta-T %l| Quit %k Meta-Q";

enum
{
	kChangeText = 1,
	kQuit
};

/******************************************************************************
 Constructor

 ******************************************************************************/

ClipboardDir::ClipboardDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	// Set up the window and its contents.
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ClipboardDir::~ClipboardDir()
{
	// We don't need to delete anything here, since the director deletes the
	// window automatically, and the window will delete anything within it.
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
ClipboardDir::BuildWindow()
{
	// Create the window and give it to the director.
	JXWindow* window = jnew JXWindow(this, 200,100, JGetString("WindowTitle::ClipboardDir"));
	assert( window != nullptr );

	// This sets the minimum and maximum size to be the
	// current size.
	window->LockCurrentSize();

	// Create the menu bar and make it expand horizontally as the window expands.
	JXMenuBar* menuBar =
		jnew JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
						0,0, 200,kJXDefaultMenuBarHeight);
	assert( menuBar != nullptr );

	// Create the menu and attach it to the menu bar.
	itsTextMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::ClipboardDir"));

	// Set the menu items.
	itsTextMenu->SetMenuItems(kTextMenuStr);

	// The menu items don't need to be disabled
	itsTextMenu->SetUpdateAction(JXMenu::kDisableNone);

	// Listen for messages from the menu.
	itsTextMenu->AttachHandler(this, &ClipboardDir::HandleTextMenu);

	// Create the object to hold the text.
	itsText =
		jnew ClipboardWidget(JGetString("Text::ClipboardDir"), menuBar, window,
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			0, kJXDefaultMenuBarHeight, 200, 100-kJXDefaultMenuBarHeight);
	assert ( itsText != nullptr );
}

/******************************************************************************
 HandleTextMenu

	Respond to the selected menu item.

 ******************************************************************************/

void
ClipboardDir::HandleTextMenu
	(
	const JIndex index
	)
{
	// Respond to the different menu items.
	if (index == kChangeText)
	{
		// Create the dialog with text from our text widget object.
		auto* dlog = jnew JXGetStringDialog(
			JGetString("DialogTitle::ClipboardDir"),
			JGetString("DialogPrompt::ClipboardDir"),
			itsText->GetText());
		assert ( dlog != nullptr );

		// Activate the dialog.
		if (dlog->DoDialog())
		{
			// Get the text from the dialog and pass it to the text widget object.
			itsText->SetText(dlog->GetString());
		}
	}

	else if (index == kQuit)
	{
		// Quit the program. The application object is one of the few global
		// objects that can be accessed from anywhere within the application.

		JXGetApplication()->Quit();
	}
}
