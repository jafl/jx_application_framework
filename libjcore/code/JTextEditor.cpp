/******************************************************************************
 JTextEditor.cpp

	Class to edit styled text.  This class only manages the view (layout
	and caret location / selection) and controller (pointer and key
	actions).  The underlying JStyledText maintains the model (data
	and styles).

	The only functions that are shadowed from JStyledText are those
	that perform operations based on the view, e.g, modify the currently
	selected text / styles.  Modifications can always safely be done
	directly on the JStyledText, assuming you have the required data.

	CaretLineChanged is broadcast by default.  If anybody requests
	CaretLocationChanged, CaretLineChanged will no longer be broadcast,
	since it duplicates the information.  Generic classes should accept
	either message to avoid depending on the setting.

	When the text ends with a newline, we have to draw the caret on the
	next line.  This is a special case because (charIndex == bufLength+1)
	would normally catch this.  We handle this special case internally
	rather than appending an extra item to itsLineStarts because this keeps
	the issue internal rather than forcing clients to deal with it.

	Derived classes must implement the following functions:

		TERefresh
			Put an update event in the queue to redraw the text.

		TERefreshRect
			Put an update event in the queue to redraw part of the text.

		TEUpdateDisplay
			Redraw whatever was requested by TERefresh() and TERefreshRect().
			(This is not pure virtual because some systems automatically
			 update the window after each event.)

		TERedraw
			Redraw the text immediately.

		TESetGUIBounds
			Set the bounds of the GUI object to match our size.  The original
			height and vertical position of the change can be used to update
			scrolltabs.  If change position is negative, don't update the
			scrolltabs.

		TEWidthIsBeyondDisplayCapacity
			Returns kJTrue if the given bounds width is too wide for the
			current graphics system to handle.  If so, breakCROnly is
			turned off.

		TEScrollToRect
			Scroll the text to make the given rectangle visible, possibly
			centering it in the middle of the display.  Return kJTrue
			if scrolling was necessary.

		TEScrollForDrag
			Scroll the text to make the given point visible.  Return kJTrue
			if scrolling was necessary.

		TEScrollForDND
			Same as TEScrollForDrag(), but separate, since some frameworks
			do the scrolling automatically.

		TESetVertScrollStep
			Set the vertical step size and page context used when scrolling
			the text.

		TECaretShouldBlink
			If blink is kJTrue, reset the timer and make the caret blink by
			calling TEShow/HideCaret().  Otherwise, call TEHideCaret().
			The derived class constructor must call TECaretShouldBlink(kJTrue)
			because it is pure virtual for us.

		TEUpdateClipboard
			Update the system clipboard with the given text/style.

		TEGetClipboard
			Returns kJTrue if there is something pasteable on the system
			clipboard.

		TEBeginDND
			Returns kJTrue if it is able to start a Drag-And-Drop session
			with the OS.  From then on, the derived class should call the
			TEHandleDND*() functions.  It must also call TEDNDFinished()
			when the drag ends.  If TEBeginDND() returns kJFalse, then we
			will manage an internal DND session.  In this case, the derived
			class should continue to call the usual TEHandleMouse*()
			functions.

		TEPasteDropData
			Get the data that was dropped and use Paste(text,style)
			to insert it.  (Before this is called, the insertion point
			is set so that Paste() will work correctly.)

		TEHasSearchText
			Return kJTrue if the user has entered any text for which to search.

	To draw page headers and footers while printing, override the
	following routines:

		GetPrintHeaderHeight
			Return the height required for the page header.

		DrawPrintHeader
			Draw the page header.  JTable will lock the header afterwards.

		GetPrintFooterHeight
			Return the height required for the page footer.

		DrawPrintFooter
			Draw the page footer.  JTable will lock the footer afterwards.

	The default implementation of tabs rounds the location up to the nearest
	multiple of itsDefTabWidth.  To implement non-uniform tabs or tabs on a
	line-by-line basis, override the following function:

		GetTabWidth
			Given the index of the tab character (charIndex) and the horizontal
			position on the line (in pixels) where the tab character starts (x),
			return the width of the tab character.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JTextEditor.h>
#include <JTEKeyHandler.h>
#include <JPagePrinter.h>
#include <JFontManager.h>
#include <JListUtil.h>
#include <JRunArrayIterator.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <JLatentPG.h>
#include <JMinMax.h>
#include <jTextUtil.h>
#include <jASCIIConstants.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jMouseUtil.h>
#include <jFileUtil.h>
#include <jTime.h>
#include <jGlobals.h>
#include <jAssert.h>

typedef JStyledText::TextIndex TextIndex;
typedef JStyledText::TextCount TextCount;
typedef JStyledText::TextRange TextRange;

const JCoordinate kDefLeftMarginWidth = 10;
const JCoordinate kRightMarginWidth   = 2;

JBoolean JTextEditor::theCopyWhenSelectFlag = kJFalse;

// JBroadcaster message types

const JUtf8Byte* JTextEditor::kTypeChanged          = "TypeChanged::JTextEditor";
const JUtf8Byte* JTextEditor::kCaretLineChanged     = "CaretLineChanged::JTextEditor";
const JUtf8Byte* JTextEditor::kCaretLocationChanged = "CaretLocationChanged::JTextEditor";

/******************************************************************************
 Constructor

	Colors must be from the same colorManager given to JStyledText.

	*** Derived classes must call RecalcAll().  We can't call it due to
	*** pure virtual functions.

 ******************************************************************************/

JTextEditor::JTextEditor
	(
	const Type			type,
	JStyledText*		text,
	const JBoolean		ownsText,
	const JFontManager*	fontManager,
	const JBoolean		breakCROnly,
	const JColorID		caretColor,
	const JColorID		selectionColor,
	const JColorID		outlineColor,
	const JColorID		wsColor,
	const JCoordinate	width
	)
	:
	itsType(type),
	itsText(text),
	itsOwnsTextFlag(ownsText),

	itsFontManager(fontManager),

	itsCaretColor(caretColor),
	itsSelectionColor(selectionColor),
	itsSelectionOutlineColor(outlineColor),
	itsWhitespaceColor(wsColor),

	itsKeyHandler(NULL),
	itsPrevTextLength(0),
	itsDragType(kInvalidDrag),
	itsPrevDragType(kInvalidDrag),
	itsIsDragSourceFlag(kJFalse)
{
	itsActiveFlag              = kJFalse;
	itsSelActiveFlag           = kJFalse;
	itsCaretVisibleFlag        = kJFalse;
	itsPerformDNDFlag          = kJFalse;
	itsAutoIndentFlag          = kJFalse;
	itsMoveToFrontOfTextFlag   = kJFalse;
	itsBcastLocChangedFlag     = kJFalse;
	itsBreakCROnlyFlag         = breakCROnly;
	itsIsPrintingFlag          = kJFalse;
	itsDrawWhitespaceFlag      = kJFalse;
	itsCaretMode               = kLineCaret;

	itsWidth           = width - kDefLeftMarginWidth - kRightMarginWidth;
	itsHeight          = 0;
	itsGUIWidth        = itsWidth;
	itsLeftMarginWidth = kDefLeftMarginWidth;
	itsDefTabWidth     = 36;	// pixesl => 1/2 inch
	itsMaxWordWidth    = 0;

	itsLineStarts = jnew JArray<TextIndex>;
	assert( itsLineStarts != NULL );
	itsLineStarts->SetCompareFunction(JStyledText::CompareCharacterIndices);
	itsLineStarts->SetSortOrder(JListT::kSortAscending);

	itsLineGeom = jnew JRunArray<LineGeometry>;
	assert( itsLineGeom != NULL );

	itsCaretLoc      = CaretLocation(TextIndex(1,1),1);
	itsCaretX        = 0;
	itsInsertionFont = itsText->CalcInsertionFont(TextIndex(1,1));

	if (type == kFullEditor)
		{
		itsText->SetBlockSizes(4096, 128);
		itsLineStarts->SetBlockSize(128);
		itsLineGeom->SetBlockSize(128);
		}

	SetKeyHandler(NULL);

	ListenTo(itsText);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTextEditor::~JTextEditor()
{
	if (itsOwnsTextFlag)
		{
		jdelete itsText;
		}

	jdelete itsLineStarts;
	jdelete itsLineGeom;
	jdelete itsKeyHandler;
}

/******************************************************************************
 SetType (protected)

	This is protected because most derived classes will not be written
	to expect the type to change.

	Broadcasts TypeChanged.

 ******************************************************************************/

void
JTextEditor::SetType
	(
	const Type type
	)
{
	itsType = type;
	TERefresh();
	Broadcast(TypeChanged(type));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JTextEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsText &&
		message.Is(JStyledText::kTextSet))
		{
		assert( !TEIsDragging() );

		if (itsText->IsEmpty())
			{
			itsInsertionFont = itsText->GetDefaultFont();
			}

		RecalcAll();
		SetCaretLocation(CaretLocation(TextIndex(1,1),1));
		}

	else if (sender == itsText &&
			 message.Is(JStyledText::kTextChanged))
		{
		const JStyledText::TextChanged* info =
			dynamic_cast<const JStyledText::TextChanged*>(&message);
		assert( info != NULL );
		const TextRange& r = info->GetRange();
		if (r.charRange.GetCount() == itsText->GetText().GetCharacterCount())
			{
			RecalcAll();
			}
		else
			{
			Recalc(r, info->GetRedrawRange(), info->IsDeletion());
			}
		}

	else if (sender == itsText &&
			 message.Is(JStyledText::kDefaultFontChanged))
		{
		if (itsText->IsEmpty())
			{
			itsInsertionFont = itsText->CalcInsertionFont(TextIndex(1,1));
			RecalcAll();
			}
		}

	// something else

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JTextEditor::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	assert( sender != itsText );
	JBroadcaster::ReceiveGoingAway(sender);
}

/******************************************************************************
 Search and replace

	We only support regular expressions because there is no advantage in
	optimizing for literal strings.

 ******************************************************************************/

/******************************************************************************
 SearchForward

	Look for the next match beyond the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JStringMatch
JTextEditor::SearchForward
	(
	const JRegex&	regex,
	const JBoolean	entireWord,
	const JBoolean	wrapSearch,
	JBoolean*		wrapped
	)
{
	const TextIndex i(
		itsSelection.IsEmpty() ? itsCaretLoc.location.charIndex :
		itsSelection.charRange.last + 1,

		itsSelection.IsEmpty() ? itsCaretLoc.location.byteIndex :
		itsSelection.byteRange.last + 1);

	const JStringMatch m =
		itsText->SearchForward(i, regex, entireWord, wrapSearch, wrapped);
	if (!m.IsEmpty())
		{
		SetSelection(m);
		}

	return m;
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JStringMatch
JTextEditor::SearchBackward
	(
	const JRegex&	regex,
	const JBoolean	entireWord,
	const JBoolean	wrapSearch,
	JBoolean*		wrapped
	)
{
	const TextIndex i(
		itsSelection.IsEmpty() ? itsCaretLoc.location.charIndex :
		itsSelection.charRange.first,

		itsSelection.IsEmpty() ? itsCaretLoc.location.byteIndex: 
		itsSelection.byteRange.first);

	const JStringMatch m =
		itsText->SearchBackward(i, regex, entireWord, wrapSearch, wrapped);
	if (!m.IsEmpty())
		{
		SetSelection(m);
		}

	return m;
}

/******************************************************************************
 SelectionMatches

	Returns kJTrue if the current selection matches the given search criteria.
	This returns the JStringMatch required for a replace.

 ******************************************************************************/

JStringMatch
JTextEditor::SelectionMatches
	(
	const JRegex&	regex,
	const JBoolean	entireWord
	)
{
	if (itsSelection.IsEmpty() ||
		(entireWord && !itsText->IsEntireWord(itsSelection)))
		{
		return JStringMatch(itsText->GetText());
		}

	// We cannot match only the selected text, because that will fail if
	// there are look-behind or look-ahead assertions.

	JStringIterator iter(itsText->GetText());
	iter.UnsafeMoveTo(kJIteratorStartBefore, itsSelection.charRange.first, itsSelection.byteRange.first);
	if (iter.Next(regex))
		{
		const JStringMatch& m = iter.GetLastMatch();
		if (m.GetCharacterRange() == itsSelection.charRange)
			{
			return m;
			}
		}

	return JStringMatch(itsText->GetText());
}

/******************************************************************************
 ReplaceSelection (protected)

	Replace the current selection with the given replace text.

	*** Something must be selected.
		We do not check that it matches the search string.

 ******************************************************************************/

void
JTextEditor::ReplaceSelection
	(
	const JStringMatch&	match,
	const JString&		replaceStr,
	const JBoolean		replaceIsRegex,
	const JBoolean		preserveCase
	)
{
	assert( HasSelection() );

	JIndex charIndex, byteIndex;
	if (!itsSelection.IsEmpty())
		{
		charIndex = itsSelection.charRange.first;
		byteIndex = itsSelection.byteRange.first;
		}
	else
		{
		charIndex = itsCaretLoc.location.charIndex;
		byteIndex = itsCaretLoc.location.byteIndex;
		}

	const TextCount count =
		itsText->ReplaceMatch(match, replaceStr, replaceIsRegex, itsInterpolator, preserveCase);

	if (count.charCount > 0)
		{
		SetSelection(TextRange(TextIndex(charIndex, byteIndex), count));
		}
}

/******************************************************************************
 ReplaceAll

	Replace every occurrence of the search pattern with the replace string.
	Returns kJTrue if it modified the text.

 ******************************************************************************/

JBoolean
JTextEditor::ReplaceAll
	(
	const JRegex&	regex,
	const JBoolean	entireWord,
	const JString&	replaceStr,
	const JBoolean	replaceIsRegex,
	const JBoolean	preserveCase,
	const JBoolean	restrictToSelection
	)
{
	if (restrictToSelection && !HasSelection())
		{
		return kJFalse;
		}

	TextRange r(
		!itsSelection.IsEmpty() ? itsSelection.charRange :
		JCharacterRange(1, itsText->GetText().GetCharacterCount()),

		!itsSelection.IsEmpty() ? itsSelection.byteRange :
		JUtf8ByteRange(1, itsText->GetText().GetByteCount()));

	r = itsText->ReplaceAllInRange(r, regex, entireWord,
									 replaceStr, replaceIsRegex,
									 itsInterpolator, preserveCase);

	if (restrictToSelection && !r.IsEmpty())
		{
		SetSelection(r);
		return kJTrue;
		}
	else if (!r.IsEmpty())
		{
		SetCaretLocation(TextIndex(
			itsText->GetText().GetCharacterCount()+1,
			itsText->GetText().GetByteCount()+1));
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SearchForward

	Look for the next match beyond the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor::SearchForward
	(
	const JStyledText::FontMatch&	match,
	const JBoolean						wrapSearch,
	JBoolean*							wrapped
	)
{
	const TextIndex i(
		itsSelection.IsEmpty() ? itsCaretLoc.location.charIndex :
		itsSelection.charRange.last + 1,

		itsSelection.IsEmpty() ? itsCaretLoc.location.byteIndex :
		itsSelection.byteRange.last + 1);

	TextRange range;
	const JBoolean found = itsText->SearchForward(match, i, wrapSearch, wrapped, &range);
	if (found)
		{
		SetSelection(range);
		}

	return found;
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor::SearchBackward
	(
	const JStyledText::FontMatch&	match,
	const JBoolean						wrapSearch,
	JBoolean*							wrapped
	)
{
	const TextIndex i(
		itsSelection.IsEmpty() ? itsCaretLoc.location.charIndex :
		itsSelection.charRange.first,

		itsSelection.IsEmpty() ? itsCaretLoc.location.byteIndex: 
		itsSelection.byteRange.first);

	TextRange range;
	const JBoolean found = itsText->SearchBackward(match, i, wrapSearch, wrapped, &range);
	if (found)
		{
		SetSelection(range);
		}

	return found;
}

/******************************************************************************
 PrivateSetBreakCROnly (private)

 ******************************************************************************/

void
JTextEditor::PrivateSetBreakCROnly
	(
	const JBoolean breakCROnly
	)
{
	itsBreakCROnlyFlag = breakCROnly;
	if (!itsBreakCROnlyFlag)
		{
		itsWidth = itsGUIWidth;
		}
}

/******************************************************************************
 TEGetMinPreferredGUIWidth

 ******************************************************************************/

JCoordinate
JTextEditor::TEGetMinPreferredGUIWidth()
	const
{
	if (itsBreakCROnlyFlag)
		{
		return itsLeftMarginWidth + itsWidth + kRightMarginWidth;
		}
	else
		{
		return itsLeftMarginWidth + itsMaxWordWidth + kRightMarginWidth;
		}
}

/******************************************************************************
 TESetBoundsWidth (protected)

	Call this to notify us of a change in aperture width.

 ******************************************************************************/

void
JTextEditor::TESetBoundsWidth
	(
	const JCoordinate width
	)
{
	itsGUIWidth = width - itsLeftMarginWidth - kRightMarginWidth;
	assert( itsGUIWidth > 0 );
	TEGUIWidthChanged();
}

/******************************************************************************
 TESetLeftMarginWidth

	For multi-line input areas it helps to have a wide left margin where
	one can click and drag down to select entire lines.

 ******************************************************************************/

void
JTextEditor::TESetLeftMarginWidth
	(
	const JCoordinate origWidth
	)
{
	const JCoordinate width = JMax((JCoordinate) kMinLeftMarginWidth, origWidth);

	if (width != itsLeftMarginWidth)
		{
		itsGUIWidth       += itsLeftMarginWidth;
		itsLeftMarginWidth = width;
		itsGUIWidth       -= itsLeftMarginWidth;
		TEGUIWidthChanged();
		}
}

/******************************************************************************
 TEGUIWidthChanged (private)

 ******************************************************************************/

void
JTextEditor::TEGUIWidthChanged()
{
	if (itsBreakCROnlyFlag)
		{
		TERefresh();
		}
	else if (itsWidth != itsGUIWidth)
		{
		itsWidth = itsGUIWidth;
		RecalcAll();
		}
}

/******************************************************************************
 SetAllFontNameAndSize (protected)

	This function is useful for unstyled text editors that allow the user
	to change the font and size.

	It preserves the styles, in case they are controlled by the program.
	(e.g. context sensitive hilighting)

	You can choose whether or not to throw out all Undo information.
	Unstyled text editors can usually preserve Undo, since they will not
	allow the user to modify styles.  (We explicitly ask for this because
	it is too easy to forget about the issue.)

 ******************************************************************************/

void
JTextEditor::SetAllFontNameAndSize
	(
	const JString&		name,
	const JSize			size,
	const JCoordinate	tabWidth,
	const JBoolean		breakCROnly,
	const JBoolean		clearUndo
	)
{
	itsDefTabWidth = tabWidth;
	PrivateSetBreakCROnly(breakCROnly);

	itsInsertionFont.Set(name, size, itsInsertionFont.GetStyle());

	itsText->SetAllFontNameAndSize(name, size, clearUndo);
}

/******************************************************************************
 Cut

 ******************************************************************************/

void
JTextEditor::Cut()
{
	if (!TEIsDragging())
		{
		Copy();
		DeleteSelection();
		itsText->DeactivateCurrentUndo();
		}
}

/******************************************************************************
 Copy

 ******************************************************************************/

void
JTextEditor::Copy()
{
	JString text;
	JRunArray<JFont> style;
	GetSelection(&text, &style);
	TEUpdateClipboard(text, style);
}

/******************************************************************************
 GetClipboard

	Returns the text and style that would be pasted if Paste() were called.

	style can be NULL.  If it is not NULL, it can come back empty even if
	the function returns kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor::GetClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	if (style != NULL)
		{
		return TEGetClipboard(text, style);
		}
	else
		{
		JRunArray<JFont> tempStyle;
		return TEGetClipboard(text, &tempStyle);
		}
}

/******************************************************************************
 Paste

 ******************************************************************************/

void
JTextEditor::Paste()
{
	JString text;
	JRunArray<JFont> style;
	if (TEGetClipboard(&text, &style))
		{
		JRunArray<JFont>* s = (style.IsEmpty() ? NULL : &style);
		Paste(text, s);
		}
}

/******************************************************************************
 Paste

	style can be NULL

 ******************************************************************************/

void
JTextEditor::Paste
	(
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	if (itsIsDragSourceFlag)
		{
		return;
		}

	TextRange range;

	const JBoolean hadSelection = HasSelection();
	if (hadSelection)
		{
		range = itsSelection;
		}
	else
		{
		range.charRange.SetFirstAndCount(itsCaretLoc.location.charIndex, 0);
		range.byteRange.SetFirstAndCount(itsCaretLoc.location.byteIndex, 0);
		}

	range = itsText->Paste(range, text, style);

	SetCaretLocation(range.GetAfter());
}

/******************************************************************************
 GetInsertionIndex (protected)

	Return the index where new text will be typed or pasted.

 ******************************************************************************/

TextIndex
JTextEditor::GetInsertionIndex()
	const
{
	if (!itsSelection.IsEmpty())
		{
		return itsSelection.GetFirst();
		}
	else
		{
		return itsCaretLoc.location;
		}
}

/******************************************************************************
 GetSelection

 ******************************************************************************/

JBoolean
JTextEditor::GetSelection
	(
	JString* text
	)
	const
{
	if (!itsSelection.IsEmpty())
		{
		text->Set(itsText->GetText().GetBytes(), itsSelection.byteRange);
		return kJTrue;
		}
	else
		{
		text->Clear();
		return kJFalse;
		}
}

JBoolean
JTextEditor::GetSelection
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	style->RemoveAll();

	if (GetSelection(text))
		{
		style->AppendSlice(itsText->GetStyles(), itsSelection.charRange);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetSelection (private)

 ******************************************************************************/

void
JTextEditor::SetSelection
	(
	const TextRange&	range,
	const JBoolean		needCaretBcast
	)
{
	if (itsIsDragSourceFlag)
		{
		return;
		}

	itsText->DeactivateCurrentUndo();
	itsPrevDragType = kInvalidDrag;		// avoid wordSel and lineSel pivots

	if (itsText->IsEmpty() || itsSelection.charRange == range.charRange)
		{
		return;
		}

	assert( !range.IsEmpty() );
	assert( itsText->GetText().CharacterIndexValid(range.charRange.first) );
	assert( itsText->GetText().CharacterIndexValid(range.charRange.last) );
	assert( itsText->GetText().ByteIndexValid(range.byteRange.first) );
	assert( itsText->GetText().ByteIndexValid(range.byteRange.last) );

	const JBoolean hadSelection            = !itsSelection.IsEmpty();
	const CaretLocation origCaretLoc       = itsCaretLoc;
	const JUtf8ByteRange origByteSelection = itsSelection.byteRange;

	itsCaretLoc  = CaretLocation();
	itsSelection = range;

	const JIndex newStartLine = GetLineForByte(itsSelection.byteRange.first);
	const JIndex newEndLine   = GetLineForByte(itsSelection.byteRange.last);

	if (needCaretBcast)
		{
		BroadcastCaretMessages(
			CaretLocation(itsSelection.GetFirst(), newStartLine),
			kJTrue);
		}

	TECaretShouldBlink(kJFalse);

	if (theCopyWhenSelectFlag && itsType != kStaticText)
		{
		Copy();
		}

	// We only optimize heavily for the case when one end of the
	// selection remains fixed because this is the case during mouse drags.

	if (hadSelection && origByteSelection.first == itsSelection.byteRange.first)
		{
		const JIndex origEndLine = GetLineForByte(origByteSelection.last);
		TERefreshLines(JMin(origEndLine, newEndLine),
					   JMax(origEndLine, newEndLine));
		}
	else if (hadSelection && origByteSelection.last == itsSelection.byteRange.last)
		{
		const JIndex origStartLine = GetLineForByte(origByteSelection.first);
		TERefreshLines(JMin(origStartLine, newStartLine),
					   JMax(origStartLine, newStartLine));
		}
	else if (hadSelection)
		{
		const JIndex origStartLine = GetLineForByte(origByteSelection.first);
		const JIndex origEndLine   = GetLineForByte(origByteSelection.last);
		TERefreshLines(origStartLine, origEndLine);
		TERefreshLines(newStartLine, newEndLine);
		}
	else
		{
		TERefreshCaret(origCaretLoc);
		TERefreshLines(newStartLine, newEndLine);
		}
}

/******************************************************************************
 DeleteSelection

 ******************************************************************************/

void
JTextEditor::DeleteSelection()
{
	if (!itsIsDragSourceFlag && !itsSelection.IsEmpty())
		{
		itsText->DeleteText(itsSelection);
		SetCaretLocation(itsSelection.GetFirst());
		}
}

/******************************************************************************
 TabSelectionLeft

	Remove tabs from the beginning of every selected line.  If nothing
	is selected, we select the line that the caret is on.

 ******************************************************************************/

void
JTextEditor::TabSelectionLeft
	(
	const JSize		tabCount,
	const JBoolean	force
	)
{
	if (TEIsDragging() || itsText->IsEmpty())
		{
		return;
		}

	TextRange r(
		itsText->GetParagraphStart(itsSelection.IsEmpty() ? itsCaretLoc.location : itsSelection.GetFirst()),
		itsSelection.IsEmpty() ? itsCaretLoc.location : itsSelection.GetAfter());

	r = itsText->Outdent(r, tabCount, force);

	if (!r.IsEmpty())
		{
		SetSelection(r);
		}
}

/******************************************************************************
 TabSelectionRight

	Insert a tab at the beginning of every selected line.  If nothing
	is selected, we select the line that the caret is on.

 ******************************************************************************/

void
JTextEditor::TabSelectionRight
	(
	const JSize tabCount
	)
{
	if (TEIsDragging() || itsText->IsEmpty())
		{
		return;
		}

	TextRange r(
		itsText->GetParagraphStart(itsSelection.IsEmpty() ? itsCaretLoc.location : itsSelection.GetFirst()),
		itsSelection.IsEmpty() ? itsCaretLoc.location : itsSelection.GetAfter());

	r = itsText->Indent(r, tabCount);

	SetSelection(r);
}

/******************************************************************************
 AnalyzeWhitespace

	Check all indent whitespace.

	If default is tabs, check for indent containing spaces.  Allow up to
	N-1 spaces at the end of the line, where N = tab width.

	If default is spaces, check for any indent containing tabs.

	If mixed indentation, show whitespace.  Set mode based on majority of
	indentation.

	Updates tabWidth value if it decides that spaces should be used for
	indentation.

 ******************************************************************************/

void
JTextEditor::AnalyzeWhitespace
	(
	JSize* tabWidth
	)
{
	JBoolean useSpaces, isMixed;
	*tabWidth = JAnalyzeWhitespace(itsText->GetText(), *tabWidth,
								   itsText->TabInsertsSpaces(),
								   &useSpaces, &isMixed);

	itsText->TabShouldInsertSpaces(useSpaces);
	ShouldShowWhitespace(isMixed);
}

/******************************************************************************
 CleanSelectedWhitespace

	Clean up the indentation whitespace and strip trailing whitespace in
	the selected range.

 ******************************************************************************/

inline void
JTextEditor::CleanSelectedWhitespace
	(
	const JBoolean align
	)
{
	TextRange r;
	if (GetSelection(&r))
		{
		r = itsText->CleanWhitespace(r, align);
		SetSelection(r);
		}
}

/******************************************************************************
 Paginate (protected)

	Returns breakpoints for cutting text into pages.  The first breakpoint
	is always zero, and the last breakpoint is the height of the text.
	Thus, it is easy to calculate the width of what is printed on each page
	from (breakpt->GetElement(i+1) - breakpt->GetElement(i) + 1).

 ******************************************************************************/

void
JTextEditor::Paginate
	(
	const JCoordinate		pageHeight,
	JArray<JCoordinate>*	breakpts
	)
	const
{
	assert( pageHeight > 0 );

	breakpts->RemoveAll();
	breakpts->AppendElement(0);

	const JSize count = GetLineCount();

	JRunArrayIterator<LineGeometry> iter(itsLineGeom);
	LineGeometry geom;

	JIndex prev = 1, i = 0;
	JCoordinate h = 0;
	do
		{
		// find the number of strips that will fit on this page

		while (i < count && h <= pageHeight)
			{
			i++;
			const JBoolean ok = iter.Next(&geom);
			assert( ok );
			h += geom.height;
			}

		JCoordinate pageH = h;
		if (h > pageHeight && i > prev)
			{
			// The last line didn't fit on the page,
			// so leave it for the next page.

			pageH -= geom.height;
			i--;
			const JBoolean ok = iter.Prev(&geom);
			assert( ok );
			h = 0;
			}
		else if (h > pageHeight)
			{
			// The line won't fit on any page.  Put
			// as much as possible on this page and leave
			// the rest for the next page.

			pageH = pageHeight;
			h    -= pageHeight;
			}
		else
			{
			// Everything fits on the page, so there is no residual.

			h = 0;
			}

		breakpts->AppendElement(breakpts->GetLastElement() + pageH);
		prev = i;
		}
		while (i < count || h > 0);
}

/******************************************************************************
 Print

	This function could be optimized to save the necessary state so
	TESetBoundsWidth() doesn't have to be called again at the end.

 ******************************************************************************/

void
JTextEditor::Print
	(
	JPagePrinter& p
	)
{
	// avoid notifing GUI from within Recalc()
	// (We put it here to allow OpenDocument() to adjust things.)

	itsIsPrintingFlag = kJTrue;

	if (!p.OpenDocument())
		{
		itsIsPrintingFlag = kJFalse;
		return;
		}

	const JCoordinate headerHeight = GetPrintHeaderHeight(p);
	const JCoordinate footerHeight = GetPrintFooterHeight(p);

	// We deactivate before printing so the selection or the
	// caret won't be printed.

	const JBoolean savedActive = TEIsActive();
	TEDeactivate();

	// adjust to the width of the page

	const JCoordinate savedWidth = TEGetBoundsWidth();
	const JCoordinate pageWidth  = p.GetPageWidth();
	TESetBoundsWidth(pageWidth);

	// paginate

	JArray<JCoordinate> breakpts;
	Paginate(p.GetPageHeight()-headerHeight-footerHeight, &breakpts);

	// print each page

	const JSize pageCount = breakpts.GetElementCount() - 1;

	JBoolean cancelled = kJFalse;
	for (JIndex i=1; i<=pageCount; i++)
		{
		if (!p.NewPage())
			{
			cancelled = kJTrue;
			break;
			}

		if (headerHeight > 0)
			{
			DrawPrintHeader(p, headerHeight);
			p.LockHeader(headerHeight);
			}
		if (footerHeight > 0)
			{
			DrawPrintFooter(p, footerHeight);
			p.LockFooter(footerHeight);
			}

		JPoint topLeft(0, breakpts.GetElement(i));
		JPoint bottomRight(pageWidth, breakpts.GetElement(i+1));
		JRect clipRect(JPoint(0,0), bottomRight - topLeft);
		p.SetClipRect(clipRect);

		p.ShiftOrigin(-topLeft);
		TEDraw(p, JRect(topLeft,bottomRight));
		}

	if (!cancelled)
		{
		p.CloseDocument();
		}

	itsIsPrintingFlag = kJFalse;

	// restore the original state and width

	if (savedActive)
		{
		TEActivate();
		}

	TESetBoundsWidth(savedWidth);
}

/******************************************************************************
 Print header and footer (virtual protected)

	Derived classes can override these functions if they want to
	print a header or a footer.

 ******************************************************************************/

JCoordinate
JTextEditor::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return 0;
}

JCoordinate
JTextEditor::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return 0;
}

void
JTextEditor::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
}

void
JTextEditor::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
}

/******************************************************************************
 GetCmdStatus (protected)

	Returns an array indicating which commands can be performed in the
	current state.

 ******************************************************************************/

JArray<JBoolean>
JTextEditor::GetCmdStatus
	(
	JString*	crmActionText,
	JString*	crm2ActionText,
	JBoolean*	isReadOnly
	)
	const
{
	JArray<JBoolean> flags(kCmdCount);
	for (JIndex i=1; i<=kCmdCount; i++)
		{
		flags.AppendElement(kJFalse);
		}

	*isReadOnly = kJTrue;

	if (itsType == kStaticText || !itsActiveFlag)
		{
		return flags;
		}

	// Edit menu

	if (itsSelection.IsEmpty())
		{
		*crmActionText  = JGetString("CRMCaretAction::JTextEditor");
		*crm2ActionText = JGetString("CRM2CaretAction::JTextEditor");
		}
	else
		{
		flags.SetElement(kCopyCmd, kJTrue);
		*crmActionText  = JGetString("CRMSelectionAction::JTextEditor");
		*crm2ActionText = JGetString("CRM2SelectionAction::JTextEditor");
		}

	flags.SetElement(kSelectAllCmd,      kJTrue);
	flags.SetElement(kToggleReadOnlyCmd, kJTrue);
	flags.SetElement(kShowWhitespaceCmd, kJTrue);

	if (itsType == kFullEditor)
		{
		*isReadOnly = kJFalse;

		flags.SetElement(kPasteCmd, kJTrue);
		if (!itsSelection.IsEmpty())
			{
			flags.SetElement(kCutCmd,       kJTrue);
			flags.SetElement(kDeleteSelCmd, kJTrue);
			}

		if (!itsText->IsEmpty())
			{
			flags.SetElement(kCheckSpellingCmd,      kJTrue);
			flags.SetElement(kCleanRightMarginCmd,   kJTrue);
			flags.SetElement(kCoerceRightMarginCmd,  kJTrue);
			flags.SetElement(kShiftSelLeftCmd,       kJTrue);
			flags.SetElement(kShiftSelRightCmd,      kJTrue);
			flags.SetElement(kForceShiftSelLeftCmd,  kJTrue);
			flags.SetElement(kCleanAllWhitespaceCmd, kJTrue);
			flags.SetElement(kCleanAllWSAlignCmd,    kJTrue);

			if (!itsSelection.IsEmpty())
				{
				flags.SetElement(kCheckSpellingSelCmd,   kJTrue);
				flags.SetElement(kCleanWhitespaceSelCmd, kJTrue);
				flags.SetElement(kCleanWSAlignSelCmd,    kJTrue);
				}
			}

		JBoolean canUndo, canRedo;
		if (itsText->HasMultipleUndo(&canUndo, &canRedo))
			{
			flags.SetElement(kUndoCmd, canUndo);
			flags.SetElement(kRedoCmd, canRedo);
			}
		else if (itsText->HasSingleUndo())
			{
			flags.SetElement(kUndoCmd, kJTrue);
			flags.SetElement(kRedoCmd, kJFalse);
			}
		}

	// Search & Replace menu

	flags.SetElement(kFindDialogCmd, kJTrue);

	if (!itsText->IsEmpty())
		{
		flags.SetElement(kFindClipboardBackwardCmd, kJTrue);
		flags.SetElement(kFindClipboardForwardCmd,  kJTrue);

		if (TEHasSearchText())
			{
			flags.SetElement(kFindNextCmd,     kJTrue);
			flags.SetElement(kFindPreviousCmd, kJTrue);

			if (itsType == kFullEditor)
				{
				flags.SetElement(kReplaceAllCmd,  kJTrue);

				if (HasSelection())
					{
					flags.SetElement(kReplaceSelectionCmd,      kJTrue);
					flags.SetElement(kReplaceFindNextCmd,       kJTrue);
					flags.SetElement(kReplaceFindPrevCmd,       kJTrue);
					flags.SetElement(kReplaceAllInSelectionCmd, kJTrue);
					}
				}
			}
		}

	if (HasSelection())
		{
		flags.SetElement(kEnterSearchTextCmd,       kJTrue);
		flags.SetElement(kEnterReplaceTextCmd,      kJTrue);
		flags.SetElement(kFindSelectionBackwardCmd, kJTrue);
		flags.SetElement(kFindSelectionForwardCmd,  kJTrue);
		}

	return flags;
}

/******************************************************************************
 TEDraw (protected)

	Draw everything that is visible in the given rectangle.

 ******************************************************************************/

void
JTextEditor::TEDraw
	(
	JPainter&		p,
	const JRect&	rect
	)
{
	p.ShiftOrigin(itsLeftMarginWidth, 0);

	// draw the text

	TEDrawText(p, rect);

	// if DND, draw drop location and object being dragged

	if (itsDragType == kDragAndDrop && itsDropLoc.location.charIndex > 0)
		{
		TEDrawCaret(p, itsDropLoc);
		}

	// otherwise, draw insertion caret

	else if (itsActiveFlag && itsCaretVisibleFlag && itsSelection.IsEmpty() &&
			 itsType == kFullEditor)
		{
		TEDrawCaret(p, itsCaretLoc);
		}

	// clean up

	p.ShiftOrigin(-itsLeftMarginWidth, 0);
}

/******************************************************************************
 TEDrawText (private)

	Draw the text that is visible in the given rectangle.

 ******************************************************************************/

void
JTextEditor::TEDrawText
	(
	JPainter&		p,
	const JRect&	rect
	)
{
	if (itsText->IsEmpty())
		{
		return;
		}

	const JSize lineCount = GetLineCount();

	JCoordinate h;
	const JIndex startLine = CalcLineIndex(rect.top, &h);

	// draw selection region

	if (itsActiveFlag && !itsSelection.IsEmpty() && itsType != kStaticText)
		{
		TEDrawSelection(p, rect, startLine, h);
		}

	// draw text, one line at a time

	JIndex runIndex, firstInRun;
	const JBoolean found = itsText->GetStyles().FindRun(GetLineStart(startLine).charIndex, &runIndex, &firstInRun);
	assert( found );

	JStringIterator* siter = itsText->GetConstIterator(kJIteratorStartBefore, GetLineStart(startLine));

	JRunArrayIterator<LineGeometry> giter(itsLineGeom, kJIteratorStartBefore, startLine);
	LineGeometry geom;
	for (JIndex i=startLine; i<=lineCount; i++)
		{
		const JBoolean ok = giter.Next(&geom);
		assert( ok );

		TEDrawLine(p, h, geom, siter, i, &runIndex, &firstInRun);

		h += geom.height;
		if (h >= rect.bottom)
			{
			break;
			}
		}

	itsText->DisposeConstIterator(siter);
}

/******************************************************************************
 TEDrawLine (private)

	Draw the text on the given line.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the line.

 ******************************************************************************/

inline void
teDrawSpaces
	(
	const JStyledText*	stb,
	const JIndex		startChar,
	const JInteger		direction,		// +1/-1
	const JIndex		trueRunEnd,
	JPainter&			p,
	const JCoordinate	left,
	const JCoordinate	ycenter,
	const JFont&		f,
	const JColorID		wsColor
	)
{
	const JUtf8Byte* text          = stb->GetText().GetBytes();
	const JRunArray<JFont>& styles = stb->GetStyles();

	JCoordinate l = left;
	JSize w       = f.GetCharWidth(p.GetFontManager(), ' ');

	p.SetLineWidth(1);
	p.SetPenColor(wsColor);

	JIndex i = startChar;
	JPoint pt;
	while (text[i-1] == ' ')
		{
		if ((direction == +1 && i > trueRunEnd) ||
			(direction == -1 && i < trueRunEnd))
			{
			JFont f = styles.GetElement(i);
			w       = f.GetCharWidth(p.GetFontManager(), ' ');
			}

		if (direction == -1)
			{
			l -= w;
			}
		pt.Set(l + w/2 - 1, ycenter);
		p.Point(pt);
		if (direction == +1)
			{
			l += w;
			}
		i += direction;
		}
}

void
JTextEditor::TEDrawLine
	(
	JPainter&			p,
	const JCoordinate	top,
	const LineGeometry&	geom,
	JStringIterator*	iter,
	const JIndex		lineIndex,
	JIndex*				runIndex,
	JIndex*				firstInRun
	)
{
	TEDrawInMargin(p, JPoint(-itsLeftMarginWidth, top), geom, lineIndex);

	JBoolean wsInit = kJFalse;
	JRect wsRect;
	JCoordinate wsYCenter = 0;
	if (itsDrawWhitespaceFlag)
		{
		wsRect.Set(top+3, 0, top+geom.height-4, 0);
		if (wsRect.height() < 3)
			{
			wsRect.top    -= (3 - wsRect.height())/2;
			wsRect.bottom += 3 - wsRect.height();
			}
		wsRect.bottom -= (wsRect.height() % 2);
		wsRect.right  += wsRect.height();

		wsYCenter = wsRect.ycenter();
		}

	const TextCount lineLength = GetLineLength(lineIndex);
	assert( lineLength.charCount > 0 );

	JIndex startChar = iter->GetNextCharacterIndex();
	JIndex endChar   = iter->GetPrevCharacterIndex() + lineLength.charCount;
	JCoordinate left = 0;
	JUtf8Character c;
	JBoolean lineEndsWithNewline = kJFalse;

	const JRunArray<JFont> styles = itsText->GetStyles();
	while (startChar <= endChar)
		{
		JSize runLength        = styles.GetRunLength(*runIndex);
		const JSize trueRunEnd = *firstInRun + runLength-1;

		runLength -= startChar - *firstInRun;
		if (startChar + runLength-1 > endChar)
			{
			runLength = endChar - startChar + 1;
			}

		const JFont& f = styles.GetRunDataRef(*runIndex);

		// If the line starts with spaces, we have to draw them.

		if (!wsInit && itsDrawWhitespaceFlag)
			{
			teDrawSpaces(itsText, startChar, +1, trueRunEnd,
						 p, left, wsYCenter, f, itsWhitespaceColor);
			}
		wsInit = kJTrue;

		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JCharacterRange cr;
		cr.SetFirstAndCount(startChar, runLength);

		iter->BeginMatch();

		JBoolean foundTab = kJFalse;
		while (iter->GetNextCharacterIndex() <= cr.last)
			{
			const JBoolean ok = iter->Next(&c);
			assert( ok );

			if (c == '\t')
				{
				iter->SkipPrev();
				foundTab = kJTrue;
				break;
				}
			}

		if (c == '\n')	// some fonts draw stuff for \n
			{
			iter->SkipPrev();
			endChar--;
			lineEndsWithNewline = kJTrue;
			}

		const JStringMatch& m = iter->FinishMatch();
		runLength             = m.GetCharacterCount();

		if (runLength > 0)
			{
			p.SetFont(f);
			JCoordinate ascent,descent;
			p.GetLineHeight(&ascent, &descent);

			const JString s = m.GetString();
			p.String(left, top + geom.ascent - ascent, s);

			// we only care if there is more text on the line

			if (startChar + runLength <= endChar || itsDrawWhitespaceFlag)
				{
				left += p.GetStringWidth(s);
				}
			}

		if (foundTab)
			{
			if (itsDrawWhitespaceFlag)
				{
				p.SetLineWidth(1);
				p.SetPenColor(itsWhitespaceColor);

				wsRect.left  = left;
				wsRect.right = left + wsRect.height();

				const JCoordinate xc = wsRect.xcenter();
				const JCoordinate yc = wsRect.ycenter();
				const JPoint top   (xc, wsRect.top);
				const JPoint left  (wsRect.left, yc);
				const JPoint bottom(xc, wsRect.bottom);
				const JPoint right (wsRect.right, yc);

				p.Line(left, right);
				p.Line(top, right);
				p.LineTo(bottom);
				}

			if (itsDrawWhitespaceFlag)
				{
				teDrawSpaces(itsText, startChar + runLength - 1, -1, *firstInRun,
							 p, left, wsYCenter, f, itsWhitespaceColor);
				}

			left += GetTabWidth(startChar + runLength, left);
			runLength++;

			if (itsDrawWhitespaceFlag)
				{
				teDrawSpaces(itsText, startChar + runLength, +1, trueRunEnd,
							 p, left, wsYCenter, f, itsWhitespaceColor);
				}
			}

		startChar += runLength;
		if (startChar > trueRunEnd)		// move to next style run
			{
			*firstInRun = startChar;
			(*runIndex)++;
			}
		}

	// account for newline

	if (lineEndsWithNewline)
		{
		if (itsDrawWhitespaceFlag)
			{
			p.SetLineWidth(1);
			p.SetPenColor(itsWhitespaceColor);

			JRect rect(top+3, left, top+geom.height-2, left);
			rect.right += rect.height();
			const JCoordinate xc = rect.xcenter();
			const JCoordinate yc = rect.ycenter();

			JCoordinate delta = rect.height()/4;
			if (delta < 1)
				{
				delta = 1;
				}

			const JPoint pt1(xc, rect.top);
			const JPoint pt2(xc, rect.bottom);
			const JPoint pt3(xc - delta, rect.bottom - delta);
			const JPoint pt4(xc, rect.bottom - 2*delta);

			p.Line(pt1, pt2);
			p.LineTo(pt3);
			p.LineTo(pt4);

			const JFont& f = styles.GetRunDataRef(*runIndex);
			teDrawSpaces(itsText, endChar, -1, *firstInRun,
						 p, left, wsYCenter, f, itsWhitespaceColor);
			}

		const JSize runLength = styles.GetRunLength(*runIndex);
		if (startChar >= *firstInRun + runLength-1)
			{
			*firstInRun += runLength;
			(*runIndex)++;
			}

		iter->SkipNext();	// skip over newline character
		}
}

/******************************************************************************
 TEDrawInMargin (virtual protected)

	Scribble in the margins, e.g., bookmarks, line numbers, etc.

 ******************************************************************************/

void
JTextEditor::TEDrawInMargin
	(
	JPainter&			p,
	const JPoint&		topLeft,
	const LineGeometry&	geom,
	const JIndex		lineIndex
	)
{
}

/******************************************************************************
 TEDrawSelection (private)

	Draw the selection region.

 ******************************************************************************/

void
JTextEditor::TEDrawSelection
	(
	JPainter&			p,
	const JRect&		rect,
	const JIndex		startVisLine,
	const JCoordinate	startVisLineTop
	)
{
	assert( itsActiveFlag && !itsSelection.IsEmpty() && itsType != kStaticText );

	// calculate intersection of selection region and drawing region

	JIndex startLine = GetLineForChar(itsSelection.charRange.first);
	JIndex endLine   = GetLineForChar(itsSelection.charRange.last);

	const JIndex origStartLine = startLine;
	const JIndex origEndLine   = endLine;

	TextIndex startChar;
	JCoordinate x, y;

	JCoordinate endVisLineTop;
	const JIndex endVisLine = CalcLineIndex(rect.bottom, &endVisLineTop);
	if (startVisLine > endLine || endVisLine < startLine)
		{
		return;
		}
	else if (startVisLine > startLine)
		{
		startLine = startVisLine;
		startChar = GetLineStart(startVisLine);
		x         = 0;
		y         = startVisLineTop;
		}
	else
		{
		startChar = itsSelection.GetFirst();
		x         = GetCharLeft(CaretLocation(itsSelection.GetFirst(), startLine));
		y         = GetLineTop(startLine);
		}

	if (endLine > endVisLine)
		{
		endLine = endVisLine;
		}

	// draw the selection region

	const JColorID savedColor = p.GetPenColor();
	const JBoolean savedFill     = p.IsFilling();
	if (itsSelActiveFlag)
		{
		p.SetPenColor(itsSelectionColor);
		p.SetFilling(kJTrue);
		}
	else
		{
		p.SetPenColor(itsSelectionOutlineColor);
		p.SetFilling(kJFalse);
		}

	const JCoordinate xmax = JMax(itsGUIWidth, itsWidth);

	JRunArrayIterator<LineGeometry> iter(itsLineGeom, kJIteratorStartBefore, startLine);
	LineGeometry geom;
	for (JIndex i=startLine; i<=endLine; i++)
		{
		const JBoolean ok = iter.Next(&geom);
		assert( ok );

		JCoordinate w;
		TextIndex endChar = GetLineEnd(i);
		if (endChar.charIndex > itsSelection.charRange.last)
			{
			endChar = itsSelection.GetLast(*itsText);
			w       = GetStringWidth(startChar, endChar);
			}
		else
			{
			w = xmax - x;
			}

		const JCoordinate bottom = y + geom.height - 1;
		if (itsSelActiveFlag)
			{
			// solid rectangle

			p.Rect(x, y, w, geom.height);
			}
		else if (origStartLine == origEndLine)
			{
			// empty rectangle

			p.Rect(x, y, w, geom.height);
			}
		else if (i == origStartLine)
			{
			// top of selection region

			p.Line(0,bottom, x,bottom);
			p.LineTo(x,y);
			p.LineTo(x+w,y);
			p.LineTo(x+w,bottom);
			}
		else if (i == origEndLine)
			{
			// bottom of selection region

			p.Line(0,y, 0,bottom);
			p.LineTo(w,bottom);
			p.LineTo(w,y);
			p.LineTo(xmax,y);
			}
		else
			{
			// vertical sides of selection region

			p.Line(0,y, 0,bottom);
			p.Line(xmax,y, xmax,bottom);
			}

		startChar = itsText->AdjustTextIndex(endChar, +1);

		x  = 0;		// all lines after the first start at the far left
		y += geom.height;
		}

	// clean up

	p.SetPenColor(savedColor);
	p.SetFilling(savedFill);
}

/******************************************************************************
 TEDrawCaret (private)

 ******************************************************************************/

void
JTextEditor::TEDrawCaret
	(
	JPainter&				p,
	const CaretLocation&	caretLoc
	)
{
	assert( itsType == kFullEditor );

	JCoordinate x, y1, y2;

	if (caretLoc.location.charIndex == itsText->GetText().GetCharacterCount()+1 &&
		itsText->EndsWithNewline())
		{
		x  = -1;
		y1 = GetLineBottom(caretLoc.lineIndex) + 1;
		y2 = y1 + GetEWNHeight()-1;
		}
	else
		{
		x = GetCharLeft(caretLoc) - 1;
		if (x >= itsWidth)
			{
			x = itsWidth;		// keep inside bounds
			}

		y1 = GetLineTop(caretLoc.lineIndex);
		y2 = y1 + GetLineHeight(caretLoc.lineIndex)-1;
		}

	const JColorID savedColor = p.GetPenColor();
	p.SetPenColor(itsCaretColor);

	if (itsCaretMode == kBlockCaret)
		{
		JCoordinate w = 5;
		if (itsText->CharacterIndexValid(caretLoc.location.charIndex))
			{
			JStringIterator* iter =
				itsText->GetConstIterator(kJIteratorStartBefore, caretLoc.location);

			JUtf8Character c;
			const JBoolean ok = iter->Next(&c);
			assert( ok );

			if (c != '\n')
				{
				w = GetCharWidth(caretLoc);
				}

			itsText->DisposeConstIterator(iter);
			}
		p.Rect(x,y1, w+1,y2-y1);
		}
	else
		{
		p.Line(x,y1, x,y2);
		}

	p.SetPenColor(savedColor);
}

/******************************************************************************
 TEWillDragAndDrop (protected)

 ******************************************************************************/

JBoolean
JTextEditor::TEWillDragAndDrop
	(
	const JPoint&	pt,
	const JBoolean	extendSelection,
	const JBoolean	dropCopy
	)
	const
{
	if (itsActiveFlag && itsType != kStaticText && itsPerformDNDFlag &&
		!itsSelection.IsEmpty() && !extendSelection)
		{
		return PointInSelection(pt);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PointInSelection (protected)

 ******************************************************************************/

JBoolean
JTextEditor::PointInSelection
	(
	const JPoint& origPt
	)
	const
{
	if (HasSelection())
		{
		const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
		const CaretLocation caretLoc = CalcCaretLocation(pt);

		return JI2B(itsSelection.charRange.first <= caretLoc.location.charIndex &&
					caretLoc.location.charIndex <= itsSelection.charRange.last+1);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 TEHandleMouseDown (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleMouseDown
	(
	const JPoint&	origPt,
	const JSize		clickCount,
	const JBoolean	extendSelection,
	const JBoolean	partialWord
	)
{
	assert( itsActiveFlag );

	itsDragType         = kInvalidDrag;
	itsIsDragSourceFlag = kJFalse;
	if (itsType == kStaticText)
		{
		return;
		}

	itsText->DeactivateCurrentUndo();

	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	const CaretLocation caretLoc = CalcCaretLocation(pt);

	itsStartPt = itsPrevPt = pt;

	if (!itsSelection.IsEmpty() && extendSelection &&
		caretLoc.location.charIndex <= itsSelection.charRange.first)
		{
		itsDragType =
			(itsPrevDragType == kInvalidDrag ? kSelectDrag : itsPrevDragType);
		itsSelectionPivot = itsSelection.GetAfter();
		itsPrevPt.x--;
		TEHandleMouseDrag(origPt);
		}
	else if (!itsSelection.IsEmpty() && extendSelection)
		{
		itsDragType =
			(itsPrevDragType == kInvalidDrag ? kSelectDrag : itsPrevDragType);
		itsSelectionPivot = itsSelection.GetFirst();
		itsPrevPt.x--;
		TEHandleMouseDrag(origPt);
		}
	else if (itsPerformDNDFlag && clickCount == 1 && !itsSelection.IsEmpty() &&
			 itsSelection.charRange.first <= caretLoc.location.charIndex &&
			 caretLoc.location.charIndex <= itsSelection.charRange.last+1)
		{
		itsDragType = kDragAndDrop;
		itsDropLoc  = CaretLocation();
		}
	else if (extendSelection)
		{
		itsDragType       = kSelectDrag;
		itsSelectionPivot = itsCaretLoc.location;	// save this before SetSelection()

		if (caretLoc.location.charIndex < itsCaretLoc.location.charIndex)
			{
			SetSelection(TextRange(caretLoc.location, itsCaretLoc.location));
			}
		else if (itsCaretLoc.location.charIndex < caretLoc.location.charIndex)
			{
			SetSelection(TextRange(itsCaretLoc.location, caretLoc.location));
			}
		}
	else if (clickCount == 1)
		{
		if (caretLoc != itsCaretLoc)
			{
			SetCaretLocation(caretLoc);
			}
		itsDragType       = kSelectDrag;
		itsSelectionPivot = caretLoc.location;
		}
	else if (clickCount == 2)
		{
		itsDragType = (partialWord ? kSelectPartialWordDrag : kSelectWordDrag);
		TEGetDoubleClickSelection(caretLoc.location, partialWord, kJFalse,
								  &itsWordSelPivot);
		SetSelection(itsWordSelPivot);
		}
	else if (clickCount == 3)
		{
		itsDragType     = kSelectLineDrag;
		itsLineSelPivot = caretLoc.lineIndex;
		SetSelection(TextRange(
			GetLineStart(caretLoc.lineIndex),
			itsText->AdjustTextIndex(GetLineEnd(caretLoc.lineIndex), +1)));
		}

	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleMouseDrag (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleMouseDrag
	(
	const JPoint& origPt
	)
{
	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	if (itsDragType == kInvalidDrag || pt == itsPrevPt)
		{
		return;
		}

	TEScrollForDrag(origPt);

	const CaretLocation caretLoc = CalcCaretLocation(pt);

	if (itsDragType == kSelectDrag && caretLoc.location.charIndex < itsSelectionPivot.charIndex)
		{
		SetSelection(TextRange(caretLoc.location, itsSelectionPivot), kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectDrag && caretLoc.location.charIndex == itsSelectionPivot.charIndex)
		{
		SetCaretLocation(caretLoc);
		}
	else if (itsDragType == kSelectDrag && caretLoc.location.charIndex > itsSelectionPivot.charIndex)
		{
		SetSelection(TextRange(itsSelectionPivot, caretLoc.location), kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectWordDrag || itsDragType == kSelectPartialWordDrag)
		{
		TextRange range;
		TEGetDoubleClickSelection(caretLoc.location,
								  JI2B(itsDragType == kSelectPartialWordDrag),
								  kJTrue, &range);

		const JIndexRange cr = JCovering(itsWordSelPivot.charRange, range.charRange),
						  br = JCovering(itsWordSelPivot.byteRange, range.byteRange);
		SetSelection(TextRange(
			*static_cast<const JCharacterRange*>(&cr),
			*static_cast<const JUtf8ByteRange*>(&br)),
			kJFalse);

		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectLineDrag)
		{
		SetSelection(TextRange(
			GetLineStart( JMin(itsLineSelPivot, caretLoc.lineIndex) ),
			GetLineEnd(   JMax(itsLineSelPivot, caretLoc.lineIndex) )),
			kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kDragAndDrop && JMouseMoved(itsStartPt, pt))
		{
		itsDragType = kInvalidDrag;		// wait for TEHandleDNDEnter()
		if (TEBeginDND())
			{
			itsIsDragSourceFlag = kJTrue;
			// switches to TEHandleDND*()
			}
		}

	itsPrevPt = pt;

	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleMouseUp (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleMouseUp()
{
	// set itsSelectionPivot for "shift-arrow" selection

	if (!itsSelection.IsEmpty() &&
		(itsDragType == kSelectDrag ||
		 itsDragType == kSelectPartialWordDrag ||
		 itsDragType == kSelectWordDrag ||
		 itsDragType == kSelectLineDrag))
		{
		if ((itsDragType == kSelectWordDrag ||
			 itsDragType == kSelectPartialWordDrag) &&
			itsWordSelPivot.charRange.last  == itsSelection.charRange.last &&
			itsWordSelPivot.charRange.first != itsSelection.charRange.first)
			{
			itsSelectionPivot = itsSelection.GetAfter();
			}
		else if (itsDragType == kSelectLineDrag &&
				 GetLineEnd(itsLineSelPivot).charIndex   == itsSelection.charRange.last &&
				 GetLineStart(itsLineSelPivot).charIndex != itsSelection.charRange.first)
			{
			itsSelectionPivot = itsSelection.GetAfter();
			}

		if (itsSelectionPivot.charIndex == itsSelection.charRange.last+1)
			{
			itsCaretX = GetCharLeft(CalcCaretLocation(itsSelection.GetFirst()));
			}
		else
			{
			itsSelectionPivot = itsSelection.GetFirst();
			itsCaretX         = GetCharRight(CalcCaretLocation(itsSelection.GetLast(*itsText)));
			}
		}

	// save drag type for "extend" drag

	if (itsDragType == kSelectDrag ||
		itsDragType == kSelectPartialWordDrag ||
		itsDragType == kSelectWordDrag ||
		itsDragType == kSelectLineDrag)
		{
		itsPrevDragType = itsDragType;
		}
	else
		{
		itsPrevDragType = kInvalidDrag;
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 TEHandleDNDEnter (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleDNDEnter()
{
	itsDragType = kDragAndDrop;
	itsDropLoc  = CaretLocation();
	TERefreshCaret(itsCaretLoc);		// hide the typing caret
}

/******************************************************************************
 TEHandleDNDHere (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleDNDHere
	(
	const JPoint&	origPt,
	const JBoolean	dropOnSelf
	)
{
	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	if (itsDragType != kDragAndDrop || pt == itsPrevPt)
		{
		return;
		}

	TEScrollForDND(origPt);
	TERefreshCaret(itsDropLoc);

	const CaretLocation caretLoc = CalcCaretLocation(pt);

	CaretLocation dropLoc;
	if (!dropOnSelf ||
		caretLoc.location.charIndex   <= itsSelection.charRange.first ||
		itsSelection.charRange.last+1 <= caretLoc.location.charIndex)
		{
		dropLoc = caretLoc;		// only drop outside selection
		}
	itsDropLoc = dropLoc;
	TERefreshCaret(itsDropLoc);

	itsPrevPt = pt;
	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleDNDLeave (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleDNDLeave()
{
	assert( itsDragType == kDragAndDrop );

	TERefreshCaret(itsDropLoc);
	itsDragType = kInvalidDrag;
}

/******************************************************************************
 TEHandleDNDDrop (protected)

	If we are not the source, the derived class has to figure out how to get
	the data from the target.  Once it has the data, it should call
	Paste(text,style).

 ******************************************************************************/

void
JTextEditor::TEHandleDNDDrop
	(
	const JPoint&	pt,
	const JBoolean	dropOnSelf,
	const JBoolean	dropCopy
	)
{
	assert( itsDragType == kDragAndDrop );

	TEHandleDNDHere(pt, dropOnSelf);

	if (dropOnSelf && itsDropLoc.location.charIndex > 0)
		{
		TextRange r;
		const JBoolean ok = itsText->MoveText(itsSelection, itsDropLoc.location, dropCopy, &r);
		assert( ok );

		SetSelection(r);
		TEScrollToSelection(kJFalse);
		}
	else if (!dropOnSelf)
		{
		SetCaretLocation(itsDropLoc);
		TEPasteDropData();

		const TextIndex index = GetInsertionIndex();
		if (index.charIndex > itsDropLoc.location.charIndex)
			{
			SetSelection(TextRange(itsDropLoc.location, index));
			}
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 TEGetDoubleClickSelection (private)

	Select the word that was clicked on.  By computing the end of the word
	found by GetWordStart(), we avoid selecting two words when the user
	double clicks on the space between them.

 ******************************************************************************/

void
JTextEditor::TEGetDoubleClickSelection
	(
	const TextIndex&	origIndex,
	const JBoolean		partialWord,
	const JBoolean		dragging,
	TextRange*			range
	)
{
	TextIndex startIndex, endIndex;
	if (partialWord)
		{
		startIndex = itsText->GetPartialWordStart(origIndex);
		endIndex   = itsText->GetPartialWordEnd(startIndex);
		}
	else
		{
		startIndex = itsText->GetWordStart(origIndex);
		endIndex   = itsText->GetWordEnd(startIndex);
		}

	// To select the word, the caret location should be inside the word
	// or on the character following the word.

	const JSize textLength = itsText->GetText().GetCharacterCount();

	if ((startIndex.charIndex <= origIndex.charIndex && origIndex.charIndex <= endIndex.charIndex) ||
		(!dragging &&
		 ((origIndex.charIndex == endIndex.charIndex+1 && itsText->CharacterIndexValid(endIndex.charIndex+1)) ||
		  (origIndex.charIndex == textLength+1 && endIndex.charIndex == textLength))) )
		{
		*range = TextRange(startIndex, itsText->AdjustTextIndex(endIndex, +1));
		}

	// Otherwise, we select the character that was clicked on

	else
		{
		range->first = range->last = JMin(charIndex, bufLength);
		if (range->first > 1 &&
			itsText->GetCharacter(range->first)   == '\n' &&
			itsText->GetCharacter(range->first-1) != '\n')
			{
			(range->first)--;
			(range->last)--;
			}
		}
}

/******************************************************************************
 TEHitSamePart

 ******************************************************************************/

JBoolean
JTextEditor::TEHitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	const CaretLocation loc1 = CalcCaretLocation(pt1);
	const CaretLocation loc2 = CalcCaretLocation(pt2);
	return JI2B(loc1 == loc2);
}

/******************************************************************************
 SetKeyHandler

	Passing NULL resets to the default key handler

 ******************************************************************************/

void
JTextEditor::SetKeyHandler
	(
	JTEKeyHandler* handler
	)
{
	jdelete itsKeyHandler;

	if (handler == NULL)
		{
		handler = jnew JTEDefaultKeyHandler(this);
		assert( handler != NULL );
		}

	itsKeyHandler = handler;
	itsKeyHandler->Initialize();
}

/******************************************************************************
 TEHandleKeyPress (protected)

	Returns kJTrue if the key was processed.

 ******************************************************************************/

JBoolean
JTextEditor::TEHandleKeyPress
	(
	const JCharacter	origKey,
	const JBoolean		selectText,
	const CaretMotion	motion,
	const JBoolean		deleteToTabStop
	)
{
	assert( itsActiveFlag );
	assert( (!itsSelection.IsEmpty() && itsCaretLoc.charIndex == 0) ||
			( itsSelection.IsEmpty() && itsCaretLoc.charIndex >  0) );

	if (itsType == kStaticText)
		{
		return kJFalse;
		}
	else if (TEIsDragging())
		{
		return kJTrue;
		}

	// pre-processing

	JCharacter key = origKey;
	if (key == '\r')
		{
		key = '\n';
		}

	// handler

	const JBoolean processed =
		itsKeyHandler->HandleKeyPress(key, selectText, motion, deleteToTabStop);

	if (processed)
		{
		// We redraw the display immediately because it is very disconcerting
		// when the display does not instantly show the changes.

		TEUpdateDisplay();
		}
	return processed;
}

/******************************************************************************
 InsertKeyPress (private)

 ******************************************************************************/

void
JTextEditor::InsertKeyPress
	(
	const JCharacter key
	)
{
	JBoolean isNew;
	JTEUndoTyping* typingUndo = GetTypingUndo(&isNew);
	typingUndo->HandleCharacter();

	const JBoolean hadSelection = !itsSelection.IsEmpty();
	if (hadSelection)
		{
		itsInsertionFont = itsStyles->GetElement(itsSelection.first);
		DeleteText(itsSelection);
		itsCaretLoc = CalcCaretLocation(itsSelection.first);
		itsSelection.SetToNothing();
		}
	const JCharacter s[2]   = { key, '\0' };
	const JSize pasteLength = InsertText(itsCaretLoc, s);
	assert( pasteLength == 1 );
	Recalc(itsCaretLoc, 1, hadSelection, kJFalse);
	SetCaretLocation(itsCaretLoc.charIndex+1);

	typingUndo->Activate();		// cancel SetCaretLocation()

	if (key == '\n' && itsAutoIndentFlag)
		{
		AutoIndent(typingUndo);
		}

	NewUndo(typingUndo, isNew);
}

/******************************************************************************
 BackwardDelete (private)

	Delete characters preceding the insertion caret.

 ******************************************************************************/

void
JTextEditor::BackwardDelete
	(
	const JBoolean		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle
	)
{
	assert( itsSelection.IsEmpty() );

	itsText.BackwardDelete(lineStart, itsCaretLoc.location, deleteToTabStop, returnText, returnStyle);

	const JFont f = itsStyles->GetElement(startIndex);	// preserve font
//	DeleteText(deleteRange);
	Recalc(startIndex, 1, kJTrue, kJFalse);
	SetCaretLocation(startIndex);
	if (itsPasteStyledTextFlag)
		{
		itsInsertionFont = f;
		}

	typingUndo->Activate();		// cancel SetCaretLocation()
}

/******************************************************************************
 ForwardDelete (private)

	Delete characters following the insertion caret.

 ******************************************************************************/

void
JTextEditor::ForwardDelete
	(
	const JBoolean		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle
	)
{
	assert( itsSelection.IsEmpty() );

	itsText.ForwardDelete(lineStart, itsCaretLoc.location, deleteToTabStop, returnText, returnStyle);

	Recalc(itsCaretLoc, 1, kJTrue, kJFalse);
	SetCaretLocation(itsCaretLoc.charIndex);

	typingUndo->Activate();		// cancel SetCaretLocation()
}

/******************************************************************************
 SetCaretLocation

	Move the caret to the specified point.

 ******************************************************************************/

void
JTextEditor::SetCaretLocation
	(
	const JIndex origCharIndex
	)
{
	JIndex charIndex = JMax(origCharIndex, JIndex(1));
	charIndex        = JMin(charIndex, itsText.GetCharacterCount()+1);

	SetCaretLocation( CalcCaretLocation(TextIndex(charIndex, 0)) );
}

// private

void
JTextEditor::SetCaretLocation
	(
	const CaretLocation& caretLoc
	)
{
	if (itsIsDragSourceFlag)
		{
		return;
		}

	DeactivateCurrentUndo();

	const JBoolean hadSelection      = !itsSelection.IsEmpty();
	const CaretLocation origCaretLoc = itsCaretLoc;

	if (hadSelection)
		{
		TERefreshLines(GetLineForChar(itsSelection.first),
					   GetLineForChar(itsSelection.last));
		}

	itsSelection.SetToNothing();
	itsCaretLoc = caretLoc;
	itsCaretX   = GetCharLeft(itsCaretLoc);

	if (hadSelection || origCaretLoc != itsCaretLoc || !itsPasteStyledTextFlag)
		{
		itsInsertionFont = CalcInsertionFont(itsCaretLoc);
		}

	if (!TEScrollTo(itsCaretLoc))
		{
		TERefreshCaret(origCaretLoc);
		TERefreshCaret(itsCaretLoc);
		}

	BroadcastCaretMessages(itsCaretLoc,
		JI2B(hadSelection || origCaretLoc.lineIndex != itsCaretLoc.lineIndex));

	TECaretShouldBlink(kJTrue);
}

/******************************************************************************
 BroadcastCaretMessages (private)

 ******************************************************************************/

void
JTextEditor::BroadcastCaretMessages
	(
	const CaretLocation&	caretLoc,
	const JBoolean			lineChanged
	)
{
	if (itsBcastLocChangedFlag)
		{
		JIndex line = caretLoc.lineIndex;
		JIndex col  = GetColumnForChar(caretLoc);
		if (caretLoc.charIndex == itsText->GetLength()+1 &&
			EndsWithNewline())
			{
			line++;
			col = 1;
			}
		Broadcast(CaretLocationChanged(line, col));
		}
	else if (lineChanged)
		{
		JIndex line = caretLoc.lineIndex;
		if (caretLoc.charIndex == itsText->GetLength()+1 &&
			EndsWithNewline())
			{
			line++;
			}
		Broadcast(CaretLineChanged(line));
		}
}

/******************************************************************************
 GetColumnForChar

	Returns the column that the specified character is in.  If the caret is
	at the far left, it is column 1.

	Given that this is only useful with monospace fonts, the CRM tab width
	is used to calculate the column when tabs are encountered, by calling
	CRMGetTabWidth().

 ******************************************************************************/

JIndex
JTextEditor::GetColumnForChar
	(
	const JIndex charIndex
	)
	const
{
	CaretLocation loc = CalcCaretLocation(charIndex);
	return itsText.GetColumnForChar(GetLineStart(loc.lineIndex), loc.location);
}

/******************************************************************************
 GoToColumn

 ******************************************************************************/

void
JTextEditor::GoToColumn
	(
	const JIndex lineIndex,
	const JIndex columnIndex
	)
{
	const JSize lineCount = GetLineCount();

	CaretLocation caretLoc(0, lineIndex);
	if (lineIndex > lineCount && EndsWithNewline())
		{
		caretLoc.charIndex = itsText->GetLength() + 1;
		caretLoc.lineIndex = lineCount;
		}
	else
		{
		caretLoc.charIndex    = GetLineStart(lineIndex);
		const JIndex endIndex = GetLineEnd(lineIndex);
		JIndex col            = 1;
		while (col < columnIndex && caretLoc.charIndex < endIndex)
			{
			if (itsText->GetCharacter(caretLoc.charIndex) == '\t')
				{
				col += CRMGetTabWidth(col-1);
				}
			else
				{
				col++;
				}
			caretLoc.charIndex++;
			}
		}

	SetCaretLocation(caretLoc);
}

/******************************************************************************
 GoToLine

 ******************************************************************************/

void
JTextEditor::GoToLine
	(
	const JIndex lineIndex
	)
{
	JIndex trueIndex      = lineIndex;
	const JSize lineCount = GetLineCount();

	CaretLocation caretLoc;
	if (trueIndex > lineCount && EndsWithNewline())
		{
		caretLoc.charIndex = itsText->GetLength() + 1;
		caretLoc.lineIndex = lineCount;
		}
	else
		{
		if (trueIndex == 0)
			{
			trueIndex = 1;
			}
		else if (trueIndex > lineCount)
			{
			trueIndex = lineCount;
			}
		caretLoc.charIndex = GetLineStart(trueIndex);
		caretLoc.lineIndex = trueIndex;
		}

	TEScrollToRect(CalcCaretRect(caretLoc), kJTrue);
	if (IsReadOnly())
		{
		SetSelection(GetLineStart(caretLoc.lineIndex),
					 GetLineEnd(caretLoc.lineIndex));
		}
	else
		{
		SetCaretLocation(caretLoc);
		}
}

/******************************************************************************
 SelectLine

 ******************************************************************************/

void
JTextEditor::SelectLine
	(
	const JIndex lineIndex
	)
{
	GoToLine(lineIndex);
	if (itsSelection.IsEmpty())
		{
		SetSelection(GetLineStart(itsCaretLoc.lineIndex),
					 GetLineEnd(itsCaretLoc.lineIndex));
		}
}

/******************************************************************************
 GoToBeginningOfLine

 ******************************************************************************/

void
JTextEditor::GoToBeginningOfLine()
{
	CaretLocation caretLoc;
	if (!itsSelection.IsEmpty())
		{
		caretLoc = CalcCaretLocation(itsSelection.first);
		}
	else
		{
		caretLoc = itsCaretLoc;
		}

	if (caretLoc.charIndex == itsText->GetLength()+1 &&
		EndsWithNewline())
		{
		// set current value so we scroll to it
		}
	else if (itsMoveToFrontOfTextFlag && !itsText->IsEmpty())
		{
		const JIndex firstChar = GetLineStart(caretLoc.lineIndex);
		JIndex lastChar        = GetLineEnd(caretLoc.lineIndex);
		if (itsText->GetCharacter(lastChar) == '\n')
			{
			lastChar--;
			}

		JIndex firstTextChar = firstChar;
		JString linePrefix;
		JSize prefixLength;
		JIndex ruleIndex = 0;
		if (lastChar < firstChar ||
			!CRMGetPrefix(&firstTextChar, lastChar,
						  &linePrefix, &prefixLength, &ruleIndex) ||
			CRMLineMatchesRest(JIndexRange(firstChar, lastChar)))
			{
			firstTextChar = lastChar+1;
			}

		caretLoc.charIndex =
			caretLoc.charIndex <= firstTextChar ? firstChar : firstTextChar;
		}
	else
		{
		caretLoc.charIndex = GetLineStart(caretLoc.lineIndex);
		}

	SetCaretLocation(caretLoc);
}

/******************************************************************************
 GoToEndOfLine

 ******************************************************************************/

void
JTextEditor::GoToEndOfLine()
{
	if (!itsSelection.IsEmpty())
		{
		const JIndex lineIndex = GetLineForChar(itsSelection.last);
		const JIndex charIndex = GetLineEnd(lineIndex);
		if (isspace(itsText->GetCharacter(charIndex)))
			{
			SetCaretLocation(charIndex);
			}
		else
			{
			SetCaretLocation(charIndex+1);
			}
		}
	else if (itsCaretLoc.charIndex == itsText->GetLength()+1 &&
			 EndsWithNewline())
		{
		SetCaretLocation(itsCaretLoc);	// scroll to it
		}
	else if (!itsText->IsEmpty())
		{
		const JIndex charIndex = GetLineEnd(itsCaretLoc.lineIndex);
		const JCharacter c     = itsText->GetCharacter(charIndex);
		if (isspace(c) && (charIndex < itsText->GetLength() || c == '\n'))
			{
			SetCaretLocation(charIndex);
			}
		else
			{
			SetCaretLocation(charIndex+1);
			}
		}
}

/******************************************************************************
 CRLineIndexToVisualLineIndex

	Returns the index that the user sees that corresponds to the given
	index that would be seen without word wrap.

	This function is required to work for arbitrarily large, invalid line
	indices.

 ******************************************************************************/

JIndex
JTextEditor::CRLineIndexToVisualLineIndex
	(
	const JIndex crLineIndex
	)
	const
{
	if (itsBreakCROnlyFlag)
		{
		return JMin(crLineIndex, GetLineCount());
		}
	else
		{
		JIndex charIndex = 0;
		for (JIndex i=1; i<=crLineIndex; i++)
			{
			const JIndex newCharIndex = GetParagraphEnd(charIndex+1);
			if (newCharIndex == charIndex)	// end of text
				{
				break;
				}
			charIndex = newCharIndex;
			}
		return GetLineForChar(GetParagraphStart(charIndex));
		}
}

/******************************************************************************
 VisualLineIndexToCRLineIndex

	Returns the index that would be seen without word wrap that corresponds
	to the given index that the user sees.

	This function is required to work for arbitrarily large, invalid line
	indices.

 ******************************************************************************/

JIndex
JTextEditor::VisualLineIndexToCRLineIndex
	(
	const JIndex origVisualLineIndex
	)
	const
{
	const JIndex visualLineIndex = JMin(origVisualLineIndex, GetLineCount());
	if (itsBreakCROnlyFlag)
		{
		return visualLineIndex;
		}
	else
		{
		JSize crLineIndex         = 1;
		const JIndex endCharIndex = GetLineStart(visualLineIndex);
		for (JIndex i=1; i<endCharIndex; i++)
			{
			if (itsText->GetCharacter(i) == '\n')
				{
				crLineIndex++;
				}
			}
		return crLineIndex;
		}
}

/******************************************************************************
 TEScrollTo (private)

	Scroll to the specified insertion point.  Returns kJTrue if scrolling
	was actually necessary.

 ******************************************************************************/

JBoolean
JTextEditor::TEScrollTo
	(
	const CaretLocation& caretLoc
	)
{
	return TEScrollToRect(CalcCaretRect(caretLoc), kJFalse);
}

/******************************************************************************
 CalcCaretRect (private)

	Calculate the rectangle enclosing the caret when it is at the given
	location.  We set the width of the rectangle to be the left margin
	width so that the entire left margin will scroll into view when the
	caret is at the far left.

 ******************************************************************************/

JRect
JTextEditor::CalcCaretRect
	(
	const CaretLocation& caretLoc
	)
	const
{
	JRect r;

	if (caretLoc.charIndex == itsText->GetLength()+1 && EndsWithNewline())
		{
		r.top    = GetLineBottom(caretLoc.lineIndex) + 1;
		r.bottom = r.top + GetEWNHeight();
		r.left   = 0;
		r.right  = itsLeftMarginWidth + kRightMarginWidth;
		}
	else
		{
		r.top    = GetLineTop(caretLoc.lineIndex);
		r.bottom = r.top + GetLineHeight(caretLoc.lineIndex);
		r.left   = GetCharLeft(caretLoc);
		r.right  = r.left + itsLeftMarginWidth + kRightMarginWidth;
		}

	return r;
}

/******************************************************************************
 TEScrollToSelection (private)

	Scroll to make the selection or caret visible.  Returns kJTrue if scrolling
	was actually necessary.

 ******************************************************************************/

JBoolean
JTextEditor::TEScrollToSelection
	(
	const JBoolean centerInDisplay
	)
{
	if (!itsSelection.IsEmpty())
		{
		const CaretLocation start = CalcCaretLocation(itsSelection.first);
		const CaretLocation end   = CalcCaretLocation(itsSelection.last);

		JRect r;
		r.top    = GetLineTop(start.lineIndex);
		r.bottom = GetLineBottom(end.lineIndex);
		if (start.lineIndex == end.lineIndex)
			{
			r.left  = GetCharLeft(start) + itsLeftMarginWidth;
			r.right = GetCharRight(end) + itsLeftMarginWidth;
			}
		else
			{
			r.left  = 0;
			r.right = itsLeftMarginWidth + itsWidth + kRightMarginWidth;
			}

		return TEScrollToRect(r, centerInDisplay);
		}
	else
		{
		return TEScrollToRect(CalcCaretRect(itsCaretLoc), centerInDisplay);
		}
}

/******************************************************************************
 TEUpdateDisplay (virtual protected)

	Called to update the window after mouse and keyboard events.
	Not pure virtual because some systems do this automatically.

 ******************************************************************************/

void
JTextEditor::TEUpdateDisplay()
{
}

/******************************************************************************
 TERefreshLines (protected)

	Redraw the specified range of lines.

 ******************************************************************************/

void
JTextEditor::TERefreshLines
	(
	const JIndex firstLine,
	const JIndex lastLine
	)
{
	JRect r(GetLineTop(firstLine), 0, GetLineBottom(lastLine)+1,
			itsLeftMarginWidth + JMax(itsGUIWidth, itsWidth) + kRightMarginWidth);
	if (lastLine == GetLineCount() && EndsWithNewline())
		{
		r.bottom += GetEWNHeight();
		}
	TERefreshRect(r);
}

/******************************************************************************
 TERefreshCaret (private)

 ******************************************************************************/

void
JTextEditor::TERefreshCaret
	(
	const CaretLocation& caretLoc
	)
{
	if (caretLoc.charIndex == itsText->GetLength()+1 &&
		EndsWithNewline())
		{
		const JRect r(itsHeight - GetEWNHeight(), 0, itsHeight,
					  itsLeftMarginWidth + JMax(itsGUIWidth, itsWidth) +
					  kRightMarginWidth);
		TERefreshRect(r);
		}
	else if (itsLineStarts->IndexValid(caretLoc.lineIndex))
		{
		TERefreshLines(caretLoc.lineIndex, caretLoc.lineIndex);
		}
}

/******************************************************************************
 MoveCaretVert (protected)

	Moves the caret up or down the specified number of lines.

 ******************************************************************************/

void
JTextEditor::MoveCaretVert
	(
	const JInteger deltaLines
	)
{
	assert( itsSelection.IsEmpty() );

	const JSize lineCount  = GetLineCount();
	const JIndex lineIndex = itsCaretLoc.lineIndex;

	JIndex newLineIndex = 0;
	if (deltaLines > 0 && lineIndex + deltaLines <= lineCount)
		{
		newLineIndex = lineIndex + deltaLines;
		}
	else if (deltaLines > 0)
		{
		newLineIndex = lineCount;
		}
	else if (deltaLines < 0 && lineIndex > (JSize) -deltaLines)
		{
		newLineIndex = lineIndex + deltaLines;
		}
	else if (deltaLines < 0)
		{
		newLineIndex = 1;
		}

	if (newLineIndex > 0)
		{
		const JCoordinate saveX = itsCaretX;

		const JPoint pt(itsCaretX, GetLineTop(newLineIndex)+1);
		SetCaretLocation(CalcCaretLocation(pt));

		itsCaretX = saveX;
		}
}

/******************************************************************************
 GetLineForChar

	Returns the line that contains the specified character.  Since the
	array is sorted, we can use a binary search.  We can use kAnyMatch
	because we know the values are unique.

 ******************************************************************************/

JIndex
JTextEditor::GetLineForChar
	(
	const JIndex charIndex
	)
	const
{
	if (charIndex == 0)
		{
		return 1;
		}

	itsLineStarts->SetCompareFunction(JStyledText::CompareCharacterIndices);

	JBoolean found;
	JIndex lineIndex =
		itsLineStarts->SearchSorted1(TextIndex(charIndex, 0), JListT::kAnyMatch, &found);
	if (!found)
		{
		lineIndex--;	// wants to insert -after- the value
		}
	return lineIndex;
}

/******************************************************************************
 GetLineForByte

	Returns the line that contains the specified byte.  Since the
	array is sorted, we can use a binary search.  We can use kAnyMatch
	because we know the values are unique.

 ******************************************************************************/

JIndex
JTextEditor::GetLineForByte
	(
	const JIndex byteIndex
	)
	const
{
	if (byteIndex == 0)
		{
		return 1;
		}

	itsLineStarts->SetCompareFunction(JStyledText::CompareByteIndices);

	JBoolean found;
	JIndex lineIndex =
		itsLineStarts->SearchSorted1(TextIndex(0, byteIndex), JListT::kAnyMatch, &found);
	if (!found)
		{
		lineIndex--;	// wants to insert -after- the value
		}
	return lineIndex;
}

/******************************************************************************
 GetLineEnd

	Returns the last character on the specified line.

 ******************************************************************************/

JTextEditor::TextIndex
JTextEditor::GetLineEnd
	(
	const JIndex lineIndex
	)
	const
{
	if (lineIndex < GetLineCount())
		{
		TextIndex i = GetLineStart(lineIndex+1);
		i.charIndex--;
		i.byteIndex--;	// newline is single byte
		return i;
		}
	else
		{
		return itsPrevTextLength;	// consistent with rest of output from Recalc()
		}
}

/******************************************************************************
 GetLineTop

	Returns the top of the specified line.

 ******************************************************************************/

JCoordinate
JTextEditor::GetLineTop
	(
	const JIndex endLineIndex
	)
	const
{
	return (endLineIndex == 1 ? 0 :
			itsLineGeom->SumElements(1, endLineIndex-1, GetLineHeight));
}

/******************************************************************************
 GetCharLeft (private)

	Returns the starting x coordinate of the specified character, excluding
	the left margin width.

 ******************************************************************************/

JCoordinate
JTextEditor::GetCharLeft
	(
	const CaretLocation& charLoc
	)
	const
{
	const JIndex firstChar = GetLineStart(charLoc.lineIndex);

	JCoordinate x = 0;
	if (charLoc.charIndex > firstChar)
		{
		x = GetStringWidth(firstChar, charLoc.charIndex-1);
		}
	return x;
}

/******************************************************************************
 GetCharRight (private)

	Returns the ending x coordinate of the specified character, excluding
	the left margin width.

 ******************************************************************************/

inline JCoordinate
JTextEditor::GetCharRight
	(
	const CaretLocation& charLoc
	)
	const
{
	return GetCharLeft(charLoc) + GetCharWidth(charLoc);
}

/******************************************************************************
 GetCharWidth (private)

	Returns the width of the specified character.

 ******************************************************************************/

JCoordinate
JTextEditor::GetCharWidth
	(
	const CaretLocation& charLoc
	)
	const
{
	const JCharacter c = itsText->GetCharacter(charLoc.charIndex);
	if (c != '\t')
		{
		const JFont f = itsStyles->GetElement(charLoc.charIndex);
		return f.GetCharWidth(c);
		}
	else
		{
		return GetTabWidth(charLoc.charIndex, GetCharLeft(charLoc));
		}
}

/******************************************************************************
 GetStringWidth (private)

	Returns the width of the specified character range.

	The second version updates *runIndex,*firstInRun so that they
	are correct for endIndex+1.

 ******************************************************************************/

JCoordinate
JTextEditor::GetStringWidth
	(
	const JIndex startIndex,
	const JIndex endIndex
	)
	const
{
	JIndex runIndex, firstInRun;
	const JBoolean found = itsStyles->FindRun(startIndex, &runIndex, &firstInRun);
	assert( found );

	return GetStringWidth(startIndex, endIndex, &runIndex, &firstInRun);
}

JCoordinate
JTextEditor::GetStringWidth
	(
	const JIndex	origStartIndex,
	const JIndex	endIndex,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
	const
{
	assert( IndexValid(origStartIndex) );
	assert( IndexValid(endIndex) );

	// preWidth stores the width of the characters preceding origStartIndex
	// on the line containing origStartIndex.  We calculate this -once- when
	// it is first needed.  (i.e. when we hit the first tab character)

	JCoordinate width    = 0;
	JCoordinate preWidth = -1;

	JIndex startIndex = origStartIndex;
	while (startIndex <= endIndex)
		{
		JSize runLength        = itsStyles->GetRunLength(*runIndex);
		const JSize trueRunEnd = *firstInRun + runLength-1;

		runLength -= startIndex - *firstInRun;
		if (startIndex + runLength-1 > endIndex)
			{
			runLength = endIndex - startIndex + 1;
			}

		const JFont& f = itsStyles->GetRunDataRef(*runIndex);

		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JIndex tabIndex;
		if (LocateTab(startIndex, startIndex + runLength-1, &tabIndex))
			{
			runLength = tabIndex - startIndex;
			}
		else
			{
			tabIndex = 0;
			}

		if (runLength > 0)
			{
			width += f.GetStringWidth(itsText->GetCString() + startIndex-1, runLength);
			}
		if (tabIndex > 0)
			{
			if (preWidth < 0)
				{
				// recursion: max depth 1
				preWidth = GetCharLeft(CalcCaretLocation(origStartIndex));
				assert( preWidth >= 0 );
				}
			width += GetTabWidth(startIndex + runLength, preWidth + width);
			runLength++;
			}

		startIndex += runLength;
		if (startIndex > trueRunEnd)	// move to next style run
			{
			*firstInRun = startIndex;
			(*runIndex)++;
			}
		}

	return width;
}

/******************************************************************************
 LocateTab (private)

	Returns the index of the first tab character, starting from startIndex.
	If no tab is found, returns kJFalse, and *tabIndex = 0.

 ******************************************************************************/

JBoolean
JTextEditor::LocateTab
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	JIndex*			tabIndex
	)
	const
{
	for (JIndex i=startIndex; i<=endIndex; i++)
		{
		if (itsText->GetCharacter(i) == '\t')
			{
			*tabIndex = i;
			return kJTrue;
			}
		}

	*tabIndex = 0;
	return kJFalse;
}

/******************************************************************************
 GetTabWidth (virtual protected)

	Given the index of the tab character (charIndex) and the horizontal
	position on the line (in pixels) where the tab character starts (x),
	return the width of the tab character.

	This default implementation rounds the location up to the nearest
	multiple of itsDefTabWidth.

 ******************************************************************************/

JCoordinate
JTextEditor::GetTabWidth
	(
	const JIndex		charIndex,
	const JCoordinate	x
	)
	const
{
	const JCoordinate xnew = ((x / itsDefTabWidth) + 1) * itsDefTabWidth;
	return (xnew - x);
}

/******************************************************************************
 RecalcAll (protected)

 ******************************************************************************/

void
JTextEditor::RecalcAll
	(
	const JBoolean needAdjustStyles
	)
{
	if (itsBreakCROnlyFlag)
		{
		itsWidth = 0;
		}
	itsMaxWordWidth = 0;

	itsLineStarts->RemoveAll();
	itsLineStarts->AppendElement(TextIndex(1,1));

	itsLineGeom->RemoveAll();

	Recalc(CaretLocation(TextIndex(1,1),1), itsText.GetCharacterCount(),
		   kJFalse, kJTrue, needAdjustStyles);
}

/******************************************************************************
 Recalc (private)

	Recalculate layout, starting from caretLoc and continuing at least
	through charCount characters.

	*** We assume that nothing above caretLoc.lineIndex needs to be recalculated.
		We also assume that the first line to recalculate already exists.

 ******************************************************************************/

void
JTextEditor::Recalc
	(
	const CaretLocation&	origCaretLoc,
	const JSize				origMinCharCount,
	const JBoolean			deletion,
	const JBoolean			needCaretBcast,
	const JBoolean			needAdjustStyles
	)
{
	JCoordinate maxLineWidth = 0;
	if (itsBreakCROnlyFlag && GetLineCount() == 1)
		{
		itsWidth = 0;
		}

	JCoordinate origY = -1;
	if (!itsLineGeom->IsEmpty())
		{
		origY = GetLineTop(origCaretLoc.lineIndex);
		}

	const JSize bufLength  = itsText->GetLength();

	CaretLocation caretLoc = origCaretLoc;
	JSize minCharCount     = JMax((JSize) 1, origMinCharCount);
	JIndexRange redrawRange;
	if (bufLength > 0 && needAdjustStyles)
		{
		JIndexRange recalcRange(caretLoc.charIndex, caretLoc.charIndex + minCharCount - 1);
		redrawRange = recalcRange;
		AdjustStylesBeforeRecalc(*itsText, itsStyles, &recalcRange, &redrawRange, deletion);
		caretLoc.charIndex = recalcRange.first;
		minCharCount       = recalcRange.GetLength();

		if (recalcRange.Contains(redrawRange))
			{
			redrawRange.SetToNothing();
			}

		assert( caretLoc.charIndex <= origCaretLoc.charIndex );
		assert( caretLoc.charIndex + minCharCount - 1 >=
				origCaretLoc.charIndex + origMinCharCount - 1 );
		assert( itsText->GetLength() == itsStyles->GetElementCount() );

		if (caretLoc.charIndex < origCaretLoc.charIndex)
			{
			while (caretLoc.charIndex < itsLineStarts->GetElement(caretLoc.lineIndex))
				{
				(caretLoc.lineIndex)--;
				}
			}
		}

	if (caretLoc.charIndex == bufLength+1)
		{
		(caretLoc.charIndex)--;
		caretLoc.lineIndex = GetLineForChar(caretLoc.charIndex);
		}

	JIndex firstLineIndex, lastLineIndex;
	if (bufLength > 0)
		{
		Recalc1(bufLength, caretLoc, minCharCount, &maxLineWidth,
				&firstLineIndex, &lastLineIndex);

		if (!redrawRange.IsEmpty())
			{
			firstLineIndex = JMin(firstLineIndex, GetLineForChar(redrawRange.first));
			lastLineIndex  = JMax(lastLineIndex,  GetLineForChar(redrawRange.last));
			}
		}
	else
		{
		itsLineStarts->RemoveAll();
		itsLineStarts->AppendElement(1);

		itsLineGeom->RemoveAll();

		const JFont f = CalcInsertionFont(1);
		JCoordinate ascent,descent;
		const JCoordinate h = f.GetLineHeight(&ascent, &descent);
		itsLineGeom->AppendElement(LineGeometry(h, ascent));

		firstLineIndex = lastLineIndex = 1;
		}

	// If the caret is visible, recalculate the line index.

	JBoolean lineChanged = kJFalse;
	if (itsSelection.IsEmpty())
		{
		const JIndex origLine = itsCaretLoc.lineIndex;
		itsCaretLoc.lineIndex = GetLineForChar(itsCaretLoc.charIndex);
		lineChanged           = JI2B(itsCaretLoc.lineIndex != origLine);
		}

	// If we only break at newlines, we control our width.

	if (itsBreakCROnlyFlag && maxLineWidth > itsWidth)
		{
		itsWidth = maxLineWidth;
		}

	// If all the lines are the same height, set the scrolling step size to that.

	if (itsLineGeom->GetRunCount() == 1 && !itsIsPrintingFlag)
		{
		TESetVertScrollStep(GetLineHeight(1));
		}
	else if (!itsIsPrintingFlag)
		{
		TESetVertScrollStep(itsDefFont.GetLineHeight());
		}

	// recalculate the height

	const JSize lineCount = GetLineCount();
	JCoordinate newHeight = GetLineTop(lineCount) + GetLineHeight(lineCount);
	if (EndsWithNewline())
		{
		newHeight += GetEWNHeight();
		}

	JBoolean needRefreshLines = kJTrue;
	if (!itsIsPrintingFlag && newHeight != itsHeight)
		{
		TERefresh();	// redraw everything if the line heights changed
		needRefreshLines = kJFalse;
		}

	itsHeight = newHeight;

	// notify the derived class of our new size

	if (!itsIsPrintingFlag)
		{
		TESetGUIBounds(itsLeftMarginWidth + itsWidth + kRightMarginWidth, itsHeight, origY);
		}

	// save text length for next time

	itsPrevTextLength = bufLength;

	// show the changes

	if (!itsIsPrintingFlag && needRefreshLines)
		{
		TERefreshLines(firstLineIndex, lastLineIndex);
		}

	// This has to be last so everything is correctly set.

	if (needCaretBcast && itsSelection.IsEmpty())
		{
		BroadcastCaretMessages(itsCaretLoc, lineChanged);
		}
	else if (needCaretBcast)
		{
		BroadcastCaretMessages(CalcCaretLocation(itsSelection.first), kJTrue);
		}

	// avoid ludicrous width

	if (TEWidthIsBeyondDisplayCapacity(itsWidth))
		{
		SetBreakCROnly(kJFalse);
		}
}

/******************************************************************************
 Recalc1 (private)

	Subroutine called by Recalc().

	(firstLineIndex, lastLineIndex) gives the range of lines that had
	to be recalculated.

 ******************************************************************************/

void
JTextEditor::Recalc1
	(
	const JSize				bufLength,
	const CaretLocation&	caretLoc,
	const JSize				origMinCharCount,
	JCoordinate*			maxLineWidth,
	JIndex*					firstLineIndex,
	JIndex*					lastLineIndex
	)
{
	JIndex lineIndex = caretLoc.lineIndex;
	if (!itsBreakCROnlyFlag && lineIndex > 1 &&
		caretLoc.charIndex <= bufLength &&
		NoPrevWhitespaceOnLine(*itsText, caretLoc))
		{
		// If we start in the first word on the line, it
		// might actually belong on the previous line.

		lineIndex--;
		}

	JIndex firstChar   = GetLineStart(lineIndex);
	JSize minCharCount = origMinCharCount + (caretLoc.charIndex - firstChar);
	*firstLineIndex    = lineIndex;

	JIndex runIndex, firstInRun;
	const JBoolean found = itsStyles->FindRun(firstChar, &runIndex, &firstInRun);
	assert( found );

	JSize totalCharCount = 0;
	*maxLineWidth        = itsWidth;
	while (1)
		{
		JCoordinate lineWidth;
		const JSize charCount = RecalcLine(bufLength, firstChar, lineIndex, &lineWidth,
										   &runIndex, &firstInRun);
		totalCharCount += charCount;
		if (*maxLineWidth < lineWidth)
			{
			*maxLineWidth = lineWidth;
			}
		const JIndex endChar = firstChar + charCount-1;
		assert( endChar <= bufLength );

		// remove line starts that are further from the end than the new one
		// (we use (bufLength - endChar) so subtraction won't produce negative numbers)

		while (lineIndex < GetLineCount() &&
			   (itsPrevTextLength+1) - GetLineStart(lineIndex+1) >
					bufLength - endChar)
			{
			itsLineStarts->RemoveElement(lineIndex+1);
			itsLineGeom->RemoveElement(lineIndex+1);
			}

		// check if we are done

		if (endChar >= bufLength)
			{
			// We reached the end of the text.

			break;
			}
		else if (totalCharCount >= minCharCount &&
				 lineIndex < GetLineCount() &&
				 itsPrevTextLength - GetLineStart(lineIndex+1) ==
					bufLength - (endChar+1))
			{
			// The rest of the line starts merely shift.

			const JSize lineCount = itsLineStarts->GetElementCount();
			assert( lineCount > lineIndex );
			const long delta = endChar+1 - GetLineStart(lineIndex+1);
			if (delta != 0)
				{
				const JIndex* lineStart = itsLineStarts->GetCArray();
				for (JIndex i=lineIndex+1; i<=lineCount; i++)
					{
					itsLineStarts->SetElement(i, lineStart[i-1] + delta);
					}
				}
			break;
			}

		// insert the new line start

		lineIndex++;
		firstChar += charCount;

		itsLineStarts->InsertElementAtIndex(lineIndex, firstChar);
		itsLineGeom->InsertElementAtIndex(lineIndex, LineGeometry());

		// This catches the case when the new and old line starts
		// are equally far from the end, but we haven't recalculated
		// far enough yet, so the above breakout code didn't trigger.

		if (lineIndex < GetLineCount() &&
			   itsPrevTextLength - GetLineStart(lineIndex+1) ==
					bufLength - (endChar+1))
			{
			itsLineStarts->RemoveElement(lineIndex+1);
			itsLineGeom->RemoveElement(lineIndex+1);
			}
		}

	*lastLineIndex = lineIndex;
}

/******************************************************************************
 RecalcLine (private)

	Recalculate the line starting with firstChar.  Returns the number
	of characters on the line.  Sets the appropriate values in itsLineGeom.
	Sets *lineWidth to the width of the line in pixels.

	If insertLine is kJTrue, then this line is new, so we insert a new
	element into itsLineGeom.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the line.

 ******************************************************************************/

JSize
JTextEditor::RecalcLine
	(
	const JSize		bufLength,
	const JIndex	firstCharIndex,
	const JIndex	lineIndex,
	JCoordinate*	lineWidth,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
{
	JSize charCount = 0;
	*lineWidth      = 0;

	JIndex gswRunIndex   = *runIndex;
	JIndex gswFirstInRun = *firstInRun;

	if (itsBreakCROnlyFlag)
		{
		JIndex endIndex = firstCharIndex;
		if (!itsText->LocateNextSubstring("\n", &endIndex))
			{
			endIndex = itsText->GetLength();
			}
		charCount  = endIndex - firstCharIndex + 1;
		*lineWidth = GetStringWidth(firstCharIndex, endIndex,
									&gswRunIndex, &gswFirstInRun);
		}

	else
		{
		// include leading whitespace

		JBoolean endOfLine;
		charCount = IncludeWhitespaceOnLine(bufLength, firstCharIndex,
											lineWidth, &endOfLine,
											&gswRunIndex, &gswFirstInRun);
		JIndex charIndex = firstCharIndex + charCount;

		// Add words until we hit the right margin, a newline,
		// or the end of the text.

		while (charIndex <= bufLength && !endOfLine)
			{
			// get the next word

			JIndex prevIndex = charIndex;
			if (!LocateNextWhitespace(bufLength, &charIndex))
				{
				charIndex = bufLength+1;
				}

			// check if the word fits on this line

			JCoordinate dw =
				GetStringWidth(prevIndex, charIndex-1, &gswRunIndex, &gswFirstInRun);
			if (itsMaxWordWidth < dw)
				{
				itsMaxWordWidth = dw;
				}
			if (*lineWidth + dw > itsWidth)
				{
				if (prevIndex != firstCharIndex)
					{
					// this word goes on the next line

					charIndex = prevIndex;
					}
				else
					{
					// put as much of this word as possible on the line

					assert( *lineWidth == 0 && charCount == 0 );
					charCount = GetSubwordForLine(bufLength, lineIndex,
												  firstCharIndex, lineWidth);
					}
				break;
				}

			// put the word on this line

			*lineWidth += dw;
			charCount  += charIndex - prevIndex;

			// include the whitespace after the word

			JSize wsCount =
				IncludeWhitespaceOnLine(bufLength, charIndex, lineWidth, &endOfLine,
										&gswRunIndex, &gswFirstInRun);
			charIndex += wsCount;
			charCount += wsCount;
			}
		}

	// update geometry for this line

	const JSize runCount = itsStyles->GetRunCount();
	const JSize lastChar = firstCharIndex + charCount-1;

	JCoordinate maxAscent=0, maxDescent=0;
	while (*runIndex <= runCount)
		{
		const JFont& f = itsStyles->GetRunDataRef(*runIndex);
		JCoordinate ascent, descent;
		f.GetLineHeight(&ascent, &descent);

		if (ascent > maxAscent)
			{
			maxAscent = ascent;
			}
		if (descent > maxDescent)
			{
			maxDescent = descent;
			}

		const JIndex firstInNextRun = *firstInRun + itsStyles->GetRunLength(*runIndex);
		if (firstInNextRun <= lastChar+1)
			{
			(*runIndex)++;
			*firstInRun = firstInNextRun;
			}
		if (firstInNextRun > lastChar)
			{
			break;
			}
		}

	const LineGeometry geom(maxAscent+maxDescent, maxAscent);
	if (lineIndex <= itsLineGeom->GetElementCount())
		{
		itsLineGeom->SetElement(lineIndex, geom);
		}
	else
		{
		itsLineGeom->AppendElement(geom);
		}

	// return number of characters on line

	return charCount;
}

/******************************************************************************
 IncludeWhitespaceOnLine (private)

	*** Only for use by RecalcLine()

	Starting with the given index, return the number of consecutive whitespace
	characters encountered.  Increments *lineWidth with the width of this
	whitespace.  If we encounter a newline, we stop beyond it and set
	*endOfLine to kJTrue.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the whitespace.

 ******************************************************************************/

JSize
JTextEditor::IncludeWhitespaceOnLine
	(
	const JSize		bufLength,
	const JIndex	origStartIndex,
	JCoordinate*	lineWidth,
	JBoolean*		endOfLine,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
	const
{
	*endOfLine = kJFalse;

	JIndex startIndex = origStartIndex;
	JIndex endIndex   = startIndex;
	JSize wsCount     = 0;

	while (endIndex <= bufLength)
		{
		const JCharacter c = itsText->GetCharacter(endIndex);
		if (!isspace(c))
			{
			break;
			}

		wsCount++;
		if (c == '\t')
			{
			if (endIndex > startIndex)
				{
				*lineWidth += GetStringWidth(startIndex, endIndex-1,
											 runIndex, firstInRun);
				}
			*lineWidth += GetTabWidth(endIndex, *lineWidth);
			startIndex  = endIndex+1;

			// update *runIndex,*firstInRun after passing tab character

			const JSize runLength = itsStyles->GetRunLength(*runIndex);
			if (startIndex > *firstInRun + runLength-1)
				{
				*firstInRun += runLength;
				(*runIndex)++;
				}

			// tab characters can wrap to the next line

			if (!itsBreakCROnlyFlag && *lineWidth > itsWidth)
				{
				*endOfLine = kJTrue;
				break;
				}
			}

		endIndex++;
		if (c == '\n')
			{
			*endOfLine = kJTrue;
			break;
			}
		}

	if (endIndex > startIndex)
		{
		*lineWidth += GetStringWidth(startIndex, endIndex-1, runIndex, firstInRun);
		}

	return wsCount;
}

/******************************************************************************
 LocateNextWhitespace (private)

	*** Only for use by RecalcLine()

	Find the next whitespace character, starting from the given index.
	Returns kJFalse if it doesn't find any.

	*** If we only break at newlines, only newlines are considered to be
		whitespace.

 ******************************************************************************/

JBoolean
JTextEditor::LocateNextWhitespace
	(
	const JSize	bufLength,
	JIndex*		startIndex
	)
	const
{
	for (JIndex i=*startIndex; i<=bufLength; i++)
		{
		const JCharacter c = itsText->GetCharacter(i);
		if ((!itsBreakCROnlyFlag && isspace(c)) ||
			( itsBreakCROnlyFlag && c == '\n'))
			{
			*startIndex = i;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetSubwordForLine (private)

	*** Only for use by RecalcLine()

	Starting with the given index, return the number of characters that
	will fit on a line.  Sets *lineWidth to the width of the line in pixels.
	We always put at least one character on the line.

 ******************************************************************************/

JSize
JTextEditor::GetSubwordForLine
	(
	const JSize		bufLength,
	const JIndex	lineIndex,
	const JIndex	startIndex,
	JCoordinate*	lineWidth
	)
	const
{
	*lineWidth = 0;

	JIndex endIndex = 0;
	CaretLocation caretLoc(0, lineIndex);
	for (JIndex i=startIndex; i<=bufLength; i++)
		{
		caretLoc.charIndex   = i;
		const JCoordinate dw = GetCharWidth(caretLoc);
		if (i > startIndex && *lineWidth + dw > itsWidth)
			{
			endIndex = i-1;
			break;
			}
		else
			{
			*lineWidth += dw;
			}
		}
	if (endIndex == 0)
		{
		endIndex = bufLength;
		}

	return (endIndex - startIndex + 1);
}

/******************************************************************************
 NoPrevWhitespaceOnLine (private)

	Returns kJTrue if there is no whitespace between startIndex-1 and
	the first character on the line.

	Called by Recalc1() to decide whether or not to start on the
	previous line.

 ******************************************************************************/

JBoolean
JTextEditor::NoPrevWhitespaceOnLine
	(
	const JCharacter*		str,
	const CaretLocation&	startLoc
	)
	const
{
	const JIndex firstChar = GetLineStart(startLoc.lineIndex);
	for (JIndex i = startLoc.charIndex - 1; i>firstChar; i--)
		{
		if (isspace(str[i-1]))
			{
			return kJFalse;
			}
		}

	return kJTrue;	// we hit the start of the string
}

/******************************************************************************
 CalcCaretLocation (protected)

	Return the closest insertion point.  If the line ends with a space,
	the last possible insertion point is in front of this space

 ******************************************************************************/

JTextEditor::CaretLocation
JTextEditor::CalcCaretLocation
	(
	const JPoint& pt
	)
	const
{
	const JSize bufLength = itsText->GetLength();
	if (bufLength == 0)
		{
		return CaretLocation(TextIndex(1,1),1);
		}
	else if (pt.y >= itsHeight)
		{
		return CaretLocation(bufLength+1, GetLineCount());
		}

	// find the line that was clicked on

	JCoordinate lineTop;
	const JIndex lineIndex = CalcLineIndex(pt.y, &lineTop);
	if (EndsWithNewline() &&
		itsHeight - GetEWNHeight() < pt.y && pt.y <= itsHeight)
		{
		return CaretLocation(bufLength+1, GetLineCount());
		}

	// find the closest insertion point

	JIndex charIndex = 0;
	{
	const JIndex lineStart = GetLineStart(lineIndex);

	JIndex lineEnd = GetLineEnd(lineIndex);
	if ((lineEnd < bufLength || EndsWithNewline()) &&
		isspace(itsText->GetCharacter(lineEnd)))
		{
		lineEnd--;
		}

	JCoordinate prevD = pt.x;
	if (prevD <= 0)
		{
		charIndex = lineStart;
		}
	else
		{
		JCoordinate x = 0;
		for (JIndex i=lineStart; i<=lineEnd; i++)
			{
			x += GetCharWidth(CaretLocation(i, lineIndex));
			const JCoordinate d = pt.x - x;
			if (d == 0)
				{
				charIndex = i+1;
				break;
				}
			else if (d < 0 && prevD <= -d)
				{
				charIndex = i;
				break;
				}
			else if (d < 0 && prevD > -d)
				{
				charIndex = i+1;
				break;
				}
			prevD = d;
			}

		if (charIndex == 0)
			{
			charIndex = lineEnd+1;
			}
		}
	}

	return CaretLocation(charIndex, lineIndex);
}

/******************************************************************************
 CalcCaretLocation (private)

	byteIndex can be zero.

 ******************************************************************************/

inline JTextEditor::CaretLocation
JTextEditor::CalcCaretLocation
	(
	const TextIndex& index
	)
	const
{
	const JIndex lineIndex = GetLineForChar(index.charIndex);

	JIndex byteIndex = index.byteIndex;
	if (byteIndex == 0)
		{
		const TextIndex& i = itsLineStarts->GetElement(lineIndex);

		byteIndex += JString::CountBytes(itsText.GetBytes() + i.byteIndex - 1,
										 charIndex - i.charIndex);
		}

	return CaretLocation(index.charIndex, byteIndex, lineIndex);
}

/******************************************************************************
 CalcLineIndex (private)

 ******************************************************************************/

JIndex
JTextEditor::CalcLineIndex
	(
	const JCoordinate	y,
	JCoordinate*		lineTop
	)
	const
{
	if (y < 0)
		{
		*lineTop = 0;
		return 1;
		}
	else
		{
		JIndex lineIndex;
		itsLineGeom->FindPositiveSum(y, 1, &lineIndex, lineTop, GetLineHeight);
		return lineIndex;
		}
}

/******************************************************************************
 GetLineHeight (static private)

	Used by GetLineTop() and CalcLineIndex().

 ******************************************************************************/

JInteger
JTextEditor::GetLineHeight
	(
	const LineGeometry& data
	)
{
	return data.height;
}

/******************************************************************************
 GetEWNHeight (private)

	Not inline to avoid including JFontManager.h in header file

 ******************************************************************************/

JCoordinate
JTextEditor::GetEWNHeight()
	const
{
	const JFont f = CalcInsertionFont(
						TextIndex(itsText.GetCharacterCount() + 1,
								  itsText.GetByteCount() + 1));
	return f.GetLineHeight();
}
