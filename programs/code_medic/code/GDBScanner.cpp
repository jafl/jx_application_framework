/******************************************************************************
 GDBScanner.cpp

	A string scanner for gdb output.

	BASE CLASS = GDBOutputFlexLexer

	Copyright © 1997 by Dustin Laurence.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include <GDBScanner.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	The argument is a pointer to the YYSTYPE variable that the lexer should use
	to return semantic values.  It may be NULL if the Bison convention is to be
	used exclusively.  The default constructor sets it to NULL.

 *****************************************************************************/

GDBScanner::GDBScanner()
	:
	GDBOutputFlexLexer(),
	itsBytesRead(0),
	itsResetFlag(kJTrue)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBScanner::~GDBScanner()
{
}

/******************************************************************************
 Reset

 *****************************************************************************/

void
GDBScanner::Reset()
{
	itsResetFlag = kJTrue;
}

/******************************************************************************
 AppendInput

 *****************************************************************************/

void
GDBScanner::AppendInput
	(
	const JCharacter* string
	)
{
	const JCharacter* p1 = string;
	const JCharacter* p2;
	while (1)
		{
		p2 = strchr(p1, '\n');
		if (p2 == NULL)
			{
			break;
			}

		if (!itsPartialBuffer.IsEmpty())
			{
			itsInputBuffer += itsPartialBuffer;
			itsPartialBuffer.Clear();
			}
		itsInputBuffer.Append(p1, p2 - p1 + 1);
		p1 = p2 + 1;
		}

	// Accumulate partial line for later parsing if:
	// 1: the first character is a meta character
	// 2: we have already accumulated something else

	if (strchr("(^~@&*+=", *p1) != NULL || !itsPartialBuffer.IsEmpty())
		{
		itsPartialBuffer += p1;
		}
	else
		{
		itsInputBuffer += p1;
		}
}

/******************************************************************************
 LexerInput (virtual protected)

 *****************************************************************************/

FlexLexerInputSize
GDBScanner::LexerInput
	(
	char*				flexBuf,
	FlexLexerInputSize	flexBufSize
	)
{
	assert( flexBuf != NULL && flexBufSize > 0 );

	const int charLeft = itsInputBuffer.GetLength() - itsBytesRead;
	const int numCopy  = (charLeft > flexBufSize) ? flexBufSize : charLeft;

	if (numCopy > 0)
		{
		memcpy(flexBuf, itsInputBuffer.GetCString() + itsBytesRead, numCopy);
		itsBytesRead += numCopy;
		}

	return numCopy;
}

/******************************************************************************
 Flush

 *****************************************************************************/

void
GDBScanner::Flush()
{
	if (itsBytesRead > 0)
		{
		itsInputBuffer.RemoveSubstring(1, itsBytesRead);
		itsBytesRead = 0;
		}
}

/******************************************************************************
 ExtractCommandId (private)

 *****************************************************************************/

void
GDBScanner::ExtractCommandId
	(
	const JCharacter*	yytext,
	const JSize			yyleng
	)
{
	const JCharacter* s = yytext + yyleng-4;
	while (isdigit(s[-1]))
		{
		s--;
		}
	const_cast<JCharacter*>(yytext)[yyleng-3] = '\0';
	const JBoolean ok = JString::ConvertToUInt(s, &(itsData.number));
	assert( ok );
}

/******************************************************************************
 TranslateMIOutput (static)

 *****************************************************************************/

void
GDBScanner::TranslateMIOutput
	(
	JString* data
	)
{
	JSize count = data->GetLength();
	for (JIndex i=1; i<=count; i++)
		{
		if (data->GetCharacter(i) == '\\')
			{
			data->RemoveSubstring(i, i);
			count--;

			if (data->GetCharacter(i) == 'n')
				{
				data->SetCharacter(i, '\n');
				}
			else if (data->GetCharacter(i) == 't')
				{
				data->SetCharacter(i, '\t');
				}
			}
		}
}
