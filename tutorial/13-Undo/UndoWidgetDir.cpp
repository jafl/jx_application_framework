/******************************************************************************
 UndoWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "UndoWidgetDir.h"
#include "UndoWidget.h"
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <jXGlobals.h>
#include <jAssert.h>

// These define the menu title and the menu items.
// The '|' separates menu items.  The complete syntax
// is described in JXTextMenuData.doc in SetMenuItems()
// and ParseMenuItemStr().

static const JCharacter* kActionsMenuTitleStr = "Actions";
static const JCharacter* kActionsMenuStr = 
	"Undo %k Meta-Z|Redo %k Meta-Shift-Z"
	"|Quit %k Meta-Q";
	
enum
{
	kUndo = 1,
	kRedo,
	kQuit
};

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoWidgetDir::UndoWidgetDir
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

UndoWidgetDir::~UndoWidgetDir()
{
}

/******************************************************************************
 BuildWindow
 	 	This is a convenient and organized way of putting all of the initial 
 	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
UndoWidgetDir::BuildWindow()
{
		// Create the window
	JXWindow* window = new JXWindow(this, 300,200, "Undo Program");
    assert( window != NULL );
    
    // Give the window to the director
    SetWindow(window);
    
    // Set the window sizing
    window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

    // Create the menu bar so that it stays on top, but expands as the window
    // expands. 
    JXMenuBar* menuBar = 
    	new JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop, 
    					0,0, 300,kJXDefaultMenuBarHeight);
    assert( menuBar != NULL );
    
    // Attach our menu to the menu bar.
    itsActionsMenu = menuBar->AppendTextMenu(kActionsMenuTitleStr);
    
    // Set the menu items in our menu.
    itsActionsMenu->SetMenuItems(kActionsMenuStr);
    
    // Set the menu to never disable the menu items.
    itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
    
    // The director needs to listen to the menu for messages. The message
    // we care about is JXMenu::kItemSelected.
	ListenTo(itsActionsMenu);
	
	// Create the scrollbar set
	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 
			0,kJXDefaultMenuBarHeight, 300,200 - kJXDefaultMenuBarHeight);
	assert( scrollbarSet != NULL );

	// Create the custom widget with the scrollbarset as its enclosure	
	itsWidget = 
		new UndoWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(), 
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( itsWidget != NULL );
	
	// Fit the widget within the scrollbarset enclosure
	itsWidget->FitToEnclosure(kJTrue, kJTrue);
}

/******************************************************************************
 Receive (protected)

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
UndoWidgetDir::Receive
	(
	JBroadcaster* 	sender, 
	const Message&	message
	)
{
	// Check to see if our menu is broadcasting a message. It simply
	// broadcasts to everyone who is listening (via "ListenTo(...)")
	if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{

		// We need to cast the sender in order to get access to its
		// member function - GetIndex() in this case.
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );

		// Pass the index to a menu handler function
		HandleActionsMenu(selection->GetIndex());
		}

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		// The menu is about to open, so we need to update its
		UpdateActionsMenu();
		}

	// If we don't handle the message, we need to pass it to the base class	
	else
		{
		JXWindowDirector::Receive(sender,message);
		}
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
UndoWidgetDir::UpdateActionsMenu()
{
	if (itsWidget->HasUndo())
		{
		itsActionsMenu->EnableItem(kUndo);
		}
	else
		{
		itsActionsMenu->DisableItem(kUndo);
		}
	if (itsWidget->HasRedo())
		{
		itsActionsMenu->EnableItem(kRedo);
		}
	else
		{
		itsActionsMenu->DisableItem(kRedo);
		}
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
UndoWidgetDir::HandleActionsMenu
	(
	const JIndex index
	)
{
	// Check to see which menu item was selected, and respond accordingly
	if (index == kUndo)
		{
		itsWidget->Undo();
		}
	else if (index == kRedo)
		{
		itsWidget->Redo();
		}
	else if (index == kQuit)
		{
		(JXGetApplication())->Quit();
		}
}
