/******************************************************************************
 JXTextEditor.cpp

	Implements a complete styled text editor, built on top of JTextEditor.

	BASE CLASS = JXTEBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JXTextEditor.h"
#include "JXWindow.h"
#include "JXMenuBar.h"
#include "JXFontNameMenu.h"
#include "JXFontSizeMenu.h"
#include "JXTEStyleMenu.h"
#include "JXScrollbarSet.h"
#include "JXScrollbar.h"
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextEditor::JXTextEditor
	(
	JStyledText*		text,
	const bool			ownsText,
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
	JXTEBase(kFullEditor, text, ownsText, false, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h),
	itsFontMenu(nullptr),
	itsSizeMenu(nullptr),
	itsStyleMenu(nullptr)
{
	WantInput(true, true);
	ShouldAllowDragAndDrop(true);
	GetText()->GetUndoRedoChain()->SetLastSaveLocation();

	scrollbarSet->GetVScrollbar()->SetScrollDelay(0);

	// add the menus

	itsMenuBar = menuBar;

	if (itsMenuBar != nullptr)
	{
		AppendEditMenu(menuBar, true, true, true, true, true, true, true, true);
		AppendSearchReplaceMenu(menuBar);

		itsFontMenu = jnew JXFontNameMenu(JGetString("FontMenuTitle::JXGlobal"), true, menuBar,
										  kFixedLeft, kFixedTop, 0,0, 10,10);
		menuBar->AppendMenu(itsFontMenu);
		ListenTo(itsFontMenu);

		itsSizeMenu = jnew JXFontSizeMenu(itsFontMenu, JGetString("SizeMenuTitle::JXGlobal"), menuBar,
										  kFixedLeft, kFixedTop, 0,0, 10,10);
		menuBar->AppendMenu(itsSizeMenu);
		ListenTo(itsSizeMenu);

		itsStyleMenu = jnew JXTEStyleMenu(this, menuBar,
										  kFixedLeft, kFixedTop, 0,0, 10,10);
		menuBar->AppendMenu(itsStyleMenu);
	}

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
 ShareMenus

 ******************************************************************************/

void
JXTextEditor::ShareMenus
	(
	JXTextEditor* te
	)
{
	ShareEditMenu(te, true, true, true, true);
	ShareSearchMenu(te);

	itsMenuBar = te->itsMenuBar;

	itsFontMenu = te->itsFontMenu;
	ListenTo(itsFontMenu);

	itsSizeMenu = te->itsSizeMenu;
	ListenTo(itsSizeMenu);

	itsStyleMenu = te->itsStyleMenu;
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXTextEditor::HandleFocusEvent()
{
	if (itsStyleMenu != nullptr)
	{
		AdjustMenuBar();
		itsStyleMenu->SetTE(this);
	}

	JXTEBase::HandleFocusEvent();
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
		if (HasFocus())
		{
			itsFontMenu->SetFontName(GetCurrentFont().GetName());
		}
	}
	else if (sender == itsFontMenu && message.Is(JXFontNameMenu::kNameChanged))
	{
		if (HasFocus())
		{
			const JString fontName = itsFontMenu->GetFontName();
			SetCurrentFontName(fontName);
		}
	}

	else if (sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeNeedsUpdate))
	{
		if (HasFocus())
		{
			itsSizeMenu->SetFontSize(GetCurrentFont().GetSize());
		}
	}
	else if (sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged))
	{
		if (HasFocus())
		{
			SetCurrentFontSize(itsSizeMenu->GetFontSize());
		}
	}

	else
	{
		if (sender == this && HasFocus() && message.Is(JTextEditor::kTypeChanged))
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
	if (itsMenuBar == nullptr)
	{
		return;
	}

	const bool active = GetType() == kFullEditor;

	itsFontMenu->SetActive(active);
	itsSizeMenu->SetActive(active);
	itsStyleMenu->SetActive(active);
}
