/******************************************************************************
 GMessageEditor.cc

	<Description>

	BASE CLASS = public JXTEBase

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "GMessageEditor.h"
#include "GMessageEditDir.h"
#include "GPrefsMgr.h"
#include "GMGlobals.h"

#include <JXWindowDirector.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXFontManager.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXSpellChecker.h>
#include <jXUtil.h>

#include <JFontManager.h>
#include <JFontStyle.h>
#include <jAssert.h>

// Edit menu

static const JCharacter* kPasteQuotedStr   = "Paste quoted";
static const JCharacter* kQuoteSelectedStr = "Quote selected";

static const JCharacter* kPasteQuotedID    = "PasteQuoted::Arrow";
static const JCharacter* kQuoteSelectedID  = "QuoteSelected::Arrow";

enum
{
	kPasteQuotedCmd = 0,
	kQuoteSelectedCmd
};

/******************************************************************************
 Constructor

 *****************************************************************************/

GMessageEditor::GMessageEditor
	(
	GMessageEditDir*	dir,
	JXMenuBar*			menuBar,
	JXScrollbarSet*	scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor, kJFalse, kJFalse, scrollbarSet, enclosure,
				hSizing, vSizing, x, y, w, h),
	itsDir(dir),
	itsUsingBaseClassDND(kJTrue)
{
	WantInput(kJTrue, kJTrue);
	ShouldAllowDragAndDrop(kJTrue);
	UseMultipleUndo();
	(scrollbarSet->GetVScrollbar())->SetScrollDelay(0);

	SetPTPrinter(GMGetPTPrinter());
	AppendEditMenu(menuBar, kJTrue, kJTrue, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse);
	JXTextMenu* editMenu;
	GetEditMenu(&editMenu);
	const JSize editCount = editMenu->GetItemCount();
	itsStartEditCmd = editCount + 1;
	editMenu->ShowSeparatorAfter(editCount);
	editMenu->AppendItem(kPasteQuotedStr, JXMenu::kPlainType, "", "Meta-Shift-V", kPasteQuotedID);
	editMenu->AppendItem(kQuoteSelectedStr, JXMenu::kPlainType, NULL, NULL, kQuoteSelectedID);

	AppendSearchReplaceMenu(menuBar);

	SetDefaultFont(GGetPrefsMgr()->GetDefaultMonoFont(), GGetPrefsMgr()->GetDefaultFontSize());
	JSize tabChars = GGetPrefsMgr()->GetTabCharWidth();
	SetCRMTabCharCount(tabChars);
	JCoordinate charWidth =
		TEGetFontManager()->GetCharWidth(GGetPrefsMgr()->GetDefaultMonoFont(),
			GGetPrefsMgr()->GetDefaultFontSize(), JFontStyle(), ' ');
	SetDefaultTabWidth(charWidth * tabChars);

	CRMRuleList* rules = new CRMRuleList();
	assert(rules != NULL);
	JTextEditor::CRMRule rule("[[:space:]]*(>+[[:space:]]*)+", "[[:space:]]*(>+[[:space:]]*)+", "$0");
	rules->AppendElement(rule);
	SetCRMRuleList(rules, kJTrue);

	ListenTo(GGetPrefsMgr());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMessageEditor::~GMessageEditor()
{
}

/******************************************************************************
 CRMIsPrefixChar

 *****************************************************************************/

JBoolean
GMessageEditor::CRMIsPrefixChar
	(
	const JCharacter c
	)
	const
{
	return JConvertToBoolean( c == ' ' || c == '\t' || c == '>' );
}

/******************************************************************************
 GetPasteText

 *****************************************************************************/

JBoolean
GMessageEditor::GetPasteText
	(
	JString* text
	)
{
	JRunArray<JFont> style;
	if (TEOwnsClipboard())
		{
		GetClipboard(text);
		return kJTrue;
		}
	else if (TEGetExternalClipboard(text, &style))
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GMessageEditor::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == GGetPrefsMgr() && message.Is(GPrefsMgr::kFontChanged))
		{
		JString fontName	= GGetPrefsMgr()->GetDefaultMonoFont();
		JSize fontSize		= GGetPrefsMgr()->GetDefaultFontSize();
		JBoolean changed =
			JConvertToBoolean(
				(fontName != GetDefaultFontName()) ||
				(fontSize != GetDefaultFontSize()));
		if (changed)
			{
			const JFontManager* fontMgr = GetFontManager();
			const JFloat h1 = fontMgr->GetLineHeight(GetDefaultFontName(),
													 GetDefaultFontSize(), JFontStyle());
			const JFloat h2 = fontMgr->GetLineHeight(fontName, fontSize, JFontStyle());
			JFloat vScrollScale    = h2 / h1;

			JXScrollbar *hScrollbar, *vScrollbar;
			const JBoolean ok = GetScrollbars(&hScrollbar, &vScrollbar);
			assert( ok );
			vScrollbar->PrepareForScaledMaxValue(vScrollScale);

			JSize tabCharCount = GGetPrefsMgr()->GetTabCharWidth();

			SetAllFontNameAndSize(fontName, fontSize, tabCharCount, kJFalse, kJFalse);
			}
		}
	else
		{
		JXTextMenu* editMenu;
		GetEditMenu(&editMenu);
		if (sender == editMenu && message.Is(JXMenu::kNeedsUpdate))
			{
			UpdateEditMenu();
			}
		else if (sender == editMenu && message.Is(JXMenu::kItemSelected))
			{
			 const JXMenu::ItemSelected* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != NULL );
			HandleEditMenu(selection->GetIndex());
			}

		JXTEBase::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateMessageMenu

 ******************************************************************************/

void
GMessageEditor::UpdateEditMenu()
{
	JXTextMenu* editMenu;
	GetEditMenu(&editMenu);

	if (HasFocus())
		{
		editMenu->EnableItem(kPasteQuotedCmd + itsStartEditCmd);
		if (HasSelection())
			{
			editMenu->EnableItem(kQuoteSelectedCmd + itsStartEditCmd);
			}
		}
}

/******************************************************************************
 HandleEditMenu

 ******************************************************************************/

void
GMessageEditor::HandleEditMenu
	(
	const JIndex mindex
	)
{
	JIndex index = mindex - itsStartEditCmd;
	if (index == kPasteQuotedCmd)
		{
		JString text;
		if (GetPasteText(&text))
			{
			GQuoteString(&text, kJTrue);
			Paste(text);
			}
		JIndex startIndex;
		GetCaretLocation(&startIndex);

		SelectLine(GetLineForChar(startIndex));
		TEScrollToSelection(kJFalse);
		SetCaretLocation(startIndex);
		}
	else if (index == kQuoteSelectedCmd)
		{
		JIndex start;
		JIndex end;
		if (GetSelection(&start, &end))
			{
			JIndex lineStart	= GetLineForChar(start);
			JIndex lineEnd		= GetLineForChar(end);
			SetSelection(GetLineStart(lineStart), GetLineEnd(lineEnd));
			}
		JString text;
		if (GetSelection(&text))
			{
			GQuoteString(&text, kJTrue);
			Paste(text);
			}
		}
}

/******************************************************************************
 GetPlainTextTabWidth (virtual protected)

	Derived classes must override this function in order to print tabs
	correctly from PrintPlainText().  They should return the number of
	spaces that the tab should be converted to.

 ******************************************************************************/

JSize
GMessageEditor::GetPlainTextTabWidth
	(
	const JIndex charIndex,
	const JIndex textColumn
	)
	const
{
	return 8 - (textColumn % 8);
}

/******************************************************************************
 IsCharacterInWord (virtual protected)

	Returns kJTrue if the given character should be considered part of
	a word.  The default definition is [A-Za-z0-9].

 ******************************************************************************/

JBoolean
GMessageEditor::IsCharacterInWord
	(
	const JString&	text,
	const JIndex	charIndex
	)
	const
{
	if (JXTEBase::IsCharacterInWord(text, charIndex))
		{
		return kJTrue;
		}
	return JConvertToBoolean(text.GetCharacter(charIndex) == '\'');
}

/******************************************************************************
 WillAcceptDrop

 ******************************************************************************/

JBoolean
GMessageEditor::WillAcceptDrop
	(
	const JArray<Atom>& typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	itsUsingBaseClassDND	= kJFalse;
	JXSelectionManager* selMgr	= GetSelectionManager();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i = 1; i <= typeCount; i++)
		{
		Atom type = typeList.GetElement(i);
		if (type == selMgr->GetURLXAtom())
			{
			return kJTrue;
			}
		}
	itsUsingBaseClassDND	= kJTrue;
	return JXTEBase::WillAcceptDrop(typeList, action, pt, time, source);
}

/******************************************************************************
 HandleDNDEnter

 ******************************************************************************/

void
GMessageEditor::HandleDNDEnter()
{
	if (itsUsingBaseClassDND)
		{
		JXTEBase::HandleDNDEnter();
		}
}

/******************************************************************************
 HandleDNDHere

 ******************************************************************************/

void
GMessageEditor::HandleDNDHere
	(
	const JPoint& pt,
	const JXWidget* source
	)
{
	if (itsUsingBaseClassDND)
		{
		JXTEBase::HandleDNDHere(pt, source);
		}
}

/******************************************************************************
 HandleDNDLeave

 ******************************************************************************/

void
GMessageEditor::HandleDNDLeave()
{
	if (itsUsingBaseClassDND)
		{
		JXTEBase::HandleDNDLeave();
		}
}

/******************************************************************************
 HandleDNDDrop

 ******************************************************************************/

void
GMessageEditor::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>& typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*	source
	)
{
	if (itsUsingBaseClassDND)
		{
		JXTEBase::HandleDNDDrop(pt, typeList, action, time, source);
		itsUsingBaseClassDND	= kJFalse;
		return;
		}
	JBoolean dropOnSelf = kJTrue;
	if (source != this)
		{
		dropOnSelf = kJFalse;
		}

	HandleDNDLeave();

	unsigned char* data = NULL;
	JSize dataLength;
	Atom returnType;
	JXSelectionManager* selManager = GetSelectionManager();
	JXSelectionManager::DeleteMethod delMethod;
	const Atom dndName = GetDNDManager()->GetDNDSelectionName();
	if (selManager->GetData(dndName, time, selManager->GetURLXAtom(),
									 &returnType, &data, &dataLength, &delMethod))
		{
		JPtrArray<JString> fileNameList(JPtrArrayT::kForgetAll), urlList(JPtrArrayT::kForgetAll);
		JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);
		const JSize count	= fileNameList.GetElementCount();
		for (JIndex i = 1; i <= count; i++)
			{
			itsDir->FileDropped(*(fileNameList.NthElement(i)), JI2B(source != NULL));
			}
		selManager->DeleteData(&data, delMethod);
		Focus();
		}
}
