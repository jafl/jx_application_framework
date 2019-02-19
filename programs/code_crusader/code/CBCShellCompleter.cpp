/******************************************************************************
 CBCShellCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2000 by John Lindal.

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

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBCShellCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBCShellCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = kJFalse;
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
	CBStringCompleter(kCBCShellLang, kKeywordCount, kKeywordList, kJTrue)
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

JBoolean
CBCShellCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const JBoolean			includeNS
	)
	const
{
	return JI2B(c.IsAlnum() || c == '_');
}
