/******************************************************************************
 CBRubyCompleter.cpp

	Reference:  http://www.ruby-lang.org/

	BASE CLASS = CBStringCompleter

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CBRubyCompleter.h"
#include <jAssert.h>

CBRubyCompleter* CBRubyCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	"BEGIN", "END",
	"alias", "and",
	"begin", "break",
	"case", "class",
	"def", "defined", "do",
	"else", "elsif", "end", "ensure",
	"false", "for",
	"if", "in",
	"module",
	"next", "nil", "not",
	"or",
	"redo", "rescue", "retry", "return",
	"self", "super",
	"then", "true",
	"undef", "unless", "until",
	"when", "while",
	"yield",

	"Array", "Float", "Integer", "String",
	"at_exit", "autoload",
	"binding",
	"caller", "catch", "chop", "chomp",
	"eval", "exec", "exit",
	"fail", "fork", "format",
	"gets", "global_variables", "gsub",
	"iterator",
	"lambda", "load", "local_variables", "loop",
	"open",
	"p", "print", "printf", "proc", "putc", "puts",
	"raise", "rand", "readline", "readlines", "require",
	"select", "sleep", "split", "sprintf", "srand", "sub", "syscall", "system",
	"test", "trace_var", "trap",
	"untrace_var"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBRubyCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBRubyCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBRubyCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBRubyCompleter::CBRubyCompleter()
	:
	CBStringCompleter(kCBRubyLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBRubyCompleter::~CBRubyCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (. and ::) to allow completion of
	fully qualified names.  We also include $ and @ since these specify
	variable types.

 ******************************************************************************/

JBoolean
CBRubyCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_' || c == '$' || c == '@' ||
				(includeNS && (c == '.' || c == ':')));
}
