/******************************************************************************
 CBSearchTE.cpp

	Class to search files and report back to main Code Crusader process.

	BASE CLASS = JTextEditor

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBSearchTE.h"
#include "CBSearchFontManager.h"
#include "CBSearchColorManager.h"
#include "CBSearchTextDialog.h"
#include "cbmUtil.h"
#include "cbGlobals.h"
#include <JXColorManager.h>
#include <JStringIterator.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <stdio.h>
#include <jAssert.h>

const JSize kMaxQuoteCharCount = 500;

static const JUtf8Byte* kDisconnectStr         = "\0";
const JUtf8Byte kDisconnect                    = '\0';
static const JUtf8Byte* kRecordTerminatorStr   = "\1";
const JUtf8Byte kRecordTerminator              = '\1';
const JUtf8Byte CBSearchTE::kNewMatchLine      = '\2';
const JUtf8Byte CBSearchTE::kRepeatMatchLine   = '\3';
const JUtf8Byte CBSearchTE::kIncrementProgress = '\4';
const JUtf8Byte CBSearchTE::kError             = '\5';

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchTE::CBSearchTE()
	:
	JTextEditor(kFullEditor, jnew JStyledText(kJFalse, kJFalse), kJTrue,
				jnew CBSearchFontManager, kJTrue,
				1,1,1,1, 1000000)
{
	assert( TEGetFontManager() != nullptr );

	RecalcAll(kJTrue);
	GetText()->SetCharacterInWordFunction(CBMIsCharacterInWord);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchTE::~CBSearchTE()
{
}

/******************************************************************************
 SetProtocol (static)

 ******************************************************************************/

void
CBSearchTE::SetProtocol
	(
	CBSearchDocument::RecordLink* link
	)
{
	link->SetProtocol(kRecordTerminatorStr, 1, kDisconnectStr, 1);
}

/******************************************************************************
 SearchFiles

 ******************************************************************************/

void
CBSearchTE::SearchFiles
	(
	const JPtrArray<JString>&	fileList,
	const JPtrArray<JString>&	nameList,
	const JBoolean				onlyListFiles,
	const JBoolean				listFilesWithoutMatch,
	std::ostream&				output
	)
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	JBoolean entireWord, wrapSearch, preserveCase;
	const JBoolean ok =
		CBGetSearchTextDialog()->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase);
	assert( ok );

	const JSize count = fileList.GetElementCount();
	assert( count == nameList.GetElementCount() );

	for (JIndex i=1; i<=count; i++)
		{
		const JString* file = fileList.GetElement(i);
		const JString* name = nameList.GetElement(i);
		SearchFile(*file, *name, onlyListFiles, listFilesWithoutMatch,
				   output, *searchRegex, entireWord);

		// increment progress

		output << kIncrementProgress << kRecordTerminator;
		output.flush();

		// toss temp files once we are done

		if (*file != *name)
			{
			JRemoveFile(*file);
			}
		}

	output << kDisconnect;
	output.flush();
}

/******************************************************************************
 SearchFile (private)

 ******************************************************************************/

void
CBSearchTE::SearchFile
	(
	const JString&	fileName,
	const JString&	printName,		// so we display it correctly to the user
	const JBoolean	onlyListFiles,
	const JBoolean	listFilesWithoutMatch,
	std::ostream&	output,

	const JRegex&	searchRegex,
	const JBoolean	entireWord
	)
{
	if (!JFileExists(fileName))
		{
		output << kError << "Not found:  ";
		fileName.Print(output);
		output << kRecordTerminator;
		}
	else if (!JFileReadable(fileName))
		{
		output << kError << "Not readable:  ";
		fileName.Print(output);
		output << kRecordTerminator;
		}

	JStyledText::PlainTextFormat format;
	if (IsKnownBinaryFile(printName) ||
		!GetText()->ReadPlainText(fileName, &format, kJFalse))
		{
		// don't search binary files (cleaning is too slow and pointless)

		output << kError;
		JGetString("NotSearchedBinary::CBSearchTE").Print(output);
		fileName.Print(output);
		output << kRecordTerminator;
		return;
		}

	JBoolean foundMatch = kJFalse;
	JStyledText::TextRange prevQuoteRange;
	JBoolean prevQuoteTruncated = kJFalse;

	while (1)
		{
		JBoolean wrapped;
		const JStringMatch m = SearchForward(searchRegex, entireWord, kJFalse, &wrapped);
		if (m.IsEmpty())
			{
			break;
			}

		foundMatch = kJTrue;
		if (onlyListFiles || listFilesWithoutMatch)
			{
			if (!listFilesWithoutMatch)
				{
				output << printName;
				output << kRecordTerminator;
				}
			break;
			}

		JStringIterator iter(GetText()->GetText());

		const JStyledText::TextRange origMatchRange(
			m.GetCharacterRange(), m.GetUtf8ByteRange());

		JStyledText::TextRange matchRange = origMatchRange;
		if (matchRange.charRange.GetCount() > kMaxQuoteCharCount)
			{
			matchRange.charRange.last = matchRange.charRange.first + kMaxQuoteCharCount - 1;

			iter.UnsafeMoveTo(kJIteratorStartBefore, matchRange.charRange.first, matchRange.byteRange.first);
			iter.MoveTo(kJIteratorStartAfter, matchRange.charRange.last);
			matchRange.byteRange.last = iter.GetPrevByteIndex();
			}

		const JStyledText::TextRange origQuoteRange(
			GetText()->GetParagraphStart(matchRange.GetFirst()),
			GetText()->AdjustTextIndex(GetText()->GetParagraphEnd(matchRange.GetLast(*GetText())), +1));

		JStyledText::TextRange quoteRange = origQuoteRange;
		if (prevQuoteRange.charRange.Contains(matchRange.charRange))
			{
			quoteRange = prevQuoteRange;
			}
		else if (quoteRange.charRange.GetCount() > kMaxQuoteCharCount)
			{
			const JSize extraCount =
				(kMaxQuoteCharCount - matchRange.charRange.GetCount())/2;

			const JIndex first = matchRange.charRange.first > extraCount ? 
									matchRange.charRange.first - extraCount : 1;

			if (quoteRange.charRange.first < first)
				{
				iter.UnsafeMoveTo(kJIteratorStartBefore, quoteRange.charRange.first, quoteRange.byteRange.first);
				iter.MoveTo(kJIteratorStartBefore, first);
				quoteRange.charRange.first = first;
				quoteRange.byteRange.first = iter.GetNextByteIndex();
				}

			const JIndex last = matchRange.charRange.last + extraCount;
			if (last < quoteRange.charRange.last)
				{
				iter.UnsafeMoveTo(kJIteratorStartAfter, quoteRange.charRange.last, quoteRange.byteRange.last);
				iter.MoveTo(kJIteratorStartAfter, last);
				quoteRange.charRange.last = last;
				quoteRange.byteRange.last = iter.GetPrevByteIndex();
				}
			}

		matchRange.charRange -= quoteRange.charRange.first-1;
		matchRange.byteRange -= quoteRange.byteRange.first-1;

		if (quoteRange.charRange == prevQuoteRange.charRange)
			{
			if (prevQuoteTruncated)
				{
				matchRange.charRange += 3;
				matchRange.byteRange += 3;
				}
			output << kRepeatMatchLine;
			output << ' ' << matchRange.charRange;
			output << ' ' << matchRange.byteRange;
			}
		else
			{
			prevQuoteTruncated = kJFalse;

			iter.UnsafeMoveTo(kJIteratorStartBefore, quoteRange.charRange.first, quoteRange.byteRange.first);
			iter.BeginMatch();
			iter.UnsafeMoveTo(kJIteratorStartAfter, quoteRange.charRange.last, quoteRange.byteRange.last);
			JString quoteText = iter.FinishMatch().GetString();

			if (quoteRange.charRange.first != origQuoteRange.charRange.first)
				{
				quoteText.Prepend("...");
				matchRange.charRange += 3;
				matchRange.byteRange += 3;
				prevQuoteTruncated = kJTrue;
				}
			if (quoteRange.charRange.last != origQuoteRange.charRange.last)
				{
				quoteText.Append("...");
				}
			if (matchRange.charRange.GetCount() != origMatchRange.charRange.GetCount())
				{
				matchRange.charRange.last += 3;	// underline ellipsis
				matchRange.byteRange.last += 3;
				}

			output << kNewMatchLine;
			output << ' ' << printName;
			output << ' ' << GetLineForChar(quoteRange.charRange.first);
			output << ' ' << quoteText;
			output << ' ' << matchRange.charRange;
			output << ' ' << matchRange.byteRange;
			}
		output << kRecordTerminator;

		prevQuoteRange = quoteRange;
		}

	if (listFilesWithoutMatch && !foundMatch)
		{
		output << printName;
		output << kRecordTerminator;
		}
}

/******************************************************************************
 IsKnownBinaryFile (private)

 ******************************************************************************/

static const JUtf8Byte* kSuffix[] =
{
	".o", ".a", ".so", ".dylib"
};

const JSize kSuffixCount = sizeof(kSuffix) / sizeof(JUtf8Byte*);

JBoolean
CBSearchTE::IsKnownBinaryFile
	(
	const JString& fileName
	)
	const
{
	if (CBGetPrefsManager()->GetFileType(fileName) == kCBBinaryFT)
		{
		return kJTrue;
		}

	for (JUnsignedOffset i=0; i<kSuffixCount; i++)
		{
		if (fileName.EndsWith(kSuffix[i]))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 ReplaceAllForward

	Replace every occurrence of the search string with the replace string,
	starting from the current location.  Returns kJTrue if it replaced anything.

 ******************************************************************************/

JBoolean
CBSearchTE::ReplaceAllForward()
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	JBoolean entireWord, wrapSearch, preserveCase;
	if (JXGetSearchTextDialog()->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase))
		{
		return JTextEditor::ReplaceAll(
					*searchRegex, entireWord,
					replaceStr, interpolator, preserveCase, kJFalse);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 TERefresh (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TERefresh()
{
}

/******************************************************************************
 TERefreshRect (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TERefreshRect
	(
	const JRect& rect
	)
{
}

/******************************************************************************
 TERedraw (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TERedraw()
{
}

/******************************************************************************
 TESetGUIBounds (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TESetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h,
	const JCoordinate changeY
	)
{
}

/******************************************************************************
 TEWidthIsBeyondDisplayCapacity (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchTE::TEWidthIsBeyondDisplayCapacity
	(
	const JSize width
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 TEScrollToRect (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchTE::TEScrollToRect
	(
	const JRect&	rect,
	const JBoolean	centerInDisplay
	)
{
	return kJTrue;
}

/******************************************************************************
 TEScrollForDrag (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchTE::TEScrollForDrag
	(
	const JPoint& pt
	)
{
	return kJTrue;
}

/******************************************************************************
 TEScrollForDND (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchTE::TEScrollForDND
	(
	const JPoint& pt
	)
{
	return kJTrue;
}

/******************************************************************************
 TESetVertScrollStep (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TESetVertScrollStep
	(
	const JCoordinate vStep
	)
{
}

/******************************************************************************
 TEUpdateClipboard (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TEUpdateClipboard
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
}

/******************************************************************************
 TEGetClipboard (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchTE::TEGetClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 TEBeginDND (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchTE::TEBeginDND()
{
	return kJFalse;
}

/******************************************************************************
 TEPasteDropData (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TEPasteDropData()
{
}

/******************************************************************************
 TECaretShouldBlink (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TECaretShouldBlink
	(
	const JBoolean blink
	)
{
}

/******************************************************************************
 TEHasSearchText (virtual)

 ******************************************************************************/

JBoolean
CBSearchTE::TEHasSearchText()
	const
{
	return kJFalse;
}
