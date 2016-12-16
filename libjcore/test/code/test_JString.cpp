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
#include <typeinfo>

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

	const std::type_info& clazz1 = typeid(s1);
	std::cout << "type of JString instance: " << clazz1.name() << std::endl;

	const std::type_info& clazz2 = typeid(JString);
	std::cout << "type of JString class: " << clazz2.name() << std::endl;

	std::cout << "s3 should equal s2" << std::endl;
	if (s3 == s2) std::cout << "s3 equals s2" << std::endl;				// comparison

	s1 = s2;													// assignment
	s1 = "Again!";

	s3 += " ";													// concatenation
	s3 += s1;

	std::cout << "s3 should not equal s2" << std::endl;
	if (s3 != s2) std::cout << "s3 no longer equals s2" << std::endl;		// anti-comparison

	s3.SetCharacter(s3.GetLength(), '?');

	std::cout << "s1 should be \"Again!\"" << std::endl;
	std::cout << "s1=" << s1 << std::endl;
	std::cout << "s2 should be \"Hello World\"" << std::endl;
	std::cout << "s2=" << s2 << std::endl;
	std::cout << "s3 should be \"Hello World Again?\"" << std::endl;
	std::cout << "s3=" << s3 << std::endl;

	JWaitForReturn();

JString s4("hi there!", 2);

	std::cout << "s4= " << s4 << std::endl;
	std::cout << "s4[0]= " << s4.GetCharacter(1) << std::endl;

	s4 = "Oh! Not " + s3.GetSubstring(1,11) + " " + s1 + "!";	// addition
	s4.SetCharacter(s4.GetLength() - 6, 'a');
	s4.InsertSubstring(", no", 3);
	s4.Append(" \t\n  ");
	s4.TrimWhitespace();

	std::cout << "s4 should be \"Oh, no! Not Hello World again!!\"" << std::endl;
	std::cout << "s4=" << s4 << std::endl;

	std::cout << "inside s4, s2 should start at position 13" << std::endl;
	if (s4.LocateSubstring(s2, &charIndex))
		{
		std::cout << "inside s4, s2 starts at position " << charIndex << std::endl;
		}
	else
		{
		std::cout << "s2 not found inside s4" << std::endl;
		}

	std::cout << "inside s4, the last ! should be at position 31" << std::endl;
	if (s4.LocateLastSubstring("!", &charIndex))
		{
		std::cout << "inside s4, the last ! is at position " << charIndex << std::endl;
		}
	else
		{
		std::cout << "! not found inside s4" << std::endl;
		}

	s4.Clear();

	std::cout << "s4 should be \"\"" << std::endl;
	std::cout << "s4=" << s4 << std::endl;
	std::cout << "(length = " << s4.GetLength() << ')' << std::endl;

	std::cout << "inside s4, s2 should not be found" << std::endl;
	if (s4.LocateSubstring(s2, &charIndex))
		{
		std::cout << "inside s4, s2 starts at position " << charIndex << std::endl;
		}
	else
		{
		std::cout << "s2 not found inside s4" << std::endl;
		}
	JWaitForReturn();

	std::cout << "Enter a string: ";									// stream extraction
	std::cin >> s2;
	JInputFinished();
	std::cout << "New s2: " << s2 << std::endl;
	std::cout << "(length = " << s2.GetLength() << ')' << std::endl;

	const JCharacter* kTestFile = "test_JString.dat";
	if (JFileExists(kTestFile))
		{
		std::ifstream testFile(kTestFile);
		long testCount;
		testFile >> testCount;
		for (long i=1; i<=testCount; i++)
			{
			testFile >> s2;
			std::cout << "test string: " << s2 << std::endl;
			}
		}
	else
		{
		std::cerr << std::endl;
		std::cerr << "Unable to find \"" << kTestFile << '"' << std::endl;
		}

	JWaitForReturn();

JString& s5 = s2;

	std::cout << "s5 = s2& = " << s5 << std::endl;

JString* sptr = jnew JString("This string is on the stack!");	// create on stack

	std::cout << *sptr << std::endl;		// you can inspect sptr with Inspector!

	jdelete sptr;
	sptr = NULL;

	if (!sptr) std::cout << "It -was- on the stack" << std::endl;			// operator!

	std::cout << std::endl;
	std::cout << "Testing replace:" << std::endl;
	s1 = "abc";
	s1.ReplaceSubstring(1,1, "xyz");
	std::cout << "xyzbc ?= " << s1 << std::endl;
	s1 = "abc";
	s1.ReplaceSubstring(2,2, "xyz");
	std::cout << "axyzc ?= " << s1 << std::endl;
	s1 = "abc";
	s1.ReplaceSubstring(3,3, "xyz");
	std::cout << "abxyz ?= " << s1 << std::endl;
	s1 = "abc";
	s1.ReplaceSubstring(1,3, "xyz");
	std::cout << "xyz ?= " << s1 << std::endl;
	s1 = "abc";
	s1.RemoveSubstring(1,2);
	std::cout << "c ?= " << s1 << std::endl;
	s1 = "abc";
	s1.RemoveSubstring(2,3);
	std::cout << "a ?= " << s1 << std::endl;

	JWaitForReturn();

	TestCopyMaxN();
	JWaitForReturn();

JString s6 = "5.235e1";

	double x;

	std::cout << s6 << " should convert to a float" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToFloat(&x) << std::endl;
	std::cout << "Result: " << x << std::endl;

	s6 = "0xFF";
	std::cout << s6 << " should convert to a float" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToFloat(&x) << std::endl;
	std::cout << "Result: " << x << std::endl;

	s6 = " \t 32.1 \t";
	std::cout << s6 << " should convert to a float" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToFloat(&x) << std::endl;
	std::cout << "Result: " << x << std::endl;

	s6 = "5ea";
	std::cout << s6 << " should not convert to a float" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToFloat(&x) << std::endl;
	std::cout << "Result: " << x << std::endl;

	s6 = "5 a";
	std::cout << s6 << " should not convert to a float" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToFloat(&x) << std::endl;
	std::cout << "Result: " << x << std::endl;

	s6 = "5.3-3";
	std::cout << s6 << " should not convert to a float" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToFloat(&x) << std::endl;
	std::cout << "Result: " << x << std::endl;

	s6 = "hi!";
	std::cout << s6 << " should not convert to a float" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToFloat(&x) << std::endl;
	std::cout << "Result: " << x << std::endl;

	JWaitForReturn();

	JInteger y;

	s6 = "357\t";
	std::cout << s6 << " should convert to an integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToInteger(&y) << std::endl;
	std::cout << "Result: " << y << std::endl;

	s6 = " -18";
	std::cout << s6 << " should convert to an integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToInteger(&y) << std::endl;
	std::cout << "Result: " << y << std::endl;

	s6 = " 0xFF";
	std::cout << s6 << " should convert to an integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToInteger(&y) << std::endl;
	std::cout << "Result: " << y << std::endl;

	s6 = "11111110";
	std::cout << s6 << " base 2 should convert to an integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToInteger(&y,2) << std::endl;
	std::cout << "Result: " << y << std::endl;

	s6 = " 0x ";
	std::cout << s6 << " should not convert to an integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToInteger(&y) << std::endl;
	std::cout << "Result: " << y << std::endl;

	s6 = "3.57e3";
	std::cout << s6 << " should not convert to an integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToInteger(&y) << std::endl;
	std::cout << "Result: " << y << std::endl;

	s6 = "hi!";
	std::cout << s6 << " should not convert to an integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToInteger(&y) << std::endl;
	std::cout << "Result: " << y << std::endl;

	JWaitForReturn();

	JUInt z;

	s6 = "357\t";
	std::cout << s6 << " should convert to an unsigned integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToUInt(&z) << std::endl;
	std::cout << "Result: " << z << std::endl;

	s6 = " 0xFF";
	std::cout << s6 << " should convert to an unsigned integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToUInt(&z) << std::endl;
	std::cout << "Result: " << z << std::endl;

	s6 = " 0x ";
	std::cout << s6 << " should not convert to an unsigned integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToUInt(&z) << std::endl;
	std::cout << "Result: " << z << std::endl;

	s6 = " -18";
	std::cout << s6 << " should not convert to an unsigned integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToUInt(&z) << std::endl;
	std::cout << "Result: " << z << std::endl;

	s6 = "3.57e3";
	std::cout << s6 << " should not convert to an unsigned integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToUInt(&z) << std::endl;
	std::cout << "Result: " << z << std::endl;

	s6 = "hi!";
	std::cout << s6 << " should not convert to an unsigned integer" << std::endl;
	std::cout << "Did it work: " << s6.ConvertToUInt(&z) << std::endl;
	std::cout << "Result: " << z << std::endl;

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

	std::cout << dp1 << std::endl;
	std::cout << dp2 << std::endl;
	std::cout << LONG_MIN  << " = " << s7  << std::endl;
	std::cout << ULONG_MAX << " = " << s8  << std::endl;
	std::cout << ULONG_MAX << " = " << s15 << std::endl;
	std::cout << 137       << " = " << s9  << std::endl;
	std::cout << 137.5     << " = " << s14 << std::endl;
	std::cout << 10        << " = " << s16 << std::endl;
	std::cout << 100       << " = " << s17 << std::endl;
	std::cout << 0.5       << " = " << s18 << std::endl;
	std::cout << 0.6       << " = " << s27 << std::endl;
	std::cout << 1.0e-105  << " = " << s21 << std::endl;

JString s19(1, JString::kPrecisionAsNeeded, JString::kUseGivenExponent, -3);
JString s20(1, JString::kPrecisionAsNeeded, JString::kUseGivenExponent, +2);

	std::cout << "1000e-3 = " << s19 << std::endl;
	std::cout << "0.01e+2 = " << s20 << std::endl;

JString s10(537.6, 2, JString::kForceExponent);

	std::cout << "5.376e2 to 2 places = " << s10 << std::endl;

JString s11(5.373, 2);
	std::cout << "5.373 to 2 places = " << s11 << std::endl;
JString s12(5.374, 2);
	std::cout << "5.374 to 2 places = " << s12 << std::endl;
JString s13(5.375, 2);
	std::cout << "5.375 to 2 places = " << s13 << std::endl;
JString s22(5.376, 2);
	std::cout << "5.376 to 2 places = " << s22 << std::endl;

JString s23(5.373, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	std::cout << "5.373 to 3 digits = " << s23 << std::endl;
JString s24(5.374, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	std::cout << "5.374 to 3 digits = " << s24 << std::endl;
JString s25(5.375, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	std::cout << "5.375 to 3 digits = " << s25 << std::endl;
JString s26(5.376, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	std::cout << "5.376 to 3 digits = " << s26 << std::endl;

JString s28(9.995, 2);
	std::cout << "9.995 to 2 places = " << s28 << std::endl;

	std::cout << std::endl << "Ready to start memory leak test";
	std::cout << std::endl << "Record memory usage now";
	JWaitForReturn();

	for (JIndex i=1; i<=100000; i++)
		{
		TestLeak();
		}

	std::cout << std::endl << "Memory leak test finished";
	std::cout << std::endl << "Compare memory usage with recorded value";
	JWaitForReturn();

	std::cout << std::endl;
	std::cout << "System clock started on " << JConvertToTimeStamp(0) << std::endl;
	std::cout << "System clock ends on " << JConvertToTimeStamp(J_TIME_T_MAX) << std::endl;
	std::cout << "Test concluded on " << JGetTimeStamp() << std::endl;

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

	std::cout << std::endl;

	for (JIndex testnum=0; testnum<kTestMax; testnum++)
		{
		std::cout << "Trying to copy string \"" << stringList[testnum];
		std::cout << "\" of length " << strlen(stringList[testnum]) << " into ";
		std::cout << kStringLength << " byte string " << std::endl;

		const JBoolean allCopied =
			JCopyMaxN(stringList[testnum], kStringLength, string);

		if (JNegate(strcmp(string, stringList[testnum])) == allCopied &&
			JCompareMaxN(string, stringList[testnum], kStringLength-1, kJTrue))
			{
			std::cout << "correct";
			}
		else
			{
			std::cout << "WRONG";
			}

		std::cout << ", copy is      \"" << string << "\" with length ";
		std::cout << strlen(string) << std::endl << std::endl;
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
