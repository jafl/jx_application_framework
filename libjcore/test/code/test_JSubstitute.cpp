/******************************************************************************
 test_JSubstitute.cpp

	A test program for JSubstitute.

	Copyright (C) 1997 by Dustin Laurence.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

int main()
{
	std::cout << "Current locale: " << setlocale(LC_ALL, "") << std::endl;
	return JTestManager::Execute();
}

JTEST(Escape1)
{
	JString oldString, newString;

	JSubstitute sub;
	sub.IgnoreUnrecognized();

	// A string with an unrecognized escape
	oldString = "\\i\\iabc\\i\\idef\\i\\i";
	newString = oldString;

	// Test "do nothing" default
	sub.Substitute(&newString);
	JAssertStringsEqual(oldString, newString);


	// Test ignore unrecognized
	newString = oldString;
	sub.IgnoreUnrecognized(false);
	sub.Substitute(&newString);
	JAssertStringsEqual("iiabciidefii", newString);


	// Test a basic table entry
	newString = oldString;
	sub.SetEscape('i', "<>");
	sub.Substitute(&newString);
	JAssertStringsEqual("<><>abc<><>def<><>", newString);


	// Entries should be case-sensitive
	oldString = "\\I\\Iabc\\I\\Idef\\I\\I";
	newString = oldString;
	sub.IgnoreUnrecognized();
	sub.Substitute(&newString);
	JAssertStringsEqual(oldString, newString);


	// Test end-of-string behavior
	oldString = "abcdef\\";
	newString = oldString;
	sub.IgnoreUnrecognized();
	sub.Substitute(&newString);
	JAssertStringsEqual("abcdef", newString);

	newString = oldString;
	sub.IgnoreUnrecognized(false);
	sub.Substitute(&newString);
	JAssertStringsEqual("abcdef", newString);


	// Test clearing an entry
	oldString = "abc\\idef";
	newString = oldString;
	sub.ClearEscape('i');
	sub.IgnoreUnrecognized();
	sub.Substitute(&newString);
	JAssertStringsEqual(oldString, newString);
}

JTEST(Escape2)
{
	JString oldString, newString;

	JSubstitute sub;
	sub.IgnoreUnrecognized();

	// A string with a control-character escape
	oldString = "\\cG\\cGabc\\cG\\cGdef\\cG\\cG";
	newString = oldString;

	// Test a control substitution
	sub.UseControlEscapes();
	sub.Substitute(&newString);
	JAssertStringsEqual("\a\aabc\a\adef\a\a", newString);


	// But this should fail
	oldString = "\\CG\\CGabc\\CG\\CGdef\\CG\\CG";
	newString = oldString;
	sub.Substitute(&newString);
	JAssertStringsEqual(oldString, newString);


	// So should this
	oldString = "\\cg\\cgabc\\cg\\cgdef\\cg\\cg";
	newString = oldString;
	sub.Substitute(&newString);
	JAssertStringsEqual(oldString, newString);


	// Check the allowed range

	// This one should be disallowed
	oldString = "abc\\c@def";
	newString = oldString;
	sub.Substitute(&newString);
	JAssertStringsEqual(oldString, newString);


	// This one should work
	oldString = "\\cA\\cAabc\\cA\\cAdef\\cA\\cA";
	newString = oldString;
	sub.Substitute(&newString);
	JAssertStringsEqual("\01\01abc\01\01def\01\01", newString);


	// So should this one
	oldString = "abc\\c_def";
	newString = oldString;
	sub.Substitute(&newString);
	JAssertStringsEqual("abc\037def", newString);


	// But this should fail
	oldString = "abc\\c@`def";
	newString = oldString;
	sub.Substitute(&newString);
	JAssertStringsEqual(oldString, newString);


	// Test end-of-string behavior
	oldString = "abcdef\\c";
	newString = oldString;
	sub.IgnoreUnrecognized();
	sub.Substitute(&newString);
	JAssertStringsEqual("abcdef", newString);

	newString = oldString;
	sub.IgnoreUnrecognized(false);
	sub.Substitute(&newString);
	JAssertStringsEqual("abcdef", newString);

	oldString = "abcdef\\cM";
	newString = oldString;
	sub.IgnoreUnrecognized();
	sub.Substitute(&newString);
	JAssertStringsEqual("abcdef\r", newString);

	newString = oldString;
	sub.IgnoreUnrecognized(false);
	sub.Substitute(&newString);
	JAssertStringsEqual("abcdef\r", newString);


	// Check ClearAllOrdinary
	oldString = "abc\\cKdef";
	newString = oldString;
	sub.ClearAllEscapes();
	sub.IgnoreUnrecognized();
	sub.Substitute(&newString);
	JAssertStringsEqual("abc\vdef", newString);


	// Check reset
	oldString = "abc\\cGdef";
	newString = oldString;
	sub.Reset();
	sub.IgnoreUnrecognized();
	sub.Substitute(&newString);
	JAssertStringsEqual(oldString, newString);
}

JTEST(Variables)
{
	JString oldString, newString;

	JSubstitute sub;
	sub.DefineVariable("a", JString("x", JString::kNoCopy));
	sub.DefineVariable("b", JString("y", JString::kNoCopy));

	oldString = "a$aab$bb";
	newString = oldString;
	sub.Substitute(&newString);
	JAssertStringsEqual("axabyb", newString);

	sub.UndefineVariable("a");
	sub.DefineVariables("foo|bar");

	oldString = "a$aa$foob$bb$bar";
	newString = oldString;
	sub.Substitute(&newString);
	JAssertStringsEqual("aaafoobybbar", newString);
}

JTEST(ContainsError)
{
	JSubstitute sub;
	sub.UseControlEscapes();
	sub.DefineVariable("a", JString("x", JString::kNoCopy));
	sub.DefineVariable("b", JString("y", JString::kNoCopy));

	JString s;
	JCharacterRange r;
	JError err = JNoError();

	s   = "$";
	err = sub.ContainsError(s, &r);
	JAssertStringsEqual(JSubstitute::kLoneDollar, err.GetType());
	JAssertEqual(JCharacterRange(1,1), r);

	s   = "foo $a $d";
	err = sub.ContainsError(s, &r);
	JAssertStringsEqual(JSubstitute::kLoneDollar, err.GetType());
	JAssertEqual(JCharacterRange(8,8), r);

	s   = "\\";
	err = sub.ContainsError(s, &r);
	JAssertStringsEqual(JSubstitute::kTrailingBackslash, err.GetType());
	JAssertEqual(JCharacterRange(1,1), r);

	s   = "\\c";
	err = sub.ContainsError(s, &r);
	JAssertStringsEqual(JSubstitute::kIllegalControlChar, err.GetType());
	JAssertEqual(JCharacterRange(1,2), r);

	s   = "\\cX \\c$";
	err = sub.ContainsError(s, &r);
	JAssertStringsEqual(JSubstitute::kIllegalControlChar, err.GetType());
	JAssertEqual(JCharacterRange(5,7), r);
}
