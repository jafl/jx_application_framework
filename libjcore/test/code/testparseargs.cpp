/******************************************************************************
 test_JString.cc

	Program to test JString class.

	Written by John Lindal.

 ******************************************************************************/

#include <jProcessUtil.h>
#include <JString.h>
#include <stdarg.h>

JBoolean
Test
	(
	const JCharacter*	cmd,
	const JSize			argc,
	...
	)
{
	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	JParseArgsForExec(cmd, &argList);

	const JSize count = argList.GetElementCount();
	if (count != argc)
		{
		cerr << "wrong arg count (" << count << ") for" << endl;
		cerr << cmd << endl;
		cerr << "expecting " << argc << endl << endl;
		return kJFalse;
		}

	va_list ap;
	va_start(ap, argc);

	for (JIndex i=1; i<=count; i++)
		{
		const JString* arg = argList.NthElement(i);
		if (*arg != va_arg(ap, const JCharacter*))
			{
			cerr << "wrong arg (" << i << ") for" << endl;
			cerr << cmd << endl;
			for (JIndex i=1; i<=count; i++)
				{
				cerr << *(argList.NthElement(i)) << endl;
				}
			cerr << endl;
			return kJFalse;
			}
		}

	va_end(ap);

	return kJTrue;
}

int
main
	(
	int argc,
	char** argv
	)
{
	cout << "No news is good news!" << endl << endl;

	Test(" ", 0);

	Test("cmd", 1, "cmd");

	Test("ls abc", 2, "ls", "abc");
	Test("ls  abc ", 2, "ls", "abc");

	Test("'abc", 1, "abc");
	Test("'abc xyz", 1, "abc xyz");
	Test("'abc' \\\"abc\\\"", 2, "abc", "\"abc\"");
	Test("'abc' \\\"abc\\", 2, "abc", "\"abc");

	Test("scp alice:'*.c' .", 3, "scp", "alice:*.c", ".");
	Test(" scp \"alice:'*.c'\" .	", 3, "scp", "alice:'*.c'", ".");
	Test("scp 'alice:\\'*.c\\'' .", 3, "scp", "alice:'*.c'", ".");

	Test("'abc' \"xyz\"", 2, "abc", "xyz");

	Test(";", 1, ";");
	Test("x;", 2, "x", ";");
	Test("x;y", 3, "x", ";", "y");
	Test("x; y", 3, "x", ";", "y");
	Test("x ;y", 3, "x", ";", "y");
	Test("x ; y", 3, "x", ";", "y");
	Test("'x ; y'", 1, "x ; y");
	Test("'x'; y", 3, "x", ";", "y");
	Test("'x;' y", 2, "x;", "y");
	Test("x\\;y", 1, "x;y");
	Test("\\;", 1, ";");
	Test("echo a\\; \\; echo b", 5, "echo", "a;", ";", "echo", "b");

	Test(" \"--regex-jhtml=/<a name=([^ >\\\"]+)/\\\\1/s,section/ei\" ",
		 1, "--regex-jhtml=/<a name=([^ >\"]+)/\\1/s,section/ei");
	Test(" \"--regex-jhtml=/<a name=\\\"([^\\\"]+)\\\"/\\\\1/s,section/ei\" ",
		 1, "--regex-jhtml=/<a name=\"([^\"]+)\"/\\1/s,section/ei");
	Test(" \"--regex-jlex=/^<([^<>]+)>\\\\{/\\\\1/s,state/ei\" ",
		 1, "--regex-jlex=/^<([^<>]+)>\\{/\\1/s,state/ei");
	Test(" \"--regex-jbison=/^[ \t]*([[:alpha:]_.][[:alnum:]_.]*)[ \t\n]*:/\\\\1/N,nontermdef/ei\" ",
		 1, "--regex-jbison=/^[ \t]*([[:alpha:]_.][[:alnum:]_.]*)[ \t\n]*:/\\1/N,nontermdef/ei");
	Test(" \"--regex-jbison=/^[ \t]*%type([ \t\n]*<[^>]*>)?[ \t\n]*([[:alpha:]_.][[:alnum:]_.]*)/\\\\2/n,nontermdecl/ei\" ",
		 1, "--regex-jbison=/^[ \t]*%type([ \t\n]*<[^>]*>)?[ \t\n]*([[:alpha:]_.][[:alnum:]_.]*)/\\2/n,nontermdecl/ei");
	Test(" \"--regex-jbison=/^[ \t]*%(token|right|left|nonassoc)([ \t\n]*<[^>]*>)?[ \t\n]*([[:alpha:]_.][[:alnum:]_.]*)/\\\\3/t,termdecl/ei\" ",
		 1, "--regex-jbison=/^[ \t]*%(token|right|left|nonassoc)([ \t\n]*<[^>]*>)?[ \t\n]*([[:alpha:]_.][[:alnum:]_.]*)/\\3/t,termdecl/ei");
	Test(" \"--regex-jmake=/^ *([A-Z0-9_]+)[ \t]*:([^=]|$)/\\\\1/t,target/ei\" ",
		 1, "--regex-jmake=/^ *([A-Z0-9_]+)[ \t]*:([^=]|$)/\\1/t,target/ei");
	Test(" \"--regex-jmake=/^[ \t]*([A-Z0-9_]+)[ \t]*:?=/\\\\1/v,variable/ei\" ",
		 1, "--regex-jmake=/^[ \t]*([A-Z0-9_]+)[ \t]*:?=/\\1/v,variable/ei");

	return 0;
}
