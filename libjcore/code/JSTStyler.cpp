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
			each one.  When SetStyle() returns kJFalse, stop scanning.

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
#include "JFontManager.h"
#include "JListUtil.h"
#include "JMinMax.h"
#include "jStreamUtil.h"
#include "JStopWatch.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"

#if defined __GNUC__ && ! defined __clang__
	// hack for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431
	#undef __DEPRECATED
#endif

#include <strstream>

#if defined __GNUC__ && ! defined __clang__
	#define __DEPRECATED 1
#endif

#include "jAssert.h"

typedef JStyledText::TextIndex TextIndex;
typedef JStyledText::TextCount TextCount;
typedef JStyledText::TextRange TextRange;

const JSize kDecimationFactor = 50;
const JSize kListBlockSize    = 50;

#define DEBUG_RUN_INFO		0	// boolean
#define DEBUG_TIMING_INFO	0	// boolean

#define run_assert(styles, index, runIndex, firstIndexInRun) \
	assert( check_run(styles, index, runIndex, firstIndexInRun) )

#if DEBUG_RUN_INFO

static JIndex debugRunIndex, debugFirstInRun;

#define check_run(styles, index, runIndex, firstIndexInRun) \
	styles->FindRun(index, &debugRunIndex, &debugFirstInRun) && \
	debugRunIndex == runIndex && debugFirstInRun == firstIndexInRun

#else

#define check_run(styles, index, runIndex, firstIndexInRun)		kJTrue

#endif

/******************************************************************************
 Constructor

 ******************************************************************************/

JSTStyler::JSTStyler()
{
	itsActiveFlag  = kJTrue;

	itsST          = nullptr;
	itsFontMgr     = nullptr;
	itsText        = nullptr;
	itsStyles      = nullptr;
	itsDefFont     = nullptr;
	itsRecalcRange = nullptr;
	itsRedrawRange = nullptr;

	itsTokenStartList   = nullptr;
	itsTokenStartFactor = kDecimationFactor;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTStyler::~JSTStyler()
{
}

/******************************************************************************
 UpdateStyles

	*range is inside [1, text.GetCharacterCount()+1]

 ******************************************************************************/

void
JSTStyler::UpdateStyles
	(
	const JStyledText*	st,
	const JString&		text,
	JRunArray<JFont>*	styles,
	TextRange*			recalcRange,
	TextRange*			redrawRange,
	const JBoolean		deletion,
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

	TokenData tokenData;
	if (recalcRange->charRange.first == 1 && recalcRange->charRange.last >= textLength)
		{
		itsRedoAllFlag = kJTrue;
		itsCheckRange.Set(1, textLength);

		tokenStartList->RemoveAll();
		tokenData = TokenData(1, GetFirstTokenExtraData());
		tokenStartList->AppendElement(tokenData);

		styles->RemoveAll();
		}
	else
		{
		itsRedoAllFlag = kJFalse;

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

		JIndex runIndex1, firstIndexInRun1;
		JBoolean ok = styles->FindRun(firstIndex, &runIndex1, &firstIndexInRun1);
		assert( ok );

		JIndex runIndex2        = runIndex1;
		JIndex firstIndexInRun2 = firstIndexInRun1;
		ok = styles->FindRun(firstIndex, lastIndex, &runIndex2, &firstIndexInRun2);
		assert( ok );
		run_assert(styles, lastIndex, runIndex2, firstIndexInRun2);

		itsCheckRange.Set(firstIndexInRun1, firstIndexInRun2 + styles->GetRunLength(runIndex2)-1);

		// let derived class expand the range

		JCharacterRange savedRange = itsCheckRange;
		PreexpandCheckRange(text, *styles, recalcRange->charRange, deletion, &itsCheckRange);
		assert( itsCheckRange.Contains(savedRange) &&
				itsCheckRange.last <= styles->GetElementCount() );

		// find nearest token in front of itsCheckRange

		if (tokenStartList->IsEmpty())
			{
			tokenData = TokenData(1, GetFirstTokenExtraData());
			tokenStartList->AppendElement(tokenData);
			}
		else
			{
			JBoolean foundTokenStart;
			TokenData target(itsCheckRange.first, TokenExtra());
			JIndex tokenStartIndex =
				tokenStartList->SearchSorted1(target, JListT::kAnyMatch, &foundTokenStart);
			if (!foundTokenStart)
				{
				tokenStartIndex--;	// wants to insert -after- the value
				}
			tokenData = tokenStartList->GetElement(tokenStartIndex);

			// the rest of the token starts are invalid

			const JSize tokenStartCount = tokenStartList->GetElementCount();
			if (tokenStartIndex < tokenStartCount)
				{
				tokenStartList->RemoveNextElements(tokenStartIndex+1, tokenStartCount - tokenStartIndex);
				}
			}

		// While typing in one place, it is much faster to back up from itsCheckRange
		// than to start from the top.

		itsTokenRunIndex   = runIndex1;
		itsTokenFirstInRun = firstIndexInRun1;
		ok = styles->FindRun(firstIndex, tokenData.startIndex, &itsTokenRunIndex, &itsTokenFirstInRun);
		assert( ok );
		run_assert(styles, tokenData.startIndex, itsTokenRunIndex, itsTokenFirstInRun);
		}

	// prepare to accumulate new token starts

	itsTokenStartList  = tokenStartList;
	itsTokenStartCount = 0;
	itsTokenStart      = tokenData.startIndex;

	// scan the text and adjust the styles

	std::istrstream input(text.GetRawBytes(), textLength);
	JSeekg(input, itsTokenStart-1);

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

	Scan(input, tokenData.data);

	#if DEBUG_TIMING_INFO
	timer.StopTimer();
	std::cout << "JSTStyler: " << timer.PrintTimeInterval() << std::endl;
	#endif

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
	deletion is kJTrue if the modification was that text was deleted.

 ******************************************************************************/

void
JSTStyler::PreexpandCheckRange
	(
	const JString&			text,
	const JRunArray<JFont>&	styles,
	const JCharacterRange&	modifiedRange,
	const JBoolean			deletion,
	JCharacterRange*		checkRange
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
	const JIndex newEndIndex
	)
{
	if (itsCheckRange.last < newEndIndex)
		{
		itsCheckRange.last = JMin(newEndIndex, itsText->GetCharacterCount());
		}
}

/******************************************************************************
 SetStyle (protected)

	Returns kJTrue if the lexer should keep going.

 ******************************************************************************/

JBoolean
JSTStyler::SetStyle
	(
	const JCharacterRange&	range,
	const JFontStyle&		style
	)
{
	assert( !range.IsEmpty() );

	if (itsCheckRange.last < range.first)
		{
		// we are beyond the range where anything could have changed
		return kJFalse;
		}

	assert( range.first == itsTokenStart );

	if (itsRedoAllFlag)
		{
		JFont f = *itsDefFont;
		f.SetStyle(style);
		itsStyles->AppendElements(f, range.GetCount());
		}
	else if (range.last >= itsCheckRange.first)
		{
		JCharacterRange fontRange;
		fontRange.SetFirstAndCount(itsTokenFirstInRun, itsStyles->GetRunLength(itsTokenRunIndex));
		const JBoolean beyondCurrentRun = !fontRange.Contains(range);

		JFont f                     = itsStyles->GetRunData(itsTokenRunIndex);
		const JFontStyle& origStyle = f.GetStyle();

		const JBoolean styleExtendsBeyondToken =
			JI2B( !origStyle.IsBlank() && range.last < fontRange.last );

		if (beyondCurrentRun || styleExtendsBeyondToken || style != origStyle)
			{
			// extend the check range if we slop over into another style run
			// (HTML: type '<' after 'x' in "x<!--<br><h3>text</h3>-->")

			// extend past end of run to insure that we keep going until we
			// find a correctly styled token

			if (beyondCurrentRun || styleExtendsBeyondToken)
				{
				JIndex runIndex   = itsTokenRunIndex;
				JIndex firstInRun = itsTokenFirstInRun;
				const JBoolean ok = itsStyles->FindRun(itsTokenStart, range.last,
													   &runIndex, &firstInRun);
				assert( ok );
				ExtendCheckRange(firstInRun + itsStyles->GetRunLength(runIndex));
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

			itsStyles->RemoveNextElements(range.first, range.GetCount(),
										  &itsTokenRunIndex, &itsTokenFirstInRun);

			run_assert(itsStyles, range.first, itsTokenRunIndex, itsTokenFirstInRun);

			itsStyles->InsertElementsAtIndex(range.first, f, range.GetCount(),
											 &itsTokenRunIndex, &itsTokenFirstInRun);

			run_assert(itsStyles, range.first, itsTokenRunIndex, itsTokenFirstInRun);
			}
		}

	itsTokenStart = range.last+1;
	if (!itsRedoAllFlag)
		{
		const JBoolean ok = itsStyles->FindRun(range.first, itsTokenStart,
											   &itsTokenRunIndex, &itsTokenFirstInRun);
		assert( ok || range.last == itsText->GetCharacterCount() );
		assert( range.last == itsText->GetCharacterCount() ||
				check_run(itsStyles, itsTokenStart, itsTokenRunIndex, itsTokenFirstInRun) );
		}

	return JConvertToBoolean( range.last < itsCheckRange.last );
}

/******************************************************************************
 NewTokenStartList (static)

	Allocates a tokenStartList with a reasonable block size.

 ******************************************************************************/

JArray<JSTStyler::TokenData>*
JSTStyler::NewTokenStartList()
{
	JArray<TokenData>* list = jnew JArray<TokenData>(kListBlockSize);
	assert( list != nullptr );
	list->SetSortOrder(JListT::kSortAscending);
	list->SetCompareFunction(CompareTokenStarts);
	return list;
}

/******************************************************************************
 SaveTokenStart (protected)

	Automatically decimates the stream of token starts by itsTokenStartFactor.

 ******************************************************************************/

void
JSTStyler::SaveTokenStart
	(
	const TokenExtra& data
	)
{
	itsTokenStartCount++;
	if (itsTokenStartCount >= itsTokenStartFactor)
		{
		itsTokenStartCount = 0;
		itsTokenStartList->AppendElement(TokenData(itsTokenStart, data));
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

JListT::CompareResult
JSTStyler::CompareTokenStarts
	(
	const TokenData& t1,
	const TokenData& t2
	)
{
	return JCompareIndices(t1.startIndex, t2.startIndex);
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
	assert( range.last < itsTokenStart );
	assert( itsRecalcRange != nullptr );

	ExpandTextRange(itsRecalcRange, range);

	// adjust the styles

	JFont f = itsStyles->GetElement(range.first);
	f.SetStyle(style);

	itsStyles->SetNextElements(range.first, range.GetCount(), f);

	// update state

	if (itsTokenStart <= itsStyles->GetElementCount())
		{
		const JBoolean ok =
			itsStyles->FindRun(itsTokenStart, &itsTokenRunIndex, &itsTokenFirstInRun);
		assert( ok );
		}
}

/******************************************************************************
 OnlyColorChanged (private)

	We don't use JFontStyle& because we modify the values.

 ******************************************************************************/

inline JBoolean
JSTStyler::OnlyColorChanged
	(
	JFontStyle s1,
	JFontStyle s2
	)
	const
{
	s1.color = s2.color = 0;	// make sure the color is the same
	return JI2B(s1 == s2);		// avoids maintenance when fields are added
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

	*r1 = TextRange(start, after);
}
