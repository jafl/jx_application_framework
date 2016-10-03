/******************************************************************************
 GNBNoteEditor.cc

	Implements a source code editor, built on top of JTextEditor.

	BASE CLASS = JXTextEditor

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GNBNoteEditor.h>
#include <GNBGlobals.h>
#include <GNBPrefsMgr.h>
#include "GNBNoteDir.h"

#include "editcopy.xpm"
#include "editcut.xpm"
#include "editpaste.xpm"

#include <JXToolBar.h>

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXXFontMenu.h>
#include <JXFontSizeMenu.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXTEStyleMenu.h>
#include <JXImage.h>
#include <JXPTPrinter.h>

#include <JString.h>
#include <JFontManager.h>
#include <JFontStyle.h>
#include <JUserNotification.h>
#include <JRegex.h>

#include <jASCIIConstants.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jGlobals.h>
#include <jTime.h>

#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <jAssert.h>

const JSize kDefTabCharCount    = 4;
const JSize kIrrelevantFontSize = kJDefaultFontSize;

// Default print settings

static const JCharacter* kDefPrintCmd = "lpr";
const JSize kDefLinesPerPage          = 60;
const JSize kHeaderLineCount          = 4;

// Font menu

static const JCharacter* kFontMenuTitleStr = "Font";
static const JCharacter* kFontRegex        = "^[0-9]x[0-9]*$";
static const JCharacter* kDefaultFontName  = "6x13";

// Font size menu (Courier is used if no other fonts are available.)

static const JCharacter* kSizeMenuTitleStr = "Size";

// JBroadcaster message types

const JCharacter* GNBNoteEditor::kFontWillChange = "GNBNoteEditor::FontWillChange";

/******************************************************************************
 Constructor

 ******************************************************************************/

GNBNoteEditor::GNBNoteEditor
	(
	GNBNoteDir* 		dir,
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
	JXTextEditor(menuBar, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w,h),
	itsDir(dir)
{
/*	WantInput(kJTrue, kJTrue);

	(scrollbarSet->GetVScrollbar())->SetScrollDelay(0);

	// adjust the edit menu

	AppendEditMenu(menuBar, kJFalse, kJFalse, kJFalse, kJFalse);
	JXTextMenu* editMenu;
	GetEditMenu(&editMenu);

	JIndex mIndex;
	JBoolean ok = EditMenuCmdToIndex(kCutCmd, &mIndex);
	assert(ok);

	JXImage* image = new JXImage(GetDisplay(), GetColormap(), JXPM(editcut));
	assert(image != NULL);
	editMenu->SetItemImage(mIndex, image, kJTrue);

	ok = EditMenuCmdToIndex(kCopyCmd, &mIndex);
	assert(ok);

	image = new JXImage(GetDisplay(), GetColormap(), JXPM(editcopy));
	assert(image != NULL);
	editMenu->SetItemImage(mIndex, image, kJTrue);

	ok = EditMenuCmdToIndex(kPasteCmd, &mIndex);
	assert(ok);

	image = new JXImage(GetDisplay(), GetColormap(), JXPM(editpaste));
	assert(image != NULL);
	editMenu->SetItemImage(mIndex, image, kJTrue);

	AppendSearchReplaceMenu(menuBar);

	// add the font or size menu

	JXTEStyleMenu* styleMenu	=
		JXTEStyleMenu::Create(this, menuBar, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert(styleMenu != NULL);
	menuBar->AppendMenu(styleMenu);

	ShouldAllowDragAndDrop(kJTrue);

	JString fontname;
	JSize	fontsize;
	GNBGetPrefsMgr()->GetDefaultFont(&fontname, &fontsize);
	SetDefaultFont(fontname, fontsize);
*/

	ListenTo(this);		// for change in line number


	SetPTPrinter(GNBGetPTPrinter());
	ListenTo(GNBGetPTPrinter());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GNBNoteEditor::~GNBNoteEditor()
{
}

/******************************************************************************
 LoadDefaultToolBar (public)

 ******************************************************************************/

void
GNBNoteEditor::LoadDefaultToolBar
	(
	JXToolBar* 		toolBar
	)
{
	JXTextMenu* editMenu;
	GetEditMenu(&editMenu);

	JIndex mIndex;
	JBoolean ok = EditMenuCmdToIndex(kCutCmd, &mIndex);
	assert(ok);

	toolBar->NewGroup();
	toolBar->AppendButton(editMenu, mIndex);

	ok = EditMenuCmdToIndex(kCopyCmd, &mIndex);
	assert(ok);

	toolBar->AppendButton(editMenu, mIndex);

	ok = EditMenuCmdToIndex(kPasteCmd, &mIndex);
	assert(ok);

	toolBar->AppendButton(editMenu, mIndex);
}

/******************************************************************************
 Print

 ******************************************************************************/

void
GNBNoteEditor::Print()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GNBNoteEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GNBGetPTPrinter() && message.Is(JXPTPrinter::kPrintSetupFinished))
		{
		JXTextEditor::Receive(sender, message);
		itsDir->PrintFinished();
		}
	else
		{
		JXTextEditor::Receive(sender, message);
		}
}
