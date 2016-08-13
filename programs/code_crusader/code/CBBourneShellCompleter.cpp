/******************************************************************************
 CBBourneShellCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBBourneShellCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBBourneShellCompleter* CBBourneShellCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	"case", "do", "done", "elif", "else", "esac", "fi", "for", "function",
	"if", "in", "select", "then", "until", "while",

	"true", "false", "source", "alias", "bg", "bind", "break", "builtin",
	"cd", "command", "continue", "declare", "typeset", "dirs", "echo",
	"enable", "eval", "exec", "exit", "export", "fc", "fg", "getopts",
	"hash", "help", "history", "jobs", "kill", "let", "local", "logout",
	"popd", "pushd", "pwd", "read", "readonly", "return", "set", "shift",
	"suspend", "test", "times", "trap", "type", "ulimit", "umask",
	"unalias", "unset", "wait"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBBourneShellCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBBourneShellCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBBourneShellCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBBourneShellCompleter::CBBourneShellCompleter()
	:
	CBStringCompleter(kCBBourneShellLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBBourneShellCompleter::~CBBourneShellCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

JBoolean
CBBourneShellCompleter::IsWordCharacter
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
