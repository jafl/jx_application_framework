/******************************************************************************
 UndoWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "UndoWidgetDir.h"
#include "UndoWidget.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jAssert.h>

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
 BuildWindow
		This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

#include "UndoWidgetDir-Actions.h"

void
UndoWidgetDir::BuildWindow()
{
	// Create the window
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::UndoWidgetDir"));
	assert( window != nullptr );

	// Set the window sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the menu bar so that it stays on top, but expands as the window
	// expands.
	JXMenuBar* menuBar =
		jnew JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
						0,0, 300,kJXDefaultMenuBarHeight);

	// Attach our menu to the menu bar.
	itsActionsMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::UndoWidgetDir"));

	// Set the menu items in our menu.
	itsActionsMenu->SetMenuItems(kActionsMenuStr);

	// Set the menu to never disable the menu items.
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);

	// The director needs to listen to the menu for messages.
	itsActionsMenu->AttachHandlers(this,
		&UndoWidgetDir::UpdateActionsMenu,
		&UndoWidgetDir::HandleActionsMenu);

	// Apply configuration defined in the menu editor.
	ConfigureActionsMenu(itsActionsMenu);

	// Create the scrollbar set
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kJXDefaultMenuBarHeight, 300,200 - kJXDefaultMenuBarHeight);

	// Create the custom widget with the scrollbarset as its enclosure
	itsWidget =
		jnew UndoWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( itsWidget != nullptr );

	// Fit the widget within the scrollbarset enclosure
	itsWidget->FitToEnclosure(true, true);
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
UndoWidgetDir::UpdateActionsMenu()
{
	bool hasUndo, hasRedo;
	itsWidget->GetUndoRedoChain()->HasMultipleUndo(&hasUndo, &hasRedo);
	itsActionsMenu->SetItemEnabled(kUndo, hasUndo);
	itsActionsMenu->SetItemEnabled(kRedo, hasRedo);
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
		JXGetApplication()->Quit();
	}
}
