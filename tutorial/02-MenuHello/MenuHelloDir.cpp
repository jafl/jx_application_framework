/******************************************************************************
 MenuHelloDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "MenuHelloDir.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// This defines the menu items.
// The '|' separates menu items.  The complete syntax
// is described in JXTextMenuData.cpp in SetMenuItems()
// and ParseMenuItemStr().

static const JUtf8Byte* kTextMenuStr =
	"Hello world! | Goodbye cruel world!";

enum
{
	kHello = 1,
	kGoodbye
};

/******************************************************************************
 Constructor

 ******************************************************************************/

MenuHelloDir::MenuHelloDir
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

MenuHelloDir::~MenuHelloDir()
{
	// We must not delete the widgets because the framework does this
	// automatically.
}

/******************************************************************************
 BuildWindow

	This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
MenuHelloDir::BuildWindow()
{
	// Create the window and give it to the director.
	JXWindow* window = jnew JXWindow(this, 200,100, JGetString("WindowTitle::MenuHelloDir"));
	assert( window != nullptr );

	// This sets the minimum and maximum size to be the
	// current size.
	window->LockCurrentSize();

	// Create the menu bar so that it stays on top, but expands as the window
	// expands.
	JXMenuBar* menuBar =
		jnew JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
						0,0, 200,kJXDefaultMenuBarHeight);
	assert( menuBar != nullptr );

	// Attach our menu to the menu bar.
	itsTextMenu = menuBar->AppendTextMenu(JGetString("TextMenuTitle::MenuHelloDir"));

	// Set the menu items in our menu.
	itsTextMenu->SetMenuItems(kTextMenuStr);

	// Set the menu to never disable the menu items.
	itsTextMenu->SetUpdateAction(JXMenu::kDisableNone);

	// The director needs to listen to the menu for messages. The message
	// we care about is JXMenu::ItemSelected.
	ListenTo(itsTextMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		HandleTextMenu(msg.GetIndex());
	}));

	// Create the object to display the text. This time, we need to keep a
	// pointer to it so we can change the text later.
	itsText =
		jnew JXStaticText(JGetString("HelloText::MenuHelloDir"), window,
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			20, 40, 160, 20);
	assert( itsText != nullptr );
}

/******************************************************************************
 HandleTextMenu

	Respond when a menu item has been selected

 ******************************************************************************/

void
MenuHelloDir::HandleTextMenu
	(
	const JIndex index
	)
{
	// Check to see which menu item was selected, and set the text accordingly
	if (index == kHello)
	{
		itsText->GetText()->SetText(JGetString("HelloText::MenuHelloDir"));
	}

	else if (index == kGoodbye)
	{
		itsText->GetText()->SetText(JGetString("GoodbyeText::MenuHelloDir"));
	}
}
