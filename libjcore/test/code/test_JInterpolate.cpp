/******************************************************************************
 test_JInterpolate.cpp

	A test program for JInterpolate.

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 2017 by John Lindal.

 *****************************************************************************/

#include "JTestManager.h"
#include "JInterpolate.h"
#include "JRegex.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include <locale.h>
#include "jAssert.h"

int main()
{
	std::cout << "Current locale: " << setlocale(LC_ALL, "") << std::endl;
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JString src("        DRegex::Replace(      ", 0);
	JRegex regex("D([[:alnum:]_]+)::([[:alpha:]_]+)");
	JInterpolate interpolator;

	JCharacterRange r;
	JAssertOK(interpolator.ContainsError(JString("J$1::$002", JString::kNoCopy), &r));

	JStringIterator iter(src);
	JAssertTrue(iter.Next(regex));
	JString result = interpolator.Interpolate(
		JString("J$1::$002", JString::kNoCopy),
		iter.GetLastMatch());
	JAssertStringsEqual("JRegex::Replace", result);

	JAssertOK(interpolator.ContainsError(JString("\\$a \\$- \\$ $00 $1 $+2 '$+3' '$9' \"$-1\" $-2 $-3 '$-4' '$-10' \\$", JString::kNoCopy), &r));

	result = interpolator.Interpolate(
		JString("\\$a \\$- \\$ $00 $1 $+2 '$+3' '$9' \"$-1\" $-2 $-3 '$-4' '$-10' \\$", JString::kNoCopy),
		iter.GetLastMatch());
	JAssertStringsEqual(
		"$a $- $ DRegex::Replace Regex Replace '' '' \"Replace\" Regex DRegex::Replace '' '' $",
		result);

	JAssertOK(interpolator.ContainsError(JString("J" "$+0001::" "$-1", JString::kNoCopy), &r));

	result = interpolator.Interpolate(
		JString("J" "$+0001::" "$-1", JString::kNoCopy),
		iter.GetLastMatch());
	JAssertStringsEqual(
		"JRegex::Replace",
		result);
}

JTEST(Adjacent)
{
	JString src(" XabX ", JString::kNoCopy);
	JRegex regex("X(a)(b)X");
	JInterpolate interpolator;

	JCharacterRange r;
	JAssertOK(interpolator.ContainsError(JString("$2$3$0$-4$1", JString::kNoCopy), &r));

	JStringIterator iter(src);
	JAssertTrue(iter.Next(regex));
	JString result = interpolator.Interpolate(
		JString("$2$3$0$-4$1", JString::kNoCopy),
		iter.GetLastMatch());
	JAssertStringsEqual("bXabXa", result);
}

JTEST(LargeIndex)
{
	JString src("abcdefghijklmno", JString::kNoCopy);
	JRegex regex("(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)");
	JInterpolate interpolator;

	// Various replace patterns which reproduce the original
	const JUtf8Byte* patternList[] =
	{
		"$0",
		"$1$2$3$4$5$6$7$8$9$10$11$12$13$14$15$16",
		"$-17$-15$-14$-13$-12$-11$-10$-9$-8$-7$-6$-5$-4$-3$-2$-1",
		"$-16",
		nullptr
	};

	JStringIterator iter(src);
	JAssertTrue(iter.Next(regex));

	JCharacterRange r;

	JIndex i=0;
	while (patternList[i] != nullptr)
	{
		JAssertOK(interpolator.ContainsError(JString(patternList[i], JString::kNoCopy), &r));

		JString result = interpolator.Interpolate(
			JString(patternList[i], JString::kNoCopy),
			iter.GetLastMatch());
		JAssertStringsEqualWithMessage(src, result,
			("large index pattern #" + JString(i)).GetBytes());

		++i;
	}
}

JTEST(NamedMatch)
{
	JString src(" XabX ", JString::kNoCopy);
	JRegex regex("X(?P<foo>a)(?P<bar>b)X");
	JInterpolate interpolator;

	JCharacterRange r;
	JAssertOK(interpolator.ContainsError(JString("${bar} ${foo}", JString::kNoCopy), &r));

	JStringIterator iter(src);
	JAssertTrue(iter.Next(regex));
	JString result = interpolator.Interpolate(
		JString("${bar} ${foo}", JString::kNoCopy),
		iter.GetLastMatch());
	JAssertStringsEqual("b a", result);
}
