/******************************************************************************
 JStyledText.cpp

	Class to manage styled text.

	Only public functions are required to call NewUndo(), and only if the
	action to be performed changes the text or styles.  Private functions
	must *not* call NewUndo(), because several manipulations may be
	required to perform one user command, and only the user command as a
	whole is undoable.  (Otherwise, the user may get confused.)

	Because of this convention, public functions that affect undo should
	only be a public interface to a private function.  The public function
	calls NewUndo() and then calls the private function.  The private
	function does all the work, but doesn't modify the undo information.
	This allows other private functions to use the routine (private
	version) without modifying the undo information.

	In order to guarantee that the TextChanged message means "text has
	already changed", NewUndo() must be called -after- making the
	modification.  (even though the Undo object has to be created before
	the modifications)

	TextSet is different from TextChanged because documents will typically
	use only the latter for setting their "unsaved" flag.

	Functionality yet to be implemented:

		Search & replace for text and styles combined

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JStyledText.h"
#include "JUndoRedoChain.h"
#include "JSTUndoTyping.h"
#include "JSTUndoPaste.h"
#include "JSTUndoMove.h"
#include "JSTUndoStyle.h"
#include "JSTUndoTabShift.h"
#include "JFontManager.h"
#include "JColorManager.h"
#include "JListUtil.h"
#include "JRunArrayIterator.h"
#include "JRegex.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "JInterpolate.h"
#include "JLatentPG.h"
#include "JMinMax.h"
#include "jTextUtil.h"
#include "jASCIIConstants.h"
#include "jFStreamUtil.h"
#include "jStreamUtil.h"
#include "jFileUtil.h"
#include "jGlobals.h"
#include "jAssert.h"

using FontIterator = JRunArrayIterator<JFont>;

const JFileVersion kCurrentPrivateFormatVersion = 1;

const JSize kUNIXLineWidth    = 75;
const JSize kUNIXTabCharCount = 8;

inline bool
isWhitespace
	(
	const JUtf8Character& c
	)
{
	return c == ' ' || c == '\t';
}

// JBroadcaster message types

const JUtf8Byte* JStyledText::kTextSet            = "TextSet::JStyledText";
const JUtf8Byte* JStyledText::kTextChanged        = "TextChanged::JStyledText";
const JUtf8Byte* JStyledText::kDefaultFontChanged = "DefaultFontChanged::JStyledText";
const JUtf8Byte* JStyledText::kUndoFinished       = "UndoFinished::JStyledText";
const JUtf8Byte* JStyledText::kWillBeBusy         = "WillBeBusy::JStyledText";

/******************************************************************************
 Constructor

 ******************************************************************************/

JStyledText::JStyledText
	(
	const bool useMultipleUndo,
	const bool pasteStyledText
	)
	:
	itsPasteStyledTextFlag(pasteStyledText),
	itsDefaultFont(JFontManager::GetDefaultFont()),
	itsCharInWordFn(nullptr)
{
	itsStyles    = jnew JRunArray<JFont>;
	itsUndoChain = jnew JUndoRedoChain(useMultipleUndo);

	itsTabToSpacesFlag   = false;
	itsAutoIndentFlag    = false;
	itsCRMLineWidth      = kUNIXLineWidth;
	itsCRMTabCharCount   = kUNIXTabCharCount;
	itsCRMRuleList       = nullptr;
	itsOwnsCRMRulesFlag  = false;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JStyledText::JStyledText
	(
	const JStyledText& source
	)
	:
	JBroadcaster(source),

	itsText( source.itsText ),
	itsPasteStyledTextFlag( source.itsPasteStyledTextFlag ),
	itsTabToSpacesFlag( source.itsTabToSpacesFlag ),
	itsAutoIndentFlag( source.itsAutoIndentFlag ),

	itsDefaultFont( source.itsDefaultFont ),

	itsCharInWordFn(nullptr),

	itsCRMLineWidth( source.itsCRMLineWidth ),
	itsCRMTabCharCount( source.itsCRMTabCharCount )
{
	itsStyles = jnew JRunArray<JFont>(*source.itsStyles);

	if (source.itsCharInWordFn != nullptr)
	{
		itsCharInWordFn = jnew std::function(*source.itsCharInWordFn);
	}

	bool canUndo, canRedo;
	itsUndoChain = jnew JUndoRedoChain(
		source.itsUndoChain->HasMultipleUndo(&canUndo, &canRedo));
	itsUndoChain->SetUndoDepth(source.itsUndoChain->GetUndoDepth());

	itsCRMRuleList       = nullptr;
	itsOwnsCRMRulesFlag  = false;
	if (source.itsCRMRuleList != nullptr)
	{
		itsCRMRuleList      = jnew CRMRuleList(*source.itsCRMRuleList);
		itsOwnsCRMRulesFlag = true;
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStyledText::~JStyledText()
{
	jdelete itsStyles;
	jdelete itsUndoChain;
	jdelete itsCharInWordFn;

	ClearCRMRuleList();
}

/******************************************************************************
 SetText

	Returns false if illegal characters had to be removed.

	Clears undo history.

	style can safely be nullptr or itsStyles.

 ******************************************************************************/

bool
JStyledText::SetText
	(
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	ClearUndo();

	JRunArray<JFont> tmpStyle;
	tmpStyle.SetMinLgSize(itsStyles->GetMinLgSize());
	if (style != nullptr)
	{
		tmpStyle = *style;
	}
	else
	{
		tmpStyle.AppendItems(itsDefaultFont, text.GetCharacterCount());
	}

	JString* cleanText           = nullptr;
	JRunArray<JFont>* cleanStyle = nullptr;

	bool okToInsert, cleaned;
	const bool allocated = CleanText(text, tmpStyle, &cleanText, &cleanStyle, &okToInsert);
	if (okToInsert && allocated)
	{
		itsText    = *cleanText;
		*itsStyles = *cleanStyle;
		cleaned    = true;
	}
	else if (okToInsert)
	{
		itsText    = text;
		*itsStyles = tmpStyle;
		cleaned    = false;
	}
	else
	{
		itsText.Clear();
		itsStyles->RemoveAll();
		cleaned = true;
	}

	if (allocated)
	{
		jdelete cleanText;
		jdelete cleanStyle;
	}

	if (!itsText.IsEmpty())
	{
		TextRange origRange, recalcRange, redrawRange;
		origRange = recalcRange = redrawRange = SelectAll();
		AdjustStylesBeforeBroadcast(itsText, itsStyles, &recalcRange, &redrawRange, false);
		assert( recalcRange.charRange == origRange.charRange );
		assert( recalcRange.byteRange == origRange.byteRange );
		assert( redrawRange.charRange == origRange.charRange );
		assert( redrawRange.byteRange == origRange.byteRange );
		assert( itsText.GetCharacterCount() == itsStyles->GetItemCount() );
	}

	Broadcast(TextSet());
	return !cleaned;
}

/******************************************************************************
 ReadPlainText

	If acceptBinaryFile == true, returns false if illegal characters had to
	be removed.  If acceptBinaryFile == false, returns false without loading
	the file if the file contains illegal characters.

	We read directly into itsText to avoid making two copies of the
	file's data.  (The file could be very large.)

	Clears undo history.

 ******************************************************************************/

static const JUtf8Byte  kUNIXNewlineChar      = '\n';
static const JUtf8Byte* kMacintoshNewline     = "\r";
static const JUtf8Byte  kMacintoshNewlineChar = '\r';
static const JUtf8Byte* kDOSNewline           = "\r\n";
static const JUtf8Byte  k1stDOSNewlineChar    = '\r';

bool
JStyledText::ReadPlainText
	(
	const JString&		fileName,
	PlainTextFormat*	format,
	const bool			acceptBinaryFile
	)
{
	JReadFile(fileName, &itsText);

	if (ContainsIllegalChars(itsText))
	{
		if (!acceptBinaryFile)
		{
			return false;
		}

		// It is probably a binary file, so we shouldn't mess with it.

		*format = kUNIXText;
	}

	else if (itsText.Contains(kDOSNewline))
	{
		*format = kDOSText;

		Broadcast(WillBeBusy());

		JSize byteCount;
		JGetFileLength(fileName, &byteCount);

		JLatentPG pg(100);
		pg.FixedLengthProcessBeginning(byteCount,
				JGetString("ConvertFromDOS::JStyledText"), false, true);

		// Converting itsText in memory is more than 100 times slower,
		// but we have to normalize the bytes after reading, so we have to
		// allocate a new block.

		auto* buffer = jnew JUtf8Byte[ byteCount ];

		std::ifstream input(fileName.GetBytes());
		JIndex i = 0;
		while (true)
		{
			const JUtf8Byte c = input.get();
			if (input.eof() || input.fail())
			{
				break;
			}

			if (c != k1stDOSNewlineChar)
			{
				buffer[i] = c;
				i++;
			}
			else
			{
				byteCount--;
				pg.IncrementProgress(i - pg.GetCurrentStepCount());
			}
		}
		input.close();

		itsText.Set(buffer, byteCount);
		jdelete [] buffer;

		pg.ProcessFinished();
	}

	else if (itsText.Contains(kMacintoshNewline))
	{
		*format = kMacintoshText;

		ConvertFromMacintoshNewlinetoUNIXNewline(&itsText);
	}

	else
	{
		*format = kUNIXText;
	}

	return SetText(itsText, nullptr);
}

/******************************************************************************
 ConvertFromMacintoshNewlinetoUNIXNewline (private static)

	Efficient hack to replace bytes in place.

 ******************************************************************************/

void
JStyledText::ConvertFromMacintoshNewlinetoUNIXNewline
	(
	JString* text
	)
{
	auto* s = const_cast<JUtf8Byte*>(text->GetBytes());
	while (*s != 0)
	{
		if (*s == kMacintoshNewlineChar)
		{
			*s = kUNIXNewlineChar;
		}
		s++;
	}
}

/******************************************************************************
 WritePlainText

 ******************************************************************************/

void
JStyledText::WritePlainText
	(
	const JString&			fileName,
	const PlainTextFormat	format
	)
	const
{
	std::ofstream output(fileName.GetBytes());
	WritePlainText(output, format);
}

void
JStyledText::WritePlainText
	(
	std::ostream&			output,
	const PlainTextFormat	format
	)
	const
{
	if (format == kUNIXText)
	{
		itsText.Print(output);
		return;
	}

	const JUtf8Byte* newlineStr = nullptr;
	if (format == kDOSText)
	{
		newlineStr = kDOSNewline;
	}
	else if (format == kMacintoshText)
	{
		newlineStr = kMacintoshNewline;
	}
	assert( newlineStr != nullptr );

	const JUtf8Byte* text = itsText.GetBytes();
	const JSize byteCount = itsText.GetByteCount();
	JIndex start          = 0;
	for (JUnsignedOffset i=0; i<byteCount; i++)
	{
		if (text[i] == kUNIXNewlineChar)
		{
			if (start < i)
			{
				output.write(text + start, i - start);
			}
			output << newlineStr;
			start = i+1;
		}
	}

	if (start < byteCount)
	{
		output.write(text + start, byteCount - start);
	}
}

/******************************************************************************
 ReadPrivateFormat

	See WritePrivateFormat() for version information.

	Clears undo history.

 ******************************************************************************/

bool
JStyledText::ReadPrivateFormat
	(
	std::istream& input
	)
{
	Broadcast(WillBeBusy());

	JString text;
	JRunArray<JFont> style;
	style.SetMinLgSize(itsStyles->GetMinLgSize());
	if (ReadPrivateFormat(input, &text, &style))
	{
		SetText(text, &style);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ReadPrivateFormat (static)

	See WritePrivateFormat() for version information.

 ******************************************************************************/

bool
JStyledText::ReadPrivateFormat
	(
	std::istream&		input,
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	text->Clear();
	style->RemoveAll();

	// version

	JFileVersion vers;
	input >> vers;

	if (vers > kCurrentPrivateFormatVersion)
	{
		return false;
	}

	// text

	JSize charCount;
	input >> charCount;
	input.ignore(1);
	if (charCount > 0)
	{
		text->Read(input, charCount);
	}

	// list of font names

	JSize fontCount;
	input >> fontCount;

	JPtrArray<JString> fontNameList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=fontCount; i++)
	{
		auto* name = jnew JString;
		input >> *name;
		fontNameList.Append(name);
	}

	// list of rgb values

	JSize colorCount;
	input >> colorCount;

	JArray<JColorID> colorList;

	JRGB color;
	for (JIndex i=1; i<=colorCount; i++)
	{
		input >> color;
		colorList.AppendItem(JColorManager::GetColorID(color));
	}

	// styles

	JSize runCount;
	input >> runCount;

	JSize size;
	JFontStyle fStyle;
	for (JIndex i=1; i<=runCount; i++)
	{
		JSize charCount;
		input >> charCount;

		JIndex fontIndex;
		input >> fontIndex;

		input >> size;
		input >> JBoolFromString(fStyle.bold)
			  >> JBoolFromString(fStyle.italic)
			  >> JBoolFromString(fStyle.strike);
		input >> fStyle.underlineCount;

		JIndex colorIndex;
		input >> colorIndex;
		fStyle.color = colorList.GetItem(colorIndex);

		style->AppendItems(
			JFontManager::GetFont(*fontNameList.GetItem(fontIndex), size, fStyle),
			charCount);
	}

	return true;
}

/******************************************************************************
 WritePrivateFormat

 ******************************************************************************/

void
JStyledText::WritePrivateFormat
	(
	std::ostream& output
	)
	const
{
	if (!IsEmpty())
	{
		WritePrivateFormat(output, kCurrentPrivateFormatVersion,
						   itsText, *itsStyles, SelectAll());
	}
	else
	{
		output << kCurrentPrivateFormatVersion;
		output << " 0 0 0 0";
	}
}

/******************************************************************************
 WritePrivateFormat (static)

	We have to be able to write each version because this is what we
	put on the clipboard.

	version 1:  initial version.

 ******************************************************************************/

void
JStyledText::WritePrivateFormat
	(
	std::ostream&			output,
	const JFileVersion		vers,
	const JString&			text,
	const JRunArray<JFont>&	style,
	const JCharacterRange&	charRange
	)
{
	assert( !charRange.IsEmpty() );
	assert( text.RangeValid(charRange) );

	TextRange range;
	range.charRange = charRange;

	if (charRange.first == 1 && charRange.last == text.GetCharacterCount())
	{
		range.byteRange.Set(1, text.GetByteCount());
	}
	else
	{
		JStringIterator iter(text);
		range = CharToTextRange(charRange, &iter);
	}

	WritePrivateFormat(output, vers, text, style, range);
}

// static private

void
JStyledText::WritePrivateFormat
	(
	std::ostream&			output,
	const JFileVersion		vers,
	const JString&			text,
	const JRunArray<JFont>&	style,
	const TextRange&		range
	)
{
	assert( vers <= kCurrentPrivateFormatVersion );

	// write version

	output << vers;

	// write text efficiently

	output << ' ' << range.charRange.GetCount() << ' ';
	output.write(text.GetRawBytes() + range.byteRange.first - 1, range.byteRange.GetCount());

	// build lists of font names and colors

	JSize styleRunCount = 0;
	JPtrArray<JString> fontNameList(JPtrArrayT::kDeleteAll);
	fontNameList.SetCompareFunction(JCompareStringsCaseSensitive);
	JArray<JColorID> colorList;
	colorList.SetCompareFunction(std::function([](const JColorID c1, const JColorID c2)
	{
		return c1 <=> c2;
	}));

	FontIterator iter(style, JListT::kStartBefore, range.charRange.first);
	do
	{
		const JFont f           = iter.GetRunData();
		const JString& fontName = f.GetName();
		bool found;
		const JIndex fontIndex =
			fontNameList.SearchSortedOTI(const_cast<JString*>(&fontName), JListT::kAnyMatch, &found);
		if (!found)
		{
			fontNameList.InsertAtIndex(fontIndex, fontName);
		}

		const JColorID color = f.GetStyle().color;
		const JIndex colorIndex =
			colorList.SearchSortedOTI(color, JListT::kAnyMatch, &found);
		if (!found)
		{
			colorList.InsertItemAtIndex(colorIndex, color);
		}

		styleRunCount++;
		iter.NextRun();
	}
		while (iter.GetRunStart() <= range.charRange.last);

	// write list of font names

	output << ' ' << fontNameList.GetItemCount();

	for (const auto* fontName : fontNameList)
	{
		output << ' ' << *fontName;
	}

	// write list of rgb values

	output << ' ' << colorList.GetItemCount();

	for (auto id : colorList)
	{
		output << ' ' << JColorManager::GetRGB(id);
	}

	// write styles

	output << ' ' << styleRunCount;

	iter.MoveTo(JListT::kStartBefore, range.charRange.first);
	bool keepGoing = true;
	while (keepGoing)
	{
		const JFont  f          = iter.GetRunData();
		const JString& fontName = f.GetName();

		JIndex fontIndex;
		bool found = fontNameList.SearchSorted(const_cast<JString*>(&fontName), JListT::kAnyMatch, &fontIndex);
		assert( found );

		const JFontStyle& fStyle = f.GetStyle();

		JIndex colorIndex;
		found = colorList.SearchSorted(fStyle.color, JListT::kAnyMatch, &colorIndex);
		assert( found );

		const JIndex firstInRun = iter.GetRunStart();
		const JSize remaining   = iter.GetRemainingInRun();
		iter.NextRun();
		keepGoing = iter.GetRunStart() <= range.charRange.last;

		output << ' ' << (keepGoing ? remaining : range.charRange.last - firstInRun + 1);
		output << ' ' << fontIndex;
		output << ' ' << f.GetSize();
		output << ' ' << JBoolToString(fStyle.bold)
					  << JBoolToString(fStyle.italic)
					  << JBoolToString(fStyle.strike);
		output << ' ' << fStyle.underlineCount;
		output << ' ' << colorIndex;
	}
}

/******************************************************************************
 Search and replace

	We only support regular expressions because there is no advantage in
	optimizing for literal strings.

 ******************************************************************************/

/******************************************************************************
 SearchForward

	Look for the next match beyond the given position.

 ******************************************************************************/

JStringMatch
JStyledText::SearchForward
	(
	const TextIndex&	startIndex,
	const JRegex&		regex,
	const bool			entireWord,
	const bool			wrapSearch,
	bool*				wrapped
	)
	const
{
	TextIndex i = startIndex;

	*wrapped = false;
	if (i.charIndex > itsText.GetCharacterCount() && wrapSearch)
	{
		i.charIndex = 1;
		i.byteIndex = 1;
		*wrapped    = true;
	}
	else if (i.charIndex > itsText.GetCharacterCount())
	{
		return JStringMatch(itsText);
	}

	JStringIterator iter(itsText);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, i.charIndex, i.byteIndex);

	while (true)
	{
		if (iter.Next(regex))
		{
			const JStringMatch& m = iter.GetLastMatch();
			if (!entireWord || IsEntireWord(m))
			{
				return m;
			}
		}

		if (iter.AtEnd() && wrapSearch && !(*wrapped) && startIndex.charIndex > 1)
		{
			iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
			*wrapped  = true;
		}
		else if (iter.AtEnd() || (*wrapped && iter.GetNextCharacterIndex() >= startIndex.charIndex))
		{
			break;
		}
	}

	return JStringMatch(itsText);
}

/******************************************************************************
 SearchBackward

	Look for the match before the given position.

 ******************************************************************************/

JStringMatch
JStyledText::SearchBackward
	(
	const TextIndex&	startIndex,
	const JRegex&		regex,
	const bool			entireWord,
	const bool			wrapSearch,
	bool*				wrapped
	)
	const
{
	TextIndex i = startIndex;

	*wrapped = false;
	if (i.charIndex == 1 && wrapSearch)
	{
		i.charIndex = itsText.GetCharacterCount();
		i.byteIndex = itsText.GetByteCount();
		*wrapped  = true;
	}
	else if (i.charIndex == 1)
	{
		return JStringMatch(itsText);
	}

	JStringIterator iter(itsText);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, i.charIndex, i.byteIndex);

	while (true)
	{
		if (iter.Prev(regex))
		{
			const JStringMatch& m = iter.GetLastMatch();
			if (!entireWord || IsEntireWord(m))
			{
				return m;
			}
		}

		if (iter.AtBeginning() && wrapSearch && !(*wrapped) &&
			startIndex.charIndex < itsText.GetCharacterCount())
		{
			iter.MoveTo(JStringIterator::kStartAtEnd, 0);
			*wrapped = true;
		}
		else if (iter.AtBeginning() ||
				  (*wrapped && iter.GetPrevCharacterIndex() <= startIndex.charIndex))
		{
			break;
		}
	}

	return JStringMatch(itsText);
}

/******************************************************************************
 ReplaceMatch

	Replace the specified range with the given replace text.

 ******************************************************************************/

JStyledText::TextCount
JStyledText::ReplaceMatch
	(
	const JStringMatch&	match,
	const JString&		replaceStr,
	JInterpolate*		interpolator,
	const bool			preserveCase,
	const bool			createUndo
	)
{
	const JString replaceText =
		PrepareReplaceMatch(match, replaceStr, interpolator, preserveCase);

	const TextRange r(match.GetCharacterRange(), match.GetUtf8ByteRange());
	if (createUndo)
	{
		Paste(r, replaceText);	// handles undo
	}
	else
	{
		PrivatePaste(r, replaceText, nullptr);
	}

	return TextCount(replaceText.GetCharacterCount(), replaceText.GetByteCount());
}

/******************************************************************************
 PrepareReplaceMatch (private)

	Construct the replace text.

 ******************************************************************************/

JString
JStyledText::PrepareReplaceMatch
	(
	const JStringMatch&	match,
	const JString&		replaceStr,
	JInterpolate*		interpolator,
	const bool			preserveCase
	)
	const
{
	JString replaceText;
	if (interpolator != nullptr)
	{
		replaceText = interpolator->Interpolate(replaceStr, match);
	}
	else
	{
		replaceText = replaceStr;
	}

	if (preserveCase)
	{
		replaceText.MatchCase(itsText.GetRawBytes(), match.GetUtf8ByteRange());
	}

	return replaceText;
}

/******************************************************************************
 ReplaceAllInRange

	Replace every occurrence of the search pattern with the replace string.
	Returns the resulting text range.

 ******************************************************************************/

JStyledText::TextRange
JStyledText::ReplaceAllInRange
	(
	const TextRange&	range,
	const JRegex&		regex,
	const bool			entireWord,
	const JString&		replaceStr,
	JInterpolate*		interpolator,
	const bool			preserveCase
	)
{
	JString text;
	JRunArray<JFont> styles;
	styles.SetMinLgSize(itsStyles->GetMinLgSize());
	FontIterator styleIter(&styles);

	if (range.charRange.GetCount() == itsText.GetCharacterCount())	// avoid counting characters
	{
		text   = itsText;
		styles = *itsStyles;
	}
	else
	{
		text.Set(itsText.GetRawBytes(), range.byteRange);
		styleIter.InsertSlice(*itsStyles, range.charRange);
	}

	JLatentPG pg(100);
	pg.VariableLengthProcessBeginning(JGetString("ReplacingText::JStyledText"), true, true);

	bool changed = false;

	JRunArray<JFont> newStyles;
	newStyles.SetMinLgSize(itsStyles->GetMinLgSize());

	JStringIterator textIter(&text);
	while (textIter.Next(regex))
	{
		const JStringMatch& match = textIter.GetLastMatch();
		if (entireWord && !IsEntireWord(text, match))
		{
			continue;
		}

		const JString replaceText =
			PrepareReplaceMatch(match, replaceStr, interpolator, preserveCase);

		const JString s(text, JString::kNoCopy);
		JStringIterator textIter2(s);
		textIter2.UnsafeMoveTo(JStringIterator::kStartBeforeChar, match.GetCharacterRange().first, match.GetUtf8ByteRange().first);

		styleIter.MoveTo(JListT::kStartBefore, match.GetCharacterRange().first);

		newStyles.RemoveAll();
		newStyles.AppendItems(CalcInsertionFont(textIter2, styleIter),
								 replaceText.GetCharacterCount());

		styleIter.RemoveNext(match.GetCharacterRange().GetCount());
		textIter.RemoveLastMatch();		// invalidates match

		const TextCount count = InsertText(&textIter, &styleIter, replaceText, newStyles);
		textIter.SkipNext(count.charCount);

		changed = true;
		if (!pg.IncrementProgress())
		{
			break;
		}
	}
	textIter.Invalidate();
	styleIter.Invalidate();

	pg.ProcessFinished();

	if (changed)
	{
		Paste(range, text, &styles);	// handles undo

		return TextRange(range.GetFirst(),
			TextCount(text.GetCharacterCount(), text.GetByteCount()));
	}
	else
	{
		return TextRange();
	}
}

/******************************************************************************
 IsEntireWord (protected)

	Return true if the given character range is a single, complete word.

 ******************************************************************************/

bool
JStyledText::IsEntireWord
	(
	const JString&		text,
	const TextRange&	range
	)
	const
{
	const JString s(text, JString::kNoCopy);	// avoid potential double iterator

	JStringIterator iter(s);
	JUtf8Character c;

	if (range.charRange.first > 1)
	{
		iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, range.charRange.first, range.byteRange.first);
		if (iter.Prev(&c) && IsCharacterInWord(c))
		{
			return false;
		}
	}

	if (range.charRange.last < text.GetCharacterCount())
	{
		iter.UnsafeMoveTo(JStringIterator::kStartAfterChar, range.charRange.last, range.byteRange.last);
		if (iter.Next(&c) && IsCharacterInWord(c))
		{
			return false;
		}
	}

	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, range.charRange.first, range.byteRange.first);
	while (iter.Next(&c))
	{
		if (!IsCharacterInWord(c))
		{
			return false;
		}

		if (iter.AtEnd() || iter.GetNextCharacterIndex() > range.charRange.last)
		{
			break;
		}
	}

	return true;
}

/******************************************************************************
 SearchForward

	Look for the next match beyond the given position.

 ******************************************************************************/

void
jComputeForwardFontRange
	(
	const JStyledText::TextIndex&	start,
	const JString&					text,
	const FontIterator&				iter,
	const bool						wrapped,
	JStyledText::TextRange*			range
	)
{
	range->charRange.Set(iter.GetRunStart(), iter.GetRunEnd());

	if (wrapped)
	{
		range->byteRange = JString::CharacterToUtf8ByteRange(text.GetRawBytes(), range->charRange);
	}
	else	// optimize from where we started
	{
		const JSize byteOffset =
			JString::CountBytes(
				text.GetRawBytes() + start.byteIndex - 1,
				range->charRange.first - start.charIndex);

		range->byteRange.SetFirstAndCount(
			start.byteIndex + byteOffset,
			JString::CountBytes(
				text.GetRawBytes() + start.byteIndex + byteOffset - 1,
				range->charRange.GetCount()));
	}
}

bool
JStyledText::SearchForward
	(
	const std::function<bool(const JFont&)>	match,

	const TextIndex&	start,
	const bool			wrapSearch,
	bool*				wrapped,
	TextRange*			range
	)
	const
{
	*wrapped = false;
	range->SetToNothing();

	FontIterator iter(*itsStyles, JListT::kStartBefore, start.charIndex);
	if (!iter.AtEnd() && iter.GetRemainingInRun() < iter.GetRunLength())
	{
		iter.NextRun();
	}

	if (iter.AtEnd())
	{
		if (!wrapSearch)
		{
			return false;
		}
		iter.MoveTo(JListT::kStartAtBeginning, 0);
		*wrapped = true;
	}

	do
	{
		if (match(iter.GetRunData()))
		{
			jComputeForwardFontRange(start, itsText, iter, *wrapped, range);
			return true;
		}

		iter.NextRun();
	}
		while (!iter.AtEnd());

	if (!wrapSearch || *wrapped)
	{
		return false;
	}

	iter.MoveTo(JListT::kStartAtBeginning, 0);
	*wrapped = true;

	do
	{
		if (match(iter.GetRunData()))
		{
			jComputeForwardFontRange(start, itsText, iter, *wrapped, range);
			return true;
		}

		iter.NextRun();
	}
		while (iter.GetRunStart() < start.charIndex);

	return false;
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return true.

 ******************************************************************************/

void
jComputeBackwardFontRange
	(
	const JStyledText::TextIndex&	start,
	const JString&					text,
	const FontIterator&				iter,
	const bool						wrapped,
	JStyledText::TextRange*			range
	)
{
	range->charRange.Set(iter.GetRunStart(), iter.GetRunEnd());

	if (wrapped)
	{
		range->byteRange = JString::CharacterToUtf8ByteRange(text.GetRawBytes(), range->charRange);
	}
	else	// optimize from where we started
	{
		JSize byteOffset;
		bool ok =
			JString::CountBytesBackward(
				text.GetRawBytes(), start.byteIndex - 1,
				start.charIndex - 1 - range->charRange.last, &byteOffset);
		assert( ok );

		JSize byteCount;
		ok = JString::CountBytesBackward(
				text.GetRawBytes(), start.byteIndex - 1 - byteOffset,
				range->charRange.GetCount(), &byteCount);
		assert( ok );

		range->byteRange.Set(
			start.byteIndex - byteOffset - byteCount,
			start.byteIndex - byteOffset - 1);
	}
}

bool
JStyledText::SearchBackward
	(
	const std::function<bool(const JFont&)>	match,

	const TextIndex&	start,
	const bool			wrapSearch,
	bool*				wrapped,
	TextRange*			range
	)
	const
{
	*wrapped = false;
	range->SetToNothing();

	FontIterator iter(*itsStyles, JListT::kStartBefore, start.charIndex);
	if (!iter.AtBeginning() && iter.GetRemainingInRun() < iter.GetRunLength())
	{
		iter.SkipPrev(iter.GetRunLength() - iter.GetRemainingInRun());
	}

	if (iter.AtBeginning())
	{
		if (!wrapSearch)
		{
			return false;
		}
		iter.MoveTo(JListT::kStartAtEnd, 0);
		*wrapped = true;
	}

	do
	{
		iter.PrevRun();
		if (match(iter.GetRunData()))
		{
			jComputeBackwardFontRange(start, itsText, iter, *wrapped, range);
			return true;
		}
	}
		while (!iter.AtBeginning());

	if (!wrapSearch || *wrapped)
	{
		return false;
	}

	iter.MoveTo(JListT::kStartAtEnd, 0);
	iter.PrevRun();
	*wrapped = true;

	do
	{
		if (match(iter.GetRunData()))
		{
			jComputeBackwardFontRange(start, itsText, iter, *wrapped, range);
			return true;
		}

		iter.PrevRun();
	}
		while (iter.GetRunEnd() > start.charIndex);

	return false;
}

/******************************************************************************
 Set font

	Returns true if anything actually changed

	Handles undo if !clearUndo

 ******************************************************************************/

bool
JStyledText::SetFontName
	(
	const TextRange&	range,
	const JString&		name,
	const bool			clearUndo
	)
{
	bool isNew;
	JSTUndoStyle* undo = nullptr;

	if (clearUndo)
	{
		ClearUndo();
	}
	else
	{
		undo = GetStyleUndo(range, &isNew);
	}

	JFont f1 = itsDefaultFont, f2 = itsDefaultFont;
	bool changed = false;
	FontIterator iter(itsStyles, JListT::kStartBefore, range.charRange.first);
	JIndex i;
	while (iter.GetNextItemIndex(&i) && i <= range.charRange.last)
	{
		const bool ok = iter.Next(&f1, JListT::kStay);
		assert( ok );

		f2 = f1;
		f2.SetName(name);
		if (f2 != f1)
		{
			const JIndex end = JMin(iter.GetRunEnd(), range.charRange.last);
			iter.SetNext(f2, end-i+1);
			changed = true;
		}
		else
		{
			iter.SkipNext();
		}
	}
	iter.Invalidate();

	if (changed)
	{
		AdjustFontToDisplayGlyphs(range, itsText, itsStyles);

		if (undo != nullptr && isNew)
		{
			itsUndoChain->NewUndo(undo);
		}
		BroadcastTextChanged(range, 0, 0, false, false);
	}
	else
	{
		jdelete undo;
	}

	return changed;
}

bool
JStyledText::SetFontSize
	(
	const TextRange&	range,
	const JSize			size,
	const bool		clearUndo
	)
{
	#define LocalVarName   size
	#define GetElementName GetSize()
	#define SetElementName SetSize
	#include "JStyledTextSetFont.th"
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

bool
JStyledText::SetFontBold
	(
	const TextRange&	range,
	const bool		bold,
	const bool		clearUndo
	)
{
	#define LocalVarName   bold
	#define GetElementName GetStyle().bold
	#define SetElementName SetBold
	#include "JStyledTextSetFont.th"
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

bool
JStyledText::SetFontItalic
	(
	const TextRange&	range,
	const bool		italic,
	const bool		clearUndo
	)
{
	#define LocalVarName   italic
	#define GetElementName GetStyle().italic
	#define SetElementName SetItalic
	#include "JStyledTextSetFont.th"
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

bool
JStyledText::SetFontUnderline
	(
	const TextRange&	range,
	const JSize			count,
	const bool		clearUndo
	)
{
	#define LocalVarName   count
	#define GetElementName GetStyle().underlineCount
	#define SetElementName SetUnderlineCount
	#include "JStyledTextSetFont.th"
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

bool
JStyledText::SetFontStrike
	(
	const TextRange&	range,
	const bool		strike,
	const bool		clearUndo
	)
{
	#define LocalVarName   strike
	#define GetElementName GetStyle().strike
	#define SetElementName SetStrike
	#include "JStyledTextSetFont.th"
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

bool
JStyledText::SetFontColor
	(
	const TextRange&	range,
	const JColorID		color,
	const bool		clearUndo
	)
{
	#define LocalVarName   color
	#define GetElementName GetStyle().color
	#define SetElementName SetColor
	#include "JStyledTextSetFont.th"
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

bool
JStyledText::SetFontStyle
	(
	const TextRange&	range,
	const JFontStyle&	style,
	const bool		clearUndo
	)
{
	#define LocalVarName   style
	#define GetElementName GetStyle()
	#define SetElementName SetStyle
	#include "JStyledTextSetFont.th"
	#undef LocalVarName
	#undef GetElementName
	#undef SetElementName
}

void
JStyledText::SetFont
	(
	const TextRange&	range,
	const JFont&		f,
	const bool			clearUndo
	)
{
	bool isNew;
	JSTUndoStyle* undo = nullptr;

	if (clearUndo)
	{
		ClearUndo();
	}
	else
	{
		undo = GetStyleUndo(range, &isNew);
	}

	FontIterator iter(itsStyles, JListT::kStartBefore, range.charRange.first);
	iter.SetNext(f, range.charRange.GetCount());
	iter.Invalidate();

	AdjustFontToDisplayGlyphs(range, itsText, itsStyles);

	if (undo != nullptr && isNew)
	{
		itsUndoChain->NewUndo(undo);
	}
	BroadcastTextChanged(range, 0, 0, false, false);
}

// protected - for JSTUndoStyle

void
JStyledText::SetFont
	(
	const TextRange&		range,
	const JRunArray<JFont>&	fontList
	)
{
	assert( range.GetCount().charCount == fontList.GetItemCount() );

	JFont f = itsDefaultFont;
	FontIterator fIter(fontList);
	FontIterator sIter(itsStyles, JListT::kStartBefore, range.charRange.first);

	while (fIter.Next(&f) && sIter.SetNext(f)) { /* copy to itsStyles */ }
	fIter.Invalidate();
	sIter.Invalidate();

	AdjustFontToDisplayGlyphs(range, itsText, itsStyles);

	BroadcastTextChanged(range, 0, 0, false, false);
}

/******************************************************************************
 SetAllFontNameAndSize

	This function is useful for unstyled text editors that allow the user
	to change the font and size.

	It preserves the styles, in case they are controlled by the program.
	(e.g. context sensitive hilighting)

	You can choose whether or not to throw out all Undo information.
	Unstyled text editors can usually preserve Undo, since they will not
	allow the user to modify styles.  (We explicitly ask for this because
	it is too easy to forget about the issue.)

	Modifies existing undo's.  Does not create a new undo.

 ******************************************************************************/

void
JStyledText::SetAllFontNameAndSize
	(
	const JString&	name,
	const JSize		size,
	const bool	clearUndo
	)
{
	if (clearUndo)
	{
		ClearUndo();
	}

	if (!itsStyles->IsEmpty())
	{
		FontIterator iter(itsStyles);

		while (!iter.AtEnd())
		{
			JFont f = iter.GetRunData();
			f.Set(name, size, f.GetStyle());
			iter.SetNext(f, iter.GetRunLength());
		}
	}

	const TextRange& all = SelectAll();
	AdjustFontToDisplayGlyphs(all, itsText, itsStyles);

	JPtrArray<JUndo>* list;
	JUndo* undo;
	if (itsUndoChain->GetUndoList(&list))
	{
		for (auto* u : *list)
		{
			dynamic_cast<JSTUndoBase&>(*u).SetFont(name, size);
		}
	}
	else if (itsUndoChain->GetCurrentUndo(&undo))
	{
		dynamic_cast<JSTUndoBase&>(*undo).SetFont(name, size);
	}

	itsDefaultFont.Set(name, size, itsDefaultFont.GetStyle());

	BroadcastTextChanged(all, 0, 0, false, false);
}

/******************************************************************************
 Copy

	Returns true if there was anything to copy.  style can be nullptr.
	If function returns false, text and style are not modified.

 ******************************************************************************/

bool
JStyledText::Copy
	(
	const TextRange&	range,
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	if (!range.IsEmpty())
	{
		text->Set(itsText.GetRawBytes(), range.byteRange);

		if (style != nullptr)
		{
			style->RemoveAll();
			style->AppendSlice(*itsStyles, range.charRange);

			assert( style->GetItemCount() == text->GetCharacterCount() );
		}

		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Paste

	style can be nullptr

 ******************************************************************************/

JStyledText::TextRange
JStyledText::Paste
	(
	const TextRange&		range,
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	assert( style == nullptr || style->GetItemCount() == text.GetCharacterCount() );

	bool isNew;
	JSTUndoPaste* newUndo = GetPasteUndo(range, &isNew);
	assert( newUndo != nullptr );

	const TextCount pasteCount = PrivatePaste(range, text, style);
	newUndo->SetCount(pasteCount);

	if (isNew)
	{
		itsUndoChain->NewUndo(newUndo);
	}

	const JInteger charDelta = pasteCount.charCount - range.charRange.GetCount(),
				   byteDelta = pasteCount.byteCount - range.byteRange.GetCount();

	TextRange r(range.GetFirst(), pasteCount);
	BroadcastTextChanged(r, charDelta, byteDelta, !range.IsEmpty());
	return r;
}

/******************************************************************************
 PrivatePaste (private)

	Returns number of characters that were actually inserted.

	style can be nullptr.

 ******************************************************************************/

JStyledText::TextCount
JStyledText::PrivatePaste
	(
	const TextRange&		range,
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	if (!range.IsEmpty())
	{
		PrivateDeleteText(range);
	}

	if (itsPasteStyledTextFlag && style != nullptr)
	{
		return InsertText(range.GetFirst(), text, *style);
	}
	else
	{
		return InsertText(range.GetFirst(), text,
			itsPasteStyledTextFlag ?
				CalcInsertionFont(range.GetFirst()) :
				itsDefaultFont);
	}
}

/******************************************************************************
 InsertText (private)

	Returns number of characters / bytes that were actually inserted.

	In second version, iterator must be positioned at insertion index.

 ******************************************************************************/

JStyledText::TextCount
JStyledText::InsertText
	(
	const TextIndex&	index,
	const JString&		text,
	const JFont&		font
	)
{
	JRunArray<JFont> style;
	style.SetMinLgSize(itsStyles->GetMinLgSize());
	style.AppendItems(font, text.GetCharacterCount());

	return InsertText(index, text, style);
}

JStyledText::TextCount
JStyledText::InsertText
	(
	const TextIndex&		index,
	const JString&			text,
	const JRunArray<JFont>&	style
	)
{
	JStringIterator textIter(&itsText);
	textIter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, index.charIndex, index.byteIndex);

	FontIterator styleIter(itsStyles, JListT::kStartBefore, index.charIndex);

	return InsertText(&textIter, &styleIter, text, style);
}

JStyledText::TextCount
JStyledText::InsertText
	(
	JStringIterator*		targetText,
	FontIterator*			targetStyle,
	const JString&			text,
	const JRunArray<JFont>&	style
	)
{
	if (text.IsEmpty())
	{
		return TextCount();
	}

	JString* cleanText           = nullptr;
	JRunArray<JFont>* cleanStyle = nullptr;

	bool okToInsert;
	const bool allocated = CleanText(text, style, &cleanText, &cleanStyle, &okToInsert);
	if (!okToInsert)
	{
		if (allocated)
		{
			jdelete cleanText;
			jdelete cleanStyle;
		}
		return TextCount();
	}

	if (!allocated)
	{
		cleanText  = const_cast<JString*>(&text);
		cleanStyle = const_cast<JRunArray<JFont>*>(&style);
	}

	targetText->Insert(*cleanText);
	targetStyle->InsertSlice(*cleanStyle, JIndexRange(1, cleanStyle->GetItemCount()));

	const TextCount result(cleanText->GetCharacterCount(), cleanText->GetByteCount());

	if (allocated)
	{
		jdelete cleanText;
		jdelete cleanStyle;
	}

	return result;
}

/******************************************************************************
 CleanText (private)

	Removes illegal characters, converts to UNIX newline format, adjusts
	fonts to ensure that all characters will be rendered correctly, lets
	derived class perform additional filtering.  Returns true if the text
	was modified.

	*okToInsert is true if derived class filtering accepted the text.

 ******************************************************************************/

#define COPY_FOR_CLEAN_TEXT \
	if (*cleanText == nullptr) \
	{ \
		*cleanText = jnew JString(text); \
		*cleanStyle = jnew JRunArray<JFont>(style); \
	}

bool
JStyledText::CleanText
	(
	const JString&			text,
	const JRunArray<JFont>&	style,
	JString**				cleanText,
	JRunArray<JFont>**		cleanStyle,
	bool*					okToInsert
	)
{
	*okToInsert = true;
	if (text.IsEmpty())
	{
		return false;
	}

	assert( text.GetCharacterCount() == style.GetItemCount() );

	*cleanText  = nullptr;
	*cleanStyle = nullptr;

	// remove illegal characters

	if (ContainsIllegalChars(text))
	{
		COPY_FOR_CLEAN_TEXT

		RemoveIllegalChars(*cleanText, *cleanStyle);
	}

	// convert from DOS format -- deleting is n^2, so we copy instead
	// (not using Split because the text could be very large)

	if ((*cleanText != nullptr && (**cleanText).Contains(kDOSNewline)) ||
		(*cleanText == nullptr && text.Contains(kDOSNewline)))
	{
		COPY_FOR_CLEAN_TEXT

		JString tmpText;
		tmpText.SetMinLgSize(JLCeil(std::log2((**cleanText).GetByteCount()+256)));

		JRunArray<JFont> tmpStyle((**cleanStyle).GetRunCount()+16);

		JStringIterator iter(*cleanText);
		JUtf8Character c;

		bool done = false;
		iter.BeginMatch();
		while (!done)
		{
			done = !iter.Next("\r");	// ensures that we append trailing text

			const JStringMatch& m = iter.FinishMatch();
			if (!m.IsEmpty())
			{
				tmpText.Append(m.GetString());
				tmpStyle.AppendSlice((**cleanStyle), m.GetCharacterRange());
			}
			iter.BeginMatch();
		}

		**cleanText  = tmpText;
		**cleanStyle = tmpStyle;
	}

	// convert from Macintosh format

	else if ((*cleanText != nullptr && (**cleanText).Contains(kMacintoshNewline)) ||
			 (*cleanText == nullptr && text.Contains(kMacintoshNewline)))
	{
		COPY_FOR_CLEAN_TEXT

		ConvertFromMacintoshNewlinetoUNIXNewline(*cleanText);
	}

	// allow derived classes to make additional changes
	// (last so we don't pass anything illegal to FilterText())

	if (NeedsToFilterText(*cleanText != nullptr ? **cleanText : text))
	{
		COPY_FOR_CLEAN_TEXT

		*okToInsert = FilterText(*cleanText, *cleanStyle);
	}

	if (NeedsToAdjustFontToDisplayGlyphs(*cleanText  != nullptr ? **cleanText  : text,
										 *cleanStyle != nullptr ? **cleanStyle : style))
	{
		COPY_FOR_CLEAN_TEXT

		AdjustFontToDisplayGlyphs(
			TextRange(
				JCharacterRange(1, (**cleanText).GetCharacterCount()),
				JUtf8ByteRange (1, (**cleanText).GetByteCount())),
			**cleanText, *cleanStyle);
	}

	return *cleanText != nullptr;
}

/******************************************************************************
 ContainsIllegalChars (static)

	Returns true if the given text contains characters that we will not
	accept:  00-08, 0B, 0E-1F, 7F

	We accept form feed (0C) because PrintPlainText() converts it to space.

	We accept all characters above 0x7F because JString will clean UTF-8.

 ******************************************************************************/

static JRegex illegalCharRegex("[\\0\x01-\x08\x0B\x0E-\x1F\x7F]+");

bool
JStyledText::ContainsIllegalChars
	(
	const JString& text
	)
{
	illegalCharRegex.SetUtf8(false);
	return illegalCharRegex.Match(text);
}

/******************************************************************************
 RemoveIllegalChars (static)

	Returns true if we had to remove any characters that
	ContainsIllegalChars() would flag.

	style can be nullptr or empty.

 ******************************************************************************/

bool
JStyledText::RemoveIllegalChars
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	assert( style == nullptr || style->IsEmpty() ||
			style->GetItemCount() == text->GetCharacterCount() );

	bool changed = false;

	FontIterator* styleIter = nullptr;
	if (style != nullptr && !style->IsEmpty())
	{
		styleIter = jnew FontIterator(style);
	}

	JStringIterator textIter(text);
	illegalCharRegex.SetUtf8(false);
	while (textIter.Next(illegalCharRegex))
	{
		const JStringMatch& m = textIter.GetLastMatch();

		if (styleIter != nullptr)
		{
			styleIter->MoveTo(JListT::kStartBefore, m.GetCharacterRange().first);
			styleIter->RemoveNext(m.GetCharacterRange().GetCount());
		}
		textIter.RemoveLastMatch();		// invalidates m
		changed = true;
	}

	jdelete styleIter;
	return changed;
}

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return true if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

bool
JStyledText::NeedsToFilterText
	(
	const JString& text
	)
	const
{
	return false;
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return false if the text cannot be used at all.

 ******************************************************************************/

bool
JStyledText::FilterText
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	return true;
}

/******************************************************************************
 NeedsToAdjustFontToDisplayGlyphs(virtual protected)

 ******************************************************************************/

bool
JStyledText::NeedsToAdjustFontToDisplayGlyphs
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
	return false;
}

/******************************************************************************
 AdjustFontToDisplayGlyphs (virtual protected)

	Font substitution to hopefully make all characters render.  Returns
	true if font was modified.

 ******************************************************************************/

bool
JStyledText::AdjustFontToDisplayGlyphs
	(
	const TextRange&	range,
	const JString&		text,
	JRunArray<JFont>*	style
	)
{
	return false;
}

/******************************************************************************
 DeleteText

	We create JSTUndoTyping so keys pressed after the delete key count
	as part of the undo task.

 ******************************************************************************/

void
JStyledText::DeleteText
	(
	const TextRange& range
	)
{
	auto* newUndo = jnew JSTUndoTyping(this, range);

	PrivateDeleteText(range);

	itsUndoChain->NewUndo(newUndo);

	BroadcastTextChanged(TextRange(range.GetFirst(), TextCount(0,0)),
		- (JInteger) range.charRange.GetCount(),
		- (JInteger) range.byteRange.GetCount(),
		true);
}

/******************************************************************************
 PrivateDeleteText (private)

 ******************************************************************************/

void
JStyledText::PrivateDeleteText
	(
	const TextRange& range
	)
{
	JStringIterator textIter(&itsText);
	textIter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, range.charRange.first, range.byteRange.first);

	FontIterator styleIter(itsStyles, JListT::kStartBefore, range.charRange.first);

	PrivateDeleteText(&textIter, &styleIter, range.charRange.GetCount());
}

void
JStyledText::PrivateDeleteText
	(
	JStringIterator*	textIter,
	FontIterator*		styleIter,
	const JSize			charCount
	)
{
	textIter->RemoveNext(charCount);
	styleIter->RemoveNext(charCount);
}

/******************************************************************************
 InsertCharacter

 ******************************************************************************/

JUndo*
JStyledText::InsertCharacter
	(
	const TextRange&		replaceRange,
	const JUtf8Character&	key,
	const JFont&			font,
	TextCount*				count
	)
{
	JStringIterator textIter(&itsText);
	textIter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, replaceRange.charRange.first, replaceRange.byteRange.first);

	FontIterator styleIter(itsStyles, JListT::kStartBefore, replaceRange.charRange.first);

	bool isNew = true;
	JSTUndoTyping* typingUndo;
	if (replaceRange.IsEmpty())
	{
		typingUndo = GetTypingUndo(replaceRange.GetFirst(), &isNew);
	}
	else
	{
		typingUndo = jnew JSTUndoTyping(this, replaceRange);

		PrivateDeleteText(&textIter, &styleIter, replaceRange.charRange.GetCount());
	}

	typingUndo->HandleCharacters(TextCount(1, key.GetByteCount()));

	const JString text(key.GetBytes(), key.GetByteCount(), JString::kNoCopy);

	JRunArray<JFont> style;
	style.AppendItem(font);

	*count = InsertText(&textIter, &styleIter, text, style);
	assert( count->charCount == 1 );

	textIter.Invalidate();
	styleIter.Invalidate();

	if (key == '\n' && itsAutoIndentFlag)
	{
		TextCount indentCount;
		AutoIndent(typingUndo, AdjustTextIndex(replaceRange.GetFirst(), +1), &indentCount);
		*count += indentCount;
	}

	if (isNew)
	{
		itsUndoChain->NewUndo(typingUndo);
	}

	const JInteger charDelta = count->charCount - replaceRange.charRange.GetCount(),
				   byteDelta = count->byteCount - replaceRange.byteRange.GetCount();

	BroadcastTextChanged(
		TextRange(replaceRange.GetFirst(), *count),
		charDelta, byteDelta,
		!replaceRange.IsEmpty());

	return typingUndo;
}

/******************************************************************************
 BackwardDelete

	Delete characters preceding the insertion caret.

	Returns index of character after caret.

 ******************************************************************************/

JStyledText::TextIndex
JStyledText::BackwardDelete
	(
	const TextIndex&	lineStart,
	const TextIndex&	caretIndex,
	const bool			deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle,
	JUndo**				undo
	)
{
	if (undo != nullptr)
	{
		*undo = nullptr;
	}

	if (caretIndex.charIndex <= 1)
	{
		return TextIndex(1,1);
	}

	JStringIterator iter(&itsText);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, caretIndex.charIndex, caretIndex.byteIndex);

	iter.BeginMatch();

	JUtf8Character c;
	if (deleteToTabStop && iter.Prev(&c, JStringIterator::kStay) && isWhitespace(c))
	{
		const JIndex textColumn = GetColumnForChar(lineStart, caretIndex);
		if ((textColumn-1) % itsCRMTabCharCount == 0)
		{
			JIndex deleteCount = 0;
			while (deleteCount < itsCRMTabCharCount && iter.Prev(&c))
			{
				if (c == ' ')
				{
					deleteCount++;
				}
				else if (c == '\t')
				{
					iter.Next(&c);
					JIndex i;
					if (iter.GetNextCharacterIndex(&i))
					{
						deleteCount += CRMGetTabWidth(
							GetColumnForChar(lineStart, TextIndex(i, iter.GetNextByteIndex())));
					}
					iter.Prev(&c);
				}
				else	// normal delete when close to text
				{
					iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, caretIndex.charIndex, caretIndex.byteIndex);
					iter.Prev(&c);
					break;
				}
			}
		}
		else			// normal delete when close to text
		{
			iter.Prev(&c);
		}
	}
	else
	{
		iter.Prev(&c);
	}

	const JStringMatch& match = iter.FinishMatch();
	const JCharacterRange r   = match.GetCharacterRange();

	if (returnText != nullptr)
	{
		returnText->Set(match.GetString());
	}
	if (returnStyle != nullptr)
	{
		returnStyle->RemoveAll();
		returnStyle->AppendSlice(*itsStyles, r);
	}

	bool isNew;
	JSTUndoTyping* typingUndo = GetTypingUndo(caretIndex, &isNew);
	typingUndo->HandleDelete(match, false);

	const JInteger charDelta = - (JInteger) r.GetCount(),
				   byteDelta = - (JInteger) match.GetUtf8ByteRange().GetCount();

	FontIterator styleIter(itsStyles, JListT::kStartBefore, r.first);
	styleIter.RemoveNext(r.GetCount());

	iter.RemoveLastMatch();		// invalidates match

	if (isNew)
	{
		itsUndoChain->NewUndo(typingUndo);
	}

	TextIndex returnIndex;
	if (iter.AtEnd())
	{
		returnIndex = GetBeyondEnd();
	}
	else
	{
		returnIndex = TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
	}

	iter.Invalidate();
	styleIter.Invalidate();

	BroadcastTextChanged(TextRange(returnIndex, TextCount(0,0)),
						 charDelta, byteDelta, true);

	if (undo != nullptr)
	{
		*undo = typingUndo;
	}

	return returnIndex;
}

/******************************************************************************
 ForwardDelete

	Delete characters following the insertion caret.

 ******************************************************************************/

void
JStyledText::ForwardDelete
	(
	const TextIndex&	lineStart,
	const TextIndex&	caretIndex,
	const bool		deleteToTabStop,
	JString*			returnText,
	JRunArray<JFont>*	returnStyle,
	JUndo**				undo
	)
{
	if (undo != nullptr)
	{
		*undo = nullptr;
	}

	if (caretIndex.charIndex > itsText.GetCharacterCount())
	{
		return;
	}

	JStringIterator iter(&itsText);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, caretIndex.charIndex, caretIndex.byteIndex);
	iter.BeginMatch();

	JUtf8Character c;
	if (deleteToTabStop && iter.Next(&c, JStringIterator::kStay) && isWhitespace(c))
	{
		const JIndex textColumn = GetColumnForChar(lineStart, caretIndex);
		if ((textColumn-1) % itsCRMTabCharCount == 0)
		{
			JIndex deleteCount = 0;
			while (deleteCount < itsCRMTabCharCount && iter.Next(&c))
			{
				if (c == '\t')
				{
					break;
				}
				else if (c != ' ')	// normal delete when close to text
				{
					iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, caretIndex.charIndex, caretIndex.byteIndex);
					iter.Next(&c);
					break;
				}

				deleteCount++;
			}
		}
		else						// normal delete when close to text
		{
			iter.Next(&c);
		}
	}
	else
	{
		iter.Next(&c);
	}

	const JStringMatch& match = iter.FinishMatch();
	const JCharacterRange r   = match.GetCharacterRange();

	if (returnText != nullptr)
	{
		returnText->Set(match.GetString());
	}
	if (returnStyle != nullptr)
	{
		returnStyle->RemoveAll();
		returnStyle->AppendSlice(*itsStyles, r);
	}

	bool isNew;
	JSTUndoTyping* typingUndo = GetTypingUndo(caretIndex, &isNew);
	typingUndo->HandleFwdDelete(match);

	const JInteger charDelta = - (JInteger) r.GetCount(),
				   byteDelta = - (JInteger) match.GetUtf8ByteRange().GetCount();

	FontIterator styleIter(itsStyles, JListT::kStartBefore, r.first);
	styleIter.RemoveNext(r.GetCount());

	iter.RemoveLastMatch();		// invalidates match

	if (isNew)
	{
		itsUndoChain->NewUndo(typingUndo);
	}

	iter.Invalidate();
	styleIter.Invalidate();

	BroadcastTextChanged(TextRange(caretIndex, TextCount(0,0)),
		charDelta, byteDelta, true);

	if (undo != nullptr)
	{
		*undo = typingUndo;
	}
}

/******************************************************************************
 Outdent

	Remove tabs from the beginning of every line within the given range.
	The first line is assumed to start at the beginning of the range.

 ******************************************************************************/

JStyledText::TextRange
JStyledText::Outdent
	(
	const TextRange&	origRange,
	const JSize			tabCount,
	const bool			force
	)
{
	const TextIndex pEnd = GetParagraphEnd(origRange.GetLast(*this));

	const TextRange range(JCharacterRange(origRange.charRange.first, pEnd.charIndex),
						  JUtf8ByteRange(origRange.byteRange.first, pEnd.byteIndex));

	// check that there are enough tabs at the start of every selected line,
	// ignoring lines created by wrapping

	bool sufficientWS      = true;
	JSize prefixSpaceCount     = 0;		// min # of spaces at start of line
	bool firstNonemptyLine = true;

	JStringIterator textIter(&itsText);
	textIter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, range.charRange.first, range.byteRange.first);
	JIndex i;
	do
	{
		JUtf8Character c;
		if ((!textIter.Prev(&c, JStringIterator::kStay) || c == '\n') &&
			textIter.Next(&c, JStringIterator::kStay) && c != '\n')
		{
			for (JIndex j=1; j<=tabCount; j++)
			{
				// accept itsCRMTabCharCount spaces instead of tab
				// accept fewer spaces in front of tab

				JSize spaceCount = 0;
				while (textIter.Next(&c) && c == ' ' &&
					   spaceCount < itsCRMTabCharCount)
				{
					spaceCount++;
				}

				if (textIter.AtEnd())
				{
					break;	// last line contains only whitespace
				}
				if (j == 1 && (spaceCount < prefixSpaceCount || firstNonemptyLine))
				{
					prefixSpaceCount  = spaceCount;
					firstNonemptyLine = false;
				}
				if (spaceCount >= itsCRMTabCharCount)
				{
					continue;
				}

				if (c != '\t' && c != '\n')
				{
					sufficientWS = false;
					break;
				}
				if (c == '\n')
				{
					break;	// line is blank or contains only whitespace
				}
			}
		}
	}
		while (textIter.Next("\n") &&
			   textIter.GetNextCharacterIndex(&i) &&
			   range.charRange.Contains(i));

	// If same number of spaces at front of every line, remove those even if
	// not enough to shift by one tab.

	JSize tabCharCount = itsCRMTabCharCount;
	if (!sufficientWS && prefixSpaceCount > 0 && tabCount == 1)
	{
		tabCharCount = prefixSpaceCount;
	}
	else if (force)
	{
		sufficientWS = true;
	}
	else if (!sufficientWS)
	{
		return TextRange();
	}

	bool isNew;
	JSTUndoTabShift* undo = GetTabShiftUndo(range, &isNew);

	textIter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, range.charRange.first, range.byteRange.first);
	FontIterator styleIter(itsStyles, JListT::kStartBefore, range.charRange.first);

	JCharacterRange cr = range.charRange;

	JSize deleteCount = 0;	// only deleting single-byte characters
	do
	{
		JUtf8Character c;
		if ((!textIter.Prev(&c, JStringIterator::kStay) || c == '\n') &&
			textIter.Next(&c, JStringIterator::kStay) && c != '\n')
		{
			for (JIndex j=1; j<=tabCount; j++)
			{
				// The deletion point stays in same place (charIndex) and
				// simply eats characters.

				// accept tabCharCount spaces instead of tab
				// accept fewer spaces in front of tab

				JSize spaceCount = 0;
				while (textIter.Next(&c, JStringIterator::kStay) && c == ' ' &&
					   spaceCount < tabCharCount)
				{
					spaceCount++;
					styleIter.MoveTo(JListT::kStartBefore, textIter.GetNextCharacterIndex());
					PrivateDeleteText(&textIter, &styleIter, 1);
					deleteCount++;
					cr.last--;
				}

				if (textIter.AtEnd() ||	// last line contains only whitespace
					!sufficientWS)	// only remove tabCharCount spaces at front of every line
				{
					break;
				}

				if (spaceCount >= tabCharCount)
				{
					continue;	// removed equivalent of one tab
				}

				if (c != '\t')
				{
					break;
				}
				styleIter.MoveTo(JListT::kStartBefore, textIter.GetNextCharacterIndex());
				PrivateDeleteText(&textIter, &styleIter, 1);
				deleteCount++;
				cr.last--;
			}
		}
	}
		while (textIter.Next("\n") &&
			   textIter.GetNextCharacterIndex(&i) &&
			   cr.Contains(i));

	undo->SetCount(TextCount(cr.GetCount(), range.byteRange.GetCount() - deleteCount));

	if (isNew)
	{
		itsUndoChain->NewUndo(undo);
	}

	textIter.Invalidate();
	styleIter.Invalidate();

	BroadcastTextChanged(
		TextRange(
			JCharacterRange(range.charRange.first, range.charRange.last - deleteCount),
			JUtf8ByteRange (range.byteRange.first, range.byteRange.last - deleteCount)),
		-JInteger(deleteCount), -JInteger(deleteCount),
		true, false);

	return TextRange(cr, JUtf8ByteRange(range.byteRange.first, range.byteRange.last - deleteCount));
}

/******************************************************************************
 Indent

	Insert tabs at the beginning of every line within the given range.
	The first line is assumed to start at the beginning of the range.

 ******************************************************************************/

JStyledText::TextRange
JStyledText::Indent
	(
	const TextRange&	origRange,
	const JSize			tabCount
	)
{
	const TextIndex pEnd = GetParagraphEnd(origRange.GetLast(*this));

	const TextRange range(JCharacterRange(origRange.charRange.first, pEnd.charIndex),
						  JUtf8ByteRange(origRange.byteRange.first, pEnd.byteIndex));


	bool isNew;
	JSTUndoTabShift* undo = GetTabShiftUndo(range, &isNew);

	JString tabs;
	if (itsTabToSpacesFlag)
	{
		const JSize spaceCount = tabCount*itsCRMTabCharCount;
		for (JIndex i=1; i<=spaceCount; i++)
		{
			tabs.Append(" ");
		}
	}
	else
	{
		for (JIndex i=1; i<=tabCount; i++)
		{
			tabs.Append("\t");
		}
	}

	JRunArray<JFont> style;
	style.SetMinLgSize(itsStyles->GetMinLgSize());

	JStringIterator textIter(&itsText);
	textIter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, range.charRange.first, range.byteRange.first);

	FontIterator styleIter(itsStyles, JListT::kStartBefore, range.charRange.first);

	JCharacterRange cr = range.charRange;

	JSize insertCount = 0;	// only inserting single-byte characters
	JIndex i;
	do
	{
		JUtf8Character c;
		if ((!textIter.Prev(&c, JStringIterator::kStay) || c == '\n') &&
			textIter.Next(&c, JStringIterator::kStay) && c != '\n')
		{
			styleIter.MoveTo(JListT::kStartBefore, textIter.GetNextCharacterIndex());

			style.RemoveAll();
			style.AppendItems(CalcInsertionFont(textIter, styleIter),
								 tabs.GetCharacterCount());

			const JSize count = InsertText(&textIter, &styleIter, tabs, style).charCount;
			insertCount      += count;
			cr.last          += count;
		}
	}
		while (textIter.Next("\n") &&
			   textIter.GetNextCharacterIndex(&i) &&
			   cr.Contains(i));

	undo->SetCount(TextCount(cr.GetCount(), range.byteRange.GetCount() + insertCount));

	if (isNew)
	{
		itsUndoChain->NewUndo(undo);
	}

	const JUtf8ByteRange br(range.byteRange.first, range.byteRange.last + insertCount);
	const TextRange tr(cr, br);

	textIter.Invalidate();
	styleIter.Invalidate();

	BroadcastTextChanged(tr, insertCount, insertCount, false, false);

	return tr;
}

/******************************************************************************
 MoveText

 ******************************************************************************/

bool
JStyledText::MoveText
	(
	const TextRange&	srcRange,
	const TextIndex&	origDestIndex,
	const bool			copy,
	TextRange*			newRange
	)
{
	newRange->SetToNothing();

	if (!copy &&
		(srcRange.charRange.first <= origDestIndex.charIndex &&
		 origDestIndex.charIndex <= srcRange.charRange.last + 1))
	{
		return false;
	}

	JString text;
	JRunArray<JFont> styles;
	const bool ok = Copy(srcRange, &text, &styles);
	assert( ok );

	TextIndex destIndex = origDestIndex;

	JSTUndoBase* undo = nullptr;
	bool isNew;
	if (copy)
	{
		undo = GetPasteUndo(TextRange(
			JCharacterRange(destIndex.charIndex, 0),
			JUtf8ByteRange(destIndex.byteIndex, 0)), &isNew);
	}
	else	// move
	{
		TextIndex srcIndex = srcRange.GetFirst();
		if (destIndex.charIndex > srcRange.charRange.first)
		{
			destIndex.charIndex -= text.GetCharacterCount();
			destIndex.byteIndex -= text.GetByteCount();
		}
		else
		{
			assert( destIndex.charIndex < srcRange.charRange.first );
			srcIndex.charIndex += text.GetCharacterCount();
			srcIndex.byteIndex += text.GetByteCount();
		}

		undo = GetMoveUndo(srcIndex, destIndex, srcRange.GetCount(), &isNew);

		PrivateDeleteText(srcRange);
		BroadcastTextChanged(TextRange(srcRange.GetFirst(), TextCount(0,0)),
			-JInteger(srcRange.charRange.GetCount()),
			-JInteger(srcRange.byteRange.GetCount()),
			true);
	}
	assert( undo != nullptr );

	const TextCount insertCount = InsertText(destIndex, text, styles);
	undo->SetCount(insertCount);

	if (isNew)
	{
		itsUndoChain->NewUndo(undo);
	}

	newRange->Set(destIndex, insertCount);
	BroadcastTextChanged(*newRange, insertCount.charCount, insertCount.byteCount, false);

	return true;
}

/******************************************************************************
 CleanWhitespace

	Clean up the indentation whitespace and strip trailing whitespace in
	the specified range.

	Returns the range of the resulting text.

 ******************************************************************************/

#define CLEAN_WS_ALIGNMENT \
	textIter.SkipPrev(); \
	const JCharacterRange r = textIter.FinishMatch().GetCharacterRange(); \
	styleIter.MoveTo(JListT::kStartBefore, r.first); \
	styleIter.RemoveNext(r.GetCount()); \
	textIter.RemoveLastMatch(); // invalidates m

static const JRegex newlinePattern("\n+");

JStyledText::TextRange
JStyledText::CleanWhitespace
	(
	const TextRange&	origRange,
	const bool			align
	)
{
	const TextIndex i1 = GetParagraphStart(origRange.GetFirst());
	const TextIndex i2 = GetParagraphEnd(origRange.GetLast(*this));

	const TextRange range(JCharacterRange(i1.charIndex, i2.charIndex),
						  JUtf8ByteRange(i1.byteIndex, i2.byteIndex));

	JString text;
	JRunArray<JFont> style;
	const bool ok = Copy(range, &text, &style);
	assert( ok );

	// strip trailing whitespace -- first, to clear blank lines

	JStringIterator textIter(&text);
	FontIterator styleIter(&style);
	bool keepGoing;
	do
	{
		keepGoing = textIter.Next("\n");
		textIter.SkipPrev();	// back up to check characters before newline

		JUtf8Character c;
		JSize count = 0;
		while (textIter.Prev(&c, JStringIterator::kStay) && isWhitespace(c))
		{
			textIter.RemovePrev();
			count++;
		}

		if (count > 0)
		{
			styleIter.MoveTo(JListT::kStartBefore, textIter.GetNextCharacterIndex());
			styleIter.RemoveNext(count);
		}

		textIter.SkipNext();	// skip over current newline
	}
		while (keepGoing);

	// clean indenting whitespace

	textIter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	styleIter.MoveTo(JListT::kStartAtBeginning, 0);
	JString fill;
	JFont f;
	do
	{
		JSize n = 0;
		TextIndex p(textIter.GetNextCharacterIndex(), textIter.GetNextByteIndex());
		JUtf8Character c;

		if (itsTabToSpacesFlag)
		{
			textIter.BeginMatch();
			while (textIter.Next(&c) && isWhitespace(c))
			{
				if (c == ' ')
				{
					n++;
					if (n >= itsCRMTabCharCount)
					{
						n = 0;
						textIter.BeginMatch();
					}
				}
				else	// tab
				{
					const JSize count = itsCRMTabCharCount - n;

					fill.Clear();
					for (JIndex i=1; i<=count; i++)
					{
						fill += " ";
					}

					styleIter.MoveTo(JListT::kStartBefore, textIter.GetPrevCharacterIndex());
					styleIter.Next(&f);
					styleIter.Insert(f, count-1);

					textIter.SkipPrev();
					textIter.RemoveNext();
					textIter.Insert(fill);
					textIter.SkipNext(count);

					n = 0;
					textIter.BeginMatch();
				}
			}

			if (align && 0 < n && n <= itsCRMTabCharCount/2)
			{
				CLEAN_WS_ALIGNMENT
			}
			else if (align && itsCRMTabCharCount/2 < n)
			{
				const JSize count = itsCRMTabCharCount - n;

				fill.Clear();
				for (JIndex i=1; i<=count; i++)
				{
					fill += " ";
				}

				styleIter.MoveTo(JListT::kStartBefore, textIter.GetPrevCharacterIndex());
				styleIter.Next(&f);
				styleIter.Insert(f, count);

				textIter.SkipPrev();
				textIter.Insert(fill);
			}
		}
		else
		{
			textIter.BeginMatch();
			while (textIter.Next(&c) && isWhitespace(c))
			{
				if (c == ' ')
				{
					n++;
					if (n >= itsCRMTabCharCount)
					{
						const JStringMatch& m = textIter.FinishMatch();

						const JCharacterRange r = m.GetCharacterRange();
						styleIter.MoveTo(JListT::kStartBefore, r.first);
						styleIter.RemoveNext(r.GetCount()-1);

						textIter.ReplaceLastMatch("\t");	// invalidates m

						n = 0;
						textIter.BeginMatch();
					}
				}
				else	// tab
				{
					textIter.SkipPrev();
					const JStringMatch& m = textIter.FinishMatch();
					if (!m.IsEmpty())	// remove useless preceding spaces
					{
						const JCharacterRange r = m.GetCharacterRange();
						styleIter.MoveTo(JListT::kStartBefore, r.first);
						styleIter.RemoveNext(r.GetCount());
						textIter.RemoveLastMatch();		// invalidates m
					}
					textIter.SkipNext();

					n = 0;
					textIter.BeginMatch();
				}
			}

			if (align && 0 < n && n <= itsCRMTabCharCount/2)
			{
				CLEAN_WS_ALIGNMENT
			}
			else if (align && itsCRMTabCharCount/2 < n)
			{
				textIter.SkipPrev();
				const JCharacterRange r = textIter.FinishMatch().GetCharacterRange();

				styleIter.MoveTo(JListT::kStartBefore, r.first+1);
				styleIter.RemoveNext(r.GetCount()-1);

				textIter.ReplaceLastMatch("\t");	// invalidates m
			}
		}
	}
		while (textIter.Next(newlinePattern) && !textIter.AtEnd());

	textIter.Invalidate();
	styleIter.Invalidate();

	// replace selection with cleaned text/style

	Paste(range, text, &style);		// handles undo
	return TextRange(JCharacterRange(range.charRange.first, range.charRange.first + text.GetCharacterCount() - 1),
					 JUtf8ByteRange(range.byteRange.first, range.byteRange.first + text.GetByteCount() - 1));
}

#undef CLEAN_WS_ALIGNMENT

/******************************************************************************
 CleanRightMargin

	If coerce, paragraphs are detected by looking only for blank lines.
	Otherwise, they are detected by blank lines or a change in the line prefix.

	The work done by this function can be changed by calling SetCRMRuleList().

 ******************************************************************************/

bool
JStyledText::CleanRightMargin
	(
	TextIndex*			caretIndex,
	const TextRange&	selectionRange,
	const bool			coerce
	)
{
	bool changed = false;
	TextRange origTextRange;
	JString newText;
	JRunArray<JFont> newStyles;
	newStyles.SetMinLgSize(itsStyles->GetMinLgSize());

	TextIndex newCaretIndex;
	if (selectionRange.IsEmpty())
	{
		changed = PrivateCleanRightMargin(*caretIndex, coerce,
										  &origTextRange, &newText,
										  &newStyles, &newCaretIndex);
	}
	else
	{
		const TextIndex end = AdjustTextIndex(
			GetParagraphEnd(selectionRange.GetLast(*this)), -1);

		TextRange range;
		JString text;
		JRunArray<JFont> styles;
		styles.SetMinLgSize(itsStyles->GetMinLgSize());
		*caretIndex = selectionRange.GetFirst();
		TextIndex tmpCaretIndex;
		bool first = true;
		while (true)
		{
			if (PrivateCleanRightMargin(*caretIndex, coerce,
										&range, &text, &styles, &tmpCaretIndex))
			{
				*caretIndex    = tmpCaretIndex;
				origTextRange += range;
				newText       += text;
				newStyles.AppendSlice(styles, JIndexRange(1, styles.GetItemCount()));
				if (range.charRange.last < end.charIndex)
				{
					range.charRange.last++;
					range.byteRange.last++;			// ascii
					origTextRange.charRange.last++;
					origTextRange.byteRange.last++;	// ascii
					newText.Append("\n");
					newStyles.AppendItem(newStyles.GetLastItem());
				}
			}
			else
			{
				range.Set(GetParagraphStart(*caretIndex),
						  AdjustTextIndex(GetParagraphEnd(*caretIndex), +1));

				assert( newText.IsEmpty() ||
						range.charRange.first == origTextRange.charRange.last + 1 );

				origTextRange += range;

				JStringIterator iter(itsText);
				const TextIndex after = range.GetAfter();
				iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, range.charRange.first, range.byteRange.first);
				iter.BeginMatch();
				iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, after.charIndex, after.byteIndex);
				iter.FinishMatch();
				newText += iter.GetLastMatch().GetString();

				newStyles.AppendSlice(*itsStyles, range.charRange);
			}

			if (first)
			{
				newCaretIndex = *caretIndex;
				first         = false;
			}

			if (range.charRange.last < end.charIndex && itsText.CharacterIndexValid(range.charRange.last + 1))
			{
				*caretIndex = range.GetAfter();
			}
			else
			{
				break;
			}
		}

		changed = true;
	}

	if (changed)
	{
		Paste(origTextRange, newText, &newStyles);		// handles undo
		*caretIndex = newCaretIndex;
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 PrivateCleanRightMargin (private)

	*newText is the cleaned up version of the paragraph containing the caret
	or start of selection.

	*origTextRange contains the range that was cleaned up, excluding the last
	newline.

	*newCaretIndex is the index required to maintain the position of the caret.

	Returns false if the caret was not in a paragraph.

 ******************************************************************************/

bool
JStyledText::PrivateCleanRightMargin
	(
	const TextIndex&	start,
	const bool			coerce,
	TextRange*			origTextRange,
	JString*			newText,
	JRunArray<JFont>*	newStyles,
	TextIndex*			newCaretIndex
	)
	const
{
	origTextRange->SetToNothing();
	newText->Clear();
	newStyles->RemoveAll();
	newCaretIndex->charIndex = newCaretIndex->byteIndex = 0;

	if (itsText.IsEmpty() ||
		(start.charIndex == itsText.GetCharacterCount()+1 && EndsWithNewline()))
	{
		return false;
	}

	TextIndex index;
	JIndex ruleIndex;
	JString firstLinePrefix, restLinePrefix;
	JSize firstPrefixLength, restPrefixLength;
	if (!CRMGetRange(start, coerce, origTextRange, &index,
					 &firstLinePrefix, &firstPrefixLength,
					 &restLinePrefix, &restPrefixLength, &ruleIndex))
	{
		return false;
	}

	if (start.charIndex <= index.charIndex)
	{
		*newCaretIndex = start;
	}

	// read in each word, convert it, write it out

	JSize currentCharCount = 0;
	bool requireSpace      = false;

	JString wordBuffer, spaceBuffer;
	JRunArray<JFont> wordStyles;
	wordStyles.SetMinLgSize(itsStyles->GetMinLgSize());
	while (index.charIndex <= origTextRange->charRange.last)
	{
		JSize spaceCount;
		bool hasCount = false;
		TextCount rnwCount;
		const CRMStatus status =
			CRMReadNextWord(&index, origTextRange->charRange.last,
							&spaceBuffer, &spaceCount, &wordBuffer, &wordStyles,
							currentCharCount, start, &hasCount, &rnwCount,
							*newText, requireSpace);
		requireSpace = true;

		if (status == kFinished)
		{
			break;
		}
		else if (status == kFoundWord)
		{
			CRMAppendWord(newText, newStyles, &currentCharCount, &hasCount, &rnwCount,
						  spaceBuffer, spaceCount, wordBuffer, wordStyles,
						  newText->IsEmpty() ? firstLinePrefix : restLinePrefix,
						  newText->IsEmpty() ? firstPrefixLength : restPrefixLength);

			if (hasCount)
			{
				*newCaretIndex = origTextRange->GetFirst() + rnwCount;
			}
		}
		else
		{
			assert( status == kFoundNewLine );

			CRMTossLinePrefix(&index, origTextRange->GetAfter(), ruleIndex);

			// CRMGetRange() ensures this is strictly *inside* the text,
			// so start == index will be caught elsewhere.

			if (newCaretIndex->charIndex == 0 && start.charIndex < index.charIndex)
			{
				*newCaretIndex  = origTextRange->GetFirst();
				*newCaretIndex += TextCount(newText->GetCharacterCount(), newText->GetByteCount());
			}
		}
	}

	if (start.charIndex == origTextRange->charRange.last+1)
	{
		*newCaretIndex  = origTextRange->GetFirst();
		*newCaretIndex += TextCount(newText->GetCharacterCount(), newText->GetByteCount());
	}

	assert( newCaretIndex->charIndex > 0 );
	assert( newText->GetCharacterCount() == newStyles->GetItemCount() );

	return true;
}

/*******************************************************************************
 CRMGetRange (private)

	Returns the range of characters to reformat.
	Returns false if the caret is not in a paragraph.

	caretIndex is the current location of the caret.

	If coerce, we search forward and backward from the caret location
	for blank lines and include all the text between these blank lines.
	Blank lines are defined as lines that contain nothing but prefix characters.

	Otherwise, we first search backward until we find a blank line or a
	line prefix that can't generate the following one.  Then we search
	forward until we find a blank line or the prefix changes.

	The latter method is safer because it doesn't change the prefix of any of
	the lines.  The former is useful for forcing a change in the prefix.

 ******************************************************************************/

bool
JStyledText::CRMGetRange
	(
	const TextIndex&	caretIndex,
	const bool			coerce,
	TextRange*			range,
	TextIndex*			textStartIndex,
	JString*			firstLinePrefix,
	JSize*				firstPrefixLength,
	JString*			restLinePrefix,
	JSize*				restPrefixLength,
	JIndex*				returnRuleIndex
	)
	const
{
	range->Set(GetParagraphStart(caretIndex), AdjustTextIndex(GetParagraphEnd(caretIndex), +1));
	if (range->IsEmpty())
	{
		return false;
	}

	JStringIterator iter(itsText);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, range->charRange.last+1, range->byteRange.last+1);

	JUtf8Character c;
	while (iter.Prev(&c) && c == '\n')
	{
		range->charRange.last--;
		range->byteRange.last--;
	}

	// If the line we are on is empty, quit immediately.

	TextIndex tempStart = range->GetFirst();
	JString origLinePrefix;
	JSize prefixLength;
	JIndex ruleIndex = 0;
	if (range->IsEmpty() ||
		!CRMGetPrefix(&tempStart, range->GetAfter(),
					  &origLinePrefix, &prefixLength, &ruleIndex) ||
		CRMLineMatchesRest(*range))
	{
		return false;
	}

	// search backward for a blank line or a change in the prefix (if !coerce)
	// (If range->charRange.first==2, the line above us is blank.)

	JString currLinePrefix, nextLinePrefix = origLinePrefix;
	while (range->charRange.first > 2)
	{
		const TextIndex newStart      = GetParagraphStart(AdjustTextIndex(range->GetFirst(), -1));
		const TextIndex tempBeyondEnd = AdjustTextIndex(range->GetFirst(), -1);
		tempStart                     = newStart;
		ruleIndex                     = 0;
		if (tempStart.charIndex >= range->charRange.first - 1 ||
			!CRMGetPrefix(&tempStart, tempBeyondEnd,
						  &currLinePrefix, &prefixLength, &ruleIndex) ||
			CRMLineMatchesRest(TextRange(newStart, tempBeyondEnd)) ||
			(!coerce &&
			 CRMBuildRestPrefix(currLinePrefix, ruleIndex, &prefixLength) != nextLinePrefix))
		{
			break;
		}
		range->charRange.first = newStart.charIndex;
		range->byteRange.first = newStart.byteIndex;
		nextLinePrefix         = currLinePrefix;
		ruleIndex              = 0;
	}

	// search forward for a blank line or a change in the prefix (if !coerce)
	// (If range->charRange.last==bufLength-1, the text ends with a newline.)

	*textStartIndex  = range->GetFirst();
	*returnRuleIndex = 0;
	const bool hasText =
		CRMGetPrefix(textStartIndex, range->GetAfter(),
					 firstLinePrefix, firstPrefixLength, returnRuleIndex);
	assert( hasText );

	*restLinePrefix = CRMBuildRestPrefix(*firstLinePrefix, *returnRuleIndex,
										 restPrefixLength);

	while (range->charRange.last < itsText.GetCharacterCount()-1)
	{
		tempStart        = AdjustTextIndex(range->GetAfter(), +1);
		TextIndex newEnd = GetParagraphEnd(tempStart);

		iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, newEnd.charIndex, newEnd.byteIndex);
		if (iter.Next(&c) && c == '\n')	// could hit end of text instead
		{
			newEnd = AdjustTextIndex(newEnd, -1);
		}

		const TextIndex afterNewEnd = AdjustTextIndex(newEnd, +1);

		JIndex tempRuleIndex = *returnRuleIndex;
		if (newEnd.charIndex < tempStart.charIndex ||
			!CRMGetPrefix(&tempStart, afterNewEnd,
						  &currLinePrefix, &prefixLength, &tempRuleIndex) ||
			(!coerce && currLinePrefix != *restLinePrefix))
		{
			break;
		}
		range->charRange.last = afterNewEnd.charIndex - 1;
		range->byteRange.last = afterNewEnd.byteIndex - 1;
	}

	return true;
}

/*******************************************************************************
 CRMGetPrefix

	Returns the prefix to be used for each line and updates *startIndex to point
	to the first character after the prefix.

	*columnCount can be greater than linePrefix->GetCharacterCount() because of tabs.

	Returns false if the entire range qualifies as a prefix.

 ******************************************************************************/

bool
JStyledText::CRMGetPrefix
	(
	TextIndex*			startIndex,
	const TextIndex&	beyondEndIndex,
	JString*			linePrefix,
	JSize*				columnCount,
	JIndex*				ruleIndex
	)
	const
{
	const TextRange prefixRange =
		CRMMatchPrefix(TextRange(*startIndex, beyondEndIndex), ruleIndex);

	linePrefix->Set(itsText.GetBytes(), prefixRange.byteRange);

	*startIndex   = prefixRange.GetAfter();
	*columnCount = CRMCalcColumnCount(*linePrefix);
	return startIndex->charIndex < beyondEndIndex.charIndex;
}

/*******************************************************************************
 CRMMatchPrefix (private)

	Returns the range of characters that qualifies as a prefix.

	To match a prefix, all CRMRule::first's and [ \t]* are tried.
	The longest match is used.

	If *ruleIndex > 0, CRMRule::rest for the specified rule is tried *first*,
	and if this matches, it preempts everything else.

	When the function returns, *ruleIndex is the CRMRule::first that matched.
	(It remains 0 if the default rule is used, since this doesn't have "rest".)

 ******************************************************************************/

static const JRegex defaultCRMPrefixRegex("^[ \t]*");

JStyledText::TextRange
JStyledText::CRMMatchPrefix
	(
	const TextRange&	textRange,
	JIndex*				ruleIndex
	)
	const
{
	const JString prefix(itsText.GetBytes(), textRange.byteRange, JString::kNoCopy);

	TextRange matchRange;
	if (itsCRMRuleList != nullptr && *ruleIndex > 0)
	{
		const CRMRule rule   = itsCRMRuleList->GetItem(*ruleIndex);
		const JStringMatch m = rule.rest->Match(prefix, JRegex::kIgnoreSubmatches);
		if (!m.IsEmpty() && m.GetUtf8ByteRange().first == 1)
		{
			matchRange.Set(m.GetCharacterRange(), m.GetUtf8ByteRange());
		}
	}

	if (matchRange.IsEmpty())
	{
		if (itsCRMRuleList != nullptr)
		{
			JIndex i = 0;
			for (const auto& rule : *itsCRMRuleList)
			{
				i++;
				const JStringMatch m = rule.first->Match(prefix, JRegex::kIgnoreSubmatches);
				if (!m.IsEmpty() &&
					m.GetUtf8ByteRange().first == 1 &&
					m.GetUtf8ByteRange().GetCount() > matchRange.byteRange.GetCount())
				{
					matchRange.Set(m.GetCharacterRange(), m.GetUtf8ByteRange());
					*ruleIndex = i;
				}
			}
		}

		// check equality of range::last in case prefix is empty

		const JStringMatch m = defaultCRMPrefixRegex.Match(prefix, JRegex::kIgnoreSubmatches);
		if (m.GetUtf8ByteRange().GetCount() >= matchRange.byteRange.GetCount() || itsCRMRuleList == nullptr)
		{
			matchRange.Set(m.GetCharacterRange(), m.GetUtf8ByteRange());
			*ruleIndex = 0;
		}
	}

	matchRange.charRange += textRange.charRange.first-1;
	matchRange.byteRange += textRange.byteRange.first-1;

	return matchRange;
}

/*******************************************************************************
 CRMLineMatchesRest

	Returns true if the given range is matched by any "rest" pattern.

 ******************************************************************************/

bool
JStyledText::CRMLineMatchesRest
	(
	const TextRange& range
	)
	const
{
	if (itsCRMRuleList != nullptr)
	{
		const JString s(itsText.GetBytes(), range.byteRange, JString::kNoCopy);

		for (const auto& rule : *itsCRMRuleList)
		{
			const JStringMatch m = rule.rest->Match(s, JRegex::kIgnoreSubmatches);
			if (!m.IsEmpty() &&
				m.GetUtf8ByteRange().first == 1 &&
				m.GetUtf8ByteRange().GetCount() == range.byteRange.GetCount())
			{
				return true;
			}
		}
	}

	return false;
}

/*******************************************************************************
 CRMCalcColumnCount (private)

	Returns the length of *linePrefix in characters.  This can be greater
	than linePrefix->GetLength() because of tabs.  *linePrefix may be modified.

 ******************************************************************************/

JSize
JStyledText::CRMCalcColumnCount
	(
	const JString& linePrefix
	)
	const
{
	if (linePrefix.IsEmpty())
	{
		return 0;
	}

	JSize columnCount = 0;

	JStringIterator iter(linePrefix);
	JUtf8Character c;
	while (iter.Next(&c))
	{
		if (c == '\t')
		{
			columnCount += CRMGetTabWidth(columnCount+1);
		}
		else
		{
			columnCount++;
		}
	}

	return columnCount;
}

/*******************************************************************************
 CRMBuildRestPrefix (private)

	Returns the line prefix to be used for all lines after the first one.

 ******************************************************************************/

JString
JStyledText::CRMBuildRestPrefix
	(
	const JString&	firstLinePrefix,
	const JIndex	ruleIndex,
	JSize*			columnCount
	)
	const
{
	JString s = firstLinePrefix;
	if (itsCRMRuleList != nullptr && ruleIndex > 0)
	{
		const CRMRule rule   = itsCRMRuleList->GetItem(ruleIndex);
		const JStringMatch m = rule.first->Match(s, JRegex::kIncludeSubmatches);
		assert( !m.IsEmpty() &&
				m.GetUtf8ByteRange() == JUtf8ByteRange(1, s.GetByteCount()));

		s = itsCRMRuleList->GetInterpolator()->Interpolate(*rule.replace, m);
	}

	*columnCount = CRMCalcColumnCount(s);
	return s;
}

/*******************************************************************************
 CRMTossLinePrefix (private)

	Increments *charIndex past whatever qualifies as a line prefix.

 ******************************************************************************/

void
JStyledText::CRMTossLinePrefix
	(
	TextIndex*			charIndex,
	const TextIndex&	beyondEndIndex,
	const JIndex		origRuleIndex
	)
	const
{
	JIndex ruleIndex = origRuleIndex;
	const TextRange prefixRange =
		CRMMatchPrefix(TextRange(*charIndex, beyondEndIndex), &ruleIndex);
	*charIndex += prefixRange.GetCount();
}

/*******************************************************************************
 CRMReadNextWord (private)

	Read one block of { spaces + word (non-spaces) } from itsBuffer, starting
	at *charIndex, stopping if we get as far as endCharIndex.  *spaceBuffer
	contains the whitespace (spaces + tabs) that was found.  *spaceCount is
	the equivalent number of spaces.  *wordBuffer contains the word.
	*charIndex is incremented to point to the next character to read.

	Newlines are treated separately.  If a newline is encountered while reading
	spaces, we throw out the spaces and return kFoundNewLine.  If a newline
	is encountered while reading a word, we leave it for the next time, when
	we immediately return kFoundNewLine.

	When we pass the position origCaretIndex, we set *totalCount to be
	the index into *spaceBuffer+*wordBuffer.  Otherwise, we do not change
	*totalCount.

 ******************************************************************************/

inline bool
jCRMIsEOS
	(
	const JUtf8Character& c
	)
{
	return (c == '.' || c == '?' || c == '!');
}

JStyledText::CRMStatus
JStyledText::CRMReadNextWord
	(
	TextIndex*			charIndex,
	const JIndex		endCharIndex,
	JString*			spaceBuffer,
	JSize*				spaceCount,
	JString*			wordBuffer,
	JRunArray<JFont>*	wordStyles,
	const JSize			currentCharCount,
	const TextIndex&	origCaretIndex,
	bool*				hasTotalCount,
	TextCount*			totalCount,
	const JString&		newText,
	const bool			requireSpace
	)
	const
{
	// read the whitespace

	spaceBuffer->Clear();
	*spaceCount = 0;

	JStringIterator iter(itsText);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, charIndex->charIndex, charIndex->byteIndex);

	JUtf8Character c;
	JIndex i;
	while (iter.GetNextCharacterIndex(&i) && i <= endCharIndex)
	{
		if (i == origCaretIndex.charIndex)
		{
			assert( spaceBuffer->GetCharacterCount() == spaceBuffer->GetByteCount() );
			*hasTotalCount = true;
			totalCount->Set(spaceBuffer->GetCharacterCount(), spaceBuffer->GetByteCount());	// ascii
		}

		iter.Next(&c);
		if (c == ' ')
		{
			spaceBuffer->Append(c);
			(*spaceCount)++;
		}
		else if (c == '\t')
		{
			spaceBuffer->Append(c);
			*spaceCount += CRMGetTabWidth(currentCharCount + *spaceCount + 1);
		}
		else if (c == '\n')			// we can ignore the spaces
		{
			spaceBuffer->Clear();
			*spaceCount = 0;
			charIndex->charIndex = i+1;
			charIndex->byteIndex = iter.GetPrevByteIndex()+1;
			return kFoundNewLine;
		}
		else						// found beginning of word
		{
			iter.SkipPrev();
			break;
		}
	}

	if (i > endCharIndex)	// we can ignore the spaces
	{
		return kFinished;
	}

	// read the word

	wordBuffer->Clear();

	const TextIndex wordStart(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
	while (iter.GetNextCharacterIndex(&i) && i <= endCharIndex)
	{
		if (i == origCaretIndex.charIndex)
		{
			*hasTotalCount = true;
			totalCount->Set(
				spaceBuffer->GetCharacterCount() + wordBuffer->GetCharacterCount(),
				spaceBuffer->GetByteCount() + wordBuffer->GetByteCount());
		}

		iter.Next(&c);
		if (c == ' ' || c == '\t' || c == '\n')
		{
			iter.SkipPrev();
			break;
		}

		wordBuffer->Append(c);
	}

	if (!iter.AtEnd())
	{
		charIndex->charIndex = iter.GetNextCharacterIndex();
		charIndex->byteIndex = iter.GetNextByteIndex();
	}
	else
	{
		*charIndex = GetBeyondEnd();
	}

	wordStyles->RemoveAll();
	wordStyles->AppendSlice(*itsStyles, JIndexRange(wordStart.charIndex, wordStart.charIndex + wordBuffer->GetCharacterCount() - 1));

	iter.Invalidate();

	// After a newline, the whitespace may have been tossed
	// as belonging to the prefix, but we still need some space.
	//
	// Punctuation is assumed to be the end of a sentence if the
	// following word does not start with a lower case letter.

	if (*spaceCount == 0 && requireSpace)
	{
		JStringIterator iter2(newText, JStringIterator::kStartAtEnd);

		JUtf8Character c1, c2;
		iter2.Prev(&c1);
		iter2.Prev(&c2);

		if ((jCRMIsEOS(c1) || (jCRMIsEOS(c2) && c1 =='\"')) &&
			!wordBuffer->GetFirstCharacter().IsLower())
		{
			*spaceBuffer = "  ";
		}
		else
		{
			*spaceBuffer = " ";
		}

		*spaceCount = spaceBuffer->GetCharacterCount();
		if (*hasTotalCount)
		{
			*totalCount += TextCount(*spaceCount, *spaceCount);	// ascii
		}
	}

	return kFoundWord;
}

/*******************************************************************************
 CRMAppendWord (private)

	Add the spaces and word to new text, maintaining the required line width.

	If *hasTotalCount, we convert *totalCount from spaceBuffer+wordBuffer to
	newText.

 ******************************************************************************/

void
JStyledText::CRMAppendWord
	(
	JString*				newText,
	JRunArray<JFont>*		newStyles,
	JSize*					currentCharCount,
	bool*					hasTotalCount,
	TextCount*				totalCount,
	const JString&			spaceBuffer,
	const JSize				spaceCount,
	const JString&			wordBuffer,
	const JRunArray<JFont>&	wordStyles,
	const JString&			linePrefix,
	const JSize				prefixLength
	)
	const
{
	const JSize newLineWidth = *currentCharCount + spaceCount + wordBuffer.GetCharacterCount();
	if (*currentCharCount == 0 || newLineWidth > itsCRMLineWidth)
	{
		// calculate prefix font

		JFont prefixFont = wordStyles.GetFirstItem();
		prefixFont.ClearStyle();

		// terminate previous line

		if (!newText->IsEmpty())
		{
			newText->Append("\n");
			newStyles->AppendItem(prefixFont);
		}

		if (!linePrefix.IsEmpty())
		{
			*newText += linePrefix;
			newStyles->AppendItems(prefixFont, linePrefix.GetCharacterCount());
		}

		// write word

		if (*hasTotalCount)
		{
			if (totalCount->charCount <= spaceCount)
			{
				totalCount->Set(0, 0);	// in spaces that we toss
			}
			else
			{
				*totalCount -= TextCount(spaceCount, spaceCount);	// ascii
			}

			*totalCount += TextCount(newText->GetCharacterCount(), newText->GetByteCount());
		}

		*newText += wordBuffer;
		newStyles->AppendSlice(wordStyles, JIndexRange(1, wordStyles.GetItemCount()));
		*currentCharCount = prefixLength + wordBuffer.GetCharacterCount();
	}
	else	// newLineWidth <= itsCRMLineWidth
	{
		// append spaces + word at end of line

		if (*hasTotalCount)
		{
			*totalCount += TextCount(newText->GetCharacterCount(), newText->GetByteCount());
		}

		*newText += spaceBuffer;
		newStyles->AppendItems(JCalcWSFont(newStyles->GetLastItem(),
											  wordStyles.GetFirstItem()),
								  spaceBuffer.GetCharacterCount());
		*newText += wordBuffer;
		newStyles->AppendSlice(wordStyles, JIndexRange(1, wordStyles.GetItemCount()));

		if (newLineWidth < itsCRMLineWidth)
		{
			*currentCharCount = newLineWidth;
		}
		else	// newLineWidth == itsCRMLineWidth
		{
			*currentCharCount = 0;
		}
	}
}

/******************************************************************************
 SetCRMRuleList

	To find the first line of a paragraph, one searches backward for a blank
	line:  one that matches any CRMRule::first.  The following line is the
	beginning of the paragraph.

	Each following line is part of the paragraph if it is not blank:
	no CRMRule::first matches the entire line, and neither does the
	CRMRule::rest corresponding to the CRMRule::first that matched the first
	line of the paragraph.

	The prefix of the first line is unchanged.  The prefix of the following
	lines is calculated from using CRMRule::replace on the prefix of the
	first line.

 ******************************************************************************/

void
JStyledText::SetCRMRuleList
	(
	CRMRuleList*	ruleList,
	const bool		teOwnsRuleList
	)
{
	ClearCRMRuleList();

	itsCRMRuleList      = ruleList;
	itsOwnsCRMRulesFlag = teOwnsRuleList;
}

/******************************************************************************
 ClearCRMRuleList

	With no other rules, the default is to match [ \t]*

 ******************************************************************************/

void
JStyledText::ClearCRMRuleList()
{
	if (itsOwnsCRMRulesFlag && itsCRMRuleList != nullptr)
	{
		itsCRMRuleList->DeleteAll();
		jdelete itsCRMRuleList;
	}

	itsCRMRuleList      = nullptr;
	itsOwnsCRMRulesFlag = false;
}

/******************************************************************************
 CRMRule functions

 ******************************************************************************/

JStyledText::CRMRule::CRMRule
	(
	const JString& firstPattern,
	const JString& restPattern,
	const JString& replacePattern
	)
{
	first = CreateRegex(firstPattern);
	rest  = CreateRegex(restPattern);

	replace = jnew JString(replacePattern);
}

void
JStyledText::CRMRule::CleanOut()
{
	jdelete first;
	first = nullptr;

	jdelete rest;
	rest = nullptr;

	jdelete replace;
	replace = nullptr;
}

JRegex*
JStyledText::CRMRule::CreateRegex
	(
	const JString& pattern
	)
{
	auto* r = jnew JRegex(pattern);
	return r;
}

JInterpolate*
JStyledText::CRMRule::CreateInterpolator()
{
	auto i = jnew JInterpolate;
	i->SetWhitespaceEscapes();
	return i;
}

/******************************************************************************
 CRMRuleList functions

 ******************************************************************************/

JStyledText::CRMRuleList::CRMRuleList()
{
	itsInterpolator = CRMRule::CreateInterpolator();
}

JStyledText::CRMRuleList::CRMRuleList
	(
	const CRMRuleList& source
	)
	:
	JArray<CRMRule>()
{
	for (const auto& r : source)
	{
		AppendItem(
			CRMRule(r.first->GetPattern(), r.rest->GetPattern(), *r.replace));
	}

	itsInterpolator = CRMRule::CreateInterpolator();
}

JStyledText::CRMRuleList::~CRMRuleList()
{
	jdelete itsInterpolator;
}

void
JStyledText::CRMRuleList::DeleteAll()
{
	for (auto r : *this)
	{
		r.CleanOut();
	}

	RemoveAll();
}

/******************************************************************************
 SetCRMLineWidth

 ******************************************************************************/

void
JStyledText::SetCRMLineWidth
	(
	const JSize charCount
	)
{
	assert( charCount > 0 );
	itsCRMLineWidth = charCount;
}

/******************************************************************************
 SetCRMTabCharCount

 ******************************************************************************/

void
JStyledText::SetCRMTabCharCount
	(
	const JSize charCount
	)
{
	assert( charCount > 0 );
	itsCRMTabCharCount = charCount;
}

/*******************************************************************************
 CRMGetTabWidth

	Returns the number of spaces to which the tab is equivalent by rounding
	up to the nearest multiple of GetCRMTabCharCount().  The default value
	for this is 8 since this is what all UNIX programs use.

	textColumn starts at 1 at the left margin.

 ******************************************************************************/

JSize
JStyledText::CRMGetTabWidth
	(
	const JIndex textColumn
	)
	const
{
	return itsCRMTabCharCount - ((textColumn-1) % itsCRMTabCharCount);
}

/******************************************************************************
 Undo

 ******************************************************************************/

void
JStyledText::Undo()
{
	itsUndoChain->Undo();
}

/******************************************************************************
 Redo

 ******************************************************************************/

void
JStyledText::Redo()
{
	itsUndoChain->Redo();
}

/******************************************************************************
 DeactivateCurrentUndo

 ******************************************************************************/

void
JStyledText::DeactivateCurrentUndo()
{
	itsUndoChain->DeactivateCurrentUndo();
}

/******************************************************************************
 ClearUndo

	Avoid calling this whenever possible since it throws away -all-
	undo information.

 ******************************************************************************/

void
JStyledText::ClearUndo()
{
	itsUndoChain->ClearUndo();
}

/******************************************************************************
 GetTypingUndo (private)

	Return the active JSTUndoTyping object.  If the current undo object is
	not an active JSTUndoTyping object, we create a new one that is active.

	If we create a new object, *isNew = true, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JSTUndoTyping*
JStyledText::GetTypingUndo
	(
	const TextIndex&	start,
	bool*				isNew
	)
{
	JSTUndoTyping* typingUndo = nullptr;

	JUndo* undo = nullptr;
	if (itsUndoChain->GetCurrentUndo(&undo) &&
		(typingUndo = dynamic_cast<JSTUndoTyping*>(undo)) != nullptr &&
		 typingUndo->IsActive() &&
		 typingUndo->MatchesCurrentIndex(start))
	{
		*isNew = false;
		return typingUndo;
	}
	else
	{
		typingUndo = jnew JSTUndoTyping(this, start);

		*isNew = true;
		return typingUndo;
	}
}

/******************************************************************************
 GetStyleUndo (private)

	Return the active JSTUndoStyle object.  If the current undo object is
	not an active JSTUndoStyle object, we create a new one that is active.

	If we create a new object, *isNew = true, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JSTUndoStyle*
JStyledText::GetStyleUndo
	(
	const TextRange&	range,
	bool*				isNew
	)
{
	JSTUndoStyle* styleUndo = nullptr;

	JUndo* undo = nullptr;
	if (itsUndoChain->GetCurrentUndo(&undo) &&
		(styleUndo = dynamic_cast<JSTUndoStyle*>(undo)) != nullptr &&
		 styleUndo->IsActive() &&
		 styleUndo->SameRange(range))
	{
		*isNew = false;
		return styleUndo;
	}
	else
	{
		styleUndo = jnew JSTUndoStyle(this, range);

		*isNew = true;
		return styleUndo;
	}
}

/******************************************************************************
 GetPasteUndo (private)

	Return a new JSTUndoPaste object, since they can never be reused.

	If we create a new object, *isNew = true, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JSTUndoPaste*
JStyledText::GetPasteUndo
	(
	const TextRange&	range,
	bool*				isNew
	)
{
	auto* pasteUndo = jnew JSTUndoPaste(this, range);

	*isNew = true;
	return pasteUndo;
}

/******************************************************************************
 GetTabShiftUndo (private)

	Return the active JSTUndoTabShift object.  If the current undo object is
	not an active JSTUndoTabShift object, we create a new one that is active.

	If we create a new object, *isNew = true, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JSTUndoTabShift*
JStyledText::GetTabShiftUndo
	(
	const TextRange&	range,
	bool*				isNew
	)
{
	JSTUndoTabShift* tabShiftUndo = nullptr;

	JUndo* undo = nullptr;
	if (itsUndoChain->GetCurrentUndo(&undo) &&
		(tabShiftUndo = dynamic_cast<JSTUndoTabShift*>(undo)) != nullptr &&
		 tabShiftUndo->IsActive() &&
		 tabShiftUndo->SameStartIndex(range))
	{
		*isNew = false;
		return tabShiftUndo;
	}
	else
	{
		tabShiftUndo = jnew JSTUndoTabShift(this, range);

		*isNew = true;
		return tabShiftUndo;
	}
}

/******************************************************************************
 GetMoveUndo (private)

	Return a new JSTUndoMove object, since they can never be reused.

	If we create a new object, *isNew = true, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JSTUndoMove*
JStyledText::GetMoveUndo
	(
	const TextIndex&	srcIndex,
	const TextIndex&	destIndex,
	const TextCount&	count,
	bool*				isNew
	)
{
	auto* moveUndo = jnew JSTUndoMove(this, srcIndex, destIndex, count);

	*isNew = true;
	return moveUndo;
}

/******************************************************************************
 AutoIndent (private)

	Insert the "rest" prefix based on the previous line.  If the previous
	line is nothing but whitespace, clear it.

 ******************************************************************************/

void
JStyledText::AutoIndent
	(
	JSTUndoTyping*		typingUndo,
	const TextIndex&	insertIndex,
	TextCount*			count
	)
{
	const TextIndex prevLineEnd   = AdjustTextIndex(insertIndex, -1);
	const TextIndex prevLineStart = GetParagraphStart(prevLineEnd);

	// Calculate the prefix range for the original line.

	TextIndex firstTextChar = prevLineStart;
	JString linePrefix;
	JSize columnCount;
	JIndex ruleIndex = 0;
	if (prevLineEnd.charIndex == prevLineStart.charIndex ||
		!CRMGetPrefix(&firstTextChar, prevLineEnd,
					  &linePrefix, &columnCount, &ruleIndex))
	{
		firstTextChar = prevLineEnd;
		if (firstTextChar.charIndex > prevLineStart.charIndex)
		{
			linePrefix.Set(itsText.GetBytes(),
				JUtf8ByteRange(prevLineStart.byteIndex, firstTextChar.byteIndex-1));
		}
	}
	else if (CRMLineMatchesRest(TextRange(prevLineStart, prevLineEnd)))
	{
		// CRMBuildRestPrefix() will complain if we pass in the entire
		// line, so we give it only the whitespace.

		CRMRuleList* savedRuleList = itsCRMRuleList;
		itsCRMRuleList             = nullptr;
		firstTextChar              = prevLineStart;
		ruleIndex                  = 0;
		CRMGetPrefix(&firstTextChar, prevLineEnd,
					 &linePrefix, &columnCount, &ruleIndex);
		itsCRMRuleList = savedRuleList;
	}

	assert( firstTextChar.charIndex >= prevLineStart.charIndex );

	if (firstTextChar.charIndex == prevLineStart.charIndex)
	{
		return;
	}

	// generate the prefix and include it in the undo object

	linePrefix = CRMBuildRestPrefix(linePrefix, ruleIndex, &columnCount);

	const TextCount prefixCount(linePrefix.GetCharacterCount(), linePrefix.GetByteCount());
	typingUndo->HandleCharacters(prefixCount);

	*count = InsertText(insertIndex, linePrefix, GetFont(insertIndex.charIndex-1));
	assert( count->charCount == prefixCount.charCount );

	// check the initial whitespace range on the previous line

	JStringIterator* iter = GetConstIterator(JStringIterator::kStartBeforeChar, prevLineStart);
	JUtf8Character c;

	iter->BeginMatch();
	while (iter->Next(&c, JStringIterator::kStay))
	{
		if (iter->GetNextCharacterIndex() == prevLineEnd.charIndex)
		{
			// the previous line is blank, so clear it

			const JStringMatch& m = iter->FinishMatch();
			typingUndo->HandleDelete(m, true);

			const TextRange deleteRange(m.GetCharacterRange(), m.GetUtf8ByteRange());
			PrivateDeleteText(deleteRange);

			assert( deleteRange.charRange.GetCount() == count->charCount );
			assert( deleteRange.byteRange.GetCount() == count->byteCount );

			count->charCount = count->byteCount = 0;
			break;
		}
		else if (!c.IsSpace())
		{
			break;
		}

		iter->SkipNext();
	}

	DisposeConstIterator(iter);
	iter = nullptr;
}

/******************************************************************************
 InsertSpacesForTab

	Insert spaces to use up the same amount of space that a tab would use.

 ******************************************************************************/

JStyledText::TextRange
JStyledText::InsertSpacesForTab
	(
	const TextIndex& lineStart,
	const TextRange& replaceRange
	)
{
	JIndex column = GetColumnForChar(lineStart, replaceRange.GetFirst());

	if (replaceRange.charRange.first == itsText.GetCharacterCount()+1 && EndsWithNewline())
	{
		column = 1;
	}

	const JSize spaceCount = CRMGetTabWidth(column);

	JString space;
	for (JIndex i=1; i<=spaceCount; i++)
	{
		space.Append(" ");
	}

	return Paste(replaceRange, space);
}

/******************************************************************************
 RestyleAll

 ******************************************************************************/

void
JStyledText::RestyleAll()
{
	BroadcastTextChanged(
		TextRange(TextIndex(1,1),
			TextCount(itsText.GetCharacterCount(), itsText.GetByteCount())),
		0, 0, false, true);
}

/******************************************************************************
 BroadcastTextChanged (protected)

 ******************************************************************************/

void
JStyledText::BroadcastTextChanged
	(
	const TextRange&	range,
	const JInteger		charDelta,
	const JInteger		byteDelta,
	const bool			deletion,
	const bool			adjustStyles
	)
{
	TextRange recalcRange = range,
			  redrawRange = range;

	if (!itsText.IsEmpty() && adjustStyles)
	{
		AdjustStylesBeforeBroadcast(itsText, itsStyles, &recalcRange, &redrawRange, deletion);
	}

	assert( recalcRange.charRange.Contains(range.charRange) || range.charRange.first == itsText.GetCharacterCount()+1 );
	assert( recalcRange.byteRange.Contains(range.byteRange) || range.byteRange.first == itsText.GetByteCount()+1 );
	assert( redrawRange.charRange.Contains(range.charRange) || range.charRange.first == itsText.GetCharacterCount()+1 );
	assert( redrawRange.byteRange.Contains(range.byteRange) || range.byteRange.first == itsText.GetByteCount()+1 );
	assert( itsText.GetCharacterCount() == itsStyles->GetItemCount() );

	if (recalcRange.charRange.Contains(redrawRange.charRange))
	{
		redrawRange.SetToNothing();
	}

	Broadcast(TextChanged(range, recalcRange, redrawRange, charDelta, byteDelta));
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Called before broadcasting changes.  Derived classes can override this
	to adjust the style of the affected range of text.  Ranges are in/out
	variables because the changes often slop out beyond the initial range.

	*** The endpoints of the ranges are only allowed to move outward.

	*** The contents of *styles can change, but the length must remain
		the same.  Ranges must be expanded to include all the changed
		elements.

 ******************************************************************************/

void
JStyledText::AdjustStylesBeforeBroadcast
	(
	const JString&		text,
	JRunArray<JFont>*	styles,
	TextRange*			recalcRange,
	TextRange*			redrawRange,
	const bool		deletion
	)
{
}

/******************************************************************************
 BroadcastUndoFinished (protected)

	Separate from BroadcastTextChanged() because that function may modify
	the range before sending the message.

 ******************************************************************************/

void
JStyledText::BroadcastUndoFinished
	(
	const TextRange& range
	)
{
	Broadcast(UndoFinished(range));
}

/******************************************************************************
 GetWordStart

	Return the index of the first character of the word at the given location.
	This function is required to work for charIndex == 0.

 ******************************************************************************/

JStyledText::TextIndex
JStyledText::GetWordStart
	(
	const TextIndex& index
	)
	const
{
	if (itsText.IsEmpty() || index.charIndex <= 1)
	{
		return TextIndex(1,1);
	}

	const JIndex charIndex = JMin(index.charIndex, itsText.GetCharacterCount()),
				 byteIndex = JMin(index.byteIndex, itsText.GetByteCount());

	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	iter.UnsafeMoveTo(JStringIterator::kStartAfterChar, charIndex, byteIndex);

	JUtf8Character c;
	if (iter.Prev(&c, JStringIterator::kStay) && !IsCharacterInWord(c))
	{
		while (iter.Prev(&c) && !IsCharacterInWord(c))
		{
			// find end of word
		}
	}

	while (iter.Prev(&c))
	{
		if (!IsCharacterInWord(c))
		{
			iter.SkipNext();
			break;
		}
	}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetWordEnd

	Return the index of the last character of the word at the given location.
	This function is required to work for charIndex > text length.

 ******************************************************************************/

JStyledText::TextIndex
JStyledText::GetWordEnd
	(
	const TextIndex& index
	)
	const
{
	if (itsText.IsEmpty())
	{
		return TextIndex(1,1);
	}

	const JSize bufLen = itsText.GetCharacterCount();
	if (index.charIndex >= bufLen)
	{
		return AdjustTextIndex(GetBeyondEnd(), -1);
	}

	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, index.charIndex, index.byteIndex);

	JUtf8Character c;
	if (iter.Next(&c, JStringIterator::kStay) && !IsCharacterInWord(c))
	{
		while (iter.Next(&c) && !IsCharacterInWord(c))
		{
			// find start of word
		}
	}

	while (iter.Next(&c))
	{
		if (!IsCharacterInWord(c))
		{
			iter.SkipPrev();
			break;
		}
	}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 SetCharacterInWordFunction

	Set the function that determines if a character is part of a word.
	The default is JIsAlnum(), which uses [A-Za-z0-9].

 ******************************************************************************/

void
JStyledText::SetCharacterInWordFunction
	(
	const std::function<bool(const JUtf8Character&)> f
	)
{
	itsCharInWordFn = jnew std::function(f);
}

/******************************************************************************
 IsCharacterInWord

	Returns true if the given character should be considered part of a word.

 ******************************************************************************/

bool
JStyledText::IsCharacterInWord
	(
	const JUtf8Character& c
	)
	const
{
	return itsCharInWordFn != nullptr ? (*itsCharInWordFn)(c) : DefaultIsCharacterInWord(c);
}

/******************************************************************************
 DefaultIsCharacterInWord (static private)

	Returns true if the given character should be considered part of a word.
	The definition is alphanumeric or apostrophe or underscore.

	This is not a virtual function because one needs to be able to set it
	for simple objects like input fields.

 ******************************************************************************/

bool
JStyledText::DefaultIsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return c.IsAlnum() || c == '\'' || c == '_';
}

/******************************************************************************
 GetPartialWordStart

	Return the index of the first character of the partial word at the given
	location.  This function is required to work for charIndex == 0.

	Example:  get_word Get142TheWordABCGood ABCDe
			  ^   ^    ^  ^  ^  ^   ^  ^    ^  ^
 ******************************************************************************/

JStyledText::TextIndex
JStyledText::GetPartialWordStart
	(
	const TextIndex& index
	)
	const
{
	if (itsText.IsEmpty() || index.charIndex <= 1)
	{
		return TextIndex(1,1);
	}

	const JIndex charIndex = JMin(index.charIndex, itsText.GetCharacterCount()),
				 byteIndex = JMin(index.byteIndex, itsText.GetByteCount());

	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	iter.UnsafeMoveTo(JStringIterator::kStartAfterChar, charIndex, byteIndex);

	JUtf8Character c;
	if (iter.Prev(&c, JStringIterator::kStay) && !c.IsAlnum())
	{
		while (iter.Prev(&c) && !c.IsAlnum())
		{
			// find end of word
		}
	}
	else if (!iter.Prev(&c))
	{
		return TextIndex(1,1);
	}

	JUtf8Character prev = c;
	bool foundLower = prev.IsLower();
	while (iter.Prev(&c))
	{
		foundLower = foundLower || c.IsLower();
		if (!c.IsAlnum() ||
			(prev.IsUpper() && c.IsLower()) ||
			(prev.IsUpper() && c.IsUpper() && foundLower) ||
			(prev.IsAlpha() && c.IsDigit()) ||
			(prev.IsDigit() && c.IsAlpha()))
		{
			iter.SkipNext();
			break;
		}

		prev = c;
	}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetPartialWordEnd

	Return the index of the last character of the partial word at the given
	location.  This function is required to work for charIndex > text length.

	Example:  get_word Get142TheWordABCGood
				^    ^   ^  ^  ^   ^  ^   ^
 ******************************************************************************/

JStyledText::TextIndex
JStyledText::GetPartialWordEnd
	(
	const TextIndex& index
	)
	const
{
	if (itsText.IsEmpty())
	{
		return TextIndex(1,1);
	}

	const JSize bufLen = itsText.GetCharacterCount();
	if (index.charIndex >= bufLen)
	{
		return AdjustTextIndex(GetBeyondEnd(), -1);
	}

	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, index.charIndex, index.byteIndex);

	JUtf8Character c;
	if (iter.Next(&c, JStringIterator::kStay) && !c.IsAlnum())
	{
		while (iter.Next(&c) && !c.IsAlnum())
		{
			// find start of word
		}
	}
	else if (!iter.Next(&c))
	{
		return TextIndex(1,1);
	}

	JUtf8Character prev = c, c2;
	while (iter.Next(&c))
	{
		if (!c.IsAlnum() ||
			(prev.IsLower() && c.IsUpper()) ||
			(prev.IsAlpha() && c.IsDigit()) ||
			(prev.IsDigit() && c.IsAlpha()) ||
			(iter.Next(&c2, JStringIterator::kStay) &&
			 prev.IsUpper() && c.IsUpper() && c2.IsLower()))
		{
			iter.SkipPrev();
			break;
		}

		prev = c;
	}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetParagraphStart

	Return the index of the first character in the paragraph that contains
	the character at the given location.  This function is required to work
	for charIndex == 0.

 ******************************************************************************/

JStyledText::TextIndex
JStyledText::GetParagraphStart
	(
	const TextIndex& index
	)
	const
{
	if (itsText.IsEmpty() || index.charIndex <= 1)
	{
		return TextIndex(1,1);
	}

	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, index.charIndex, index.byteIndex);

	JUtf8Character c;
	while (iter.Prev(&c, JStringIterator::kStay) && c != '\n')
	{
		iter.SkipPrev();
	}

	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetParagraphEnd

	Return the index of the newline that ends the paragraph that contains
	the character at the given location.  This function is required to work
	for charIndex > text length.

 ******************************************************************************/

JStyledText::TextIndex
JStyledText::GetParagraphEnd
	(
	const TextIndex& index
	)
	const
{
	if (itsText.IsEmpty())
	{
		return TextIndex(1,1);
	}

	const JSize bufLen = itsText.GetCharacterCount();
	if (index.charIndex >= bufLen)
	{
		return AdjustTextIndex(GetBeyondEnd(), -1);
	}

	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, index.charIndex, index.byteIndex);

	JUtf8Character c;
	while (iter.Next(&c) && c != '\n')
	{
		// find end of paragraph
	}

	iter.SkipPrev();	// get first byte of last character
	return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
}

/******************************************************************************
 GetColumnForChar

	Returns the column that the specified character is in, given the start
	of the line.  If the caret is at the far left, it is column 1.

	Since this is only useful with monospace fonts, the CRM tab width is
	used to calculate the column when tabs are encountered, by calling
	CRMGetTabWidth().

 ******************************************************************************/

JIndex
JStyledText::GetColumnForChar
	(
	const TextIndex& lineStart,
	const TextIndex& location
	)
	const
{
	if (location.charIndex > itsText.GetCharacterCount() && EndsWithNewline())
	{
		return 1;
	}

	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, lineStart.charIndex, lineStart.byteIndex);

	JIndex col = 1, charIndex;
	JUtf8Character c;
	while (iter.GetNextCharacterIndex(&charIndex) && charIndex < location.charIndex &&
		   iter.Next(&c))
	{
		col += (c == '\t' ? CRMGetTabWidth(col) : 1);
	}

	return col;
}

/******************************************************************************
 AdjustTextIndex

	Optimized by starting JStringIterator at known TextIndex.

 ******************************************************************************/

JStyledText::TextIndex
JStyledText::AdjustTextIndex
	(
	const TextIndex&	index,
	const JInteger		charDelta
	)
	const
{
	if (charDelta == 0)
	{
		return index;
	}

	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, index.charIndex, index.byteIndex);

	if (charDelta > 0)
	{
		iter.SkipNext(charDelta);
	}
	else
	{
		iter.SkipPrev(-charDelta);
	}

	if (iter.AtEnd())
	{
		return GetBeyondEnd();
	}
	else
	{
		return TextIndex(iter.GetNextCharacterIndex(), iter.GetNextByteIndex());
	}
}

/******************************************************************************
 GetConstIterator

 ******************************************************************************/

JStringIterator*
JStyledText::GetConstIterator
	(
	const JStringIterator::Position	pos,
	const TextIndex&				index
	)
	const
{
	auto* s = new(true) JString(itsText, JString::kNoCopy);
	assert( s != nullptr );

	auto* iter = jnew JStringIterator(*s);

	iter->UnsafeMoveTo(pos, index.charIndex, index.byteIndex);
	return iter;
}

/******************************************************************************
 DisposeConstIterator

 ******************************************************************************/

void
JStyledText::DisposeConstIterator
	(
	JStringIterator* iter
	)
	const
{
	auto* s = const_cast<JString*>(&iter->GetString());
	assert( s != &itsText );

	jdelete iter;
	jdelete s;
}

/******************************************************************************
 CharToTextRange

	Optimized by starting JStringIterator at start of line, computed by
	using binary search.

	lineStart can be nullptr

 ******************************************************************************/

JStyledText::TextRange
JStyledText::CharToTextRange
	(
	const TextIndex*		lineStart,
	const JCharacterRange&	charRange
	)
	const
{
	assert( !charRange.IsEmpty() );
	assert( itsText.RangeValid(charRange) );

	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator iter(s);
	if (lineStart != nullptr)
	{
		iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, lineStart->charIndex, lineStart->byteIndex);
	}

	return CharToTextRange(charRange, &iter);
}

/******************************************************************************
 CharToTextRange (static private)

	If JStringIterator is not AtBeginning(), assumes position is optimized.
	Otherwise, optimizes by starting JStringIterator at end closest to
	range.

 ******************************************************************************/

JStyledText::TextRange
JStyledText::CharToTextRange
	(
	const JCharacterRange&	charRange,
	JStringIterator*		iter
	)
{
	if (iter->AtBeginning())
	{
		const JString& s = iter->GetString();
		const JSize d1   = charRange.first;
		const JSize d2   = s.GetCharacterCount() - charRange.last;
		if (d2 < d1)
		{
			iter->MoveTo(JStringIterator::kStartAtEnd, 0);
		}
	}

	JIndex i1, i2;
	if (iter->AtEnd())
	{
		iter->MoveTo(JStringIterator::kStartAfterChar, charRange.last);
		i2 = iter->GetPrevByteIndex();
		iter->MoveTo(JStringIterator::kStartBeforeChar, charRange.first);
		i1 = iter->GetNextByteIndex();
	}
	else
	{
		iter->MoveTo(JStringIterator::kStartBeforeChar, charRange.first);
		i1 = iter->GetNextByteIndex();
		iter->MoveTo(JStringIterator::kStartAfterChar, charRange.last);
		i2 = iter->GetPrevByteIndex();
	}

	return TextRange(charRange, JUtf8ByteRange(i1, i2));
}

/******************************************************************************
 CalcInsertionFont

	Returns the font to use when inserting at the specified point.

 ******************************************************************************/

JFont
JStyledText::CalcInsertionFont
	(
	const TextIndex& index
	)
	const
{
	// create separate object on which to iterate, without copying
	const JString s(itsText, JString::kNoCopy);
	JStringIterator textIter(s);
	textIter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, index.charIndex, index.byteIndex);

	FontIterator styleIter(*itsStyles, JListT::kStartBefore, index.charIndex);

	return CalcInsertionFont(textIter, styleIter);
}

// private

JFont
JStyledText::CalcInsertionFont
	(
	JStringIterator&	textIter,
	FontIterator&		styleIter
	)
	const
{
	JUtf8Character c;
	if (textIter.Prev(&c, JStringIterator::kStay) && c == '\n' && !textIter.AtEnd())
	{
		JFont f;
		const bool ok = styleIter.Next(&f);
		assert( ok );
		styleIter.SkipPrev();
		return f;
	}
	else if (!textIter.AtBeginning())
	{
		JFont f;
		const bool ok = styleIter.Prev(&f);
		assert( ok );
		styleIter.SkipNext();
		return f;
	}
	else if (!styleIter.GetList()->IsEmpty())
	{
		return styleIter.GetList()->GetFirstItem();
	}
	else
	{
		return itsDefaultFont;
	}
}

/******************************************************************************
 CompareCharacterIndices (static)

 ******************************************************************************/

std::weak_ordering
JStyledText::CompareCharacterIndices
	(
	const TextIndex& i,
	const TextIndex& j
	)
{
	return i.charIndex <=> j.charIndex;
}

/******************************************************************************
 CompareByteIndices (static)

 ******************************************************************************/

std::weak_ordering
JStyledText::CompareByteIndices
	(
	const TextIndex& i,
	const TextIndex& j
	)
{
	return i.byteIndex <=> j.byteIndex;
}

/******************************************************************************
 TextCount operators

 ******************************************************************************/

JStyledText::TextCount&
JStyledText::TextCount::operator-=
	(
	const JStyledText::TextCount& c
	)
{
	assert( charCount >= c.charCount && byteCount >= c.byteCount );

	charCount -= c.charCount;
	byteCount -= c.byteCount;
	return *this;
}
