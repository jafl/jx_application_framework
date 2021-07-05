/******************************************************************************
 CBBourneShellCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "CBBourneShellCompleter.h"
#include <jAssert.h>

CBBourneShellCompleter* CBBourneShellCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
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

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBBourneShellCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBBourneShellCompleter;
		assert( itsSelf != nullptr );

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
	jdelete itsSelf;
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
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

JBoolean
CBBourneShellCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const JBoolean			includeNS
	)
	const
{
	return JI2B(c.IsAlnum() || c == '_');
}
