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

#include "JTextEditor.h"
#include "JTEDefaultKeyHandler.h"
#include "JPagePrinter.h"
#include "JFontManager.h"
#include "JListUtil.h"
#include "JUndo.h"
#include "JRunArrayIterator.h"
#include "JRegex.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "JLatentPG.h"
#include "JMinMax.h"
#include "jTextUtil.h"
#include "jASCIIConstants.h"
#include "jFStreamUtil.h"
#include "jStreamUtil.h"
#include "jMouseUtil.h"
#include "jFileUtil.h"
#include "jTime.h"
#include "jGlobals.h"
#include "jAssert.h"

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
	JFontManager*		fontManager,
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

	itsKeyHandler(nullptr),
	itsNeedCaretBcastFlag(kJTrue),
	itsDragType(kInvalidDrag),
	itsPrevDragType(kInvalidDrag),
	itsIsDragSourceFlag(kJFalse)
{
	assert( itsText != nullptr );

	itsActiveFlag            = kJFalse;
	itsSelActiveFlag         = kJFalse;
	itsCaretVisibleFlag      = kJFalse;
	itsPerformDNDFlag        = kJFalse;
	itsMoveToFrontOfTextFlag = kJFalse;
	itsBcastLocChangedFlag   = kJFalse;
	itsBreakCROnlyFlag       = breakCROnly;
	itsIsPrintingFlag        = kJFalse;
	itsDrawWhitespaceFlag    = kJFalse;
	itsCaretMode             = kLineCaret;

	itsWidth           = width - kDefLeftMarginWidth - kRightMarginWidth;
	itsHeight          = 0;
	itsGUIWidth        = itsWidth;
	itsLeftMarginWidth = kDefLeftMarginWidth;
	itsDefTabWidth     = 36;	// pixesl => 1/2 inch
	itsMaxWordWidth    = 0;

	itsLineStarts = jnew JArray<TextIndex>;
	assert( itsLineStarts != nullptr );
	itsLineStarts->SetCompareFunction(JStyledText::CompareCharacterIndices);
	itsLineStarts->SetSortOrder(JListT::kSortAscending);

	itsLineGeom = jnew JRunArray<LineGeometry>;
	assert( itsLineGeom != nullptr );

	itsCaretLoc      = CaretLocation(TextIndex(1,1),1);
	itsCaretX        = 0;
	itsInsertionFont = itsText->CalcInsertionFont(TextIndex(1,1));

	if (type == kFullEditor)
		{
		itsText->SetBlockSizes(4096, 128);
		itsLineStarts->SetBlockSize(128);
		itsLineGeom->SetBlockSize(128);
		}

	SetKeyHandler(nullptr);

	ListenTo(itsText);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTextEditor::~JTextEditor()
{
	if (itsOwnsTextFlag)
		{
		StopListening(itsText);
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
		assert( info != nullptr );
		const TextRange& r = info->GetRange();
		if (r.charRange.GetCount() == itsText->GetText().GetCharacterCount())
			{
			RecalcAll();
			}
		else
			{
			Recalc(r, info->GetRedrawRange());
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
	//
	// We cannot use GetConstIterator() because it uses a temporary string

	JStringIterator iter(GetText()->GetText());
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
	JInterpolate*		interpolator,
	const JBoolean		preserveCase
	)
{
	assert( HasSelection() );

	const JIndex charIndex = itsSelection.charRange.first;
	const JIndex byteIndex = itsSelection.byteRange.first;

	itsNeedCaretBcastFlag = kJFalse;

	const TextCount count =
		itsText->ReplaceMatch(match, replaceStr, interpolator, preserveCase);

	itsNeedCaretBcastFlag = kJTrue;
	if (count.charCount > 0)
		{
		SetSelection(TextRange(TextIndex(charIndex, byteIndex), count));
		}
	else
		{
		SetCaretLocation(TextIndex(charIndex, byteIndex));
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
	JInterpolate*	interpolator,
	const JBoolean	preserveCase,
	const JBoolean	restrictToSelection
	)
{
	if (restrictToSelection && !HasSelection())
		{
		return kJFalse;
		}

	TextRange r(
		restrictToSelection ? itsSelection.charRange :
		JCharacterRange(1, itsText->GetText().GetCharacterCount()),

		restrictToSelection ? itsSelection.byteRange :
		JUtf8ByteRange(1, itsText->GetText().GetByteCount()));

	itsNeedCaretBcastFlag = kJFalse;

	r = itsText->ReplaceAllInRange(r, regex, entireWord,
								   replaceStr, interpolator, preserveCase);

	itsNeedCaretBcastFlag = kJTrue;
	if (restrictToSelection && !r.IsEmpty())
		{
		SetSelection(r);
		return kJTrue;
		}
	else if (!r.IsEmpty())
		{
		SetCaretLocation(itsText->GetBeyondEnd());
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
	const JBoolean					wrapSearch,
	JBoolean*						wrapped
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
	const JBoolean					wrapSearch,
	JBoolean*						wrapped
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
		}
}

/******************************************************************************
 Copy

 ******************************************************************************/

void
JTextEditor::Copy()
	const
{
	JString text;
	JRunArray<JFont> style;
	GetSelection(&text, &style);
	TEUpdateClipboard(text, style);
}

/******************************************************************************
 GetClipboard

	Returns the text and style that would be pasted if Paste() were called.

	style can be nullptr.  If it is not nullptr, it can come back empty even if
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
	if (style != nullptr)
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
		JRunArray<JFont>* s = (style.IsEmpty() ? nullptr : &style);
		Paste(text, s);
		}
}

/******************************************************************************
 Paste

	style can be nullptr

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

	JRunArray<JFont>* style1 = nullptr;
	if (style == nullptr)
		{
		style1 = jnew JRunArray<JFont>;
		assert( style1 != nullptr );

		style1->AppendElements(
			hadSelection ?
				itsText->GetStyles().GetElement(range.charRange.first) :
			itsInsertionFont,
			text.GetCharacterCount());
		}

	itsNeedCaretBcastFlag = kJFalse;

	range = itsText->Paste(range, text, style != nullptr ? style : style1);

	itsNeedCaretBcastFlag = kJTrue;
	SetCaretLocation(range.GetAfter());

	jdelete style1;
}

/******************************************************************************
 SetSelection (protected)

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

	if (itsText->IsEmpty() ||
		(itsSelection.charRange == range.charRange &&
		 itsSelection.byteRange == range.byteRange))
		{
		return;
		}

	assert( !range.IsEmpty() );
	assert( itsText->GetText().CharacterIndexValid(range.charRange.first) );
	assert( itsText->GetText().CharacterIndexValid(range.charRange.last) );
	assert( itsText->GetText().ByteIndexValid(range.byteRange.first) );
	assert( itsText->GetText().ByteIndexValid(range.byteRange.last) );
	assert( JString::CountCharacters(itsText->GetText().GetRawBytes(), range.byteRange) == range.charRange.GetCount() );	// TODO: debugging

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
		itsNeedCaretBcastFlag = kJFalse;
		itsText->DeleteText(itsSelection);
		itsNeedCaretBcastFlag = kJTrue;
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

	itsNeedCaretBcastFlag = kJFalse;

	r = itsText->Outdent(r, tabCount, force);

	itsNeedCaretBcastFlag = kJTrue;
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

	itsNeedCaretBcastFlag = kJFalse;

	r = itsText->Indent(r, tabCount);

	itsNeedCaretBcastFlag = kJTrue;
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

void
JTextEditor::CleanSelectedWhitespace
	(
	const JBoolean align
	)
{
	TextRange r;
	if (GetSelection(&r))
		{
		itsNeedCaretBcastFlag = kJFalse;
		r = itsText->CleanWhitespace(r, align);
		itsNeedCaretBcastFlag = kJTrue;
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
	JIndex endChar   = startChar + lineLength.charCount - 1;
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
		JIndex ci;
		while (iter->GetNextCharacterIndex(&ci) && ci <= cr.last)
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

	if (IsTrailingNewline(caretLoc.location))
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
		if (itsText->GetText().CharacterIndexValid(caretLoc.location.charIndex))
			{
			const JUtf8Character c = GetCharacter(caretLoc.location);
			if (c != '\n')
				{
				w = GetCharWidth(caretLoc, c);
				}
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
		itsNeedCaretBcastFlag = kJFalse;

		TextRange r;
		const JBoolean ok = itsText->MoveText(itsSelection, itsDropLoc.location, dropCopy, &r);
		assert( ok );

		itsNeedCaretBcastFlag = kJTrue;
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
 TEGetDoubleClickSelection (protected)

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
		 ((origIndex.charIndex == endIndex.charIndex+1 && itsText->GetText().CharacterIndexValid(endIndex.charIndex+1)) ||
		  (origIndex.charIndex == textLength+1 && endIndex.charIndex == textLength))) )
		{
		*range = TextRange(startIndex, itsText->AdjustTextIndex(endIndex, +1));
		}

	// Otherwise, we select the character that was clicked on

	else
		{
		TextIndex i(JMin(origIndex.charIndex, textLength),
					JMin(origIndex.byteIndex, itsText->GetText().GetByteCount()));

		JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, i);

		if (!iter->AtBeginning())
			{
			JUtf8Character c1, c2;
			const JBoolean ok1 = iter->Prev(&c1, kJFalse);
			const JBoolean ok2 = iter->Next(&c2, kJFalse);
			assert( ok1 && ok2 );

			if (c1 != '\n' && c2 == '\n')
				{
				iter->SkipPrev();
				}
			}

		const JIndex b1 = iter->GetNextByteIndex();
		iter->SkipNext();
		const JIndex b2 = iter->GetNextByteIndex();

		*range = TextRange(
			TextIndex(iter->GetPrevCharacterIndex(), b1),
			TextCount(1, b2-b1));

		itsText->DisposeConstIterator(iter);
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

	Passing nullptr resets to the default key handler

 ******************************************************************************/

void
JTextEditor::SetKeyHandler
	(
	JTEKeyHandler* handler
	)
{
	jdelete itsKeyHandler;

	if (handler == nullptr)
		{
		handler = jnew JTEDefaultKeyHandler(this);
		assert( handler != nullptr );
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
	const JUtf8Character&	origKey,
	const JBoolean			selectText,
	const CaretMotion		motion,
	const JBoolean			deleteToTabStop
	)
{
	assert( itsActiveFlag );
	assert( (!itsSelection.IsEmpty() && itsCaretLoc.location.charIndex == 0) ||
			( itsSelection.IsEmpty() && itsCaretLoc.location.charIndex >  0) );

	if (TEIsDragging())
		{
		return kJTrue;
		}

	// pre-processing

	JUtf8Character key = origKey;
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
		// when the display does not instantly show the result.

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
	const JUtf8Character& key
	)
{
	const JBoolean hadSelection = !itsSelection.IsEmpty();
	if (hadSelection)
		{
		itsInsertionFont = itsText->GetStyles().GetElement(itsSelection.charRange.first);
		itsCaretLoc      = CalcCaretLocation(itsSelection.GetFirst());
		}

	itsNeedCaretBcastFlag = kJFalse;

	JUndo* undo = itsText->InsertCharacter(
		hadSelection ? itsSelection : TextRange(itsCaretLoc.location, TextCount(0,0)),
		key, itsInsertionFont);

	itsNeedCaretBcastFlag = kJTrue;
	itsSelection.SetToNothing();
	SetCaretLocation(itsText->AdjustTextIndex(itsCaretLoc.location, +1));

	undo->Activate();		// cancel SetCaretLocation()
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

	itsNeedCaretBcastFlag = kJFalse;

	JUndo* undo;
	const TextIndex i =
		itsText->BackwardDelete(GetLineStart(itsCaretLoc.lineIndex),
								itsCaretLoc.location, deleteToTabStop,
								returnText, returnStyle, &undo);

	itsNeedCaretBcastFlag = kJTrue;

	JFont f          = JFontManager::GetDefaultFont();
	JBoolean setFont = kJFalse;
	if (i.charIndex > 1)
		{
		f       = itsText->GetStyles().GetElement(i.charIndex-1);	// preserve font
		setFont = kJTrue;
		}

	SetCaretLocation(i);
	if (itsText->WillPasteStyledText() && setFont)
		{
		itsInsertionFont = f;
		}

	if (undo != nullptr)
		{
		undo->Activate();		// cancel SetCaretLocation()
		}
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

	itsNeedCaretBcastFlag = kJFalse;

	JUndo* undo;
	itsText->ForwardDelete(GetLineStart(itsCaretLoc.lineIndex), itsCaretLoc.location,
						   deleteToTabStop, returnText, returnStyle, &undo);

	itsNeedCaretBcastFlag = kJTrue;
	SetCaretLocation(itsCaretLoc);

	if (undo != nullptr)
		{
		undo->Activate();		// cancel SetCaretLocation()
		}
}

/******************************************************************************
 SetCaretLocation

	Move the caret in front of to the specified character.

 ******************************************************************************/

void
JTextEditor::SetCaretLocation
	(
	const JIndex origCharIndex
	)
{
	JIndex charIndex = JMax(origCharIndex, JIndex(1));
	charIndex        = JMin(charIndex, itsText->GetText().GetCharacterCount()+1);

	SetCaretLocation(TextIndex(charIndex, 0));
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

	itsText->DeactivateCurrentUndo();

	const JBoolean hadSelection      = !itsSelection.IsEmpty();
	const CaretLocation origCaretLoc = itsCaretLoc;

	if (hadSelection)
		{
		TERefreshLines(GetLineForChar(itsSelection.charRange.first),
					   GetLineForChar(itsSelection.charRange.last));
		}

	itsSelection.SetToNothing();
	itsCaretLoc = caretLoc;
	itsCaretX   = GetCharLeft(itsCaretLoc);

	if (hadSelection || origCaretLoc != itsCaretLoc || !itsText->WillPasteStyledText())
		{
		itsInsertionFont = itsText->CalcInsertionFont(itsCaretLoc.location);
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
		if (IsTrailingNewline(caretLoc.location))
			{
			line++;
			col = 1;
			}
		Broadcast(CaretLocationChanged(line, col));
		}
	else if (lineChanged)
		{
		JIndex line = caretLoc.lineIndex;
		if (IsTrailingNewline(caretLoc.location))
			{
			line++;
			}
		Broadcast(CaretLineChanged(line));
		}
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

	CaretLocation caretLoc(TextIndex(), lineIndex);
	if (lineIndex > lineCount && itsText->EndsWithNewline())
		{
		caretLoc.location  = itsText->GetBeyondEnd();
		caretLoc.lineIndex = lineCount;
		}
	else
		{
		const TextIndex end = GetLineEnd(lineIndex);
		JIndex col          = 1;

		JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, GetLineStart(lineIndex));
		JUtf8Character c;
		while (col < columnIndex && iter->GetNextCharacterIndex() < end.charIndex &&
			   iter->Next(&c))
			{
			col += (c == '\t' ? itsText->CRMGetTabWidth(col) : 1);
			}

		caretLoc.location.charIndex = iter->GetNextCharacterIndex();
		caretLoc.location.byteIndex = iter->GetNextByteIndex();

		itsText->DisposeConstIterator(iter);
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
	if (trueIndex > lineCount && itsText->EndsWithNewline())
		{
		caretLoc.location  = itsText->GetBeyondEnd();
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
		caretLoc.location  = GetLineStart(trueIndex);
		caretLoc.lineIndex = trueIndex;
		}

	TEScrollToRect(CalcCaretRect(caretLoc), kJTrue);
	if (IsReadOnly())
		{
		SetSelection(TextRange(
			GetLineStart(caretLoc.lineIndex),
			GetLineEnd(caretLoc.lineIndex)));
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
		const TextIndex beyondEnd = lineIndex < GetLineCount() ?
			GetLineStart(lineIndex+1) : itsText->GetBeyondEnd();

		SetSelection(TextRange(
			GetLineStart(itsCaretLoc.lineIndex),
			beyondEnd));
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
		caretLoc = CalcCaretLocation(itsSelection.GetFirst());
		}
	else
		{
		caretLoc = itsCaretLoc;
		}

	if (IsTrailingNewline(caretLoc.location))
		{
		// set current value so we scroll to it
		}
	else if (itsMoveToFrontOfTextFlag && !itsText->IsEmpty())
		{
		const TextIndex firstChar = GetLineStart(caretLoc.lineIndex);
		TextIndex lastChar        = GetLineEnd(caretLoc.lineIndex);
		if (GetCharacter(lastChar) == '\n')
			{
			lastChar = itsText->AdjustTextIndex(lastChar, -1);
			}

		TextIndex firstTextChar = firstChar;
		JString linePrefix;
//		JSize prefixLength;
//		JIndex ruleIndex = 0;
		if (lastChar.charIndex < firstChar.charIndex || 0)
//			!CRMGetPrefix(&firstTextChar, lastChar,
//						  &linePrefix, &prefixLength, &ruleIndex) ||
//			CRMLineMatchesRest(JIndexRange(firstChar, lastChar)))
			{
			firstTextChar = itsText->AdjustTextIndex(lastChar, +1);
			}

		caretLoc.location =
			caretLoc.location.charIndex <= firstTextChar.charIndex ? firstChar : firstTextChar;
		}
	else
		{
		caretLoc.location = GetLineStart(caretLoc.lineIndex);
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
		const JIndex lineIndex = GetLineForChar(itsSelection.charRange.last);
		const TextIndex index  = GetLineEnd(lineIndex);

		if (GetCharacter(index).IsSpace())
			{
			SetCaretLocation(index);
			}
		else
			{
			SetCaretLocation(itsText->AdjustTextIndex(index, +1));
			}
		}
	else if (IsTrailingNewline(itsCaretLoc.location))
		{
		SetCaretLocation(itsCaretLoc);	// scroll to it
		}
	else if (!itsText->IsEmpty())
		{
		const TextIndex index  = GetLineEnd(itsCaretLoc.lineIndex);
		const JUtf8Character c = GetCharacter(index);
		if (c.IsSpace() &&
			(index.charIndex < itsText->GetText().GetCharacterCount() || c == '\n'))
			{
			SetCaretLocation(index);
			}
		else
			{
			SetCaretLocation(itsText->AdjustTextIndex(index, +1));
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

	TextIndex index(1,1);
	for (JIndex i=1; i<=crLineIndex; i++)
		{
		const TextIndex newIndex = itsText->GetParagraphEnd(index);
		if (newIndex.charIndex == index.charIndex)	// end of text
			{
			break;
			}
		index = itsText->AdjustTextIndex(newIndex, +1);
		}

	return GetLineForChar(itsText->GetParagraphStart(itsText->AdjustTextIndex(index, -1)).charIndex);
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

	JSize crLineIndex        = 1;
	const TextIndex endIndex = GetLineStart(visualLineIndex);

	JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, TextIndex(1,1));
	JUtf8Character c;
	while (iter->GetNextCharacterIndex() < endIndex.charIndex && iter->Next(&c))
		{
		if (c == '\n')
			{
			crLineIndex++;
			}
		}

	itsText->DisposeConstIterator(iter);
	return crLineIndex;
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

	if (IsTrailingNewline(caretLoc.location))
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
		const CaretLocation start = CalcCaretLocation(itsSelection.GetFirst());
		const CaretLocation end   = CalcCaretLocation(itsSelection.GetLast(*itsText));

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
	if (lastLine == GetLineCount() && itsText->EndsWithNewline())
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
	if (IsTrailingNewline(caretLoc.location))
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

JStyledText::TextIndex
JTextEditor::GetLineEnd
	(
	const JIndex lineIndex
	)
	const
{
	if (lineIndex < GetLineCount())
		{
		return itsText->AdjustTextIndex(GetLineStart(lineIndex+1), -1);
		}
	else
		{
		return itsPrevTextLastIndex;	// consistent with rest of output from Recalc()
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
	const TextIndex firstChar = GetLineStart(charLoc.lineIndex);

	JCoordinate x = 0;
	if (charLoc.location.charIndex > firstChar.charIndex)
		{
		x = GetStringWidth(firstChar, itsText->AdjustTextIndex(charLoc.location, -1));
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
	const TextIndex firstChar = GetLineStart(charLoc.lineIndex);

	JCoordinate x = 0;
	if (charLoc.location.charIndex > firstChar.charIndex)
		{
		x = GetStringWidth(firstChar, charLoc.location);
		}
	return x;
}

/******************************************************************************
 GetCharWidth (private)

	Returns the width of the specified character.

 ******************************************************************************/

JCoordinate
JTextEditor::GetCharWidth
	(
	const CaretLocation&	charLoc,
	const JUtf8Character&	c
	)
	const
{
	if (c != '\t')
		{
		const JFont f = itsText->GetStyles().GetElement(charLoc.location.charIndex);
		return f.GetCharWidth(itsFontManager, c);
		}
	else
		{
		return GetTabWidth(charLoc.location.charIndex, GetCharLeft(charLoc));
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
	const TextIndex& startIndex,
	const TextIndex& endIndex
	)
	const
{
	JIndex runIndex, firstInRun;
	const JBoolean found = itsText->GetStyles().FindRun(startIndex.charIndex, &runIndex, &firstInRun);
	assert( found );

	return GetStringWidth(startIndex, endIndex, &runIndex, &firstInRun);
}

JCoordinate
JTextEditor::GetStringWidth
	(
	const TextIndex&	origStart,
	const TextIndex&	end,
	JIndex*				runIndex,
	JIndex*				firstInRun
	)
	const
{
	assert( itsText->GetText().CharacterIndexValid(origStart.charIndex) );
	assert( itsText->GetText().CharacterIndexValid(end.charIndex) );

	// preWidth stores the width of the characters preceding origStartIndex
	// on the line containing origStartIndex.  We calculate this -once- when
	// it is first needed.  (i.e. when we hit the first tab character)

	JCoordinate width    = 0;
	JCoordinate preWidth = -1;

	const JRunArray<JFont>& styles = itsText->GetStyles();

	TextIndex start = origStart;
	while (start.charIndex <= end.charIndex)
		{
		JSize runLength        = styles.GetRunLength(*runIndex);
		const JSize trueRunEnd = *firstInRun + runLength-1;

		runLength -= start.charIndex - *firstInRun;
		if (start.charIndex + runLength-1 > end.charIndex)
			{
			runLength = end.charIndex - start.charIndex + 1;
			}

		const JFont& f = styles.GetRunDataRef(*runIndex);

		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JIndex tabCharIndex, pretabByteIndex;
		const JBoolean foundTab = LocateTab(start, runLength, &tabCharIndex, &pretabByteIndex);
		if (foundTab)
			{
			runLength = tabCharIndex - start.charIndex;
			}

		if (runLength > 0)
			{
			width += f.GetStringWidth(itsFontManager,
				JString(itsText->GetText().GetBytes(),
						JUtf8ByteRange(start.byteIndex, pretabByteIndex),
						kJFalse));
			}

		if (foundTab)
			{
			if (preWidth < 0)
				{
				// recursion: max depth 1
				preWidth = GetCharLeft(CalcCaretLocation(origStart));
				assert( preWidth >= 0 );
				}
			width += GetTabWidth(start.charIndex + runLength, preWidth + width);
			runLength++;
			}

		start = itsText->AdjustTextIndex(start, runLength);
		if (start.charIndex > trueRunEnd)	// move to next style run
			{
			*firstInRun = start.charIndex;
			(*runIndex)++;
			}
		}

	return width;
}

/******************************************************************************
 LocateTab (private)

	Returns the index of the first tab character, starting from startIndex.
	If no tab is found, returns kJFalse, and tabCharIndex = 0.

 ******************************************************************************/

JBoolean
JTextEditor::LocateTab
	(
	const TextIndex&	start,
	const JSize			count,
	JIndex*				tabCharIndex,
	JIndex*				pretabByteIndex
	)
	const
{
	JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, start);

	JUtf8Character c;
	JIndex i = 1;
	while (i <= count && iter->Next(&c))
		{
		if (c == '\t')
			{
			iter->SkipPrev();
			*tabCharIndex    = iter->GetNextCharacterIndex();
			*pretabByteIndex = (iter->AtBeginning() ? 0 : iter->GetPrevByteIndex());

			itsText->DisposeConstIterator(iter);
			return kJTrue;
			}
		i++;
		}

	*tabCharIndex    = 0;
	*pretabByteIndex = iter->GetPrevByteIndex();

	itsText->DisposeConstIterator(iter);
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
JTextEditor::RecalcAll()
{
	if (itsBreakCROnlyFlag)
		{
		itsWidth = 0;
		}
	itsMaxWordWidth = 0;

	itsLineStarts->RemoveAll();
	itsLineStarts->AppendElement(TextIndex(1,1));

	itsLineGeom->RemoveAll();

	const TextRange r(TextIndex(1,1),
		TextCount(itsText->GetText().GetCharacterCount(),
				  itsText->GetText().GetByteCount()));

	Recalc(r, r);
}

/******************************************************************************
 Recalc (private)

	*** We assume that nothing above recalcRange needs to be recalculated.
		We also assume that the first line to recalculate already exists.

 ******************************************************************************/

void
JTextEditor::Recalc
	(
	const TextRange& recalcRange,
	const TextRange& redrawRange
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
		origY = GetLineTop(GetLineForChar(recalcRange.charRange.first));
		}

	JIndex firstLineIndex, lastLineIndex;
	if (!itsText->IsEmpty())
		{
		Recalc1(recalcRange, &maxLineWidth, &firstLineIndex, &lastLineIndex);

		if (!redrawRange.IsEmpty())
			{
			firstLineIndex = JMin(firstLineIndex, GetLineForChar(redrawRange.charRange.first));
			lastLineIndex  = JMax(lastLineIndex,  GetLineForChar(redrawRange.charRange.last));
			}
		}
	else
		{
		itsLineStarts->RemoveAll();
		itsLineStarts->AppendElement(TextIndex(1,1));

		itsLineGeom->RemoveAll();

		const JFont f = itsText->CalcInsertionFont(TextIndex(1,1));
		JCoordinate ascent,descent;
		const JCoordinate h = f.GetLineHeight(itsFontManager, &ascent, &descent);
		itsLineGeom->AppendElement(LineGeometry(h, ascent));

		firstLineIndex = lastLineIndex = 1;
		}

	// If the caret is visible, recalculate the line index.

	JBoolean lineChanged = kJFalse;
	if (itsSelection.IsEmpty())
		{
		const JIndex origLine = itsCaretLoc.lineIndex;
		itsCaretLoc.lineIndex = GetLineForChar(itsCaretLoc.location.charIndex);
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
		TESetVertScrollStep(itsText->GetDefaultFont().GetLineHeight(itsFontManager));
		}

	// recalculate the height

	const JSize lineCount = GetLineCount();
	JCoordinate newHeight = GetLineTop(lineCount) + GetLineHeight(lineCount);
	if (itsText->EndsWithNewline())
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

	// save for next time

	itsPrevTextLastIndex = itsText->AdjustTextIndex(itsText->GetBeyondEnd(), -1);
	itsPrevTextEnd       = itsText->SelectAll().GetCount();

	// show the changes

	if (!itsIsPrintingFlag && needRefreshLines)
		{
		TERefreshLines(firstLineIndex, lastLineIndex);
		}

	// This has to be last so everything is correctly set.

	if (itsNeedCaretBcastFlag && itsSelection.IsEmpty())
		{
		BroadcastCaretMessages(itsCaretLoc, lineChanged);
		}
	else if (itsNeedCaretBcastFlag)
		{
		BroadcastCaretMessages(CalcCaretLocation(itsSelection.GetFirst()), kJTrue);
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
	const TextRange&	range,
	JCoordinate*		maxLineWidth,
	JIndex*				firstLineIndex,
	JIndex*				lastLineIndex
	)
{
	JIndex lineIndex = GetLineForChar(range.charRange.first);
	if (!itsBreakCROnlyFlag && lineIndex > 1 && range.charRange.first > 1 &&
		NoPrevWhitespaceOnLine(range.GetFirst()))
		{
		// If we start in the first word on the line, it
		// might actually belong on the previous line.

		lineIndex--;
		}

	JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, GetLineStart(lineIndex));
	const TextIndex& end  = range.GetAfter();
	*firstLineIndex       = lineIndex;

	JIndex runIndex, firstInRun;
	const JBoolean found = itsText->GetStyles().FindRun(iter->GetNextCharacterIndex(), &runIndex, &firstInRun);
	assert( found );

	const TextCount textLength(itsText->GetText().GetCharacterCount(),
							   itsText->GetText().GetByteCount());

	*maxLineWidth = itsWidth;
	while (1)
		{
		JCoordinate lineWidth;
		RecalcLine(iter, lineIndex, &lineWidth, &runIndex, &firstInRun);

		if (*maxLineWidth < lineWidth)
			{
			*maxLineWidth = lineWidth;
			}

		// remove line starts that are further from the end than the new one
		// (we use (bufLength - endChar) so subtraction won't produce negative numbers)

		const JIndex endCharIndex = iter->GetPrevCharacterIndex();

		while (lineIndex < GetLineCount() &&
			   (itsPrevTextEnd.charCount+1) - GetLineStart(lineIndex+1).charIndex >
					textLength.charCount - endCharIndex)
			{
			itsLineStarts->RemoveElement(lineIndex+1);
			itsLineGeom->RemoveElement(lineIndex+1);
			}

		// check if we are done

		if (iter->AtEnd())
			{
			// We reached the end of the text.

			break;
			}
		else if (iter->GetNextCharacterIndex() >= end.charIndex &&
				 lineIndex < GetLineCount() &&
				 (itsPrevTextEnd.charCount+1) - GetLineStart(lineIndex+1).charIndex ==
					textLength.charCount - endCharIndex)
			{
			// The rest of the line starts merely shift.

			const JSize lineCount = GetLineCount();
			const long charDelta  = textLength.charCount - itsPrevTextEnd.charCount,
					   byteDelta  = textLength.byteCount - itsPrevTextEnd.byteCount;
			if (charDelta != 0)
				{
				TextIndex* lineStart = const_cast<TextIndex*>(itsLineStarts->GetCArray());
				for (JIndex i=lineIndex; i<lineCount; i++)
					{
					lineStart[i].charIndex += charDelta;
					lineStart[i].byteIndex += byteDelta;
					}
				}
			break;
			}

		// insert the new line start

		lineIndex++;
		itsLineStarts->InsertElementAtIndex(lineIndex,
			TextIndex(iter->GetNextCharacterIndex(), iter->GetNextByteIndex()));
		itsLineGeom->InsertElementAtIndex(lineIndex, LineGeometry());

		// This catches the case when the new and old line starts
		// are equally far from the end, but we haven't recalculated
		// far enough yet, so the above breakout code didn't trigger.

		if (lineIndex < GetLineCount() &&
			(itsPrevTextEnd.charCount+1) - GetLineStart(lineIndex+1).charIndex ==
				textLength.charCount - endCharIndex)
			{
			itsLineStarts->RemoveElement(lineIndex+1);
			itsLineGeom->RemoveElement(lineIndex+1);
			}
		}

	*lastLineIndex = lineIndex;

	itsText->DisposeConstIterator(iter);
}

/******************************************************************************
 RecalcLine (private)

	Recalculate the line at which the iterator is positioned.  Returns the
	number of characters on the line.  Sets the appropriate values in
	itsLineGeom.  Sets *lineWidth to the width of the line in pixels.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the line.

 ******************************************************************************/

void
JTextEditor::RecalcLine
	(
	JStringIterator*	iter,
	const JIndex		lineIndex,
	JCoordinate*		lineWidth,
	JIndex*				runIndex,
	JIndex*				firstInRun
	)
{
	*lineWidth = 0;

	JIndex gswRunIndex   = *runIndex;
	JIndex gswFirstInRun = *firstInRun;

	const TextIndex first(iter->GetNextCharacterIndex(), iter->GetNextByteIndex());

	if (itsBreakCROnlyFlag)
		{
		iter->Next("\n");

		// newline is single byte
		const TextIndex last(iter->GetPrevCharacterIndex(), iter->GetPrevByteIndex());

		*lineWidth = GetStringWidth(first, last, &gswRunIndex, &gswFirstInRun);
		}

	else
		{
		// include leading whitespace

		JBoolean endOfLine;
		IncludeWhitespaceOnLine(iter, lineWidth, &endOfLine,
								&gswRunIndex, &gswFirstInRun);

		// Add words until we hit the right margin, a newline,
		// or the end of the text.

		while (!iter->AtEnd() && !endOfLine)
			{
			// get the next word

			iter->BeginMatch();

			JUtf8Character c;
			while (iter->Next(&c))	// JRegex is too slow for this intensive operation
				{
				if (c.IsSpace())
					{
					iter->SkipPrev();		// don't ignore whitespace
					break;
					}
				}

			const JStringMatch& m = iter->FinishMatch();

			// check if the word fits on this line

			const TextRange r(m);
			const JCoordinate dw = GetStringWidth(r.GetFirst(), r.GetLast(*itsText),
												  &gswRunIndex, &gswFirstInRun);
			if (itsMaxWordWidth < dw)
				{
				itsMaxWordWidth = dw;
				}

			if (*lineWidth + dw > itsWidth)
				{
				if (r.charRange.first > first.charIndex)
					{
					// this word goes on the next line

					iter->UnsafeMoveTo(kJIteratorStartBefore, r.charRange.first, r.byteRange.first);
					}
				else
					{
					// put as much of this word as possible on the line

					iter->UnsafeMoveTo(kJIteratorStartBefore, first.charIndex, first.byteIndex);

					assert( *lineWidth == 0 );
					GetSubwordForLine(iter, lineIndex, lineWidth);
					}
				break;
				}

			// put the word on this line

			*lineWidth += dw;

			// include the whitespace after the word

			IncludeWhitespaceOnLine(iter, lineWidth, &endOfLine,
									&gswRunIndex, &gswFirstInRun);
			}
		}

	// update geometry for this line

	const JRunArray<JFont>& styles = itsText->GetStyles();

	const JSize runCount = styles.GetRunCount();
	const JSize lastChar = iter->GetPrevCharacterIndex();

	JCoordinate maxAscent=0, maxDescent=0;
	while (*runIndex <= runCount)
		{
		const JFont& f = styles.GetRunDataRef(*runIndex);
		JCoordinate ascent, descent;
		f.GetLineHeight(itsFontManager, &ascent, &descent);

		if (ascent > maxAscent)
			{
			maxAscent = ascent;
			}
		if (descent > maxDescent)
			{
			maxDescent = descent;
			}

		const JIndex firstInNextRun = *firstInRun + styles.GetRunLength(*runIndex);
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
}

/******************************************************************************
 IncludeWhitespaceOnLine (private)

	*** Only for use by RecalcLine()

	Return the number of consecutive whitespace characters encountered.
	Increments *lineWidth with the width of this whitespace.  If we
	encounter a newline, we stop beyond it and set *endOfLine to kJTrue.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the whitespace.

 ******************************************************************************/

void
JTextEditor::IncludeWhitespaceOnLine
	(
	JStringIterator*	iter,
	JCoordinate*		lineWidth,
	JBoolean*			endOfLine,
	JIndex*				runIndex,
	JIndex*				firstInRun
	)
	const
{
	if (iter->AtEnd())
		{
		*endOfLine = kJFalse;
		return;
		}

	*endOfLine = kJFalse;

	TextIndex first(iter->GetNextCharacterIndex(), iter->GetNextByteIndex());

	JUtf8Character c;
	while (iter->Next(&c))
		{
		if (!c.IsSpace())
			{
			iter->SkipPrev();
			break;
			}

		if (c == '\t')
			{
			if (iter->GetPrevCharacterIndex() > first.charIndex)
				{
				*lineWidth += GetStringWidth(	// tab is single byte
					first, TextIndex(iter->GetPrevCharacterIndex()-1, iter->GetPrevByteIndex()-1),
					runIndex, firstInRun);
				}
			*lineWidth += GetTabWidth(iter->GetPrevCharacterIndex(), *lineWidth);
			first       = TextIndex(iter->GetNextCharacterIndex(), iter->GetNextByteIndex());

			// update *runIndex,*firstInRun after passing tab character

			const JSize runLength = itsText->GetStyles().GetRunLength(*runIndex);
			if (first.charIndex > *firstInRun + runLength-1)
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
		else if (c == '\n')
			{
			*endOfLine = kJTrue;
			break;
			}
		}

	JIndex endCharIndex;
	if (iter->GetPrevCharacterIndex(&endCharIndex) && endCharIndex >= first.charIndex)
		{
		const TextIndex last(endCharIndex, iter->GetPrevByteIndex());
		*lineWidth += GetStringWidth(first, last, runIndex, firstInRun);
		}
}

/******************************************************************************
 GetSubwordForLine (private)

	*** Only for use by RecalcLine()

	Skip past the number of characters that will fit on a line.  Sets
	*lineWidth to the width of the line in pixels.  We always put at least
	one character on the line.

 ******************************************************************************/

void
JTextEditor::GetSubwordForLine
	(
	JStringIterator*	iter,
	const JIndex		lineIndex,
	JCoordinate*		lineWidth
	)
	const
{
	*lineWidth = 0;

	JUtf8Character c;
	JBoolean first = kJTrue;
	while (iter->Next(&c, kJFalse))
		{
		const CaretLocation caretLoc(
			TextIndex(iter->GetNextCharacterIndex(), iter->GetNextByteIndex()),
			lineIndex);

		const JCoordinate dw = GetCharWidth(caretLoc, c);
		if (!first && *lineWidth + dw > itsWidth)
			{
			break;
			}
		else
			{
			*lineWidth += dw;
			}
		first = kJFalse;
		iter->SkipNext();
		}
}

/******************************************************************************
 NoPrevWhitespaceOnLine (private)

	Returns kJTrue if there is no whitespace between index-1 and
	the first character on the line.

	Called by Recalc1() to decide whether or not to start on the
	previous line.

 ******************************************************************************/

JBoolean
JTextEditor::NoPrevWhitespaceOnLine
	(
	const TextIndex& index
	)
	const
{
	const TextIndex start = GetLineStart(GetLineForChar(index.charIndex));

	JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, index);
	JUtf8Character c;
	while (!iter->AtBeginning() &&
		   iter->GetPrevCharacterIndex() >= start.charIndex &&
		   iter->Prev(&c))
		{
		if (c.IsSpace())
			{
			itsText->DisposeConstIterator(iter);
			return kJFalse;
			}
		}

	itsText->DisposeConstIterator(iter);
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
	if (itsText->IsEmpty())
		{
		return CaretLocation(TextIndex(1,1),1);
		}
	else if (pt.y >= itsHeight)
		{
		return CaretLocation(itsText->GetBeyondEnd(), GetLineCount());
		}

	// find the line that was clicked on

	JCoordinate lineTop;
	const JIndex lineIndex = CalcLineIndex(pt.y, &lineTop);
	if (itsText->EndsWithNewline() &&
		itsHeight - GetEWNHeight() < pt.y && pt.y <= itsHeight)
		{
		return CaretLocation(itsText->GetBeyondEnd(), GetLineCount());
		}

	// find the closest insertion point

	const TextIndex lineStart = GetLineStart(lineIndex);
	if (pt.x <= 0)
		{
		return CaretLocation(GetLineStart(lineIndex), lineIndex);
		}

	TextIndex lineEnd = GetLineEnd(lineIndex);
	if (lineEnd.charIndex == lineStart.charIndex)
		{
		return CaretLocation(lineStart, lineIndex);
		}

	if ((lineEnd.charIndex < itsText->GetText().GetCharacterCount() ||
		 itsText->EndsWithNewline()) &&
		GetCharacter(lineEnd).IsSpace())
		{
		lineEnd = itsText->AdjustTextIndex(lineEnd, -1);
		}

	TextIndex index;
	JCoordinate x     = 0;
	JCoordinate prevD = pt.x;

	JRunArrayIterator<JFont> fiter(itsText->GetStyles(), kJIteratorStartBefore, lineStart.charIndex);
	JFont f;

	JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, lineStart);
	JUtf8Character c;
	JIndex charIndex;
	while (iter->GetNextCharacterIndex(&charIndex) && charIndex <= lineEnd.charIndex &&
		   iter->Next(&c) && fiter.Next(&f))
		{
		if (c != '\t')
			{
			x += f.GetCharWidth(itsFontManager, c);
			}
		else
			{
			x += GetTabWidth(iter->GetPrevCharacterIndex(), x);
			}

		const JCoordinate d = pt.x - x;
		if (d < 0 && prevD <= -d)
			{
			iter->SkipPrev();
			break;
			}
		else if (d == 0 || (d < 0 && prevD > -d))
			{
			break;
			}

		prevD = d;
		}

	if (iter->AtEnd())
		{
		index = itsText->GetBeyondEnd();
		}
	else
		{
		index = TextIndex(iter->GetNextCharacterIndex(),
						  iter->GetNextByteIndex());
		}
	const CaretLocation loc(index, lineIndex);

	itsText->DisposeConstIterator(iter);

	return loc;
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
	if (byteIndex == 0 && index.charIndex == itsText->GetText().GetCharacterCount() + 1)
		{
		byteIndex = itsText->GetText().GetByteCount() + 1;
		}
	else if (byteIndex == 0)
		{
		const TextIndex i = GetLineStart(lineIndex);
		const TextRange r =
			itsText->CharToTextRange(&i, JCharacterRange(index.charIndex, index.charIndex));
		byteIndex = r.byteRange.first;
		}

	return CaretLocation(TextIndex(index.charIndex, byteIndex), lineIndex);
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
	const JFont f = itsText->CalcInsertionFont(itsText->GetBeyondEnd());
	return f.GetLineHeight(itsFontManager);
}

/******************************************************************************
 GetCharacter (private)

	Only use if you really need only one single character

 ******************************************************************************/

JUtf8Character
JTextEditor::GetCharacter
	(
	const TextIndex& index
	)
	const
{
	JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, index);

	JUtf8Character c;
	const JBoolean ok = iter->Next(&c);
	assert( ok );

	itsText->DisposeConstIterator(iter);
	return c;
}

/******************************************************************************
 IsTrailingNewline (private)

	Returns true if the given index is a trailing newline.

 ******************************************************************************/

JBoolean
JTextEditor::IsTrailingNewline
	(
	const TextIndex& index
	)
	const
{
	return JI2B(index.charIndex == itsText->GetText().GetCharacterCount()+1 &&
				itsText->EndsWithNewline());
}
