/******************************************************************************
 test_jProcessUtil.cpp

	Program to test process utilities.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <jProcessUtil.h>
#include <JMinMax.h>
#include <stdarg.h>
#include <jAssert.h>

extern void JCleanArg(JString* arg);

int main()
{
	return JTestManager::Execute();
}

void
Test
	(
	const JUtf8Byte*	origCmd,
	const JSize			argc,
	...
	)
{
	JString cmd;
	cmd = origCmd;

	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	JParseArgsForExec(cmd, &argList);

	JSize count = argList.GetElementCount();
	JAssertEqualWithMessage(argc, count, origCmd);
	count = JMin(argc, count);

	va_list ap;
	va_start(ap, argc);

	for (JIndex i=1; i<=count; i++)
		{
		const JUtf8Byte* expected = va_arg(ap, const JUtf8Byte*);
		const JString* arg        = argList.GetElement(i);
		JAssertStringsEqualWithMessage(expected, *arg,
			(JString(origCmd, kJFalse) + ", " + JString((JUInt64) i)).GetBytes());
		}

	va_end(ap);
}

JTEST(ProgramAvailable)
{
	JAssertTrue(JProgramAvailable(JString("ls", kJFalse)));
	JAssertFalse(JProgramAvailable(JString("foo_bar_baz_shug", kJFalse)));
}

JTEST(CleanArg)
{
	JString s;

	s = "'abc'";
	JCleanArg(&s);
	JAssertStringsEqual("abc", s);

	s = "\\'abc\\'";
	JCleanArg(&s);
	JAssertStringsEqual("'abc'", s);

	s = "\\\"abc\\\"";
	JCleanArg(&s);
	JAssertStringsEqual("\"abc\"", s);

	s = "alice:'*.c'";
	JCleanArg(&s);
	JAssertStringsEqual("alice:*.c", s);

	s = "\"alice:'*.c'\"";
	JCleanArg(&s);
	JAssertStringsEqual("alice:'*.c'", s);

	s = "a\\;";
	JCleanArg(&s);
	JAssertStringsEqual("a;", s);
}

JTEST(PrepArg)
{
	JString s;

	s = "'abc'";
	JAssertStringsEqual("\\'abc\\'", JPrepArgForExec(s));

	s = "abc xyz";
	JAssertStringsEqual("\"abc xyz\"", JPrepArgForExec(s));

	s = "foo;bar";
	JAssertStringsEqual("foo\\;bar", JPrepArgForExec(s));
}

JTEST(ParseArgs)
{
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
}
