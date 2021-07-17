/******************************************************************************
 GDBScanner.cpp

	A string scanner for gdb output.

	BASE CLASS = GDBOutputFlexLexer

	Copyright (C) 1997 by Dustin Laurence.

 *****************************************************************************/

#include <GDBScanner.h>
#include <JStringIterator.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	The argument is a pointer to the YYSTYPE variable that the lexer should use
	to return semantic values.  It may be nullptr if the Bison convention is to be
	used exclusively.  The default constructor sets it to nullptr.

 *****************************************************************************/

GDBScanner::GDBScanner()
	:
	GDBOutputFlexLexer(),
	itsBytesRead(0),
	itsResetFlag(true)
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
	itsResetFlag = true;
}

/******************************************************************************
 AppendInput

 *****************************************************************************/

void
GDBScanner::AppendInput
	(
	const JString& string
	)
{
	JPtrArray<JString> split(JPtrArrayT::kDeleteAll);
	string.Split("\n", &split);

	JString* last = split.GetLastElement();
	split.RemoveElement(split.GetElementCount());

	for (JString* s : split)
	{
		if (!itsPartialBuffer.IsEmpty())
			{
			itsInputBuffer += itsPartialBuffer;
			itsPartialBuffer.Clear();
			}
		itsInputBuffer.Append(*s);
		itsInputBuffer.Append("\n");
	}

	// Accumulate partial line for later parsing if:
	// 1: the first character is a meta character
	// 2: we have already accumulated something else

	if (strchr("(^~@&*+=", last->GetBytes()[0]) != nullptr || !itsPartialBuffer.IsEmpty())
		{
		itsPartialBuffer += *last;
		}
	else
		{
		itsInputBuffer += *last;
		}

	jdelete last;
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
	assert( flexBuf != nullptr && flexBufSize > 0 );

	const int byteLeft = itsInputBuffer.GetByteCount() - itsBytesRead;

	int numCopy = 0;
	if (byteLeft <= flexBufSize)
	{
		numCopy = byteLeft;
	}
	else
	{
		const JUtf8Byte* p = itsInputBuffer.GetRawBytes() + itsBytesRead;

		JSize remainingCount = flexBufSize;
		JSize byteCount;
		while (remainingCount > 0 &&
			   (JUtf8Character::IsCompleteCharacter(p + numCopy, remainingCount, &byteCount) ||
				remainingCount >= JUtf8Character::kMaxByteCount))
			{
			numCopy        += byteCount;
			remainingCount -= byteCount;
			}
	}

	if (numCopy > 0)
		{
		memcpy(flexBuf, itsInputBuffer.GetBytes() + itsBytesRead, numCopy);
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
		JStringIterator iter(&itsInputBuffer, kJIteratorStartAfterByte, itsBytesRead);
		iter.RemoveAllPrev();
		itsBytesRead = 0;
		}
}

/******************************************************************************
 ExtractCommandId (private)

 *****************************************************************************/

void
GDBScanner::ExtractCommandId
	(
	const JUtf8Byte*	yytext,
	const JSize			yyleng
	)
{
	const JUtf8Byte* s = yytext + yyleng-4;
	while (isdigit(s[-1]))
		{
		s--;
		}
	const_cast<JUtf8Byte*>(yytext)[yyleng-3] = '\0';
	const bool ok = JString::ConvertToUInt(s, &itsData.number);
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
	JStringIterator iter(data);
	JUtf8Character c;
	while (iter.Next(&c))
		{
		if (c == '\\')
			{
			iter.RemovePrev();

			if (!iter.Next(&c))
				{
				break;
				}

			if (c == 'n')
				{
				iter.SetPrev(JUtf8Character('\n'));
				}
			else if (c == 't')
				{
				iter.SetPrev(JUtf8Character('\t'));
				}
			}
		}
}
