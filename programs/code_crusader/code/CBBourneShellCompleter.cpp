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

static bool recursiveInstance = false;

CBStringCompleter*
CBBourneShellCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBBourneShellCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
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
	CBStringCompleter(kCBBourneShellLang, kKeywordCount, kKeywordList, JString::kCompareCase)
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

bool
CBBourneShellCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_';
}
