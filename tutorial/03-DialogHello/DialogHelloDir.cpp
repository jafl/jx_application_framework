/******************************************************************************
 DialogHelloDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "DialogHelloDir.h"
#include "DHStringInputDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DialogHelloDir::DialogHelloDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DialogHelloDir::~DialogHelloDir()
{
	// We don't need to delete anything here, since the director deletes the
	// window automatically, and the window will delete anything within it.
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

#include "DialogHelloDir-Text.h"

void
DialogHelloDir::BuildWindow()
{
	// Create the window and give it to the director.
	JXWindow* window = jnew JXWindow(this, 200,100, JGetString("WindowTitle::DialogHelloDir"));

	// This sets the minimum and maximum size to be the
	// current size.
	window->LockCurrentSize();

	// Create the menu bar and make it expand horizontally as the window expands.
	JXMenuBar* menuBar =
		jnew JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
						0,0, 200,kJXDefaultMenuBarHeight);

	// Create the menu and attach it to the menu bar.
	itsTextMenu = menuBar->AppendTextMenu(JGetString("TextMenuTitle::DialogHelloDir"));

	// Set the menu items.
	itsTextMenu->SetMenuItems(kTextMenuStr);

	// The menu items don't need to be disabled
	itsTextMenu->SetUpdateAction(JXMenu::kDisableNone);

	// Listen for messages from the menu.
	itsTextMenu->AttachHandler(this, &DialogHelloDir::HandleTextMenu);

	// Apply configuration defined in the menu editor.
	ConfigureTextMenu(itsTextMenu);

	// Create the object to hold the text.
	itsText =
		jnew JXStaticText(JGetString("HelloText::DialogHelloDir"), window,
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			20, 40, 160, 20);
}

/******************************************************************************
 HandleTextMenu

	Respond to the selected menu item.

 ******************************************************************************/

void
DialogHelloDir::HandleTextMenu
	(
	const JIndex index
	)
{
	// Respond to the different menu items.
	if (index == kChangeText)
	{
		// Create the dialog with text from our static text object.
		auto* dlog = jnew DHStringInputDialog(itsText->GetText()->GetText());

		// Activate the dialog.
		if (dlog->DoDialog())
		{
			// Get the text from the dialog and pass it to the static text object.
			itsText->GetText()->SetText(dlog->GetString());
		}
	}

	else if (index == kQuit)
	{
		// Quit the program. The application object is one of the few global
		// objects that can be accessed from anywhere within the application.

		JXGetApplication()->Quit();
	}
}
