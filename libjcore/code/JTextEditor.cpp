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
			Returns true if the given bounds width is too wide for the
			current graphics system to handle.  If so, breakCROnly is
			turned off.

		TEScrollToRect
			Scroll the text to make the given rectangle visible, possibly
			centering it in the middle of the display.  Return true
			if scrolling was necessary.

		TEScrollForDrag
			Scroll the text to make the given point visible.  Return true
			if scrolling was necessary.

		TEScrollForDND
			Same as TEScrollForDrag(), but separate, since some frameworks
			do the scrolling automatically.

		TESetVertScrollStep
			Set the vertical step size and page context used when scrolling
			the text.

		TECaretShouldBlink
			If blink is true, reset the timer and make the caret blink by
			calling TEShow/HideCaret().  Otherwise, call TEHideCaret().
			The derived class constructor must call TECaretShouldBlink(true)
			because it is pure virtual for us.

		TEUpdateClipboard
			Update the system clipboard with the given text/style.

		TEGetClipboard
			Returns true if there is something pasteable on the system
			clipboard.

		TEBeginDND
			Returns true if it is able to start a Drag-And-Drop session
			with the OS.  From then on, the derived class should call the
			TEHandleDND*() functions.  It must also call TEDNDFinished()
			when the drag ends.  If TEBeginDND() returns false, then we
			will manage an internal DND session.  In this case, the derived
			class should continue to call the usual TEHandleMouse*()
			functions.

		TEPasteDropData
			Get the data that was dropped and use Paste(text,style)
			to insert it.  (Before this is called, the insertion point
			is set so that Paste() will work correctly.)

		TEHasSearchText
			Return true if the user has entered any text for which to search.

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

#include "jx-af/jcore/JTextEditor.h"
#include "jx-af/jcore/JTEDefaultKeyHandler.h"
#include "jx-af/jcore/JPagePrinter.h"
#include "jx-af/jcore/JFontManager.h"
#include "jx-af/jcore/JListUtil.h"
#include "jx-af/jcore/JUndo.h"
#include "jx-af/jcore/JRunArrayIterator.h"
#include "jx-af/jcore/JRegex.h"
#include "jx-af/jcore/JStringIterator.h"
#include "jx-af/jcore/JStringMatch.h"
#include "jx-af/jcore/JLatentPG.h"
#include "jx-af/jcore/JMinMax.h"
#include "jx-af/jcore/jTextUtil.h"
#include "jx-af/jcore/jASCIIConstants.h"
#include "jx-af/jcore/jFStreamUtil.h"
#include "jx-af/jcore/jStreamUtil.h"
#include "jx-af/jcore/jMouseUtil.h"
#include "jx-af/jcore/jFileUtil.h"
#include "jx-af/jcore/jTime.h"
#include "jx-af/jcore/jGlobals.h"
#include "jx-af/jcore/jAssert.h"

using TextIndex = JStyledText::TextIndex;
using TextCount = JStyledText::TextCount;
using TextRange = JStyledText::TextRange;

using FontIterator     = JRunArrayIterator<JFont>;
using GeometryIterator = JRunArrayIterator<JTextEditor::LineGeometry>;

const JCoordinate kDefLeftMarginWidth = 10;
const JCoordinate kRightMarginWidth   = 2;

bool JTextEditor::theCopyWhenSelectFlag = false;

// JBroadcaster message types

const JUtf8Byte* JTextEditor::kTypeChanged          = "TypeChanged::JTextEditor";
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
	const bool		ownsText,
	JFontManager*		fontManager,
	const bool		breakCROnly,
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

	itsKeyHandler(nullptr),		// make SetKeyHandler() safe
	itsDragType(kInvalidDrag),
	itsPrevDragType(kInvalidDrag),
	itsIsDragSourceFlag(false)
{
	assert( itsText != nullptr );

	itsActiveFlag              = false;
	itsSelActiveFlag           = false;
	itsCaretVisibleFlag        = false;
	itsPerformDNDFlag          = false;
	itsMoveToFrontOfTextFlag   = false;
	itsBreakCROnlyFlag         = breakCROnly;
	itsIsPrintingFlag          = false;
	itsDrawWhitespaceFlag      = false;
	itsAlwaysShowSelectionFlag = false;
	itsCaretMode               = kLineCaret;

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

	itsCaret         = CaretLocation(TextIndex(1,1),1);
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
 Copy constructor

	if text is not nullptr, we take ownership of it.

	If text is nullptr and the source does not own the JStyledText, we will
	share it.

 ******************************************************************************/

JTextEditor::JTextEditor
	(
	const JTextEditor&	source,
	JStyledText*		text
	)
	:
	itsType(source.itsType),
	itsOwnsTextFlag(text != nullptr || source.itsOwnsTextFlag),

	itsFontManager(source.itsFontManager),

	itsCaretColor(source.itsCaretColor),
	itsSelectionColor(source.itsSelectionColor),
	itsSelectionOutlineColor(source.itsSelectionOutlineColor),
	itsWhitespaceColor(source.itsWhitespaceColor),

	itsKeyHandler(nullptr),		// make SetKeyHandler() safe
	itsDragType(kInvalidDrag),
	itsPrevDragType(kInvalidDrag),
	itsIsDragSourceFlag(false)
{
	if (itsOwnsTextFlag && text != nullptr)
	{
		itsText = text;
	}
	else if (itsOwnsTextFlag)
	{
		itsText = jnew JStyledText(*source.itsText);
		assert( itsText != nullptr );
	}
	else
	{
		itsText = source.itsText;
	}

	itsActiveFlag              = false;
	itsSelActiveFlag           = false;
	itsCaretVisibleFlag        = false;
	itsPerformDNDFlag          = source.itsPerformDNDFlag;
	itsMoveToFrontOfTextFlag   = source.itsMoveToFrontOfTextFlag;
	itsBreakCROnlyFlag         = source.itsBreakCROnlyFlag;
	itsIsPrintingFlag          = false;
	itsDrawWhitespaceFlag      = source.itsDrawWhitespaceFlag;
	itsAlwaysShowSelectionFlag = source.itsAlwaysShowSelectionFlag;
	itsCaretMode               = kLineCaret;

	itsWidth           = source.itsWidth;
	itsHeight          = source.itsHeight;
	itsGUIWidth        = source.itsGUIWidth;
	itsLeftMarginWidth = source.itsLeftMarginWidth;
	itsDefTabWidth     = source.itsDefTabWidth;
	itsMaxWordWidth    = source.itsMaxWordWidth;

	itsLineStarts = jnew JArray<TextIndex>(*source.itsLineStarts);
	assert( itsLineStarts != nullptr );

	itsLineGeom = jnew JRunArray<LineGeometry>(*source.itsLineGeom);
	assert( itsLineGeom != nullptr );

	itsPrevTextLastIndex = source.itsPrevTextLastIndex;
	itsPrevTextEnd       = source.itsPrevTextEnd;

	itsCaret         = CaretLocation(TextIndex(1,1),1);
	itsCaretX        = 0;
	itsInsertionFont = itsText->CalcInsertionFont(TextIndex(1,1));

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

		RecalcAll(false);
		SetCaretLocation(CaretLocation(TextIndex(1,1),1));
		Broadcast(message);		// safer for others to receive now
	}

	else if (sender == itsText &&
			 message.Is(JStyledText::kTextChanged))
	{
		const auto* info =
			dynamic_cast<const JStyledText::TextChanged*>(&message);
		assert( info != nullptr );
		const TextRange& rcr = info->GetRecalcRange();
		if (rcr.charRange.GetCount() == itsText->GetText().GetCharacterCount())
		{
			RecalcAll(false);
		}
		else
		{
			Recalc(rcr, info->GetRedrawRange());
		}

		const TextRange& r = info->GetRange();

		const JInteger cd = info->GetCharDelta(),
					   bd = info->GetByteDelta();

		if (itsSelection.IsEmpty() &&
			r.charRange.last - cd < itsCaret.location.charIndex)
		{
			itsCaret.location.charIndex += cd;
			itsCaret.location.byteIndex += bd;
			SetCaretLocation(CalcCaretLocation(itsCaret.location));
		}
		else if (itsSelection.IsEmpty() &&
				 r.charRange.first < itsCaret.location.charIndex)
		{
			itsCaret.location.charIndex = r.charRange.first;
			itsCaret.location.byteIndex = r.byteRange.first;
			SetCaretLocation(CalcCaretLocation(itsCaret.location));
		}
		else if (!itsSelection.IsEmpty() &&
				 r.charRange.last - cd < itsSelection.charRange.first)
		{
			TextRange r  = itsSelection;
			r.charRange += cd;
			r.byteRange += bd;
			SetSelection(r, true, true);
		}
		else if (!itsSelection.IsEmpty() &&
				 r.charRange.first < itsSelection.charRange.first)
		{
			SetCaretLocation(CalcCaretLocation(r.GetFirst()));
		}
		else if (!itsSelection.IsEmpty() &&
				 r.charRange.first < itsSelection.charRange.last)
		{
			SetCaretLocation(CalcCaretLocation(itsSelection.GetFirst()));
		}

		Broadcast(message);		// safer for others to receive now
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

	else if (sender == itsText &&
			 message.Is(JStyledText::kUndoFinished))
	{
		const auto* info =
			dynamic_cast<const JStyledText::UndoFinished*>(&message);
		assert( info != nullptr );
		const TextRange r = info->GetRange();
		if (itsActiveFlag && !r.IsEmpty())
		{
			SetSelection(r);
			TEScrollToSelection(false);
		}
		else if (itsActiveFlag)
		{
			SetCaretLocation(r.GetFirst());
			TEScrollTo(itsCaret);
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
	If we find it, we select it and return true.

 ******************************************************************************/

JStringMatch
JTextEditor::SearchForward
	(
	const JRegex&	regex,
	const bool		entireWord,
	const bool		wrapSearch,
	bool*			wrapped
	)
{
	const TextIndex i(
		itsSelection.IsEmpty() ? itsCaret.location.charIndex :
		itsSelection.charRange.last + 1,

		itsSelection.IsEmpty() ? itsCaret.location.byteIndex :
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
	If we find it, we select it and return true.

 ******************************************************************************/

JStringMatch
JTextEditor::SearchBackward
	(
	const JRegex&	regex,
	const bool		entireWord,
	const bool		wrapSearch,
	bool*			wrapped
	)
{
	const TextIndex i(
		itsSelection.IsEmpty() ? itsCaret.location.charIndex :
		itsSelection.charRange.first,

		itsSelection.IsEmpty() ? itsCaret.location.byteIndex :
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

	Returns true if the current selection matches the given search criteria.
	This returns the JStringMatch required for a replace.

 ******************************************************************************/

JStringMatch
JTextEditor::SelectionMatches
	(
	const JRegex&	regex,
	const bool		entireWord
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
	const bool			preserveCase
	)
{
	assert( HasSelection() );

	const JIndex charIndex = itsSelection.charRange.first;
	const JIndex byteIndex = itsSelection.byteRange.first;

	const TextCount count =
		itsText->ReplaceMatch(match, replaceStr, interpolator, preserveCase);

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
	Returns true if it modified the text.

 ******************************************************************************/

bool
JTextEditor::ReplaceAll
	(
	const JRegex&	regex,
	const bool		entireWord,
	const JString&	replaceStr,
	JInterpolate*	interpolator,
	const bool		preserveCase,
	const bool		restrictToSelection
	)
{
	if (restrictToSelection && !HasSelection())
	{
		return false;
	}

	TextRange r(
		restrictToSelection ? itsSelection.charRange :
		JCharacterRange(1, itsText->GetText().GetCharacterCount()),

		restrictToSelection ? itsSelection.byteRange :
		JUtf8ByteRange(1, itsText->GetText().GetByteCount()));

	r = itsText->ReplaceAllInRange(r, regex, entireWord,
								   replaceStr, interpolator, preserveCase);

	if (restrictToSelection && !r.IsEmpty())
	{
		SetSelection(r);
		return true;
	}
	else if (!r.IsEmpty())
	{
		SetCaretLocation(itsText->GetBeyondEnd());
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 SearchForward

	Look for the next match beyond the current position.
	If we find it, we select it and return true.

 ******************************************************************************/

bool
JTextEditor::SearchForward
	(
	std::function<bool(const JFont&)>	match,

	const bool	wrapSearch,
	bool*		wrapped
	)
{
	const TextIndex i(
		itsSelection.IsEmpty() ? itsCaret.location.charIndex :
		itsSelection.charRange.last + 1,

		itsSelection.IsEmpty() ? itsCaret.location.byteIndex :
		itsSelection.byteRange.last + 1);

	TextRange range;
	const bool found = itsText->SearchForward(match, i, wrapSearch, wrapped, &range);
	if (found)
	{
		SetSelection(range);
	}

	return found;
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return true.

 ******************************************************************************/

bool
JTextEditor::SearchBackward
	(
	std::function<bool(const JFont&)>	match,

	const bool	wrapSearch,
	bool*		wrapped
	)
{
	const TextIndex i(
		itsSelection.IsEmpty() ? itsCaret.location.charIndex :
		itsSelection.charRange.first,

		itsSelection.IsEmpty() ? itsCaret.location.byteIndex :
		itsSelection.byteRange.first);

	TextRange range;
	const bool found = itsText->SearchBackward(match, i, wrapSearch, wrapped, &range);
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
	const bool breakCROnly
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
	const bool			breakCROnly,
	const bool			clearUndo
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
	the function returns true.

 ******************************************************************************/

bool
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

	const bool hadSelection = HasSelection();
	if (hadSelection)
	{
		range = itsSelection;
	}
	else
	{
		range.charRange.SetFirstAndCount(itsCaret.location.charIndex, 0);
		range.byteRange.SetFirstAndCount(itsCaret.location.byteIndex, 0);
	}

	JRunArray<JFont>* style1 = nullptr;
	if (style == nullptr)
	{
		style1 = jnew JRunArray<JFont>;
		assert( style1 != nullptr );

		JFont f = itsInsertionFont;
		if (hadSelection)
		{
			FontIterator iter(
				itsText->GetStyles(), kJIteratorStartBefore, range.charRange.first);
			iter.Next(&f);
		}

		style1->AppendElements(f, text.GetCharacterCount());
	}

	range = itsText->Paste(range, text, style != nullptr ? style : style1);
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
	const bool			needCaretBcast,
	const bool			ignoreCopyWhenSelect
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

	const bool hadSelection            = !itsSelection.IsEmpty();
	const CaretLocation origCaretLoc       = itsCaret;
	const JUtf8ByteRange origByteSelection = itsSelection.byteRange;

	itsCaret     = CaretLocation();
	itsSelection = range;

	const JIndex newStartLine = GetLineForByte(itsSelection.byteRange.first);
	const JIndex newEndLine   = GetLineForByte(itsSelection.byteRange.last);

	if (needCaretBcast)
	{
		BroadcastCaretMessages(
			CaretLocation(itsSelection.GetFirst(), newStartLine));
	}

	TECaretShouldBlink(false);

	if (!ignoreCopyWhenSelect && theCopyWhenSelectFlag && itsType != kStaticText)
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
		const JStyledText::TextRange r = itsSelection;
		SetCaretLocation(r.GetFirst());
		itsText->DeleteText(r);
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
	const JSize	tabCount,
	const bool	force
	)
{
	if (TEIsDragging() || itsText->IsEmpty())
	{
		return;
	}

	const TextIndex start = itsText->GetParagraphStart(
		itsSelection.IsEmpty() ? itsCaret.location : itsSelection.GetFirst());

	TextIndex beyondEnd = itsSelection.IsEmpty() ? itsCaret.location : itsSelection.GetAfter();
	if (beyondEnd.charIndex == start.charIndex)
	{
		if (GetCharacter(start) == '\n')
		{
			return;
		}
		beyondEnd = itsText->AdjustTextIndex(beyondEnd, +1);
	}

	TextRange r(start, beyondEnd);
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

	const TextIndex start = itsText->GetParagraphStart(
		itsSelection.IsEmpty() ? itsCaret.location : itsSelection.GetFirst());

	TextIndex beyondEnd = itsSelection.IsEmpty() ? itsCaret.location : itsSelection.GetAfter();
	if (beyondEnd.charIndex == start.charIndex)
	{
		if (GetCharacter(start) == '\n')
		{
			return;
		}
		beyondEnd = itsText->AdjustTextIndex(beyondEnd, +1);
	}

	TextRange r(start, beyondEnd);
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
	bool useSpaces, isMixed;
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
	const bool align
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

	GeometryIterator iter(itsLineGeom);
	LineGeometry geom;

	JIndex prev = 1, i = 0;
	JCoordinate h = 0;
	do
	{
		// find the number of strips that will fit on this page

		while (i < count && h <= pageHeight)
		{
			i++;
			const bool ok = iter.Next(&geom);
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
			const bool ok = iter.Prev(&geom);
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

	itsIsPrintingFlag = true;

	if (!p.OpenDocument())
	{
		itsIsPrintingFlag = false;
		return;
	}

	const JCoordinate headerHeight = GetPrintHeaderHeight(p);
	const JCoordinate footerHeight = GetPrintFooterHeight(p);

	// We deactivate before printing so the selection or the
	// caret won't be printed.

	const bool savedActive = TEIsActive();
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

	bool cancelled = false;
	for (JIndex i=1; i<=pageCount; i++)
	{
		if (!p.NewPage())
		{
			cancelled = true;
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

	itsIsPrintingFlag = false;

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

JArray<bool>
JTextEditor::GetCmdStatus
	(
	JString*	crmActionText,
	JString*	crm2ActionText,
	bool*	isReadOnly
	)
	const
{
	JArray<bool> flags(kCmdCount);
	for (JIndex i=1; i<=kCmdCount; i++)
	{
		flags.AppendElement(false);
	}

	*isReadOnly = true;

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
		flags.SetElement(kCopyCmd, true);
		*crmActionText  = JGetString("CRMSelectionAction::JTextEditor");
		*crm2ActionText = JGetString("CRM2SelectionAction::JTextEditor");
	}

	flags.SetElement(kSelectAllCmd,      true);
	flags.SetElement(kToggleReadOnlyCmd, true);
	flags.SetElement(kShowWhitespaceCmd, true);

	if (itsType == kFullEditor)
	{
		*isReadOnly = false;

		flags.SetElement(kPasteCmd, true);
		if (!itsSelection.IsEmpty())
		{
			flags.SetElement(kCutCmd,       true);
			flags.SetElement(kDeleteSelCmd, true);
		}

		if (!itsText->IsEmpty())
		{
			flags.SetElement(kCheckSpellingCmd,      true);
			flags.SetElement(kCleanRightMarginCmd,   true);
			flags.SetElement(kCoerceRightMarginCmd,  true);
			flags.SetElement(kShiftSelLeftCmd,       true);
			flags.SetElement(kShiftSelRightCmd,      true);
			flags.SetElement(kForceShiftSelLeftCmd,  true);
			flags.SetElement(kCleanAllWhitespaceCmd, true);
			flags.SetElement(kCleanAllWSAlignCmd,    true);

			if (!itsSelection.IsEmpty())
			{
				flags.SetElement(kCheckSpellingSelCmd,   true);
				flags.SetElement(kCleanWhitespaceSelCmd, true);
				flags.SetElement(kCleanWSAlignSelCmd,    true);
			}
		}

		bool canUndo, canRedo;
		if (itsText->HasMultipleUndo(&canUndo, &canRedo))
		{
			flags.SetElement(kUndoCmd, canUndo);
			flags.SetElement(kRedoCmd, canRedo);
		}
		else if (itsText->HasSingleUndo())
		{
			flags.SetElement(kUndoCmd, true);
			flags.SetElement(kRedoCmd, false);
		}
	}

	// Search & Replace menu

	flags.SetElement(kFindDialogCmd, true);

	if (!itsText->IsEmpty())
	{
		flags.SetElement(kFindClipboardBackwardCmd, true);
		flags.SetElement(kFindClipboardForwardCmd,  true);

		if (TEHasSearchText())
		{
			flags.SetElement(kFindNextCmd,     true);
			flags.SetElement(kFindPreviousCmd, true);

			if (itsType == kFullEditor)
			{
				flags.SetElement(kReplaceAllCmd,  true);

				if (HasSelection())
				{
					flags.SetElement(kReplaceSelectionCmd,      true);
					flags.SetElement(kReplaceFindNextCmd,       true);
					flags.SetElement(kReplaceFindPrevCmd,       true);
					flags.SetElement(kReplaceAllInSelectionCmd, true);
				}
			}
		}
	}

	if (HasSelection())
	{
		flags.SetElement(kEnterSearchTextCmd,       true);
		flags.SetElement(kEnterReplaceTextCmd,      true);
		flags.SetElement(kFindSelectionBackwardCmd, true);
		flags.SetElement(kFindSelectionForwardCmd,  true);
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

	// if DND, draw drop location

	if (itsDragType == kDragAndDrop && itsDropLoc.location.charIndex > 0)
	{
		TEDrawCaret(p, itsDropLoc);
	}

	// otherwise, draw insertion caret

	else if (itsActiveFlag && itsCaretVisibleFlag && itsSelection.IsEmpty() &&
			 itsType == kFullEditor)
	{
		TEDrawCaret(p, itsCaret);
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

	if ((itsActiveFlag || itsAlwaysShowSelectionFlag) &&
		!itsSelection.IsEmpty() && itsType != kStaticText)
	{
		TEDrawSelection(p, rect, startLine, h);
	}

	// draw text, one line at a time

	JStringIterator* textIter = itsText->GetConstIterator(kJIteratorStartBefore, GetLineStart(startLine));
	FontIterator styleIter(itsText->GetStyles(), kJIteratorStartBefore, GetLineStart(startLine).charIndex);

	GeometryIterator geomIter(itsLineGeom, kJIteratorStartBefore, startLine);
	LineGeometry geom;
	for (JIndex i=startLine; i<=lineCount; i++)
	{
		const bool ok = geomIter.Next(&geom);
		assert( ok );

		TEDrawLine(p, h, geom, textIter, &styleIter, i);

		h += geom.height;
		if (h >= rect.bottom)
		{
			break;
		}
	}

	itsText->DisposeConstIterator(textIter);
}

/******************************************************************************
 TEDrawLine (private)

	Draw the text on the given line.

 ******************************************************************************/

inline void
teDrawSpaces
	(
	JPainter&			p,
	const JInteger		direction,		// +1/-1
	JStringIterator*	textIter,
	FontIterator*		styleIter,
	const JCoordinate	left,
	const JCoordinate	ycenter,
	const JColorID		wsColor
	)
{
	const JIndex charIndex = textIter->GetNextCharacterIndex(),
				 byteIndex = textIter->GetNextByteIndex();

	p.SetLineWidth(1);
	p.SetPenColor(wsColor);

	JCoordinate l = left, w = 0;
	JUtf8Character c;
	JFont f, prevf;
	while (textIter->Next(&c) && c == ' ')
	{
		const bool ok = direction == +1 ? styleIter->Next(&f) : styleIter->Prev(&f);
		assert( ok );

		if (f != prevf)
		{
			w     = f.GetCharWidth(p.GetFontManager(), JUtf8Character(' '));
			prevf = f;
		}

		if (direction == -1)
		{
			l -= w;
		}
		p.Point(l + w/2 - 1, ycenter);
		if (direction == +1)
		{
			l += w;
		}
	}

	textIter->UnsafeMoveTo(kJIteratorStartBefore, charIndex, byteIndex);
	styleIter->MoveTo(kJIteratorStartBefore, charIndex);
}

void
JTextEditor::TEDrawLine
	(
	JPainter&			p,
	const JCoordinate	top,
	const LineGeometry&	geom,
	JStringIterator*	textIter,
	FontIterator*		styleIter,
	const JIndex		lineIndex
	)
{
	TEDrawInMargin(p, JPoint(-itsLeftMarginWidth, top), geom, lineIndex);

	bool wsInit = false;
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

	JIndex startChar = textIter->GetNextCharacterIndex();
	JIndex endChar   = startChar + lineLength.charCount - 1;
	JCoordinate left = 0;
	JUtf8Character c;
	JFont f;
	bool lineEndsWithNewline = false;

	while (startChar <= endChar)
	{
		// If the line starts with spaces, we have to draw them.

		if (!wsInit)
		{
			if (itsDrawWhitespaceFlag)
			{
				teDrawSpaces(p, +1,  textIter, styleIter, left, wsYCenter, itsWhitespaceColor);
			}
			wsInit = true;
		}

		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JCharacterRange cr(startChar, JMin(endChar, styleIter->GetRunEnd()));

		textIter->BeginMatch();

		bool foundTab = false;
		JIndex ci;
		while (textIter->GetNextCharacterIndex(&ci) && ci <= cr.last)
		{
			const bool ok = textIter->Next(&c);
			assert( ok );

			if (c == '\t')
			{
				textIter->SkipPrev();
				foundTab = true;
				break;
			}
		}

		if (c == '\n')	// some fonts draw stuff for \n
		{
			textIter->SkipPrev();
			endChar--;
			lineEndsWithNewline = true;
		}

		const JStringMatch& m = textIter->FinishMatch();
		if (!m.IsEmpty())
		{
			styleIter->MoveTo(kJIteratorStartBefore, textIter->GetPrevCharacterIndex());
			styleIter->Next(&f);
			p.SetFont(f);

			JCoordinate ascent,descent;
			p.GetLineHeight(&ascent, &descent);

			const JString s = m.GetString();
			p.StringNoSubstitutions(left, top + geom.ascent - ascent, s);

			// we only care if there is more text on the line

			startChar += m.GetCharacterCount();
			if (startChar <= endChar || itsDrawWhitespaceFlag)
			{
				left += f.GetStringWidth(itsFontManager, s);	// no substitutions
			}
		}

		if (foundTab)
		{
			const JCoordinate tabWidth = GetTabWidth(startChar, left);

			if (itsDrawWhitespaceFlag)
			{
				p.SetLineWidth(1);
				p.SetPenColor(itsWhitespaceColor);

				const JCoordinate box = JMax(4L, JMin(10L, wsRect.height(), tabWidth));

				wsRect.left  = left;
				wsRect.right = left + box;

				const JCoordinate xc = wsRect.xcenter();
				const JCoordinate yc = wsRect.ycenter();
				const JPoint topCenter   (xc, yc - box/2);
				const JPoint left        (wsRect.left, yc);
				const JPoint bottomCenter(xc, yc + box/2);
				const JPoint right       (wsRect.right, yc);

				p.Line(left, right);
				p.Line(topCenter, right);
				p.LineTo(bottomCenter);
			}

			JIndex i;
			if (itsDrawWhitespaceFlag && textIter->GetPrevCharacterIndex(&i))
			{
				teDrawSpaces(p, -1,  textIter, styleIter, left, wsYCenter, itsWhitespaceColor);
			}

			startChar++;
			left += tabWidth;
			textIter->SkipNext();	// move past tab character
			styleIter->SkipNext();

			if (itsDrawWhitespaceFlag && textIter->GetNextCharacterIndex(&i))
			{
				teDrawSpaces(p, +1,  textIter, styleIter, left, wsYCenter, itsWhitespaceColor);
			}
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

			if (endChar > 1)
			{
				teDrawSpaces(p, -1,  textIter, styleIter, left, wsYCenter, itsWhitespaceColor);
			}
		}

		textIter->SkipNext();	// skip over newline character
		styleIter->SkipNext();
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
	assert( !itsSelection.IsEmpty() );

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
	const bool savedFill     = p.IsFilling();
	if (itsSelActiveFlag)
	{
		p.SetPenColor(itsSelectionColor);
		p.SetFilling(true);
	}
	else
	{
		p.SetPenColor(itsSelectionOutlineColor);
		p.SetFilling(false);
	}

	const JCoordinate xmax = JMax(itsGUIWidth, itsWidth);

	GeometryIterator iter(itsLineGeom, kJIteratorStartBefore, startLine);
	LineGeometry geom;
	for (JIndex i=startLine; i<=endLine; i++)
	{
		const bool ok = iter.Next(&geom);
		assert( ok );

		JCoordinate w;
		TextIndex endChar = GetLineEnd(i);
		if (endChar.charIndex > itsSelection.charRange.last)
		{
			endChar = itsSelection.GetLast(*itsText);
			w       = GetStringWidth(startChar, endChar, nullptr);
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
			if (c == '\t')
			{
				w = GetTabWidth(caretLoc.location.charIndex, GetCharLeft(caretLoc));
			}
			else if (c != '\n')
			{
				FontIterator iter(itsText->GetStyles(), kJIteratorStartBefore, caretLoc.location.charIndex);
				JFont f;
				iter.Next(&f);
				w = f.GetCharWidth(itsFontManager, c);
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

bool
JTextEditor::TEWillDragAndDrop
	(
	const JPoint&	pt,
	const bool	extendSelection,
	const bool	dropCopy
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
		return false;
	}
}

/******************************************************************************
 PointInSelection (protected)

 ******************************************************************************/

bool
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

		return itsSelection.charRange.first <= caretLoc.location.charIndex &&
					caretLoc.location.charIndex <= itsSelection.charRange.last+1;
	}
	else
	{
		return false;
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
	const bool	extendSelection,
	const bool	partialWord
	)
{
	assert( itsActiveFlag );

	itsDragType         = kInvalidDrag;
	itsIsDragSourceFlag = false;
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
		itsSelectionPivot = itsCaret.location;	// save this before SetSelection()

		if (caretLoc.location.charIndex < itsCaret.location.charIndex)
		{
			SetSelection(TextRange(caretLoc.location, itsCaret.location));
		}
		else if (itsCaret.location.charIndex < caretLoc.location.charIndex)
		{
			SetSelection(TextRange(itsCaret.location, caretLoc.location));
		}
	}
	else if (clickCount == 1)
	{
		if (caretLoc != itsCaret)
		{
			SetCaretLocation(caretLoc);
		}
		itsDragType       = kSelectDrag;
		itsSelectionPivot = caretLoc.location;
	}
	else if (clickCount == 2)
	{
		itsDragType = (partialWord ? kSelectPartialWordDrag : kSelectWordDrag);
		TEGetDoubleClickSelection(caretLoc.location, partialWord, false,
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
		SetSelection(TextRange(caretLoc.location, itsSelectionPivot), false);
		BroadcastCaretMessages(caretLoc);
	}
	else if (itsDragType == kSelectDrag && caretLoc.location.charIndex == itsSelectionPivot.charIndex)
	{
		SetCaretLocation(caretLoc);
	}
	else if (itsDragType == kSelectDrag && caretLoc.location.charIndex > itsSelectionPivot.charIndex)
	{
		SetSelection(TextRange(itsSelectionPivot, caretLoc.location), false);
		BroadcastCaretMessages(caretLoc);
	}
	else if (itsDragType == kSelectWordDrag || itsDragType == kSelectPartialWordDrag)
	{
		TextRange range;
		TEGetDoubleClickSelection(caretLoc.location,
								  itsDragType == kSelectPartialWordDrag,
								  true, &range);

		const JIndexRange cr = JCovering(itsWordSelPivot.charRange, range.charRange),
						  br = JCovering(itsWordSelPivot.byteRange, range.byteRange);
		SetSelection(TextRange(
			*static_cast<const JCharacterRange*>(&cr),
			*static_cast<const JUtf8ByteRange*>(&br)),
			false);

		BroadcastCaretMessages(caretLoc);
	}
	else if (itsDragType == kSelectLineDrag)
	{
		SetSelection(TextRange(
			GetLineStart( JMin(itsLineSelPivot, caretLoc.lineIndex) ),
			itsText->AdjustTextIndex(GetLineEnd( JMax(itsLineSelPivot, caretLoc.lineIndex) ), +1)),
			false);
		BroadcastCaretMessages(caretLoc);
	}
	else if (itsDragType == kDragAndDrop && JMouseMoved(itsStartPt, pt))
	{
		itsDragType = kInvalidDrag;		// wait for TEHandleDNDEnter()
		if (TEBeginDND())
		{
			itsIsDragSourceFlag = true;
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
	else if (itsDragType == kDragAndDrop)
	{
		SetCaretLocation(CalcCaretLocation(itsPrevPt));
		itsPrevDragType = kInvalidDrag;
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
	TERefreshCaret(itsCaret);		// hide the typing caret
}

/******************************************************************************
 TEHandleDNDHere (protected)

 ******************************************************************************/

void
JTextEditor::TEHandleDNDHere
	(
	const JPoint&	origPt,
	const bool	dropOnSelf
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
		caretLoc.location.charIndex   < itsSelection.charRange.first ||
		itsSelection.charRange.last+1 < caretLoc.location.charIndex)
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
	const bool	dropOnSelf,
	const bool	dropCopy
	)
{
	assert( itsDragType == kDragAndDrop );

	TEHandleDNDHere(pt, dropOnSelf);

	if (dropOnSelf && itsDropLoc.location.charIndex > 0)
	{
		TextRange r;
		const bool ok = itsText->MoveText(itsSelection, itsDropLoc.location, dropCopy, &r);
		assert( ok );

		SetSelection(r);
		TEScrollToSelection(false);
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
	const bool			partialWord,
	const bool			dragging,
	TextRange*			range
	)
	const
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
			const bool ok1 = iter->Prev(&c1, kJIteratorStay);
			const bool ok2 = iter->Next(&c2, kJIteratorStay);
			assert( ok1 && ok2 );

			if (c1 != '\n' && c2 == '\n')
			{
				iter->SkipPrev();
			}
		}

		const JIndex b1 = iter->GetNextByteIndex();
		iter->SkipNext();

		JIndex b2;
		if (iter->AtEnd())
		{
			b2 = itsText->GetText().GetByteCount() + 1;
		}
		else
		{
			b2 = iter->GetNextByteIndex();
		}

		*range = TextRange(
			TextIndex(iter->GetPrevCharacterIndex(), b1),
			TextCount(1, b2-b1));

		itsText->DisposeConstIterator(iter);
	}
}

/******************************************************************************
 TEHitSamePart

 ******************************************************************************/

bool
JTextEditor::TEHitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	const CaretLocation loc1 = CalcCaretLocation(pt1);
	const CaretLocation loc2 = CalcCaretLocation(pt2);
	return loc1 == loc2;
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
		handler = jnew JTEDefaultKeyHandler;
		assert( handler != nullptr );
	}

	itsKeyHandler = handler;
	itsKeyHandler->Initialize(this);
}

/******************************************************************************
 TEHandleKeyPress (protected)

	Returns true if the key was processed.

 ******************************************************************************/

bool
JTextEditor::TEHandleKeyPress
	(
	const JUtf8Character&	origKey,
	const bool			selectText,
	const CaretMotion		motion,
	const bool			deleteToTabStop
	)
{
	assert( itsActiveFlag );
	assert( (!itsSelection.IsEmpty() && itsCaret.location.charIndex == 0) ||
			( itsSelection.IsEmpty() && itsCaret.location.charIndex >  0) );

	if (TEIsDragging())
	{
		return true;
	}

	// pre-processing

	JUtf8Character key = origKey;
	if (key == '\r')
	{
		key = '\n';
	}

	// handler

	const bool processed =
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
 InsertCharacter (private)

 ******************************************************************************/

void
JTextEditor::InsertCharacter
	(
	const JUtf8Character& c
	)
{
	const bool hadSelection = !itsSelection.IsEmpty();
	if (hadSelection)
	{
		FontIterator iter(itsText->GetStyles(), kJIteratorStartBefore, itsSelection.charRange.first);
		iter.Next(&itsInsertionFont);

		itsCaret = CalcCaretLocation(itsSelection.GetFirst());
	}

	TextIndex loc = itsCaret.location;
	TextCount insertCount;

	JUndo* undo = itsText->InsertCharacter(
		hadSelection ? itsSelection : TextRange(itsCaret.location, TextCount(0,0)),
		c, itsInsertionFont, &insertCount);

	loc += insertCount;
	SetCaretLocation(loc);

	undo->Activate();		// cancel SetCaretLocation()
}

/******************************************************************************
 BackwardDelete (private)

	Delete characters preceding the insertion caret.

 ******************************************************************************/

void
JTextEditor::BackwardDelete
	(
	const bool		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle
	)
{
	assert( itsSelection.IsEmpty() );

	JUndo* undo;
	const TextIndex i =
		itsText->BackwardDelete(GetLineStart(itsCaret.lineIndex),
								itsCaret.location, deleteToTabStop,
								returnText, returnStyle, &undo);

	JFont f          = JFontManager::GetDefaultFont();
	bool setFont = false;
	if (i.charIndex > 1)
	{
		FontIterator iter(itsText->GetStyles(), kJIteratorStartBefore, i.charIndex);
		iter.Prev(&f);
		setFont = true;
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
	const bool		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle
	)
{
	assert( itsSelection.IsEmpty() );

	JUndo* undo;
	itsText->ForwardDelete(GetLineStart(itsCaret.lineIndex), itsCaret.location,
						   deleteToTabStop, returnText, returnStyle, &undo);

	SetCaretLocation(itsCaret);

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

// protected

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

	const bool hadSelection      = !itsSelection.IsEmpty();
	const CaretLocation origCaretLoc = itsCaret;

	if (hadSelection)
	{
		TERefreshLines(GetLineForChar(itsSelection.charRange.first),
					   GetLineForChar(itsSelection.charRange.last));
	}

	itsSelection.SetToNothing();
	itsCaret  = caretLoc;
	itsCaretX = GetCharLeft(itsCaret);

	if (hadSelection || origCaretLoc != itsCaret || !itsText->WillPasteStyledText())
	{
		itsInsertionFont = itsText->CalcInsertionFont(itsCaret.location);
	}

	if (!TEScrollTo(itsCaret))
	{
		TERefreshCaret(origCaretLoc);
		TERefreshCaret(itsCaret);
	}

	BroadcastCaretMessages(itsCaret);

	TECaretShouldBlink(true);
}

/******************************************************************************
 BroadcastCaretMessages (private)

 ******************************************************************************/

void
JTextEditor::BroadcastCaretMessages
	(
	const CaretLocation& caretLoc
	)
{
	JIndex line = caretLoc.lineIndex;
	JIndex col  = GetColumnForChar(caretLoc);
	if (IsTrailingNewline(caretLoc.location))
	{
		line++;
		col = 1;
	}
	Broadcast(CaretLocationChanged(line, col, caretLoc.location.charIndex));
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
		JIndex charIndex;
		while (col < columnIndex && iter->GetNextCharacterIndex(&charIndex) &&
			   charIndex <= end.charIndex && iter->Next(&c))
		{
			col += (c == '\t' ? itsText->CRMGetTabWidth(col) : 1);
		}

		if (iter->AtEnd())
		{
			caretLoc.location = itsText->GetBeyondEnd();
		}
		else
		{
			caretLoc.location.charIndex = iter->GetNextCharacterIndex();
			caretLoc.location.byteIndex = iter->GetNextByteIndex();
		}

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

	TEScrollToRect(CalcCaretRect(caretLoc), true);
	if (IsReadOnly())
	{
		SetSelection(TextRange(
			GetLineStart(caretLoc.lineIndex),
			itsText->AdjustTextIndex(GetLineEnd(caretLoc.lineIndex), +1)));
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
			GetLineStart(itsCaret.lineIndex),
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
		caretLoc = itsCaret;
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
		JSize prefixLength;
		JIndex ruleIndex = 0;
		TextIndex beyondLastChar = itsText->AdjustTextIndex(lastChar, +1);
		if (lastChar.charIndex < firstChar.charIndex ||
			!itsText->CRMGetPrefix(&firstTextChar, lastChar,
								   &linePrefix, &prefixLength, &ruleIndex) ||
			itsText->CRMLineMatchesRest(TextRange(firstChar, beyondLastChar)))
		{
			firstTextChar = beyondLastChar;
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
	else if (IsTrailingNewline(itsCaret.location))
	{
		SetCaretLocation(itsCaret);	// scroll to it
	}
	else if (!itsText->IsEmpty())
	{
		const TextIndex index  = GetLineEnd(itsCaret.lineIndex);
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
		const TextIndex newIndex = itsText->GetParagraphEnd(itsText->AdjustTextIndex(index, +1));
		if (newIndex.charIndex == index.charIndex)	// end of text
		{
			break;
		}
		index = newIndex;
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
	if (itsBreakCROnlyFlag || itsText->IsEmpty())
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

	Scroll to the specified insertion point.  Returns true if scrolling
	was actually necessary.

 ******************************************************************************/

bool
JTextEditor::TEScrollTo
	(
	const CaretLocation& caretLoc
	)
{
	return TEScrollToRect(CalcCaretRect(caretLoc), false);
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

	Scroll to make the selection or caret visible.  Returns true if scrolling
	was actually necessary.

 ******************************************************************************/

bool
JTextEditor::TEScrollToSelection
	(
	const bool centerInDisplay
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
		return TEScrollToRect(CalcCaretRect(itsCaret), centerInDisplay);
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
	const JIndex lineIndex = itsCaret.lineIndex;

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

	bool found;
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

	bool found;
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
 GetCharLeft

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
		x = GetStringWidth(firstChar, itsText->AdjustTextIndex(charLoc.location, -1), nullptr);
	}
	return x;
}

/******************************************************************************
 GetCharRight

	Returns the ending x coordinate of the specified character, excluding
	the left margin width.

 ******************************************************************************/

JCoordinate
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
		x = GetStringWidth(firstChar, charLoc.location, nullptr);
	}
	return x;
}

/******************************************************************************
 GetStringWidth (private)

	Returns the width of the specified character range.

	styleIter can be nullptr.

 ******************************************************************************/

JCoordinate
JTextEditor::GetStringWidth
	(
	const TextIndex&	origStart,
	const TextIndex&	end,
	FontIterator*		styleIter
	)
	const
{
	assert( itsText->GetText().CharacterIndexValid(origStart.charIndex) );
	assert( itsText->GetText().CharacterIndexValid(end.charIndex) );

	bool allocatedStyleIter = false;
	if (styleIter == nullptr)
	{
		allocatedStyleIter = true;

		styleIter = jnew FontIterator(itsText->GetStyles(), kJIteratorStartBefore, origStart.charIndex);
		assert( styleIter != nullptr );
	}
	else
	{
		styleIter->MoveTo(kJIteratorStartBefore, origStart.charIndex);
	}

	// preWidth stores the width of the characters preceding origStartIndex
	// on the line containing origStartIndex.  We calculate this -once- when
	// it is first needed.  (i.e. when we hit the first tab character)

	JCoordinate width    = 0;
	JCoordinate preWidth = -1;

	TextIndex start = origStart;
	while (start.charIndex <= end.charIndex)
	{
		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JIndex endCharIndex = JMin(styleIter->GetRunEnd(), end.charIndex);

		JIndex tabCharIndex, pretabByteIndex;
		const bool foundTab = LocateTab(start, endCharIndex, &tabCharIndex, &pretabByteIndex);
		if (foundTab)
		{
			endCharIndex = tabCharIndex - 1;
		}

		if (endCharIndex >= start.charIndex)
		{
			width += styleIter->GetRunData().GetStringWidth(itsFontManager,
				JString(itsText->GetText().GetBytes(),
						JUtf8ByteRange(start.byteIndex, pretabByteIndex),
						JString::kNoCopy));
		}

		if (foundTab)
		{
			if (preWidth < 0)
			{
				// recursion: max depth 1
				preWidth = GetCharLeft(CalcCaretLocation(origStart));
				assert( preWidth >= 0 );
			}
			endCharIndex++;
			width += GetTabWidth(endCharIndex, preWidth + width);
		}

		start = itsText->AdjustTextIndex(start, endCharIndex - start.charIndex + 1);
		styleIter->MoveTo(kJIteratorStartAfter, endCharIndex);
	}

	if (allocatedStyleIter)
	{
		jdelete styleIter;
	}

	return width;
}

/******************************************************************************
 LocateTab (private)

	Returns the index of the first tab character, starting from startIndex.
	If no tab is found, returns false, and tabCharIndex = 0.

 ******************************************************************************/

bool
JTextEditor::LocateTab
	(
	const TextIndex&	start,
	const JIndex		endCharIndex,
	JIndex*				tabCharIndex,
	JIndex*				pretabByteIndex
	)
	const
{
	JStringIterator* iter = itsText->GetConstIterator(kJIteratorStartBefore, start);

	JUtf8Character c;
	JIndex i = start.charIndex;
	while (i <= endCharIndex && iter->Next(&c))
	{
		if (c == '\t')
		{
			iter->SkipPrev();
			*tabCharIndex    = iter->GetNextCharacterIndex();
			*pretabByteIndex = (iter->AtBeginning() ? 0 : iter->GetPrevByteIndex());

			itsText->DisposeConstIterator(iter);
			return true;
		}
		i++;
	}

	*tabCharIndex    = 0;
	*pretabByteIndex = iter->GetPrevByteIndex();

	itsText->DisposeConstIterator(iter);
	return false;
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
	const bool broadcastCaretMessages
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

	const TextRange r(TextIndex(1,1),
		TextCount(itsText->GetText().GetCharacterCount(),
				  itsText->GetText().GetByteCount()));

	Recalc(r, r);

	if (broadcastCaretMessages && HasSelection())
	{
		BroadcastCaretMessages(CalcCaretLocation(itsSelection.GetFirst()));
	}
	else if (broadcastCaretMessages)
	{
		BroadcastCaretMessages(itsCaret);
	}
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
		RecalcRange(recalcRange, &maxLineWidth, &firstLineIndex, &lastLineIndex);

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

	if (itsSelection.IsEmpty())
	{
		itsCaret.lineIndex = GetLineForChar(itsCaret.location.charIndex);
	}

	// If we only break at newlines, we control our width.

	if (itsBreakCROnlyFlag && maxLineWidth > itsWidth)
	{
		itsWidth = maxLineWidth;
	}

	// If all the lines are the same height, set the scrolling step size to that.

	if (itsLineGeom->AllElementsEqual() && !itsIsPrintingFlag)
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

	bool needRefreshLines = true;
	if (!itsIsPrintingFlag && newHeight != itsHeight)
	{
		TERefresh();	// redraw everything if the line heights changed
		needRefreshLines = false;
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

	// avoid ludicrous width -- recursive

	if (TEWidthIsBeyondDisplayCapacity(itsWidth))
	{
		SetBreakCROnly(false);
	}
}

/******************************************************************************
 RecalcRange (private)

	Subroutine called by Recalc().

	(firstLineIndex, lastLineIndex) gives the range of lines that had
	to be recalculated.

 ******************************************************************************/

void
JTextEditor::RecalcRange
	(
	const TextRange&	range,
	JCoordinate*		maxLineWidth,
	JIndex*				firstLineIndex,
	JIndex*				lastLineIndex
	)
{
	JIndex lineIndex = GetLineForChar(range.charRange.first);
	if (lineIndex > 1 &&
		(range.charRange.first > itsText->GetText().GetCharacterCount() ||

		// If we start in the first word on the line, it
		// might actually belong on the previous line.

		(!itsBreakCROnlyFlag && range.charRange.first > 1 &&
		 NoPrevWhitespaceOnLine(range.GetFirst()))))
	{
		lineIndex--;
	}

	JStringIterator* textIter = itsText->GetConstIterator(kJIteratorStartBefore, GetLineStart(lineIndex));
	const TextIndex& end      = range.GetAfter();
	*firstLineIndex           = lineIndex;

	const TextCount textLength(itsText->GetText().GetCharacterCount(),
							   itsText->GetText().GetByteCount());

	FontIterator styleIter(itsText->GetStyles(), kJIteratorStartBefore, textIter->GetNextCharacterIndex());
	GeometryIterator geomIter(itsLineGeom, kJIteratorStartBefore, lineIndex);

	*maxLineWidth = itsWidth;
	while (true)
	{
		JCoordinate lineWidth;
		RecalcLine(textIter, &styleIter, &geomIter, lineIndex, &lineWidth);
		// geomIter has been incremented

		if (*maxLineWidth < lineWidth)
		{
			*maxLineWidth = lineWidth;
		}

		// remove line starts that are further from the end than the new one
		// (we use (bufLength - endChar) so subtraction won't produce negative numbers)

		const JIndex endCharIndex = textIter->GetPrevCharacterIndex();

		while (lineIndex < GetLineCount() &&
			   (itsPrevTextEnd.charCount+1) - GetLineStart(lineIndex+1).charIndex >
					textLength.charCount - endCharIndex)
		{
			itsLineStarts->RemoveElement(lineIndex+1);
			geomIter.RemoveNext();
		}

		// check if we are done

		if (textIter->AtEnd())
		{
			// We reached the end of the text.

			assert( styleIter.AtEnd() );
			break;
		}
		else if (textIter->GetNextCharacterIndex() >= end.charIndex &&
				 lineIndex < GetLineCount() &&
				 (itsPrevTextEnd.charCount+1) - GetLineStart(lineIndex+1).charIndex ==
					textLength.charCount - endCharIndex)
		{
			// The rest of the line starts merely shift.

			const JSignedOffset charDelta = textLength.charCount - itsPrevTextEnd.charCount,
								byteDelta = textLength.byteCount - itsPrevTextEnd.byteCount;
			if (charDelta != 0 || byteDelta != 0)
			{
				auto* lineStart  = const_cast<TextIndex*>(itsLineStarts->GetCArray());
				const JSize lineCount = GetLineCount();
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
			TextIndex(textIter->GetNextCharacterIndex(), textIter->GetNextByteIndex()));
		geomIter.Insert(LineGeometry());	// next iteration will fill in values

		// This catches the case when the new and old line starts
		// are equally far from the end, but we haven't recalculated
		// far enough yet, so the above breakout code didn't trigger.

		if (lineIndex < GetLineCount() &&
			(itsPrevTextEnd.charCount+1) - GetLineStart(lineIndex+1).charIndex ==
				textLength.charCount - endCharIndex)
		{
			itsLineStarts->RemoveElement(lineIndex+1);

			geomIter.SkipNext();
			geomIter.RemoveNext();
			geomIter.SkipPrev();
		}
	}

	*lastLineIndex = lineIndex;

	itsText->DisposeConstIterator(textIter);
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
	JStringIterator*	textIter,
	FontIterator*		styleIter,
	GeometryIterator*	geomIter,
	const JIndex		lineIndex,
	JCoordinate*		lineWidth
	)
{
	*lineWidth = 0;

	const TextIndex first(textIter->GetNextCharacterIndex(), textIter->GetNextByteIndex());

	if (itsBreakCROnlyFlag)
	{
		textIter->Next("\n");

		// newline is single byte
		const TextIndex last(textIter->GetPrevCharacterIndex(), textIter->GetPrevByteIndex());

		*lineWidth = GetStringWidth(first, last, styleIter);
	}

	else
	{
		// include leading whitespace

		bool endOfLine;
		IncludeWhitespaceOnLine(textIter, styleIter, lineWidth, &endOfLine);

		// Add words until we hit the right margin, a newline,
		// or the end of the text.

		const JSize maxCharsPerLine = JLCeil(itsGUIWidth / 3.0);	// at least 3 pixels per character

		while (!textIter->AtEnd() && !endOfLine)
		{
			// get the next word

			textIter->BeginMatch();
			JSize count = 0;

			JUtf8Character c;
			while (textIter->Next(&c) && count <= maxCharsPerLine)
			{
				if (c.IsSpace())	// JRegex is too slow for this intensive operation
				{
					textIter->SkipPrev();		// don't ignore whitespace
					break;
				}
				count++;
			}

			const JStringMatch& m = textIter->FinishMatch();

			// check if the word fits on this line

			const TextRange r(m);
			const JCoordinate dw = GetStringWidth(r.GetFirst(), r.GetLast(*itsText), styleIter);
			if (itsMaxWordWidth < dw)
			{
				itsMaxWordWidth = dw;
			}

			if (*lineWidth + dw > itsWidth)
			{
				if (r.charRange.first > first.charIndex)
				{
					// this word goes on the next line

					textIter->UnsafeMoveTo(kJIteratorStartBefore, r.charRange.first, r.byteRange.first);
					styleIter->MoveTo(kJIteratorStartBefore, r.charRange.first);
				}
				else
				{
					// put as much of this word as possible on the line

					textIter->UnsafeMoveTo(kJIteratorStartBefore, first.charIndex, first.byteIndex);
					styleIter->MoveTo(kJIteratorStartBefore, first.charIndex);

					assert( *lineWidth == 0 );
					GetSubwordForLine(textIter, styleIter, lineIndex, lineWidth);
				}
				break;
			}

			// put the word on this line

			*lineWidth += dw;

			// include the whitespace after the word

			IncludeWhitespaceOnLine(textIter, styleIter, lineWidth, &endOfLine);
		}
	}

	// update geometry for this line

	styleIter->MoveTo(kJIteratorStartBefore, first.charIndex);
	const JSize lastChar = textIter->GetPrevCharacterIndex();

	JCoordinate maxAscent=0, maxDescent=0;
	JFont f;
	while (styleIter->GetRunStart() <= lastChar)
	{
		JCoordinate ascent, descent;
		styleIter->GetRunData().GetLineHeight(itsFontManager, &ascent, &descent);

		maxAscent  = JMax(maxAscent, ascent);
		maxDescent = JMax(maxDescent, descent);

		styleIter->NextRun();
	}

	styleIter->MoveTo(kJIteratorStartAfter, lastChar);

	const LineGeometry geom(maxAscent+maxDescent, maxAscent);
	if (geomIter->AtEnd())
	{
		geomIter->Insert(geom);
		geomIter->SkipNext();
	}
	else
	{
		geomIter->SetNext(geom);
	}
}

/******************************************************************************
 IncludeWhitespaceOnLine (private)

	*** Only for use by RecalcLine()

	Return the number of consecutive whitespace characters encountered.
	Increments *lineWidth with the width of this whitespace.  If we
	encounter a newline, we stop beyond it and set *endOfLine to true.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the whitespace.

 ******************************************************************************/

void
JTextEditor::IncludeWhitespaceOnLine
	(
	JStringIterator*	textIter,
	FontIterator*		styleIter,
	JCoordinate*		lineWidth,
	bool*			endOfLine
	)
	const
{
	if (textIter->AtEnd())
	{
		*endOfLine = false;
		return;
	}

	*endOfLine = false;

	TextIndex first(textIter->GetNextCharacterIndex(), textIter->GetNextByteIndex());

	JUtf8Character c;
	while (textIter->Next(&c))
	{
		if (!c.IsSpace())
		{
			textIter->SkipPrev();
			break;
		}

		if (c == '\t')
		{
			const JIndex i = textIter->GetPrevCharacterIndex();
			if (i > first.charIndex)
			{
				*lineWidth += GetStringWidth(	// tab is single byte
					first, TextIndex(i-1, textIter->GetPrevByteIndex()-1),
					styleIter);
			}
			*lineWidth += GetTabWidth(i, *lineWidth);

			styleIter->SkipNext();

			if (!textIter->AtEnd())
			{
				first = TextIndex(textIter->GetNextCharacterIndex(), textIter->GetNextByteIndex());

				// tab characters can wrap to the next line

				if (!itsBreakCROnlyFlag && *lineWidth > itsWidth)
				{
					*endOfLine = true;
					break;
				}
			}
		}
		else if (c == '\n')
		{
			*endOfLine = true;
			break;
		}
	}

	JIndex endCharIndex;
	if (textIter->GetPrevCharacterIndex(&endCharIndex) && endCharIndex >= first.charIndex)
	{
		styleIter->MoveTo(kJIteratorStartBefore, first.charIndex);

		const TextIndex last(endCharIndex, textIter->GetPrevByteIndex());
		*lineWidth += GetStringWidth(first, last, styleIter);
	}
	else
	{
		styleIter->MoveTo(kJIteratorStartBefore, textIter->GetNextCharacterIndex());
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
	JStringIterator*	textIter,
	FontIterator*		styleIter,
	const JIndex		lineIndex,
	JCoordinate*		lineWidth
	)
	const
{
	*lineWidth = 0;

	JUtf8Character c;
	JFont f;
	bool first = true;
	while (textIter->Next(&c, kJIteratorStay))
	{
		JCoordinate dw;
		if (c == '\t')
		{
			dw = GetTabWidth(textIter->GetNextCharacterIndex(), *lineWidth);
		}
		else
		{
			styleIter->Next(&f, kJIteratorStay);
			dw = f.GetCharWidth(itsFontManager, c);
		}

		if (!first && *lineWidth + dw > itsWidth)
		{
			break;
		}
		else
		{
			*lineWidth += dw;
		}
		first = false;
		textIter->SkipNext();
		styleIter->SkipNext();
	}
}

/******************************************************************************
 NoPrevWhitespaceOnLine (private)

	Returns true if there is no whitespace between index-1 and
	the first character on the line.

	Called by RecalcRange() to decide whether or not to start on the
	previous line.

 ******************************************************************************/

bool
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
			return false;
		}
	}

	itsText->DisposeConstIterator(iter);
	return true;	// we hit the start of the string
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
	if (lineEnd.charIndex == lineStart.charIndex &&
		GetCharacter(lineEnd).IsSpace())
	{
		return CaretLocation(lineStart, lineIndex);
	}
	else if (lineEnd.charIndex == lineStart.charIndex)
	{
		return CaretLocation(itsText->AdjustTextIndex(lineStart, 1), lineIndex);
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

	FontIterator fiter(itsText->GetStyles(), kJIteratorStartBefore, lineStart.charIndex);
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
 CalcCaretLocation (protected)

	byteIndex can be zero.

 ******************************************************************************/

JTextEditor::CaretLocation
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
	return JUtf8Character(itsText->GetText().GetBytes() + index.byteIndex - 1);
}

/******************************************************************************
 IsTrailingNewline (private)

	Returns true if the given index is a trailing newline.

 ******************************************************************************/

bool
JTextEditor::IsTrailingNewline
	(
	const TextIndex& index
	)
	const
{
	return index.charIndex == itsText->GetText().GetCharacterCount()+1 &&
				itsText->EndsWithNewline();
}
