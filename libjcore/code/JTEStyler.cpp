/******************************************************************************
 JTEStyler.cpp

	This is the base class for an object that can efficiently control the
	styles inside a JTextEditor.  Derive a class from JTextEditor and
	override AdjustStylesBeforeRecalc() and pass the arguments directly to
	JTEStyler::UpdateStyles().

	tokenStartList is used to avoid lexing more of the text than necessary.
	Each JTextEditor object must own one and must never modify the contents
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

#include <JTEStyler.h>
#include <JFontManager.h>
#include <JOrderedSetUtil.h>
#include <JMinMax.h>
#include <jStreamUtil.h>
#include <JStopWatch.h>
#include <strstream>
#include <jAssert.h>

const JSize kDecimationFactor = 50;
const JSize kListBlockSize    = 50;

#define DEBUG_RUN_INFO		0	// boolean
#define DEBUG_TIMING_INFO	0	// boolean

#define run_assert(styles, index, runIndex, firstIndexInRun) \
	assert( check_run(styles, index, runIndex, firstIndexInRun) )

#if DEBUG_RUN_INFO

JIndex debugRunIndex, debugFirstInRun;

#define check_run(styles, index, runIndex, firstIndexInRun) \
	styles->FindRun(index, &debugRunIndex, &debugFirstInRun) && \
	debugRunIndex == runIndex && debugFirstInRun == firstIndexInRun

#else

#define check_run(styles, index, runIndex, firstIndexInRun)		kJTrue

#endif

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEStyler::JTEStyler()
{
	itsActiveFlag  = kJTrue;

	itsTE          = NULL;
	itsFontMgr     = NULL;
	itsText        = NULL;
	itsStyles      = NULL;
	itsDefFont     = NULL;
	itsRecalcRange = NULL;
	itsRedrawRange = NULL;

	itsTokenStartList   = NULL;
	itsTokenStartFactor = kDecimationFactor;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEStyler::~JTEStyler()
{
}

/******************************************************************************
 UpdateStyles

	*range is inside [1, text.GetLength()+1]

 ******************************************************************************/

void
JTEStyler::UpdateStyles
	(
	const JTextEditor*	te,
	const JString&		text,
	JRunArray<JFont>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion,
	JArray<TokenData>*	tokenStartList
	)
{
	if (!itsActiveFlag)
		{
		tokenStartList->RemoveAll();
		return;
		}

	const JSize textLength = text.GetLength();
	if (textLength == 0)
		{
		tokenStartList->RemoveAll();
		return;
		}

	TokenData tokenData;
	if (recalcRange->first == 1 && recalcRange->last >= text.GetLength())
		{
		itsRedoAllFlag = kJTrue;
		itsCheckRange.Set(1, text.GetLength());

		tokenStartList->RemoveAll();
		tokenData = TokenData(1, GetFirstTokenExtraData());
		tokenStartList->AppendElement(tokenData);

		styles->RemoveAll();
		}
	else
		{
		itsRedoAllFlag = kJFalse;

		// calculate the range that needs to be checked

		JIndex firstIndex = recalcRange->first;
		JIndex lastIndex  = recalcRange->last;
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

		JIndexRange savedRange = itsCheckRange;
		PreexpandCheckRange(text, *styles, *recalcRange, deletion, &itsCheckRange);
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
				tokenStartList->SearchSorted1(target, JOrderedSetT::kAnyMatch, &foundTokenStart);
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

	std::istrstream input(text.GetCString(), text.GetLength());
	JSeekg(input, itsTokenStart-1);

	itsTE          = te;
	itsFontMgr     = te->TEGetFontManager();
	itsDefFont     = jnew JFont(te->GetDefaultFont());
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
	std::cout << "JTEStyler: " << timer.PrintTimeInterval() << std::endl;
	#endif

	itsTE             = NULL;
	itsFontMgr        = NULL;
	itsText           = NULL;
	itsStyles         = NULL;
	itsRecalcRange    = NULL;
	itsRedrawRange    = NULL;
	itsTokenStartList = NULL;

	jdelete itsDefFont;
	itsDefFont = NULL;
}

/******************************************************************************
 PreexpandCheckRange (virtual protected)

	*checkRange is only allowed to expand.  The default is to do nothing.

	modifiedRange is the range of text that was changed.
	deletion is kJTrue if the modification was that text was deleted.

 ******************************************************************************/

void
JTEStyler::PreexpandCheckRange
	(
	const JString&			text,
	const JRunArray<JFont>&	styles,
	const JIndexRange&		modifiedRange,
	const JBoolean			deletion,
	JIndexRange*			checkRange
	)
{
}

/******************************************************************************
 ExtendCheckRange (protected)

	Extends the end of the check range.  It is safe to call this with
	an index beyond the end of the text.

 ******************************************************************************/

void
JTEStyler::ExtendCheckRange
	(
	const JIndex newEndIndex
	)
{
	if (itsCheckRange.last < newEndIndex)
		{
		itsCheckRange.last = JMin(newEndIndex, itsText->GetLength());
		}
}

/******************************************************************************
 SetStyle (protected)

	Returns kJTrue if the lexer should keep going.

 ******************************************************************************/

JBoolean
JTEStyler::SetStyle
	(
	const JIndexRange&	range,
	const JFontStyle&	style
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
		itsStyles->AppendElements(f, range.GetLength());
		}
	else if (range.last >= itsCheckRange.first)
		{
		JIndexRange fontRange;
		fontRange.SetFirstAndLength(itsTokenFirstInRun, itsStyles->GetRunLength(itsTokenRunIndex));
		const JBoolean beyondCurrentRun = !fontRange.Contains(range);

		JFont f = itsStyles->GetRunData(itsTokenRunIndex);
		if (beyondCurrentRun || style != f.GetStyle())
			{
			// extend the check range if we slop over into another style run
			// (HTML: type '<' after 'x' in "x<!--<br><h3>text</h3>-->")

			// extend past end of run to insure that we keep going until we
			// find a correctly styled token

			if (beyondCurrentRun)
				{
				JIndex runIndex   = itsTokenRunIndex;
				JIndex firstInRun = itsTokenFirstInRun;
				const JBoolean ok = itsStyles->FindRun(itsTokenStart, range.last,
													   &runIndex, &firstInRun);
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

			if (!beyondCurrentRun && OnlyColorChanged(style, f.GetStyle()))
				{
				*itsRedrawRange += range;
				}
			else
				{
				*itsRecalcRange += range;
				}

			f = *itsDefFont;
			f.SetStyle(style);

			itsStyles->RemoveNextElements(range.first, range.GetLength(),
										  &itsTokenRunIndex, &itsTokenFirstInRun);

			run_assert(itsStyles, range.first, itsTokenRunIndex, itsTokenFirstInRun);

			itsStyles->InsertElementsAtIndex(range.first, f, range.GetLength(),
											 &itsTokenRunIndex, &itsTokenFirstInRun);

			run_assert(itsStyles, range.first, itsTokenRunIndex, itsTokenFirstInRun);
			}
		}

	itsTokenStart = range.last+1;
	if (!itsRedoAllFlag)
		{
		const JBoolean ok = itsStyles->FindRun(range.first, itsTokenStart,
											   &itsTokenRunIndex, &itsTokenFirstInRun);
		assert( ok || range.last == itsText->GetLength() );
		assert( range.last == itsText->GetLength() ||
				check_run(itsStyles, itsTokenStart, itsTokenRunIndex, itsTokenFirstInRun) );
		}

	return JConvertToBoolean( range.last < itsCheckRange.last );
}

/******************************************************************************
 NewTokenStartList (static)

	Allocates a tokenStartList with a reasonable block size.

 ******************************************************************************/

JArray<JTEStyler::TokenData>*
JTEStyler::NewTokenStartList()
{
	JArray<TokenData>* list = jnew JArray<TokenData>(kListBlockSize);
	assert( list != NULL );
	list->SetSortOrder(JOrderedSetT::kSortAscending);
	list->SetCompareFunction(CompareTokenStarts);
	return list;
}

/******************************************************************************
 SaveTokenStart (protected)

	Automatically decimates the stream of token starts by itsTokenStartFactor.

 ******************************************************************************/

void
JTEStyler::SaveTokenStart
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

JTEStyler::TokenExtra
JTEStyler::GetFirstTokenExtraData()
	const
{
	return TokenExtra();
}

/******************************************************************************
 CompareTokenStarts (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JTEStyler::CompareTokenStarts
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
JTEStyler::AdjustStyle
	(
	const JIndexRange&	range,
	const JFontStyle&	style
	)
{
	assert( range.last < itsTokenStart );
	assert( itsRecalcRange != NULL );

	*itsRecalcRange += range;

	// adjust the styles

	JFont f = itsStyles->GetElement(range.first);
	f.SetStyle(style);

	itsStyles->SetNextElements(range.first, range.GetLength(), f);

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
JTEStyler::OnlyColorChanged
	(
	JFontStyle s1,
	JFontStyle s2
	)
	const
{
	s1.color = s2.color = 0;	// make sure the color is the same
	return JI2B(s1 == s2);		// avoids maintenance when fields are added
}
