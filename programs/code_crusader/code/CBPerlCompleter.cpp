/******************************************************************************
 CBPerlCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "CBPerlCompleter.h"
#include <jAssert.h>

CBPerlCompleter* CBPerlCompleter::itsSelf = nullptr;

static const JCharacter* kKeywordList[] =
{
	"continue", "do", "else", "elsif", "for", "foreach", "goto", "if", "last",
	"next", "redo", "unless", "until", "while",

	"abs", "accept", "alarm", "atan2", "bind", "binmode", "bless", "caller",
	"chdir", "chmod", "chomp", "chop", "chown", "chr", "chroot", "close",
	"closedir", "connect", "cos", "crypt", "dbmclose", "dbmopen", "defined",
	"delete", "die", "dump", "each", "eof", "eval", "exec", "exists", "exit",
	"exp", "fcntl", "fileno", "flock", "fork", "format", "formline", "getc",
	"getlogin", "getpeername", "getpgrp", "getppid", "getpriority", "getpwnam",
	"getgrnam", "gethostbyname", "getnetbyname", "getprotobyname", "getpwuid",
	"getgrgid", "getservbyname", "gethostbyaddr", "getnetbyaddr",
	"getprotobynumber", "getservbyport", "getpwent", "getgrent", "gethostent",
	"getnetent", "getprotoent", "getservent", "setpwent", "setgrent",
	"sethostent", "setnetent", "setprotoent", "setservent", "endpwent", "endgrent",
	"endhostent", "endnetent", "endprotoent", "endservent", "getsockname",
	"getsockopt", "glob", "gmtime", "grep", "hex", "import", "index", "int",
	"ioctl", "join", "keys", "kill", "lc", "lcfirst", "length", "link", "listen",
	"local", "localtime", "lock", "log", "lstat", "map", "mkdir", "msgctl",
	"msgget", "msgrcv", "msgsnd", "my", "no", "oct", "open", "opendir", "ord",
	"our", "pack", "package", "pipe", "pop", "pos", "print", "printf",
	"prototype", "push", "quotemeta", "rand", "read", "readdir", "readline",
	"readlink", "readpipe", "recv", "ref", "rename", "require", "reset",
	"return", "reverse", "rewinddir", "rindex", "rmdir", "scalar", "seek",
	"seekdir", "select", "semctl", "semget", "semop", "send", "setpgrp",
	"setpriority", "setsockopt", "shift", "shmctl", "shmget", "shmread",
	"shmwrite", "shutdown", "sin", "sleep", "socket", "socketpair", "sort",
	"splice", "split", "sprintf", "sqrt", "srand", "stat", "study", "sub",
	"substr", "symlink", "syscall", "sysopen", "sysread", "sysseek", "system",
	"syswrite", "tell", "telldir", "tie", "tied", "time", "times", "truncate",
	"uc", "ucfirst", "umask", "undef", "unlink", "unpack", "untie", "unshift",
	"use", "utime", "values", "vec", "wait", "waitpid", "wantarray", "warn",
	"write"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBPerlCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBPerlCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBPerlCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPerlCompleter::CBPerlCompleter()
	:
	CBStringCompleter(kCBPerlLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPerlCompleter::~CBPerlCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBPerlCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_' || (includeNS && c == ':'));
}
