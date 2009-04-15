/******************************************************************************
 JPTPrinter.cpp

	This class implements the functions required to print plain text.

	BASE CLASS = JPrinter

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPTPrinter.h>
#include <JLatentPG.h>
#include <JString.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jASCIIConstants.h>
#include <jFileUtil.h>
#include <sstream>
#include <stdio.h>
#include <jAssert.h>

const JCharacter* kLineNumberMarginStr = "  ";
const JSize kLineNumberMarginWidth     = 2;

// setup information

const JFileVersion kCurrentSetupVersion = 2;
const JCharacter kSetupDataEndDelimiter = '\1';

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
	itsPrintReverseOrderFlag = kJFalse;
	itsPrintLineNumberFlag   = kJFalse;
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
	Returns kJFalse if the process was cancelled.

	Derived classes can override the header and footer functions to
	print a header and/or footer on each page.

 ******************************************************************************/

static const JCharacter kPageSeparatorStr[] = { kJFormFeedKey, '\0' };
const JSize kPageSeparatorStrLength         = 1;

JBoolean
JPTPrinter::Print
	(
	const JCharacter*	text,
	ostream&			trueOutput
	)
{
	ostream* dataOutput  = &trueOutput;
	ofstream* tempOutput = NULL;
	JString tempName;
	if (itsPrintReverseOrderFlag)
		{
		if (!(JCreateTempFile(&tempName)).OK())
			{
			return kJFalse;
			}

		tempOutput = new ofstream(tempName);
		assert( tempOutput != NULL );
		if (tempOutput->bad())
			{
			delete tempOutput;
			JRemoveFile(tempName);
			return kJFalse;
			}
		dataOutput = tempOutput;
		}

	const JSize headerLineCount = GetHeaderLineCount();
	const JSize footerLineCount = GetFooterLineCount();
	assert( itsPageHeight > headerLineCount + footerLineCount );

	const JSize lineCountPerPage = itsPageHeight - headerLineCount - footerLineCount;

	JLatentPG pg;
	pg.VariableLengthProcessBeginning("Printing page...", kJTrue, kJFalse);
	JBoolean keepGoing = kJTrue;

	JUnsignedOffset i   = 0;
	JIndex pageIndex    = 0;
	JSize printCount    = 0;
	JSize textLineCount = 0;
	while (keepGoing && text[i] != '\0')
		{
		pageIndex++;
		const JBoolean shouldPrintPage =
			JI2B(itsFirstPageIndex <= pageIndex &&
				 (itsLastPageIndex == 0 || pageIndex <= itsLastPageIndex));

		std::ostringstream bitBucket;
		ostream* output = shouldPrintPage ? dataOutput : (&bitBucket);

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
			lineNumberWidth = lastLineIndexStr.GetLength();
			}

		JSize lineCount = 0;
		while (lineCount < lineCountPerPage && text[i] != '\0')
			{
			JSize col = 0;

			if (itsPrintLineNumberFlag)
				{
				const JString lineNumberStr(textLineCount+1, 0);
				const JSize spaceCount = lineNumberWidth - lineNumberStr.GetLength();
				for (JIndex j=1; j<=spaceCount; j++)
					{
					*output << ' ';
					}
				lineNumberStr.Print(*output);
				*output << kLineNumberMarginStr;

				col += lineNumberWidth + kLineNumberMarginWidth;
				}

			if (col >= itsPageWidth)	// insures progress, even in ludicrous boundary case
				{
				col = itsPageWidth - 1;
				}

			while (col < itsPageWidth && text[i] != '\n' && text[i] != '\0')
				{
				if (text[i] == '\t')
					{
					const JSize spaceCount = itsTabWidth - (col % itsTabWidth);
					for (JIndex j=1; j<=spaceCount; j++)
						{
						*output << ' ';
						}
					col += spaceCount;
					}
				else if (text[i] == kJFormFeedKey)
					{
					*output << ' ';
					col++;
					}
				else
					{
					*output << text[i];
					col++;
					}
				i++;
				}

			*output << '\n';
			if (text[i] == '\n')
				{
				i++;
				}

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

	if (itsPrintReverseOrderFlag)
		{
		delete tempOutput;
		if (keepGoing)
			{
			JString text;
			JReadFile(tempName, &text);
			InvertPageOrder(text, trueOutput);
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
	ostream&		output,
	const JIndex	pageIndex
	)
{
}

void
JPTPrinter::PrintFooter
	(
	ostream&		output,
	const JIndex	pageIndex
	)
{
}

/******************************************************************************
 InvertPageOrder (private)

	Splits the text at kPageSeparatorStr and inverts the ordering of the blocks.

 ******************************************************************************/

void
JPTPrinter::InvertPageOrder
	(
	const JString&	text,
	ostream&		output
	)
	const
{
	JIndex endIndex   = text.GetLength() + 1;
	JIndex startIndex = endIndex - 1;
	while (text.LocatePrevSubstring(kPageSeparatorStr, kPageSeparatorStrLength, &startIndex))
		{
		const JIndex i = startIndex + kPageSeparatorStrLength;
		if (endIndex > i)
			{
			output.write(text.GetCString() + i-1, endIndex - i);
			}
		output << kPageSeparatorStr;

		endIndex = startIndex;
		startIndex--;
		}

	if (endIndex > 1)
		{
		output.write(text.GetCString(), endIndex-1);
		}
}

/******************************************************************************
 ReadPTSetup

 ******************************************************************************/

void
JPTPrinter::ReadPTSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers <= kCurrentSetupVersion)
		{
		input >> itsPageWidth >> itsPageHeight;

		if (vers >= 2)
			{
			input >> itsPrintReverseOrderFlag;
			}
		if (vers >= 1)
			{
			input >> itsPrintLineNumberFlag;
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
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsPageWidth;
	output << ' ' << itsPageHeight;
	output << ' ' << itsPrintReverseOrderFlag;
	output << ' ' << itsPrintLineNumberFlag;
	output << kSetupDataEndDelimiter;
}
