/******************************************************************************
 JXComposeScanner.cpp

	Lexer for reading X11 Compose rule files.

	BASE CLASS = public JXComposeFlexLexer

	Copyright © 1999 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JXStdInc.h>
#include <JXComposeScanner.h>
#include <string.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JXComposeScanner::JXComposeScanner()
	:
	JXComposeFlexLexer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXComposeScanner::~JXComposeScanner()
{
}

/******************************************************************************
 CheckIsCharInWord

 *****************************************************************************/

static const JCharacter* kNameSuffix[] =
{
	"abovedot", "acute", "breve", "caron", "cedilla", "circumflex",
	"diaeresis", "dotless", "doubleacute", "grave", "macron",
	"oblique", "ogonek", "ring", "sharp", "slash", "stroke", "tilde"
};

static const JBoolean kIsDiacriticalMark[] =
{
	kJTrue, kJTrue, kJTrue, kJTrue, kJTrue, kJTrue,
	kJTrue, kJTrue, kJTrue, kJTrue, kJTrue,
	kJFalse, kJTrue, kJFalse, kJTrue, kJFalse, kJTrue, kJTrue
};

static const JIndex kDiacriticalMarkType[] =	// the value zero is reserved
{
	1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11,
	12, 13, 14, 15, 12, 17, 18
};

const JSize kSuffixCount = sizeof(kNameSuffix) / sizeof(JCharacter*);

static const JCharacter* kSpecialName[] =
{
	"ae", "oe", "eth", "thorn", "eng",
	"AE", "OE", "ETH", "THORN", "ENG"
};

const JSize kSpecialCount = sizeof(kSpecialName) / sizeof(JCharacter*);

void
JXComposeScanner::CheckIsCharInWord
	(
	const JCharacter*	name,
	const JUInt			c,
	const JSize			charCount,
	JBoolean*			isCharInWord,
	JCharacter*			diacriticalMap,
	JIndex*				diacriticalMarkType
	)
	const
{
JIndex i;

	assert( kSuffixCount == sizeof(kIsDiacriticalMark)/sizeof(JBoolean) );
	assert( kSuffixCount == sizeof(kDiacriticalMarkType)/sizeof(JIndex) );

	for (i=0; i<kSuffixCount; i++)
		{
		if (strcmp(name+1, kNameSuffix[i]) == 0)
			{
			if (c < charCount)
				{
				isCharInWord[c] = kJTrue;

				if (kIsDiacriticalMark[i])
					{
					diacriticalMap[c]      = name[0];
					diacriticalMarkType[c] = kDiacriticalMarkType[i];
//					cout << char(c) << ' ' << diacriticalMap[c] << endl;
					}
				}
			return;
			}
		}

	for (i=0; i<kSpecialCount; i++)
		{
		if (strcmp(name, kSpecialName[i]) == 0)
			{
			if (c < charCount)
				{
				isCharInWord[c] = kJTrue;
				}
			return;
			}
		}
}
