/******************************************************************************
 CBREXXCompleter.cpp

	Reference:  http://www.rexswain.com/rexx.html

	BASE CLASS = CBStringCompleter

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBREXXCompleter.h"
#include <jAssert.h>

CBREXXCompleter* CBREXXCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"ABBREV", "ABS", "ADDRESS", "ARG",
	"BITAND", "BITOR", "BITXOR", "B2X", 
	"CALL", "CENTER", "CENTRE", "CHARIN", "CHAROUT", "CHARS", "CMSFLAG", "COMPARE",
	"CONDITION", "COPIES", "CSL", "C2D", "C2X",
	"DATATYPE", "DATE", "DELSTR", "DELWORD", "DIAG", "DIAGRC", "DIGITS", "DO", "DROP",
	"D2C", "D2X",
	"ERRORTEXT", "EXIT", "EXTERNALS",
	"FIND", "FORM", "FORMAT", "FUZZ",
	"IF", "INDEX", "INSERT", "INTERPRET", "ITERATE",
	"LASTPOS", "LEAVE", "LEFT", "LENGTH", "LINEIN", "LINEOUT", "LINES", "LINESIZE",
	"MAX", "MIN",
	"NOP", "NUMERIC",
	"OPTIONS", "OVERLAY",
	"PARSE", "POS", "PROCEDURE", "PULL", "PUSH",
	"QUEUE", "QUEUED",
	"RANDOM", "RETURN", "REVERSE", "RIGHT",
	"SAY", "SELECT", "SIGN", "SIGNAL", "SOURCE", "SOURCELINE", "SPACE", "STORAGE",
	"STREAM", "STRIP", "SUBSTR", "SUBWORD", "SYMBOL",
	"TIME", "TRACE", "TRANSLATE", "TRUNC",
	"UPPER", "USERID",
	"VALUE", "VERIFY",
	"WORD", "WORDINDEX", "WORDLENGTH", "WORDPOS",

	"RC", "RESULT", "SIGL"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBREXXCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBREXXCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBREXXCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBREXXCompleter::CBREXXCompleter()
	:
	CBStringCompleter(kCBREXXLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBREXXCompleter::~CBREXXCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
CBREXXCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '@' || c == '#' || c == '$' ||
				c == '\\' || c == '.' || c == '!' || c == '?' || c == '_';
}
