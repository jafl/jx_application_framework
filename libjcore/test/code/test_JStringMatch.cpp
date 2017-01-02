/******************************************************************************
 test_JStringMatch.cc

	Program to test JStringMatch class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JStringMatch.h>
#include <locale.h>
#include <jassert_simple.h>

class JStringMatchTest : public JStringMatch
{
public:

	JStringMatchTest
		(
		const JString&			target,
		const JUtf8ByteRange&	byteRange,
		JArray<JUtf8ByteRange>*	list = NULL
		)
		:
		JStringMatch(target, byteRange, NULL, list)
	{ }

	void
	TestSetFirstCharacterIndex
		(
		const JIndex index
		)
	{
		SetFirstCharacterIndex(index);
	}

	void
	TestSetLastCharacterIndex
		(
		const JIndex index
		)
	{
		SetLastCharacterIndex(index);
	}
};

int main()
{
	std::cout << "Current locale: " << setlocale(LC_ALL, "") << std::endl;
	return JUnitTestManager::Execute();
}

JTEST(Construction)
{
	JString s("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);

	JStringMatchTest m1(s, JUtf8ByteRange(1,4));
	m1.TestSetFirstCharacterIndex(1);

	JAssertEqual(4, m1.GetCharacterCount());
	JAssertEqual(4, m1.GetByteCount());
	JAssertEqual(0, m1.GetSubstringCount());
	JAssertStringsEqual("1234", m1.GetString());
	JAssertTrue(m1.GetSubstring(1).IsEmpty());

	JStringMatchTest m2(s, JUtf8ByteRange(1,4));
	m2.TestSetLastCharacterIndex(4);

	JAssertEqual(4, m2.GetCharacterCount());
	JAssertEqual(4, m2.GetByteCount());
	JAssertEqual(0, m2.GetSubstringCount());
	JAssertStringsEqual("1234", m2.GetString());
	JAssertTrue(m2.GetSubstring(1).IsEmpty());

	JStringMatchTest m3(s, JUtf8ByteRange(9,14));
	m3.TestSetFirstCharacterIndex(9);

	JAssertEqual(4, m3.GetCharacterCount());
	JAssertEqual(6, m3.GetByteCount());
	JAssertEqual(0, m3.GetSubstringCount());
	JAssertStringsEqual("90\xC2\xA9\xC3\x85", m3.GetString());
	JAssertTrue(m3.GetSubstring(1).IsEmpty());

	JStringMatchTest m4(s, JUtf8ByteRange(9,14));
	m4.TestSetLastCharacterIndex(12);

	JAssertEqual(4, m4.GetCharacterCount());
	JAssertEqual(6, m4.GetByteCount());
	JAssertEqual(0, m4.GetSubstringCount());
	JAssertStringsEqual("90\xC2\xA9\xC3\x85", m4.GetString());
	JAssertTrue(m4.GetSubstring(1).IsEmpty());

	JStringMatchTest m5(s, JUtf8ByteRange(9,14));

	JAssertEqual(JCharacterRange(9, 12), m5.GetCharacterRange());
	JAssertEqual(4, m5.GetCharacterCount());
	JAssertEqual(6, m5.GetByteCount());
	JAssertEqual(0, m5.GetSubstringCount());
	JAssertStringsEqual("90\xC2\xA9\xC3\x85", m5.GetString());
	JAssertTrue(m5.GetSubstring(1).IsEmpty());
}

JTEST(Submatches)
{
	JString s("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);

	JArray<JUtf8ByteRange>* list1 = jnew JArray<JUtf8ByteRange>();
	assert( list1 != NULL );
	list1->AppendElement(JUtf8ByteRange(11,12));
	list1->AppendElement(JUtf8ByteRange(9, 14));
	list1->AppendElement(JUtf8ByteRange(7, 7));

	JStringMatchTest m1(s, JUtf8ByteRange(7,14), list1);
	m1.TestSetLastCharacterIndex(12);

	JAssertEqual(6, m1.GetCharacterCount());
	JAssertEqual(8, m1.GetByteCount());
	JAssertEqual(3, m1.GetSubstringCount());
	JAssertStringsEqual("7890\xC2\xA9\xC3\x85", m1.GetString());
	JAssertEqual(JCharacterRange(7,12), m1.GetCharacterRange(0));
	JAssertEqual(JCharacterRange(7,14), m1.GetUtf8ByteRange(0));
	JAssertStringsEqual("\xC2\xA9", m1.GetSubstring(1));
	JAssertEqual(JCharacterRange(11,11), m1.GetCharacterRange(1));
	JAssertEqual(JUtf8ByteRange(11,12), m1.GetUtf8ByteRange(1));
	JAssertStringsEqual("90\xC2\xA9\xC3\x85", m1.GetSubstring(2));
	JAssertEqual(JCharacterRange(9,12), m1.GetCharacterRange(2));
	JAssertEqual(JUtf8ByteRange(9,14), m1.GetUtf8ByteRange(2));
	JAssertStringsEqual("7", m1.GetSubstring(3));
	JAssertEqual(JCharacterRange(7,7), m1.GetCharacterRange(3));
	JAssertEqual(JUtf8ByteRange(7,7), m1.GetUtf8ByteRange(3));
}
