/******************************************************************************
 CBLuaCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBLuaCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBLuaCompleter* CBLuaCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	// keywords

	"and", "break", "do", "else", "elseif", "end", "for", "function", "if",
	"in", "local", "nil", "not", "or", "repeat", "return", "then", "until",
	"while",

	// functions

	"_ALERT", "assert", "call", "collectgarbage", "copytagmethods", "dofile",
	"dostring", "error", "foreach", "foreachi", "getglobal", "getn",
	"gettagmethod", "globals", "newtag", "next", "print", "rawget", "setglobal",
	"settag", "settagmethod", "sort", "tag", "tonumber", "tostring", "tinsert",
	"tremove", "type", "strbyte", "strchar", "strfind", "strlen", "strlower",
	"strrep", "strsub", "strupper", "format", "gsub", "abs", "acos", "asin",
	"atan", "atan2", "ceil", "cos", "deg", "exp", "floor", "log", "log10",
	"max", "min", "mod", "rad", "sin", "sqrt", "tan", "frexp", "ldexp",
	"random", "randomseed", "openfile", "closefile", "readfrom", "writeto",
	"appendto", "remove", "rename", "flush", "seek", "set", "cur", "end",
	"tmpname", "read", "write", "clock", "date", "execute", "exit", "getenv",
	"setlocale",

	// debugging

	"getinfo", "getlocal", "setlocal", "setcallhook", "setlinehook"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBLuaCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBLuaCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBLuaCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBLuaCompleter::CBLuaCompleter()
	:
	CBStringCompleter(kCBLuaLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBLuaCompleter::~CBLuaCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

JBoolean
CBLuaCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_');
}
