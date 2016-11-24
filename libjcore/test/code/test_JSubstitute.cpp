/******************************************************************************
 test_JSubstitute.cc

	A test program for JSubstitute.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <JSubstitute.h>
#include <JString.h>
#include <jassert_simple.h>

int main()
{
	cout << "Current locale: " << setlocale(LC_ALL, "") << endl;
	return JUnitTestManager::Execute();
}

JTEST(Test1)
{
	JString oldString, newString;

	JSubstitute esc;
	esc.IgnoreUnrecognized();

	// A string with an unrecognized escape
	oldString = "\\i\\iabc\\i\\idef\\i\\i";
	newString = oldString;

	// Test "do nothing" default
	esc(&newString);
	JAssertStringsEqual(oldString, newString);


	// Test ignore unrecognized
	newString = oldString;
	esc.IgnoreUnrecognized(kJFalse);
	esc(&newString);
	JAssertStringsEqual("iiabciidefii", newString);


	// Test a basic table entry
	newString = oldString;
	JString replaceString;
	replaceString.Set("<>");
	esc.SetEscape('i', replaceString.GetBytes());
	esc(&newString);
	JAssertStringsEqual("<><>abc<><>def<><>", newString);


	// Entries should be case-sensitive
	oldString = "\\I\\Iabc\\I\\Idef\\I\\I";
	newString = oldString;
	replaceString = "<wrong>";
	esc.IgnoreUnrecognized();
	esc(&newString);
	JAssertStringsEqual(oldString, newString);


	// Test end-of-string behavior
	oldString = "abcdef\\";
	newString = oldString;
	esc.IgnoreUnrecognized();
	esc(&newString);
	JAssertStringsEqual("abcdef", newString);

	newString = oldString;
	esc.IgnoreUnrecognized(kJFalse);
	esc(&newString);
	JAssertStringsEqual("abcdef", newString);


	// Test clearing an entry
	oldString = "abc\\idef";
	newString = oldString;
	esc.ClearEscape('i');
	esc.IgnoreUnrecognized();
	esc(&newString);
	JAssertStringsEqual(oldString, newString);
}

JTEST(Test2)
{
	JString oldString, newString;

	JSubstitute esc;
	esc.IgnoreUnrecognized();

	// A string with a control-character escape
	oldString = "\\cG\\cGabc\\cG\\cGdef\\cG\\cG";
	newString = oldString;

	// Test a control substitution
	esc.UseControlEscapes();
	esc(&newString);
	JAssertStringsEqual("\a\aabc\a\adef\a\a", newString);


	// But this should fail
	oldString = "\\CG\\CGabc\\CG\\CGdef\\CG\\CG";
	newString = oldString;
	esc(&newString);
	JAssertStringsEqual(oldString, newString);


	// So should this
	oldString = "\\cg\\cgabc\\cg\\cgdef\\cg\\cg";
	newString = oldString;
	esc(&newString);
	JAssertStringsEqual(oldString, newString);


	// Check the allowed range

	// This one should be disallowed
	oldString = "abc\\c@def";
	newString = oldString;
	esc(&newString);
	JAssertStringsEqual(oldString, newString);


	// This one should work
	oldString = "\\cA\\cAabc\\cA\\cAdef\\cA\\cA";
	newString = oldString;
	esc(&newString);
	JAssertStringsEqual("\01\01abc\01\01def\01\01", newString);


	// So should this one
	oldString = "abc\\c_def";
	newString = oldString;
	esc(&newString);
	JAssertStringsEqual("abc\037def", newString);


	// But this should fail
	oldString = "abc\\c@`def";
	newString = oldString;
	esc(&newString);
	JAssertStringsEqual(oldString, newString);


	// Test end-of-string behavior
	oldString = "abcdef\\c";
	newString = oldString;
	esc.IgnoreUnrecognized();
	esc(&newString);
	JAssertStringsEqual("abcdef", newString);

	newString = oldString;
	esc.IgnoreUnrecognized(kJFalse);
	esc(&newString);
	JAssertStringsEqual("abcdef", newString);

	oldString = "abcdef\\cM";
	newString = oldString;
	esc.IgnoreUnrecognized();
	esc(&newString);
	JAssertStringsEqual("abcdef\r", newString);

	newString = oldString;
	esc.IgnoreUnrecognized(kJFalse);
	esc(&newString);
	JAssertStringsEqual("abcdef\r", newString);


	// Check ClearAllOrdinary
	oldString = "abc\\cKdef";
	newString = oldString;
	esc.ClearAllEscapes();
	esc.IgnoreUnrecognized();
	esc(&newString);
	JAssertStringsEqual("abc\vdef", newString);


	// Check reset
	oldString = "abc\\cGdef";
	newString = oldString;
	esc.Reset();
	esc.IgnoreUnrecognized();
	esc(&newString);
	JAssertStringsEqual(oldString, newString);
}
