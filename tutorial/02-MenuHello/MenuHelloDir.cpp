/******************************************************************************
 MenuHelloDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "MenuHelloDir.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <jAssert.h>

// These define the menu title and the menu items.
// The '|' separates menu items.  The complete syntax
// is described in JXTextMenuData.cc in SetMenuItems()
// and ParseMenuItemStr().

static const JCharacter* kTextMenuTitleStr = "Text";
static const JCharacter* kTextMenuStr = 
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
	JXWindow* window = new JXWindow(this, 200,100, "Hello World Program");
    assert( window != NULL );
    SetWindow(window);

	// This sets the minimum and maximum size to be the
	// current size.
    window->LockCurrentSize();

    // Create the menu bar so that it stays on top, but expands as the window
    // expands. 
    JXMenuBar* menuBar = 
    	new JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop, 
    					0,0, 200,kJXDefaultMenuBarHeight);
    assert( menuBar != NULL );
    
    // Attach our menu to the menu bar.
    itsTextMenu = menuBar->AppendTextMenu(kTextMenuTitleStr);
    
    // Set the menu items in our menu.
    itsTextMenu->SetMenuItems(kTextMenuStr);
    
    // Set the menu to never disable the menu items.
    itsTextMenu->SetUpdateAction(JXMenu::kDisableNone);
    
    // The director needs to listen to the menu for messages. The message
    // we care about is JXMenu::kItemSelected.
	ListenTo(itsTextMenu);
	
	// Create the object to display the text. This time, we need to keep a 
	// pointer to it so we can change the text later.
	itsText = 
		new JXStaticText("Hello world!", window, 
			JXWidget::kFixedLeft, JXWidget::kFixedTop,
			20, 40, 160, 20);
	assert( itsText != NULL );
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu selections.

	Inheriting from JBroadcaster allows objects to send messages to
	each other.  To receive messages from an object, one must call
	ListenTo(), as we did in BuildWindow().  Messages are delivered
	via the Receive() function.  One must always remember to check
	both the sender and the message type before taking any action.

	One must always remember to pass unhandled messages to the
	base class Receive() function so that it can process the messages
	it needs, too.

 ******************************************************************************/

void
MenuHelloDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// Check to see if our menu is broadcasting a message. It simply
	// broadcasts to everyone who is listening (via "ListenTo(...)")
	if (sender == itsTextMenu && message.Is(JXMenu::kItemSelected))
		{

		// We need to cast the sender in order to get access to its
		// member function - GetIndex() in this case.
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );

		// Pass the index to a menu handler function
		HandleTextMenu(selection->GetIndex());
		}

	// If we don't handle the message, we need to pass it to the base class	
	else
		{
		JXWindowDirector::Receive(sender,message);
		}
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
		itsText->SetText("Hello world!");
		}
		
	else if (index == kGoodbye)
		{
		itsText->SetText("Goodbye cruel world!");
		}
}
