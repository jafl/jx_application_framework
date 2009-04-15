/******************************************************************************
 test_JString.cc

	Program to test JString class.

	Written by John Lindal.

 ******************************************************************************/

#include <JString.h>
#include <jFileUtil.h>
#include <jTime.h>
#include <jMath.h>
#include <jCommandLine.h>
#include <jFStreamUtil.h>
#include <limits.h>

void TestCopyMaxN();
void TestLeak();

int main
	(
	int argc,
	char** argv
	)
{
	JIndex charIndex;

JString s1, s2 = "Hello World", s3 = s2;						// constructors

	cout << "s3 should equal s2" << endl;
	if (s3 == s2) cout << "s3 equals s2" << endl;				// comparison

	s1 = s2;													// assignment
	s1 = "Again!";

	s3 += " ";													// concatenation
	s3 += s1;

	cout << "s3 should not equal s2" << endl;
	if (s3 != s2) cout << "s3 no longer equals s2" << endl;		// anti-comparison

	s3.SetCharacter(s3.GetLength(), '?');

	cout << "s1 should be \"Again!\"" << endl;
	cout << "s1=" << s1 << endl;
	cout << "s2 should be \"Hello World\"" << endl;
	cout << "s2=" << s2 << endl;
	cout << "s3 should be \"Hello World Again?\"" << endl;
	cout << "s3=" << s3 << endl;

	JWaitForReturn();

JString s4("hi there!", 2);

	cout << "s4= " << s4 << endl;
	cout << "s4[0]= " << s4.GetCharacter(1) << endl;

	s4 = "Oh! Not " + s3.GetSubstring(1,11) + " " + s1 + "!";	// addition
	s4.SetCharacter(s4.GetLength() - 6, 'a');
	s4.InsertSubstring(", no", 3);
	s4.Append(" \t\n  ");
	s4.TrimWhitespace();

	cout << "s4 should be \"Oh, no! Not Hello World again!!\"" << endl;
	cout << "s4=" << s4 << endl;

	cout << "inside s4, s2 should start at position 13" << endl;
	if (s4.LocateSubstring(s2, &charIndex))
		{
		cout << "inside s4, s2 starts at position " << charIndex << endl;
		}
	else
		{
		cout << "s2 not found inside s4" << endl;
		}

	cout << "inside s4, the last ! should be at position 31" << endl;
	if (s4.LocateLastSubstring("!", &charIndex))
		{
		cout << "inside s4, the last ! is at position " << charIndex << endl;
		}
	else
		{
		cout << "! not found inside s4" << endl;
		}

	s4.Clear();

	cout << "s4 should be \"\"" << endl;
	cout << "s4=" << s4 << endl;
	cout << "(length = " << s4.GetLength() << ')' << endl;

	cout << "inside s4, s2 should not be found" << endl;
	if (s4.LocateSubstring(s2, &charIndex))
		{
		cout << "inside s4, s2 starts at position " << charIndex << endl;
		}
	else
		{
		cout << "s2 not found inside s4" << endl;
		}
	JWaitForReturn();

	cout << "Enter a string: ";									// stream extraction
	cin >> s2;
	JInputFinished();
	cout << "New s2: " << s2 << endl;
	cout << "(length = " << s2.GetLength() << ')' << endl;

	const JCharacter* kTestFile = "test_JString.dat";
	if (JFileExists(kTestFile))
		{
		ifstream testFile(kTestFile);
		long testCount;
		testFile >> testCount;
		for (long i=1; i<=testCount; i++)
			{
			testFile >> s2;
			cout << "test string: " << s2 << endl;
			}
		}
	else
		{
		cerr << endl;
		cerr << "Unable to find \"" << kTestFile << '"' << endl;
		}

	JWaitForReturn();

JString& s5 = s2;

	cout << "s5 = s2& = " << s5 << endl;

JString* sptr = new JString("This string is on the stack!");	// create on stack

	cout << *sptr << endl;		// you can inspect sptr with Inspector!

	delete sptr;
	sptr = NULL;

	if (!sptr) cout << "It -was- on the stack" << endl;			// operator!

	cout << endl;
	cout << "Testing replace:" << endl;
	s1 = "abc";
	s1.ReplaceSubstring(1,1, "xyz");
	cout << "xyzbc ?= " << s1 << endl;
	s1 = "abc";
	s1.ReplaceSubstring(2,2, "xyz");
	cout << "axyzc ?= " << s1 << endl;
	s1 = "abc";
	s1.ReplaceSubstring(3,3, "xyz");
	cout << "abxyz ?= " << s1 << endl;
	s1 = "abc";
	s1.ReplaceSubstring(1,3, "xyz");
	cout << "xyz ?= " << s1 << endl;
	s1 = "abc";
	s1.RemoveSubstring(1,2);
	cout << "c ?= " << s1 << endl;
	s1 = "abc";
	s1.RemoveSubstring(2,3);
	cout << "a ?= " << s1 << endl;

	JWaitForReturn();

	TestCopyMaxN();
	JWaitForReturn();

JString s6 = "5.235e1";

	double x;

	cout << s6 << " should convert to a float" << endl;
	cout << "Did it work: " << s6.ConvertToFloat(&x) << endl;
	cout << "Result: " << x << endl;

	s6 = "0xFF";
	cout << s6 << " should convert to a float" << endl;
	cout << "Did it work: " << s6.ConvertToFloat(&x) << endl;
	cout << "Result: " << x << endl;

	s6 = " \t 32.1 \t";
	cout << s6 << " should convert to a float" << endl;
	cout << "Did it work: " << s6.ConvertToFloat(&x) << endl;
	cout << "Result: " << x << endl;

	s6 = "5ea";
	cout << s6 << " should not convert to a float" << endl;
	cout << "Did it work: " << s6.ConvertToFloat(&x) << endl;
	cout << "Result: " << x << endl;

	s6 = "5 a";
	cout << s6 << " should not convert to a float" << endl;
	cout << "Did it work: " << s6.ConvertToFloat(&x) << endl;
	cout << "Result: " << x << endl;

	s6 = "5.3-3";
	cout << s6 << " should not convert to a float" << endl;
	cout << "Did it work: " << s6.ConvertToFloat(&x) << endl;
	cout << "Result: " << x << endl;

	s6 = "hi!";
	cout << s6 << " should not convert to a float" << endl;
	cout << "Did it work: " << s6.ConvertToFloat(&x) << endl;
	cout << "Result: " << x << endl;

	JWaitForReturn();

	JInteger y;

	s6 = "357\t";
	cout << s6 << " should convert to an integer" << endl;
	cout << "Did it work: " << s6.ConvertToInteger(&y) << endl;
	cout << "Result: " << y << endl;

	s6 = " -18";
	cout << s6 << " should convert to an integer" << endl;
	cout << "Did it work: " << s6.ConvertToInteger(&y) << endl;
	cout << "Result: " << y << endl;

	s6 = " 0xFF";
	cout << s6 << " should convert to an integer" << endl;
	cout << "Did it work: " << s6.ConvertToInteger(&y) << endl;
	cout << "Result: " << y << endl;

	s6 = "11111110";
	cout << s6 << " base 2 should convert to an integer" << endl;
	cout << "Did it work: " << s6.ConvertToInteger(&y,2) << endl;
	cout << "Result: " << y << endl;

	s6 = " 0x ";
	cout << s6 << " should not convert to an integer" << endl;
	cout << "Did it work: " << s6.ConvertToInteger(&y) << endl;
	cout << "Result: " << y << endl;

	s6 = "3.57e3";
	cout << s6 << " should not convert to an integer" << endl;
	cout << "Did it work: " << s6.ConvertToInteger(&y) << endl;
	cout << "Result: " << y << endl;

	s6 = "hi!";
	cout << s6 << " should not convert to an integer" << endl;
	cout << "Did it work: " << s6.ConvertToInteger(&y) << endl;
	cout << "Result: " << y << endl;

	JWaitForReturn();

	JUInt z;

	s6 = "357\t";
	cout << s6 << " should convert to an unsigned integer" << endl;
	cout << "Did it work: " << s6.ConvertToUInt(&z) << endl;
	cout << "Result: " << z << endl;

	s6 = " 0xFF";
	cout << s6 << " should convert to an unsigned integer" << endl;
	cout << "Did it work: " << s6.ConvertToUInt(&z) << endl;
	cout << "Result: " << z << endl;

	s6 = " 0x ";
	cout << s6 << " should not convert to an unsigned integer" << endl;
	cout << "Did it work: " << s6.ConvertToUInt(&z) << endl;
	cout << "Result: " << z << endl;

	s6 = " -18";
	cout << s6 << " should not convert to an unsigned integer" << endl;
	cout << "Did it work: " << s6.ConvertToUInt(&z) << endl;
	cout << "Result: " << z << endl;

	s6 = "3.57e3";
	cout << s6 << " should not convert to an unsigned integer" << endl;
	cout << "Did it work: " << s6.ConvertToUInt(&z) << endl;
	cout << "Result: " << z << endl;

	s6 = "hi!";
	cout << s6 << " should not convert to an unsigned integer" << endl;
	cout << "Did it work: " << s6.ConvertToUInt(&z) << endl;
	cout << "Result: " << z << endl;

	JWaitForReturn();

JString s7(LONG_MIN, 0, JString::kForceNoExponent);
JString s8(ULONG_MAX, 0, JString::kForceNoExponent);
JString s15(ULONG_MAX);
JString s9  = 137;
JString s14 = 137.5;
JString s16(10, 0, JString::kForceNoExponent);
JString s17(100, 0, JString::kForceNoExponent);
JString s18 = 0.5;
JString s21 = 1.0e-105;
JString s27 = 0.6;

const double dp1 = 4.0 * atan(1.0);
const double dp2 = sqrt(2);

	cout << dp1 << endl;
	cout << dp2 << endl;
	cout << LONG_MIN  << " = " << s7  << endl;
	cout << ULONG_MAX << " = " << s8  << endl;
	cout << ULONG_MAX << " = " << s15 << endl;
	cout << 137       << " = " << s9  << endl;
	cout << 137.5     << " = " << s14 << endl;
	cout << 10        << " = " << s16 << endl;
	cout << 100       << " = " << s17 << endl;
	cout << 0.5       << " = " << s18 << endl;
	cout << 0.6       << " = " << s27 << endl;
	cout << 1.0e-105  << " = " << s21 << endl;

JString s19(1, JString::kPrecisionAsNeeded, JString::kUseGivenExponent, -3);
JString s20(1, JString::kPrecisionAsNeeded, JString::kUseGivenExponent, +2);

	cout << "1000e-3 = " << s19 << endl;
	cout << "0.01e+2 = " << s20 << endl;

JString s10(537.6, 2, JString::kForceExponent);

	cout << "5.376e2 to 2 places = " << s10 << endl;

JString s11(5.373, 2);
	cout << "5.373 to 2 places = " << s11 << endl;
JString s12(5.374, 2);
	cout << "5.374 to 2 places = " << s12 << endl;
JString s13(5.375, 2);
	cout << "5.375 to 2 places = " << s13 << endl;
JString s22(5.376, 2);
	cout << "5.376 to 2 places = " << s22 << endl;

JString s23(5.373, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	cout << "5.373 to 3 digits = " << s23 << endl;
JString s24(5.374, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	cout << "5.374 to 3 digits = " << s24 << endl;
JString s25(5.375, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	cout << "5.375 to 3 digits = " << s25 << endl;
JString s26(5.376, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	cout << "5.376 to 3 digits = " << s26 << endl;

JString s28(9.995, 2);
	cout << "9.995 to 2 places = " << s28 << endl;

	cout << endl << "Ready to start memory leak test";
	cout << endl << "Record memory usage now";
	JWaitForReturn();

	for (JIndex i=1; i<=100000; i++)
		{
		TestLeak();
		}

	cout << endl << "Memory leak test finished";
	cout << endl << "Compare memory usage with recorded value";
	JWaitForReturn();

	cout << endl;
	cout << "System clock started on " << JConvertToTimeStamp(0) << endl;
	cout << "System clock ends on " << JConvertToTimeStamp(J_TIME_T_MAX) << endl;
	cout << "Test concluded on " << JGetTimeStamp() << endl;

	return 0;
}

void
TestCopyMaxN()
{
	const JIndex kStringLength = 10;
	JCharacter string [ kStringLength ];

	const JCharacter* stringList[] = {"", "1", "12345678", "123456789",
									  "1234567890", "123456789abcdef0"};
	const JSize kTestMax = sizeof(stringList) / sizeof(JCharacter*);

	cout << endl;

	for (JIndex testnum=0; testnum<kTestMax; testnum++)
		{
		cout << "Trying to copy string \"" << stringList[testnum];
		cout << "\" of length " << strlen(stringList[testnum]) << " into ";
		cout << kStringLength << " byte string " << endl;

		const JBoolean allCopied =
			JCopyMaxN(stringList[testnum], kStringLength, string);

		if (JNegate(strcmp(string, stringList[testnum])) == allCopied &&
			JCompareMaxN(string, stringList[testnum], kStringLength-1, kJTrue))
			{
			cout << "correct";
			}
		else
			{
			cout << "WRONG";
			}

		cout << ", copy is      \"" << string << "\" with length ";
		cout << strlen(string) << endl << endl;
		}
}

const JString gStr1 = "JTextEditor::Font";
const JString gStr2 = "JXTEBase";

JString GetString1();
JString GetString2();

void
TestLeak()
{
//	JString s = "This is a test string";

	const JString n1 = GetString1();
	const JString n2 = GetString2();
	const int r = JStringCompare(n1, n2, kJFalse);
}

JString
GetString1()
{
	JString stem = gStr1;

	JIndex colonIndex;
	if (stem.LocateLastSubstring("::", &colonIndex))
		{
		stem.RemoveSubstring(1, colonIndex+1);
		}

	return stem;
}

JString
GetString2()
{
	JString stem = gStr2;

	JIndex colonIndex;
	if (stem.LocateLastSubstring("::", &colonIndex))
		{
		stem.RemoveSubstring(1, colonIndex+1);
		}

	return stem;
}
