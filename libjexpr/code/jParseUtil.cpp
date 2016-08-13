/******************************************************************************
 jParseUtil.cpp

	This module provides utility functions for the JDecision and JFunction parsers.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jParseUtil.h>
#include <string.h>
#include <jAssert.h>

// Private functions

enum SkipParenResult
{
	kNoBlock,
	kSkippedBlock,
	kParenError,
	kEndOfString
};

JBoolean
JSkipBlockAtLeft(const JCharacter* sourceStart, JSize* offset);
JBoolean
JSkipBlockAtLeft(const JCharacter* sourceStart, JSize* offset,
				 const JCharacter blockOpen, const JCharacter blockClose);

SkipParenResult
JSkipBlockAtRight(const JCharacter* sourceStart,
				  const JSize sourceLength, JSize* offset);
SkipParenResult
JSkipBlockAtRight(const JCharacter* sourceStart,
				  const JSize sourceLength, JSize* offset,
				  const JCharacter blockOpen, const JCharacter blockClose);

JBoolean JParenthesesBalanced(const JCharacter* expr, const JSize length);
JSize    JStripSpaces(const JCharacter** expr, const JSize origLength);

/******************************************************************************
 JFindLastOperator

	Returns kJTrue if opstr is somewhere inside expr.
	offset is then set to the start of the last occurrence of opstr inside expr.
	Parenthesized blocks are treated as indivisible units.

 ******************************************************************************/

JBoolean
JFindLastOperator
	(
	const JCharacter*	exprStart,
	const JSize			exprLength,
	const JCharacter*	opstr,
	JSize*				offset
	)
{
	const JSize opstrLength = strlen(opstr);
	if (opstrLength > exprLength)
		{
		return kJFalse;
		}

	*offset = exprLength;
	while (*offset > exprLength-opstrLength)
		{
		(*offset)--;
		if (!JSkipBlockAtLeft(exprStart, offset))
			{
			return kJFalse;
			}
		}

	do
		{
		if (memcmp(exprStart + *offset, opstr, opstrLength) == 0)
			{
			return kJTrue;
			}
		(*offset)--;
		if (!JSkipBlockAtLeft(exprStart, offset))
			{
			return kJFalse;
			}
		}
		while (*offset > 0);

	return kJFalse;
}

/******************************************************************************
 JSkipBlockAtLeft

	Update offset to point in front of the block to the left of the current
	offset.  We also skip over spaces to the left of the block.

	blockOpen  = character that opens the block.
	blockClose = character that closes the block.

	Returns kJTrue if we didn't fall off the left end of the string.

 ******************************************************************************/

JBoolean
JSkipBlockAtLeft
	(
	const JCharacter*	sourceStart,
	JSize*				offset
	)
{
	return JConvertToBoolean(
			JSkipBlockAtLeft(sourceStart, offset, '(', ')') &&
			JSkipBlockAtLeft(sourceStart, offset, '[', ']'));
}

JBoolean
JSkipBlockAtLeft
	(
	const JCharacter*	sourceStart,
	JSize*				offset,
	const JCharacter	blockOpen,
	const JCharacter	blockClose
	)
{
	// check if there actually is the beginning of a block

	if (*offset == 0)
		{
		return kJFalse;
		}
	if (*(sourceStart + *offset) != blockClose)
		{
		return kJTrue;
		}

	// skip over the block

	JSize parenCount = 1;
	while (parenCount > 0)
		{
		(*offset)--;
		if (*(sourceStart + *offset) == blockOpen)
			{
			parenCount--;
			}
		else if (*(sourceStart + *offset) == blockClose)
			{
			parenCount++;
			}

		if (*offset == 0)
			{
			// we fell off the end of the string

			return kJFalse;
			}
		}

	// skip over blank space

	while (*(sourceStart + *offset) == ' ')
		{
		(*offset)--;
		if (*offset == 0)
			{
			return kJFalse;	// we fell off the end of the string
			}
		}

	return kJTrue;
}

/******************************************************************************
 JFindFirstOperator

	Returns kJTrue if opstr is somewhere inside expr.
	offset is then set to the start of the first occurrence of opstr inside expr.
	Parenthesized blocks are treated as indivisible units.

 ******************************************************************************/

JBoolean
JFindFirstOperator
	(
	const JCharacter*	exprStart,
	const JSize			exprLength,
	const JCharacter*	opstr,
	JSize*				offset
	)
{
	const JSize opstrLength = strlen(opstr);

	*offset = 0;
	while (*offset + opstrLength <= exprLength)
		{
		if (memcmp(exprStart + *offset, opstr, opstrLength) == 0)
			{
			return kJTrue;
			}

		const SkipParenResult skipResult =
			JSkipBlockAtRight(exprStart, exprLength, offset);
		if (skipResult == kParenError)
			{
			return kJFalse;
			}
		else if (skipResult == kNoBlock)
			{
			(*offset)++;
			}
		else if (skipResult == kEndOfString)
			{
			break;	// we might as well quit
			}
		else if (skipResult == kSkippedBlock)
			{
			// we don't need to modify offset any further
			}
		else
			{
			assert( 0 );	// in case a new result is added
			}
		}

	return kJFalse;
}

/******************************************************************************
 JSkipBlockAtRight

	Update offset to point after the parenthesized block to the right
	of the current offset.  We also skip over spaces to the right of the block.

	blockOpen  = character that opens the block.
	blockClose = character that closes the block.

	kNoBlock		-- the was no parenthesized block to be skipped
	kSkippedBlock	-- successfully skipped block
	kParenError		-- unbalanced parentheses
	kEndOfString	-- successfully skipped block and hit end of string

 ******************************************************************************/

SkipParenResult
JSkipBlockAtRight
	(
	const JCharacter*	sourceStart,
	const JSize			sourceLength,
	JSize*				offset
	)
{
	const SkipParenResult result =
		JSkipBlockAtRight(sourceStart, sourceLength, offset, '(', ')');
	if (result != kNoBlock)
		{
		return result;
		}
	else
		{
		return JSkipBlockAtRight(sourceStart, sourceLength, offset, '[', ']');
		}
}

SkipParenResult
JSkipBlockAtRight
	(
	const JCharacter*	sourceStart,
	const JSize			sourceLength,
	JSize*				offset,
	const JCharacter	blockOpen,
	const JCharacter	blockClose
	)
{
	// check if this is actually the beginning of a block

	if (*offset >= sourceLength || *(sourceStart + *offset) != blockOpen)
		{
		return kNoBlock;
		}

	// skip over the block

	JSize parenCount = 1;
	while (parenCount > 0)
		{
		(*offset)++;
		if (*offset >= sourceLength)
			{
			return kParenError;
			}

		if (*(sourceStart + *offset) == blockOpen)
			{
			parenCount++;
			}
		else if (*(sourceStart + *offset) == blockClose)
			{
			parenCount--;
			}
		}

	// move past closing parenthesis

	(*offset)++;
	if (*offset >= sourceLength)
		{
		return kEndOfString;
		}

	// skip over blank space

	while (*(sourceStart + *offset) == ' ')
		{
		(*offset)++;
		if (*offset >= sourceLength)
			{
			return kEndOfString;
			}
		}

	return kSkippedBlock;
}

/******************************************************************************
 JStripParentheses

	Returns the start and length of the substring that contains no enclosing
	sets of parentheses.

	e.g. ((x+3)*y) -> (x+3)*y (expr = origExpr + 1; length = origLength - 2)

 ******************************************************************************/

JSize
JStripParentheses
	(
	const JCharacter**	expr,
	const JSize			origLength
	)
{
	JSize newLength = JStripSpaces(expr, origLength);

	while (newLength > 1 &&
		   **expr == '(' && *(*expr + newLength - 1) == ')' &&
		   JParenthesesBalanced((*expr)+1, newLength-2))
		{
		(*expr)++;
		newLength -= 2;
		newLength = JStripSpaces(expr, newLength);
		}

	return newLength;
}

/******************************************************************************
 JParenthesesBalanced

	Returns kJTrue if all parentheses in expr are balanced.

	e.g. x+3)*(y+z is not balanced.

 ******************************************************************************/

JBoolean
JParenthesesBalanced
	(
	const JCharacter*	expr,
	const JSize			length
	)
{
	long parenCount = 0;
	for (JSize i=0; i<length; i++)
		{
		if (*(expr + i) == '(')
			{
			parenCount++;
			}
		else if (*(expr + i) == ')')
			{
			parenCount--;
			}

		// if the count goes negative, it's an instant error

		if (parenCount < 0)
			{
			return kJFalse;
			}
		}

	// if the count is nonzero, a parentheses was not closed

	return JConvertToBoolean(parenCount == 0);
}

/******************************************************************************
 JStripSpaces

	Returns the start and length of the substring that contains no leading
	or trailing spaces.

 ******************************************************************************/

JSize
JStripSpaces
	(
	const JCharacter**	expr,
	const JSize			origLength
	)
{
	JSize newLength = origLength;

	while (newLength > 0 && **expr == ' ')
		{
		(*expr)++;
		newLength--;
		}
	while (newLength > 0 && *(*expr + newLength - 1) == ' ')
		{
		newLength--;
		}

	return newLength;
}

/******************************************************************************
 JStringsEqual

	Returns kJTrue if the two strings are equal.

 ******************************************************************************/

JBoolean
JStringsEqual
	(
	const JCharacter*	s1,
	const JSize			s1Length,
	const JCharacter*	s2
	)
{
	return JConvertToBoolean( s1Length == strlen(s2) && memcmp(s2, s1, s1Length) == 0 );
}

/******************************************************************************
 JStringBeginsWith

	Returns kJTrue if source starts with substr.

 ******************************************************************************/

JBoolean
JStringBeginsWith
	(
	const JCharacter*	sourceStart,
	const JSize			sourceLength,
	const JCharacter*	substr
	)
{
	const JSize substrLength = strlen(substr);
	if (substrLength > sourceLength)
		{
		return kJFalse;
		}

	return JConvertToBoolean( memcmp(sourceStart, substr, substrLength) == 0 );
}

/******************************************************************************
 JStringContains

	Returns kJTrue if substr is somewhere inside source.
	offset is then set to the start of the last occurrence of substr inside source.

 ******************************************************************************/

JBoolean
JStringContains
	(
	const JCharacter*	sourceStart,
	const JSize			sourceLength,
	const JCharacter*	substr,
	JSize*				offset
	)
{
	const JSize substrLength = strlen(substr);

	if (sourceLength >= substrLength)	// can't subtract otherwise since unsigned
		{
		for (JSize i=0; i<=sourceLength-substrLength; i++)
			{
			if (memcmp(sourceStart+i, substr, substrLength) == 0)
				{
				*offset = i;
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/******************************************************************************
 JStringEndsWith

	Returns kJTrue if source ends with substr.

 ******************************************************************************/

JBoolean
JStringEndsWith
	(
	const JCharacter*	sourceStart,
	const JSize			sourceLength,
	const JCharacter*	substr
	)
{
	const JSize substrLength = strlen(substr);
	if (substrLength > sourceLength)
		{
		return kJFalse;
		}

	const JCharacter* compareStart = sourceStart + sourceLength - substrLength;
	return JConvertToBoolean( memcmp(compareStart, substr, substrLength) == 0 );
}
