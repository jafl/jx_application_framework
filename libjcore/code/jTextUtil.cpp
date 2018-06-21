/******************************************************************************
 jTextUtils.cpp

	Utilities for text & JTextEditor.

	Copyright (C) 2016-17 by John Lindal.

 ******************************************************************************/

#include "jTextUtil.h"
#include "JStyledText.h"
#include "JStringIterator.h"
#include "JRegex.h"
#include "jStreamUtil.h"
#include "JFontManager.h"
#include "JColorManager.h"
#include "jGlobals.h"
#include "jAssert.h"

/******************************************************************************
 JCalcWSFont

	Calculates the appropriate style for whitespace between two styled words.
	We don't recalculate the font id because we only change underline and strike.

 ******************************************************************************/

JFont
JCalcWSFont
	(
	const JFont& prevFont,
	const JFont& nextFont
	)
{
	const JFontStyle& prevStyle = prevFont.GetStyle();
	const JFontStyle& nextStyle = nextFont.GetStyle();

	const JBoolean ulMatch =
		JI2B( prevStyle.underlineCount == nextStyle.underlineCount );

	const JBoolean sMatch =
		JI2B( prevStyle.strike == nextStyle.strike );

	if (!ulMatch && !sMatch &&
		prevStyle.underlineCount == 0 && !prevStyle.strike)
		{
		return prevFont;
		}
	else if (!ulMatch && !sMatch &&
			 nextStyle.underlineCount == 0 && !nextStyle.strike)
		{
		return nextFont;
		}
	else if (!ulMatch && !sMatch)
		{
		JFont f = nextFont;
		f.SetUnderlineCount(0);
		f.SetStrike(kJFalse);
		return f;
		}
	else if (!ulMatch && prevStyle.underlineCount == 0)
		{
		return prevFont;
		}
	else if (!ulMatch && nextStyle.underlineCount == 0)
		{
		return nextFont;
		}
	else if (!ulMatch)
		{
		JFont f = nextFont;
		f.SetUnderlineCount(0);
		return f;
		}
	else if (!sMatch && !prevStyle.strike)
		{
		return prevFont;
		}
	else if (!sMatch && !nextStyle.strike)
		{
		return nextFont;
		}
	else
		{
		return nextFont;
		}
}

/******************************************************************************
 JAnalyzeWhitespace

 *****************************************************************************/

JSize
JAnalyzeWhitespace
	(
	const JString&	buffer,
	const JSize		tabWidth,
	const JBoolean	defaultUseSpaces,
	JBoolean*		useSpaces,
	JBoolean*		isMixed
	)
{
	assert( tabWidth > 0 );

	*isMixed = kJFalse;

	const JSize maxSpaceCount = 100;
	JSize spaceLines = 0, tinySpaceLines = 0, tabLines = 0,
		  spaceHisto[maxSpaceCount], spaceHistoCount = 0;
	memset(spaceHisto, 0, sizeof(spaceHisto));

	JStringIterator iter(buffer);
	JUtf8Character c;
	do
		{
		JSize spaceCount = 0, tailSpaceCount = 0;
		JBoolean tabs = kJFalse;
		while (iter.Next(&c))
			{
			if (c == ' ')
				{
				spaceCount++;
				tailSpaceCount++;
				}
			else if (c == '\t')
				{
				tabs           = kJTrue;
				tailSpaceCount = 0;
				}
			else
				{
				break;
				}
			}

		if (!tabs && 0 < spaceCount && spaceCount <= maxSpaceCount)
			{
			spaceHisto[ spaceCount-1 ]++;
			spaceHistoCount++;
			}

		if (spaceCount == tailSpaceCount && tailSpaceCount < tabWidth)
			{
			if (tabs)
				{
				tabLines++;
				}
			else if (spaceCount > 0)
				{
				tinySpaceLines++;
				}
			}
		else if (spaceCount > 0 && tabs)
			{
			*isMixed = kJTrue;

			if (defaultUseSpaces)
				{
				spaceLines++;
				}
			else
				{
				tabLines++;
				}
			}
		else if (spaceCount > 0)
			{
			spaceLines++;
			}
		}
		while ((c == "\n" || iter.Next("\n")) && !iter.AtEnd());

	if (tabLines > 0)
		{
		tabLines += tinySpaceLines;
		}
	else
		{
		spaceLines += tinySpaceLines;
		}

	if (tabLines > 0 && spaceLines > 0)
		{
		*isMixed = kJTrue;
		}

	*useSpaces = JI2B(spaceLines > tabLines || (spaceLines == tabLines && defaultUseSpaces));

	if (!*useSpaces || spaceHistoCount == 0)
		{
		return tabWidth;
		}

	// determine tab width - [2,10]

	JSize bestWidth = tabWidth, maxCount = 0;
	for (JIndex w=10; w>=2; w--)
		{
		JIndex i = w, lineCount = 0;
		do
			{
			lineCount += spaceHisto[i-1];
			i         += w;
			}
			while (i <= maxSpaceCount);

		if (lineCount > (JIndex) JRound(1.1 * maxCount))
			{
			maxCount  = lineCount;
			bestWidth = w;
			}
		}

	return bestWidth;
}

/******************************************************************************
 JReadUNIXManOutput

	Replaces the contents of the given JStyledText.

	"_\b_" => underscore
	"_\bc" => underlined c
	"c\bc" => bold c

 ******************************************************************************/

static const JRegex theExtraLinesPattern = "\n{3,}";

void
JReadUNIXManOutput
	(
	std::istream&	input,
	JStyledText*	st
	)
{
	JString buffer;
	JReadAll(input, &buffer);
	buffer.TrimWhitespace();

	JRunArray<JFont> styles;

	const JFont& defFont = st->GetDefaultFont();

	JFont boldFont = defFont;
	boldFont.SetBold(kJTrue);

	JFont ulFont = defFont;
	ulFont.SetUnderlineCount(1);

	JStringIterator iter(&buffer);
	JUtf8Character c, prev;
	while (iter.Next(&c))
		{
		if (!iter.AtEnd() && prev == '_' && c == '\b')
			{
			iter.RemovePrev(2);	// toss marker
			iter.Next(&c);		// leave character

			const JSize count = styles.GetElementCount();
			if (c == '_' &&
				(count == 1 ||
				 styles.GetElement(count-1) != ulFont))
				{
				styles.SetElement(count, defFont);
				}
			else
				{
				styles.SetElement(count, ulFont);
				}
			}
		else if (!iter.AtEnd() && c == '\b' && iter.GetPrevCharacterIndex() > 1)
			{
			iter.RemovePrev();	// toss backspace
			iter.Next(&c);
			if (c == prev)
				{
				iter.RemovePrev();	// toss duplicate
				styles.SetElement(styles.GetElementCount(), boldFont);
				}
			else
				{
				iter.SkipPrev();	// reprocess
				}
			}
		else
			{
			styles.AppendElement(defFont);
			}
		}

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter.Next(theExtraLinesPattern))
		{
		iter.ReplaceLastMatch("\n\n");
		}

	st->SetText(buffer, &styles);
}

/******************************************************************************
 JPasteUNIXTerminalOutput

	Parses text and approximates the formatting.
	Pastes the result into the existing text.
	Returns the number of characters inserted.

 ******************************************************************************/

static const JRegex theUNIXTerminalFormatPattern = "^\\[([0-9]+(?:;[0-9]+)*)m$";

JStyledText::TextRange
JPasteUNIXTerminalOutput
	(
	const JString&					text,
	const JStyledText::TextIndex&	pasteIndex,
	JStyledText*					st
	)
{
	JString buffer;
	JRunArray<JFont> styles;

	JFont f        = st->GetDefaultFont();
	const JFont f0 = f;

	JPtrArray<JString> chunkList(JPtrArrayT::kDeleteAll);
	text.Split(theUNIXTerminalFormatPattern, &chunkList, 0, kJTrue);

	JPtrArray<JString> cmdList(JPtrArrayT::kDeleteAll);

	for (const JString* chunk : chunkList)
		{
		const JStringMatch m = theUNIXTerminalFormatPattern.Match(*chunk, kJTrue);
		if (m.IsEmpty())
			{
			buffer += *chunk;
			styles.AppendElements(f, chunk->GetCharacterCount());
			}

		m.GetSubstring(1).Split(";", &cmdList);

		for (const JString* cmd : cmdList)
			{
			JUInt cmdID;
			if (!cmd->ConvertToUInt(&cmdID))
				{
				continue;
				}

			switch (cmdID)
				{
				case 0:
					f = f0;
					break;

				case 1:
					f.SetBold(kJTrue);
					break;
				case 22:
					f.SetBold(kJFalse);
					break;

				case 3:
					f.SetItalic(kJTrue);
					break;
				case 23:
					f.SetItalic(kJFalse);
					break;

				case 4:
					f.SetUnderlineCount(1);
					break;
				case 24:
					f.SetUnderlineCount(0);
					break;

				case 30:
				case 39:
					f.SetColor(JColorManager::GetBlackColor());
					break;
				case 37:
					f.SetColor(JColorManager::GetGrayColor(80));
					break;
				case 90:
					f.SetColor(JColorManager::GetGrayColor(50));
					break;
				case 31:
					f.SetColor(JColorManager::GetRedColor());
					break;
				case 32:
					f.SetColor(JColorManager::GetDarkGreenColor());	// green-on-white is impossible to read
					break;
				case 33:
					f.SetColor(JColorManager::GetBrownColor());		// yellow-on-white is impossible to read
					break;
				case 34:
					f.SetColor(JColorManager::GetBlueColor());
					break;
				case 35:
					f.SetColor(JColorManager::GetMagentaColor());
					break;
				case 36:
					f.SetColor(JColorManager::GetLightBlueColor());	// cyan-on-white is impossible to read
					break;
				}
			}
		}

	return st->Paste(JStyledText::TextRange(pasteIndex, JStyledText::TextCount()), buffer, &styles);
}

/******************************************************************************
 JReadLimitedMarkdown

	Replaces the contents of the given JStyledText.

	Parses limited markdown:  *bold*, -italic-, _underline_, `fixed width`.
	Does not handle nested styling.

 ******************************************************************************/

static const JRegex theBoldPattern       = "\\B\\*((?>[^*]+))\\*\\B";
static const JRegex theItalicPattern     = "\\B-((?>[^-]+))-\\B";
static const JRegex theUnderlinePattern  = "\\b_((?>[^_]+))_\\b";
static const JRegex theFixedWidthPattern = "`((?>[^`]+))`";

void
jReplaceMarkdownPattern
	(
	JStringIterator*	iter,
	const JRegex&		pattern,
	const JFont&		f,
	JRunArray<JFont>*	styles
	)
{
	iter->MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter->Next(pattern))
		{
		const JStringMatch& m = iter->GetLastMatch();
		const JString s       = m.GetSubstring(1);

		const JCharacterRange r(m.GetCharacterRange());
		styles->RemoveElements(r);
		styles->InsertElementsAtIndex(r.first, f, s.GetCharacterCount());

		iter->ReplaceLastMatch(s);
		}
}

void
JReadLimitedMarkdown
	(
	const JString&	text,
	JStyledText*	st
	)
{
	JString buffer = text;
	JRunArray<JFont> styles;

	JStringIterator iter(&buffer);

	JFont f = st->GetDefaultFont();
	styles.AppendElements(f, buffer.GetCharacterCount());

	f.SetBold(kJTrue);
	jReplaceMarkdownPattern(&iter, theBoldPattern, f, &styles);
	f.SetBold(kJFalse);

	f.SetItalic(kJTrue);
	jReplaceMarkdownPattern(&iter, theItalicPattern, f, &styles);
	f.SetItalic(kJFalse);

	f.SetUnderlineCount(1);
	jReplaceMarkdownPattern(&iter, theUnderlinePattern, f, &styles);
	f.SetUnderlineCount(0);

	f.SetName(JFontManager::GetDefaultMonospaceFontName());
	jReplaceMarkdownPattern(&iter, theFixedWidthPattern, f, &styles);
	// no need to reset font

	st->SetText(buffer, &styles);
}
