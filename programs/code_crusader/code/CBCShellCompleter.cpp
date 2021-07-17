/******************************************************************************
 CBCShellCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "CBCShellCompleter.h"
#include <jAssert.h>

CBCShellCompleter* CBCShellCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"break", "breaksw", "case", "continue", "default", "else", "end",
	"endif", "endsw", "foreach", "if", "then", "repeat", "switch", "while",

	"true", "false", "alias", "alloc", "bg", "bindkey", "builtins", "bye",
	"cd", "chdir", "complete", "dirs", "echo", "echotc", "eval", "exec",
	"exit", "fg", "filetest", "getspath", "getxvers", "glob", "goto",
	"hashstat", "history", "hup", "inlib", "jobs", "kill", "limit", "log",
	"login", "logout", "migrate", "newgrp", "nice", "nohup", "notify",
	"onintr", "popd", "printenv", "pushd", "rehash", "rootnode", "sched",
	"set", "setenv", "setpath", "setspath", "settc", "setty", "setxvers",
	"shift", "source", "stop", "suspend", "telltc", "time", "umask", "unalias",
	"uncomplete", "unhash", "universe", "unlimit", "unset", "unsetenv", "ver",
	"wait", "warp", "watchlog", "where", "which"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBCShellCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBCShellCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBCShellCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBCShellCompleter::CBCShellCompleter()
	:
	CBStringCompleter(kCBCShellLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCShellCompleter::~CBCShellCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
CBCShellCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_';
}
