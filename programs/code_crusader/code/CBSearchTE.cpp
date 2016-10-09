/******************************************************************************
 CBSearchTE.cpp

	Class to search files and report back to main Code Crusader process.

	BASE CLASS = JTextEditor

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSearchTE.h"
#include "CBSearchFontManager.h"
#include "CBSearchColormap.h"
#include "CBSearchTextDialog.h"
#include "cbmUtil.h"
#include "cbGlobals.h"
#include <JColormap.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <stdio.h>
#include <jAssert.h>

const JSize kMaxQuoteLength = 500;

static const JCharacter* kDisconnectStr         = "\0";
const JCharacter kDisconnect                    = '\0';
static const JCharacter* kRecordTerminatorStr   = "\1";
const JCharacter kRecordTerminator              = '\1';
const JCharacter CBSearchTE::kNewMatchLine      = '\2';
const JCharacter CBSearchTE::kRepeatMatchLine   = '\3';
const JCharacter CBSearchTE::kIncrementProgress = '\4';
const JCharacter CBSearchTE::kError             = '\5';

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchTE::CBSearchTE()
	:
	JTextEditor(kFullEditor, kJTrue, kJFalse, kJTrue,
				jnew CBSearchFontManager,
				jnew CBSearchColormap,
				1,1,1,1,1, 1000000)
{
	assert( TEGetFontManager() != NULL );
	assert( TEGetColormap() != NULL );

	RecalcAll(kJTrue);
	SetCharacterInWordFunction(CBMIsCharacterInWord);
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
	ostream&					output
	)
{
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;
	const JBoolean ok =
		(CBGetSearchTextDialog())->GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex);
	assert( ok );

	const JSize count = fileList.GetElementCount();
	assert( count == nameList.GetElementCount() );

	for (JIndex i=1; i<=count; i++)
		{
		const JString* file = fileList.NthElement(i);
		const JString* name = nameList.NthElement(i);
		SearchFile(*file, *name, onlyListFiles, listFilesWithoutMatch, output,
				   searchStr, searchIsRegex, caseSensitive, entireWord, *regex);

		// increment progress

		output << kIncrementProgress << kRecordTerminator;
		output.flush();

		// toss temp files once we are done

		if (*file != *name)
			{
			remove(*file);
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
	ostream&		output,

	const JString&	searchStr,
	const JBoolean	isRegex,
	const JBoolean	caseSensitive,
	const JBoolean	entireWord,
	const JRegex&	regex
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

	PlainTextFormat format;
	if (IsKnownBinaryFile(printName) ||
		!ReadPlainText(fileName, &format, kJFalse))
		{
		// don't search binary files (cleaning is too slow and pointless)

		output << kError << "Not searched (binary):  ";
		fileName.Print(output);
		output << kRecordTerminator;
		return;
		}

	JBoolean foundMatch = kJFalse;
	JIndexRange prevQuoteRange;
	JBoolean prevQuoteTruncated = kJFalse;

	JBoolean wrapped;
	JArray<JIndexRange> submatchList;
	while ((!isRegex &&
			SearchForward(searchStr, caseSensitive, entireWord, kJFalse, &wrapped)) ||
		   (isRegex &&
			SearchForward(regex, entireWord, kJFalse, &wrapped, &submatchList)))
		{
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
		else
			{
			JIndexRange matchRange;
			const JBoolean ok = GetSelection(&matchRange);
			assert( ok );

			JIndexRange origMatchRange = matchRange;
			matchRange.last = JMin(matchRange.last, matchRange.first + kMaxQuoteLength-1);

			const JIndexRange origQuoteRange(GetParagraphStart(matchRange.first),
											 GetParagraphEnd(matchRange.last));

			JIndexRange quoteRange = origQuoteRange;
			if (prevQuoteRange.Contains(matchRange))
				{
				quoteRange = prevQuoteRange;
				}
			else if (quoteRange.GetLength() > kMaxQuoteLength)
				{
				const JSize extraLength = (kMaxQuoteLength - matchRange.GetLength())/2;
				quoteRange.first = JMax(quoteRange.first,
					matchRange.first > extraLength ? matchRange.first - extraLength : 1);
				quoteRange.last = JMin(quoteRange.last, matchRange.last + extraLength);
				}

			matchRange -= quoteRange.first-1;
			if (quoteRange == prevQuoteRange)
				{
				if (prevQuoteTruncated)
					{
					matchRange += 3;
					}
				output << kRepeatMatchLine;
				output << ' ' << matchRange;
				}
			else
				{
				prevQuoteTruncated = kJFalse;

				JString quoteText = (GetText()).GetSubstring(quoteRange);
				if (quoteRange.first != origQuoteRange.first)
					{
					quoteText.Prepend("...");
					matchRange += 3;
					prevQuoteTruncated = kJTrue;
					}
				if (quoteRange.last != origQuoteRange.last)
					{
					quoteText.Append("...");
					}
				if (matchRange.GetLength() != origMatchRange.GetLength())
					{
					matchRange.last += 3;	// underline ellipsis
					}

				output << kNewMatchLine;
				output << ' ' << printName;
				output << ' ' << GetLineForChar(quoteRange.first);
				output << ' ' << quoteText;
				output << ' ' << matchRange;
				}
			output << kRecordTerminator;

			prevQuoteRange = quoteRange;
			}
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

static const JCharacter* kSuffix[] =
{
	".jst", ".o", ".a", ".so", ".dylib"
};

const JSize kSuffixCount = sizeof(kSuffix) / sizeof(JCharacter*);

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

	for (JIndex i=0; i<kSuffixCount; i++)
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
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;
	if ((JXGetSearchTextDialog())->GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex))
		{
		return JTextEditor::ReplaceAllForward(
					searchStr, searchIsRegex, caseSensitive, entireWord, wrapSearch,
					replaceStr, replaceIsRegex, preserveCase, *regex);
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
 TEClipboardChanged (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TEClipboardChanged()
{
}

/******************************************************************************
 TEOwnsClipboard (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchTE::TEOwnsClipboard()
	const
{
	return kJTrue;
}

/******************************************************************************
 TEGetExternalClipboard (virtual protected)

 ******************************************************************************/

JBoolean
CBSearchTE::TEGetExternalClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 TEDisplayBusyCursor (virtual protected)

 ******************************************************************************/

void
CBSearchTE::TEDisplayBusyCursor()
	const
{
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
