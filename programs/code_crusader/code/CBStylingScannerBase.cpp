/******************************************************************************
 CBStylingScannerBase.cpp

	Base class for keyword styling lexer.

	BASE CLASS = reflex::AbstractLexer<reflex::Matcher>

	Copyright © 2021 by John Lindal.

 *****************************************************************************/

#include "CBStylingScannerBase.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBStylingScannerBase::CBStylingScannerBase
	(
	const reflex::Input&	input,
	std::ostream&			os
	)
	:
	reflex::AbstractLexer<reflex::Matcher>(input, os)
{
}

/******************************************************************************
 BeginScan (virtual)

 *****************************************************************************/

void
CBStylingScannerBase::BeginScan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	while (!states_empty())
		{
		pop_state();
		}

	in(&input);
	start(0);
}

/******************************************************************************
 Undo (protected)

	*** Only use this to back up to where StartToken() will be called!

 *****************************************************************************/

void
CBStylingScannerBase::Undo
	(
	const JStyledText::TextRange&	range,
	const JSize						prevCharByteCount,
	const JString&					text
	)
{
	for (JUnsignedOffset i=text.GetByteCount()-1; i>=0; i--)
		{
		matcher().unput(text.GetBytes()[i]);
		}

	itsCurrentRange.charRange.first =
	itsCurrentRange.charRange.last  = range.charRange.first - 1;

	itsCurrentRange.byteRange.first = range.byteRange.first - prevCharByteCount;
	itsCurrentRange.byteRange.last  = range.byteRange.first - 1;
}

/******************************************************************************
 SavePPNameRange (virtual protected)

	Saves the range containing the name of the preprocessor directive.

	e.g.  ...\n  #include <iostream>  // need to read from std::cin\n...
				 ^^^^^^^^

	This is necessary because the entire line is treated as one token
	in order to simplify the context-sensitive highlighting code.

 *****************************************************************************/

void
CBStylingScannerBase::SavePPNameRange
	(
	std::function<bool(const char)>& skip
	)
{
	itsPPNameRange = itsCurrentRange;

	JIndex i=0;
	while (skip(text()[i]))
		{
		i++;
		}
	assert( i < (JSize) size() );

	itsPPNameRange.charRange.first += JString::CountCharacters(text(), i);
	itsPPNameRange.byteRange.first += i;
}
