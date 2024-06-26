/******************************************************************************
 JPTPrinter.cpp

	This class implements the functions required to print plain text.

	BASE CLASS = JPrinter

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JPTPrinter.h"
#include "JLatentPG.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "jFStreamUtil.h"
#include "jStreamUtil.h"
#include "jASCIIConstants.h"
#include "jFileUtil.h"
#include "jGlobals.h"
#include <sstream>
#include <stdio.h>
#include "jAssert.h"

const JUtf8Byte* kLineNumberMarginStr = "  ";
const JSize kLineNumberMarginWidth    = 2;

// setup information

const JFileVersion kCurrentSetupVersion = 2;
const JUtf8Byte kSetupDataEndDelimiter = '\1';

	// version 1 adds itsPrintLineNumberFlag
	// version 2 adds itsPrintReverseOrderFlag

/******************************************************************************
 Constructor

 ******************************************************************************/

JPTPrinter::JPTPrinter()
	:
	JPrinter()
{
	itsCopyCount             = 1;
	itsFirstPageIndex        = 0;
	itsLastPageIndex         = 0;
	itsPageWidth             = 80;		// characters
	itsPageHeight            = 60;		// lines
	itsTabWidth              = 8;		// characters
	itsPrintReverseOrderFlag = false;
	itsPrintLineNumberFlag   = false;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPTPrinter::~JPTPrinter()
{
}

/******************************************************************************
 Set page size

	The page width is measured in characters, while the page height is
	measured in lines.

 ******************************************************************************/

void
JPTPrinter::SetPageWidth
	(
	const JSize width
	)
{
	assert( width > 0 );
	itsPageWidth = width;
}

void
JPTPrinter::SetPageHeight
	(
	const JSize height
	)
{
	assert( height > 0 );
	itsPageHeight = height;
}

/******************************************************************************
 Print (protected)

	Prints the given text to the output stream, formatting it into
	pages based on our page width (characters) and page height (lines).
	Returns false if the process was cancelled.

	Derived classes can override the header and footer functions to
	print a header and/or footer on each page.

 ******************************************************************************/

static const JUtf8Byte kPageSeparatorStr[] = { kJFormFeedKey, '\0' };
const JSize kPageSeparatorStrLength         = 1;

bool
JPTPrinter::Print
	(
	const JString&	text,
	std::ostream&	trueOutput
	)
{
	std::ostream* dataOutput  = &trueOutput;
	std::ofstream* tempOutput = nullptr;
	JString tempName;
	if (itsPrintReverseOrderFlag)
	{
		if (!JCreateTempFile(&tempName))
		{
			return false;
		}

		tempOutput = jnew std::ofstream(tempName.GetBytes());
		if (tempOutput->bad())
		{
			jdelete tempOutput;
			JRemoveFile(tempName);
			return false;
		}
		dataOutput = tempOutput;
	}

	const JSize headerLineCount = GetHeaderLineCount();
	const JSize footerLineCount = GetFooterLineCount();
	assert( itsPageHeight > headerLineCount + footerLineCount );

	const JSize lineCountPerPage = itsPageHeight - headerLineCount - footerLineCount;

	JLatentPG pg;
	pg.VariableLengthProcessBeginning(JGetString("Printing::JPTPrinter"), true, true);
	bool keepGoing = true;

	JIndex pageIndex    = 0;
	JSize printCount    = 0;
	JSize textLineCount = 0;

	JStringIterator iter(text);
	JUtf8Character c;
	while (keepGoing && iter.Next(&c))
	{
		iter.Prev(&c);	// back up for lineCount loop

		pageIndex++;
		const bool shouldPrintPage =
			itsFirstPageIndex <= pageIndex &&
				 (itsLastPageIndex == 0 || pageIndex <= itsLastPageIndex);

		std::ostringstream bitBucket;
		std::ostream* output = shouldPrintPage ? dataOutput : (&bitBucket);

		if (shouldPrintPage)
		{
			printCount++;
			if (printCount > 1)
			{
				*output << kPageSeparatorStr;
			}
		}

		if (headerLineCount > 0)
		{
			PrintHeader(*output, pageIndex);
		}

		JSize lineNumberWidth = 0;
		if (itsPrintLineNumberFlag)
		{
			const JString lastLineIndexStr(pageIndex * lineCountPerPage, 0);
			lineNumberWidth = lastLineIndexStr.GetCharacterCount();
		}

		JSize lineCount = 0;
		while (lineCount < lineCountPerPage && iter.Next(&c))
		{
			iter.Prev(&c);	// back up for pageWidth loop

			JSize margin = 0;
			if (itsPrintLineNumberFlag)
			{
				const JString lineNumberStr(textLineCount+1, 0);
				const JSize spaceCount = lineNumberWidth - lineNumberStr.GetCharacterCount();
				for (JIndex j=1; j<=spaceCount; j++)
				{
					*output << ' ';
				}
				lineNumberStr.Print(*output);
				*output << kLineNumberMarginStr;

				margin = lineNumberWidth + kLineNumberMarginWidth;
			}

			if (margin >= itsPageWidth)	// ensures progress, even in ludicrous boundary case
			{
				margin = itsPageWidth - 1;
			}

			JSize col = 0;
			while (col < itsPageWidth - margin && iter.Next(&c) && c != '\n')
			{
				if (c == '\t')
				{
					const JSize spaceCount = itsTabWidth - (col % itsTabWidth);
					for (JIndex j=1; j<=spaceCount; j++)
					{
						*output << ' ';
					}
					col += spaceCount;
				}
				else if (c == kJFormFeedKey)
				{
					*output << ' ';
					col++;
				}
				else
				{
					output->write(c.GetBytes(), c.GetByteCount());
					col++;
				}
			}

			*output << '\n';

			lineCount++;
			textLineCount++;
		}

		if (footerLineCount > 0)
		{
			while (lineCount < lineCountPerPage)
			{
				*output << '\n';
				lineCount++;
			}

			PrintFooter(*output, pageIndex);
		}

		keepGoing = pg.IncrementProgress();
	}

	pg.ProcessFinished();
	iter.Invalidate();

	if (itsPrintReverseOrderFlag)
	{
		jdelete tempOutput;
		if (keepGoing)
		{
			JString s;
			JReadFile(tempName, &s);
			InvertPageOrder(s, trueOutput);
		}
		JRemoveFile(tempName);
	}

	return keepGoing;
}

/******************************************************************************
 Print header and footer (virtual protected)

	Derived classes can override these functions if they want to
	print a header or a footer.  The default is to do nothing.

 ******************************************************************************/

JSize
JPTPrinter::GetHeaderLineCount()
	const
{
	return 0;
}

JSize
JPTPrinter::GetFooterLineCount()
	const
{
	return 0;
}

void
JPTPrinter::PrintHeader
	(
	std::ostream&	output,
	const JIndex	pageIndex
	)
{
}

void
JPTPrinter::PrintFooter
	(
	std::ostream&	output,
	const JIndex	pageIndex
	)
{
}

/******************************************************************************
 InvertPageOrder (private)

	Splits the text at kPageSeparatorStr and inverts the ordering of the blocks.

	Does not use JRegex::Split because pages are typically large.

 ******************************************************************************/

void
JPTPrinter::InvertPageOrder
	(
	const JString&	text,
	std::ostream&	output
	)
	const
{
	JStringIterator iter(text, JStringIterator::kStartAtEnd);
	iter.BeginMatch();
	while (iter.Prev(kPageSeparatorStr, kPageSeparatorStrLength))
	{
		const JStringMatch& m = iter.FinishMatch();
		m.GetString().Print(output);
		output << kPageSeparatorStr;
		iter.BeginMatch();
	}

	const JStringMatch& m = iter.FinishMatch();
	if (!m.IsEmpty())
	{
		m.GetString().Print(output);
	}
}

/******************************************************************************
 ReadPTSetup

 ******************************************************************************/

void
JPTPrinter::ReadPTSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers <= kCurrentSetupVersion)
	{
		input >> itsPageWidth >> itsPageHeight;

		if (vers >= 2)
		{
			input >> JBoolFromString(itsPrintReverseOrderFlag);
		}
		if (vers >= 1)
		{
			input >> JBoolFromString(itsPrintLineNumberFlag);
		}
	}
	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 WritePTSetup

 ******************************************************************************/

void
JPTPrinter::WritePTSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsPageWidth;
	output << ' ' << itsPageHeight;
	output << ' ' << JBoolToString(itsPrintReverseOrderFlag)
				  << JBoolToString(itsPrintLineNumberFlag);
	output << kSetupDataEndDelimiter;
}
