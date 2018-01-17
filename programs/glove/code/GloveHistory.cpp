/******************************************************************************
 GloveHistory.cpp

	Implements a source code editor, built on top of JTextEditor.

	BASE CLASS = JXTEBase

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GloveHistory.h>
#include <GLGlobals.h>

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXXFontMenu.h>
#include <JXFontSizeMenu.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXToolBar.h>

#include <JString.h>
#include <JFontManager.h>
#include <JFontStyle.h>
#include <JUserNotification.h>
#include <JRegex.h>
#include <JPagePrinter.h>

#include <jASCIIConstants.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jGlobals.h>
#include <jTime.h>

#include <jFStreamUtil.h>
#include <stdlib.h>
#include <stdio.h>
#include <jAssert.h>

const JSize kDefTabCharCount = 4;

const JCoordinate kHeaderStartX	= 30;		

// Font menu

static const JCharacter* kFontMenuTitleStr = "Font";
static const JCharacter* kFontRegex        = "^[0-9]x[0-9]*$";
static const JCharacter* kDefaultFontName  = "6x13";

// Font size menu (Courier is used if no other fonts are available.)

static const JCharacter* kSizeMenuTitleStr = "Size";

// JBroadcaster message types

const JCharacter* GloveHistory::kFontWillChange = "GloveHistory::FontWillChange";

/******************************************************************************
 Constructor

 ******************************************************************************/

GloveHistory::GloveHistory
	(
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
	JXTEBase(kFullEditor, kJFalse, kJFalse, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	WantInput(kJTrue, kJFalse);

	(scrollbarSet->GetVScrollbar())->SetScrollDelay(0);

	itsTabCharCount     = kDefTabCharCount;

	// adjust the edit menu

	AppendEditMenu(menuBar);
	AppendSearchReplaceMenu(menuBar);

	// add the font or size menu

	JRegex regex(kFontRegex);
	if (JXXFontMenu::Create(regex, CompareFontNames, kFontMenuTitleStr, menuBar,
							kFixedLeft, kFixedTop, 0,0, 10,10, &itsFontMenu))
		{
		itsSizeMenu = NULL;
		itsFontMenu->SetFontName(kDefaultFontName);
		SetDefaultFont(GetFontManager()->GetFont(kDefaultFontName));
		menuBar->AppendMenu(itsFontMenu);
		ListenTo(itsFontMenu);
		}
	else
		{
		itsFontMenu = NULL;
		SetDefaultFont(GetFontManager()->GetDefaultMonospaceFont());

		itsSizeMenu = jnew JXFontSizeMenu(JGetMonospaceFontName(), kSizeMenuTitleStr, menuBar,
										 kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsSizeMenu != NULL );
		menuBar->AppendMenu(itsSizeMenu);
		itsSizeMenu->SetFontSize(JGetDefaultFontSize());
		ListenTo(itsSizeMenu);
		}

	AdjustTabWidth();

	SetPTPrinter(GLGetPTPrinter());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GloveHistory::~GloveHistory()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GloveHistory::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if ((sender == itsFontMenu && message.Is(JXXFontMenu::kNameChanged)) ||
		(sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged)))
		{
		AdjustFont();
		}

	else
		{
		JXTEBase::Receive(sender, message);
		}
}

/******************************************************************************
 AdjustFont (private)

 ******************************************************************************/

void
GloveHistory::AdjustFont()
{
	Broadcast(FontWillChange());	// notify others so save status won't change

	// save selection or caret location

	JIndex selStart, selEnd, caretIndex;
	const JBoolean hasSelection = HasSelection();
	JBoolean ok;
	if (hasSelection)
		{
		ok = GetSelection(&selStart, &selEnd);
		}
	else
		{
		ok = GetCaretLocation(&caretIndex);
		}
	assert( ok );

	// set font or size for entire text

	SelectAll();

	if (itsFontMenu != NULL)
		{
		JFont font = GetFontManager()->GetFont(itsFontMenu->GetFontName());
		SetCurrentFont(font);
		SetDefaultFont(font);
		}
	else
		{
		assert( itsSizeMenu != NULL );
		const JSize size = itsSizeMenu->GetFontSize();
		SetCurrentFontSize(size);
		SetDefaultFontSize(size);
		}

	// restore selection or caret location

	if (hasSelection)
		{
		SetSelection(selStart, selEnd);
		}
	else
		{
		SetCaretLocation(caretIndex);
		}

	// update the tab width

	AdjustTabWidth();
}

/******************************************************************************
 AdjustTabWidth (private)

 ******************************************************************************/

void
GloveHistory::AdjustTabWidth()
{
	JCoordinate charWidth;

	const JFontManager* fontMgr = JTextEditor::TEGetFontManager();
	if (itsFontMenu != NULL)
		{
		const JFont font = fontMgr->GetFont(itsFontMenu->GetFontName());
		charWidth = font.GetCharWidth(' ');
		}
	else
		{
		assert( itsSizeMenu != NULL );
		const JFont font = fontMgr->GetFont(JGetMonospaceFontName(), itsSizeMenu->GetFontSize());
		charWidth = font.GetCharWidth(' ');
		}

	SetDefaultTabWidth(itsTabCharCount * charWidth);
}


/******************************************************************************
 CompareFontNames (static private)

	We sort by the first number, then by the second number: #x##

 ******************************************************************************/

JListT::CompareResult
GloveHistory::CompareFontNames
	(
	JString* const & s1,
	JString* const & s2
	)
{
	const JCharacter c1 = s1->GetFirstCharacter() - '0';
	const JCharacter c2 = s2->GetFirstCharacter() - '0';

	if (c1 > c2)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (c1 < c2)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		JFloat x1;
		const JString s11  = s1->GetSubstring(3, s1->GetLength());
		const JBoolean ok1 = s11.ConvertToFloat(&x1);

		JFloat x2;
		const JString s21  = s2->GetSubstring(3, s2->GetLength());
		const JBoolean ok2 = s21.ConvertToFloat(&x2);

		if (x1 > x2)
			{
			return JListT::kFirstGreaterSecond;
			}
		else if (x1 < x2)
			{
			return JListT::kFirstLessSecond;
			}
		else
			{
			return JListT::kFirstEqualSecond;
			}
		}
}

/******************************************************************************
 GetPrintHeaderHeight (protected)

 ******************************************************************************/

JCoordinate
GloveHistory::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight() * 2;
}

/******************************************************************************
 DrawPrintHeader (protected)

 ******************************************************************************/

void
GloveHistory::DrawPrintHeader
	(
	JPagePrinter& 		p, 
	const JCoordinate	headerHeight
	)
{
	JPoint topLeft;
	topLeft.y 	= (headerHeight - p.GetLineHeight())/2;
	topLeft.x 	= kHeaderStartX;
	p.String(topLeft, "Glove session");
}

/******************************************************************************
 LoadDefaultToolBar (public)

 ******************************************************************************/

void
GloveHistory::LoadDefaultToolBar
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
