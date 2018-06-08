/******************************************************************************
 JXTextEditor.cpp

	Implements a complete styled text editor, built on top of JTextEditor.

	BASE CLASS = JXTEBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXTextEditor.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXFontNameMenu.h>
#include <JXFontSizeMenu.h>
#include <JXTEStyleMenu.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextEditor::JXTextEditor
	(
	JStyledText*		text,
	const JBoolean		ownsText,
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor, text, ownsText, kJFalse, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	WantInput(kJTrue, kJTrue);
	ShouldAllowDragAndDrop(kJTrue);
	GetText()->SetLastSaveLocation();

	(scrollbarSet->GetVScrollbar())->SetScrollDelay(0);

	// add the menus

	itsMenuBar = menuBar;

	AppendEditMenu(menuBar, kJTrue, kJTrue, kJTrue, kJTrue, kJTrue, kJTrue, kJTrue, kJTrue);
	AppendSearchReplaceMenu(menuBar);

	itsFontMenu = jnew JXFontNameMenu(JGetString("FontMenuTitle::JXTextEditor"), menuBar,
									  kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsFontMenu != nullptr );
	menuBar->AppendMenu(itsFontMenu);
	ListenTo(itsFontMenu);

	itsSizeMenu = jnew JXFontSizeMenu(itsFontMenu, JGetString("SizeMenuTitle::JXTextEditor"), menuBar,
									  kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsSizeMenu != nullptr );
	menuBar->AppendMenu(itsSizeMenu);
	ListenTo(itsSizeMenu);

	itsStyleMenu = jnew JXTEStyleMenu(this, menuBar,
									  kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsStyleMenu != nullptr );
	menuBar->AppendMenu(itsStyleMenu);

	// listen for TypeChanged

	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextEditor::~JXTextEditor()
{
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
JXTextEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFontMenu && message.Is(JXFontNameMenu::kNameNeedsUpdate))
		{
		itsFontMenu->SetFontName(GetCurrentFont().GetName());
		}
	else if (sender == itsFontMenu && message.Is(JXFontNameMenu::kNameChanged))
		{
		const JString fontName = itsFontMenu->GetFontName();
		SetCurrentFontName(fontName);
		}

	else if (sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeNeedsUpdate))
		{
		itsSizeMenu->SetFontSize(GetCurrentFont().GetSize());
		}
	else if (sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged))
		{
		SetCurrentFontSize(itsSizeMenu->GetFontSize());
		}

	else
		{
		if (sender == this && message.Is(JTextEditor::kTypeChanged))
			{
			AdjustMenuBar();
			}

		JXTEBase::Receive(sender, message);
		}
}

/******************************************************************************
 AdjustMenuBar (private)

 ******************************************************************************/

void
JXTextEditor::AdjustMenuBar()
{
	if (GetType() == kFullEditor)
		{
		JXTextMenu* searchMenu;
		const JBoolean ok = GetSearchReplaceMenu(&searchMenu);
		assert( ok );

		itsMenuBar->InsertMenuAfter(searchMenu,  itsFontMenu);
		itsMenuBar->InsertMenuAfter(itsFontMenu, itsSizeMenu);
		itsMenuBar->InsertMenuAfter(itsSizeMenu, itsStyleMenu);
		}
	else
		{
		itsMenuBar->RemoveMenu(itsFontMenu);
		itsMenuBar->RemoveMenu(itsSizeMenu);
		itsMenuBar->RemoveMenu(itsStyleMenu);
		}
}
