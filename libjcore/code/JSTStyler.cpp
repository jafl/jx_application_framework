/******************************************************************************
 JSTStyler.cpp

	This is the base class for an object that can efficiently control the
	styles inside a JStyledText.  Derive a class from JStyledText and
	override AdjustStylesBeforeBroadcast() and pass the arguments directly
	to JSTStyler::UpdateStyles().

	tokenStartList is used to avoid lexing more of the text than necessary.
	Each JStyledText object must own one and must never modify the contents
	between calls to UpdateStyles().

	Derived classes must override the following function:

		Scan
			Read tokens from the given input stream and call SetStyle() for
			each one.  When SetStyle() returns false, stop scanning.

			Every character in the input stream must be part of some token,
			and the tokens must be passed to SetStyle() in the order that they
			occur in the file.

			Every time the lexer reaches a point where it could be safely
			restarted, call SaveTokenStart().  (This cannot be done for every
			token because, for example, '*' can mean either multiplication or
			dereferencing in C, so one must start further back in order to
			get the context.)

	Derived classes can override the following function:

		PreexpandCheckRange
			Called before Scan() to allow the derived class to expand the
			range of characters that need to be checked.  The initial range
			starts with the first character in the style run containing the
			first modified character and ends with the last character in the
			style run containing the last modified character.  These points
			are obviously token boundaries.  In some cases, one needs to check
			a wider range.

	While inside Scan(), one may encounter a token that requires checking
	at least a certain distance (e.g. to the end of the line).  To handle
	this, call ExtendCheckRange() with the index of the last character that
	must be checked.  This should be done before calling SetStyle() for the
	token.

	BASE CLASS = none

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JSTStyler.h"
#include "JStringIterator.h"
#include "JListUtil.h"
#include "JMinMax.h"
#include "jStreamUtil.h"
#include "JStopWatch.h"
#include "jAssert.h"

using TextIndex = JStyledText::TextIndex;
using TextCount = JStyledText::TextCount;
using TextRange = JStyledText::TextRange;

const JSize kDecimationFactor = 50;
const JSize kMinLgListSize    = 7;

#define DEBUG_TIMING_INFO	0	// boolean

/******************************************************************************
 Constructor

 ******************************************************************************/

JSTStyler::JSTStyler()
{
	itsActiveFlag  = true;

	itsST          = nullptr;
	itsFontMgr     = nullptr;
	itsText        = nullptr;
	itsStyles      = nullptr;
	itsIterator    = nullptr;
	itsDefFont     = nullptr;
	itsRecalcRange = nullptr;
	itsRedrawRange = nullptr;

	itsTokenStartList   = nullptr;
	itsDecimationFactor = kDecimationFactor;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTStyler::~JSTStyler()
{
	jdelete itsIterator;	// paranoia
}

/******************************************************************************
 UpdateStyles

	Ranges are inside [1, text.GetCharacterCount()+1]

 ******************************************************************************/

void
JSTStyler::UpdateStyles
	(
	const JStyledText*	st,
	const JString&		text,
	JRunArray<JFont>*	styles,
	TextRange*			recalcRange,
	TextRange*			redrawRange,
	const bool			deletion,
	JArray<TokenData>*	tokenStartList
	)
{
	if (!itsActiveFlag)
	{
		tokenStartList->RemoveAll();
		return;
	}

	const JSize textLength = text.GetCharacterCount();
	if (textLength == 0)
	{
		tokenStartList->RemoveAll();
		return;
	}

	itsIterator = jnew JRunArrayIterator<JFont>(styles);

	TokenData tokenData;
	if (recalcRange->charRange.first == 1 && recalcRange->charRange.last >= textLength)
	{
		itsRedoAllFlag = true;
		itsCheckRange.charRange.Set(1, textLength);
		itsCheckRange.byteRange.Set(1, text.GetByteCount());

		tokenStartList->RemoveAll();
		tokenData = TokenData(TextIndex(1,1), GetFirstTokenExtraData());
		tokenStartList->AppendItem(tokenData);

		styles->RemoveAll();
	}
	else
	{
		itsRedoAllFlag = false;

		// calculate the range that needs to be checked

		JIndex firstIndex = recalcRange->charRange.first;
		JIndex lastIndex  = recalcRange->charRange.last;
		if ((deletion && firstIndex > 1) || firstIndex > textLength)
		{
			// This fixes the case when the last character of the token is deleted.

			firstIndex--;

			// This fixes the case when the style run and the token both
			// end at the inserted text.  (e.g. "x [ y ]" <- "x // y ]")

			lastIndex++;
		}
		if (lastIndex > textLength)
		{
			// We can't decrease recalcRange's end index, and we can't find
			// textLength+1 in *styles.

			lastIndex = textLength;
		}

		itsIterator->MoveTo(JListT::kStartBefore, JMax(lastIndex, (JIndex) 1));		// range can be (1,0)
		const JIndex endIndex = itsIterator->GetRunEnd();

		itsIterator->MoveTo(JListT::kStartBefore, firstIndex);		// see below; faster to start here
		itsCheckRange.charRange.Set(itsIterator->GetRunStart(), endIndex);
		{
		JStringIterator iter(text);
		iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, recalcRange->charRange.first, recalcRange->byteRange.first);
		iter.MoveTo(JStringIterator::kStartBeforeChar, itsIterator->GetRunStart());
		itsCheckRange.byteRange.first = iter.GetNextByteIndex();
		iter.MoveTo(JStringIterator::kStartAfterChar, endIndex);
		itsCheckRange.byteRange.last = iter.GetPrevByteIndex();
		}

		// let derived class expand the range

		TextRange savedRange = itsCheckRange;
		PreexpandCheckRange(text, *styles, recalcRange->charRange, deletion, &itsCheckRange);
		assert( itsCheckRange.byteRange.Contains(savedRange.byteRange) &&
				itsCheckRange.charRange.Contains(savedRange.charRange) &&
				itsCheckRange.charRange.last <= styles->GetItemCount() );

		// find nearest token in front of itsCheckRange

		if (tokenStartList->IsEmpty())
		{
			tokenData = TokenData(TextIndex(1,1), GetFirstTokenExtraData());
			tokenStartList->AppendItem(tokenData);
		}
		else
		{
			bool foundTokenStart;
			TokenData target(itsCheckRange.GetFirst(), TokenExtra());
			JIndex tokenStartIndex =
				tokenStartList->SearchSortedOTI(target, JListT::kAnyMatch, &foundTokenStart);
			if (!foundTokenStart)
			{
				tokenStartIndex--;	// wants to insert -after- the value
			}
			tokenData = tokenStartList->GetItem(tokenStartIndex);

			// the rest of the token starts are invalid

			const JSize tokenStartCount = tokenStartList->GetItemCount();
			if (tokenStartIndex < tokenStartCount)
			{
				tokenStartList->RemoveNextItems(tokenStartIndex+1, tokenStartCount - tokenStartIndex);
			}
		}
	}

	// prepare to accumulate new token starts

	itsTokenStartList    = tokenStartList;
	itsTokenStartCounter = 0;

	itsIterator->MoveTo(JListT::kStartBefore, tokenData.startIndex.charIndex);

	// scan the text and adjust the styles

	icharbufstream input(text.GetRawBytes(), text.GetByteCount());
	JSeekg(input, tokenData.startIndex.byteIndex-1);

	JFont f = st->GetDefaultFont();

	itsST          = st;
	itsDefFont     = &f;
	itsText        = &text;
	itsStyles      = styles;
	itsRecalcRange = recalcRange;
	itsRedrawRange = redrawRange;

	#if DEBUG_TIMING_INFO
	JStopWatch timer;
	timer.StartTimer();
	#endif

	Scan(tokenData.startIndex, input, tokenData.data);

	#if DEBUG_TIMING_INFO
	timer.StopTimer();
	std::cout << "JSTStyler: " << timer.PrintTimeInterval() << std::endl;
	#endif

	jdelete itsIterator;
	itsIterator = nullptr;

	itsST             = nullptr;
	itsDefFont        = nullptr;
	itsText           = nullptr;
	itsStyles         = nullptr;
	itsRecalcRange    = nullptr;
	itsRedrawRange    = nullptr;
	itsTokenStartList = nullptr;
}

/******************************************************************************
 PreexpandCheckRange (virtual protected)

	*checkRange is only allowed to expand.  The default is to do nothing.

	modifiedRange is the range of text that was changed.
	deletion is true if the modification was that text was deleted.

 ******************************************************************************/

void
JSTStyler::PreexpandCheckRange
	(
	const JString&			text,
	const JRunArray<JFont>&	styles,
	const JCharacterRange&	modifiedRange,
	const bool				deletion,
	TextRange*				checkRange
	)
{
}

/******************************************************************************
 ExtendCheckRange (protected)

	Extends the end of the check range.  It is safe to call this with
	an index beyond the end of the text.

 ******************************************************************************/

void
JSTStyler::ExtendCheckRange
	(
	const JIndex newEndCharIndex
	)
{
	if (itsCheckRange.charRange.last < newEndCharIndex)
	{
		JStringIterator iter(*itsText);
		iter.UnsafeMoveTo(JStringIterator::kStartAfterChar, itsCheckRange.charRange.last, itsCheckRange.byteRange.last);
		iter.MoveTo(JStringIterator::kStartAfterChar, JMin(newEndCharIndex, itsText->GetCharacterCount()));
		itsCheckRange.charRange.last = iter.GetPrevCharacterIndex();
		itsCheckRange.byteRange.last = iter.GetPrevByteIndex();
	}
}

/******************************************************************************
 SetStyle (protected)

	Returns true if the lexer should keep going.

 ******************************************************************************/

bool
JSTStyler::SetStyle
	(
	const JCharacterRange&	range,
	const JFontStyle&		style
	)
{
	assert( !range.IsEmpty() );

	if (itsCheckRange.charRange.last < range.first)
	{
		// we are beyond the range where anything could have changed
		return false;
	}

	assert( itsIterator->AtEnd() ||
			(range.first == itsIterator->GetNextItemIndex()) );

	if (itsRedoAllFlag)
	{
		JFont f = *itsDefFont;
		f.SetStyle(style);
		itsStyles->AppendItems(f, range.GetCount());
	}
	else if (range.last >= itsCheckRange.charRange.first)
	{
		const JCharacterRange fontRange(itsIterator->GetRunStart(), itsIterator->GetRunEnd());
		const bool beyondCurrentRun = !fontRange.Contains(range);

		JFont f                     = itsIterator->GetRunData();
		const JFontStyle& origStyle = f.GetStyle();

		const bool styleExtendsBeyondToken =
			!origStyle.IsBlank() && range.last < fontRange.last;

		if (beyondCurrentRun || styleExtendsBeyondToken || style != origStyle)
		{
			// extend the check range if we slop over into another style run
			// (HTML: type '<' after 'x' in "x<!--<br><h3>text</h3>-->")

			// extend past end of run to insure that we keep going until we
			// find a correctly styled token

			if (beyondCurrentRun || styleExtendsBeyondToken)
			{
				JRunArrayIterator<JFont> iter(*itsIterator);
				iter.MoveTo(JListT::kStartBefore, range.last);
				ExtendCheckRange(iter.GetRunEnd()+1);
			}

			// extend check range to next token since this token was not
			// correctly styled -- should only stop when find correctly
			// styled tokens

			else	// style != f.style
			{
				ExtendCheckRange(range.last+1);
			}

			// update the styles

			if (!beyondCurrentRun && OnlyColorChanged(style, origStyle))
			{
				ExpandTextRange(itsRedrawRange, range);
			}
			else
			{
				ExpandTextRange(itsRecalcRange, range);
			}

			f = *itsDefFont;
			f.SetStyle(style);

			if (!itsIterator->SetNext(f, range.GetCount()))
			{
				itsIterator->RemoveNext(range.GetCount());
				itsIterator->Insert(f, range.GetCount());
			}
		}
	}

	itsIterator->MoveTo(JListT::kStartAfter, range.last);
	return range.last < itsCheckRange.charRange.last;
}

/******************************************************************************
 NewTokenStartList (static)

	Allocates a tokenStartList with a reasonable block size.

 ******************************************************************************/

JArray<JSTStyler::TokenData>*
JSTStyler::NewTokenStartList()
{
	auto* list = jnew JArray<TokenData>(kMinLgListSize);
	list->SetSortOrder(JListT::kSortAscending);
	list->SetCompareFunction(CompareTokenStarts);
	return list;
}

/******************************************************************************
 SaveTokenStart (protected)

	Automatically decimates the stream of token starts by itsDecimationFactor.

 ******************************************************************************/

void
JSTStyler::SaveTokenStart
	(
	const JStyledText::TextIndex&	index,
	const TokenExtra				data
	)
{
	itsTokenStartCounter++;
	if (itsTokenStartCounter >= itsDecimationFactor)
	{
		itsTokenStartCounter = 0;
		itsTokenStartList->AppendItem(TokenData(index, data));
	}
}

/******************************************************************************
 GetFirstTokenExtraData (virtual protected)

	Derived classes can override this to store additional data.

 ******************************************************************************/

JSTStyler::TokenExtra
JSTStyler::GetFirstTokenExtraData()
	const
{
	return TokenExtra();
}

/******************************************************************************
 CompareTokenStarts (static private)

 ******************************************************************************/

std::weak_ordering
JSTStyler::CompareTokenStarts
	(
	const TokenData& t1,
	const TokenData& t2
	)
{
	return t1.startIndex.charIndex <=> t2.startIndex.charIndex;
}

/******************************************************************************
 AdjustStyle (protected)

	Call this to change the style of something that the lexer has already
	processed.

 ******************************************************************************/

void
JSTStyler::AdjustStyle
	(
	const JCharacterRange&	range,
	const JFontStyle&		style
	)
{
	assert( range.last <= itsIterator->GetPrevItemIndex() );
	assert( itsRecalcRange != nullptr );

	ExpandTextRange(itsRecalcRange, range);

	// adjust the styles

	JRunArrayIterator<JFont> iter(*itsIterator);
	iter.MoveTo(JListT::kStartAfter, range.first);

	JFont f;
	const bool ok = iter.Prev(&f);
	assert( ok );

	f.SetStyle(style);
	iter.SetNext(f, range.GetCount());
 }

/******************************************************************************
 OnlyColorChanged (private)

	We don't use JFontStyle& because we modify the values.

 ******************************************************************************/

inline bool
JSTStyler::OnlyColorChanged
	(
	JFontStyle s1,
	JFontStyle s2
	)
	const
{
	s1.color = s2.color = 0;	// make sure the color is the same
	return s1 == s2;		// avoids maintenance when fields are added
}

/******************************************************************************
 ExpandTextRange (private)

 ******************************************************************************/

void
JSTStyler::ExpandTextRange
	(
	TextRange*				r1,
	const JCharacterRange&	r2
	)
	const
{
	if (r1->charRange.Contains(r2))
	{
		return;
	}

	const JIndexRange cr = r1->charRange + r2;

	const TextIndex start =
		itsST->AdjustTextIndex(r1->GetFirst(), cr.first - r1->charRange.first);

	const TextIndex after =
		itsST->AdjustTextIndex(r1->GetAfter(), cr.last - r1->charRange.last);

	r1->Set(start, after);
}

/******************************************************************************
 GetCharacter (protected)

	Only use if you really need only one single character

 ******************************************************************************/

JUtf8Character
JSTStyler::GetCharacter
	(
	const TextIndex& index
	)
	const
{
	return JUtf8Character(itsText->GetBytes() + index.byteIndex - 1);
}
