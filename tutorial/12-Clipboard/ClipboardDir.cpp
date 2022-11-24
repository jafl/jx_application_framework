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
	// We create the dialog when we need it.
	itsDialog = nullptr;

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
	ListenTo(itsTextMenu);

	// Create the object to hold the text.
	itsText =
		jnew ClipboardWidget(JGetString("Text::ClipboardDir"), menuBar, window,
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			0, kJXDefaultMenuBarHeight, 200, 100-kJXDefaultMenuBarHeight);
	assert ( itsText != nullptr );
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu selections and deactivated dialog windows.

 ******************************************************************************/

void
ClipboardDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// Check to see if the a menu item was selected.
	if (sender == itsTextMenu && message.Is(JXMenu::kItemSelected))
	{
		// Cast the sender so we can access its functions.
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		// Handle the menu selection
		HandleTextMenu(selection->GetIndex());
	}

	// Check if the sender is our dialog and that it's been diactivated.
	else if (sender == itsDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		// Cast the sender so we can access its functions.
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );

		// If the user pressed the cancel button, this will fail.
		if (info->Successful())
		{
			// Get the next text from the dialog.
			GetNewTextFromDialog();
		}

		// The dialog is deleted (not by us) after it broadcasts this message.
		itsDialog = nullptr;
	}

	// If we don't handle the message, we need to pass it to the base class
	else
	{
		JXWindowDirector::Receive(sender,message);
	}
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
		// Create and activate the input dialog.

		SetupInputDialog();
	}

	else if (index == kQuit)
	{
		// Quit the program. The application object is one of the few global
		// objects that can be accessed from anywhere within the application.

		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 SetupInputDialog

	Create and activate the input dialog.

	Dialog windows notify you that they have been dismissed by
	broadcasting the message JXDialogDirector::kDeactivated.
	Thus, all we have to do is create it, activate it, and listen
	to it.

 ******************************************************************************/

void
ClipboardDir::SetupInputDialog()
{
	// Make sure the dialog hasn't already been created.
	assert ( itsDialog == nullptr );

	// Create the dialog with text from our text widget object.
	itsDialog = jnew JXGetStringDialog(this,
		JGetString("DialogTitle::ClipboardDir"),
		JGetString("DialogPrompt::ClipboardDir"),
		itsText->GetText());
	assert ( itsDialog != nullptr );

	// We need to listen for the dialog's deactivation message.
	ListenTo(itsDialog);

	// Activate the dialog.
	itsDialog->BeginDialog();
}

/******************************************************************************
 GetNewTextFromDialog

	Pull the new text out of the dialog.

 ******************************************************************************/

void
ClipboardDir::GetNewTextFromDialog()
{
	// The dialog must have been created if we're calling this.
	assert ( itsDialog != nullptr );

	// Get the text from the dialog and pass it to the text widget object.
	const JString& str = itsDialog->GetString();
	itsText->SetText(str);
}
