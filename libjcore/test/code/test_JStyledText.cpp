/******************************************************************************
 test_JStyledText.cc

	Program to test JStyledText class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JBroadcastTester.h>
#include <StyledText.h>
#include <JStringIterator.h>
#include <JInterpolate.h>
#include <JRegex.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

typedef JStyledText::TextIndex TextIndex;
typedef JStyledText::TextCount TextCount;
typedef JStyledText::TextRange TextRange;

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(SetText)
{
	StyledText buf;
	JAssertTrue(buf.IsEmpty());

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextSet);

	JBoolean ok = buf.SetText(JString("abcd" "\xC3\x86", 0, kJFalse));
	JAssertTrue(ok);
	JAssertStringsEqual("abcd" "\xC3\x86", buf.GetText());
	JAssertEqual(5, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	bcastTest.Expect(JStyledText::kTextSet);

	ok = buf.SetText(JString("a\vb\ac\bd" "\xC3\x86", 0, kJFalse));
	JAssertFalse(ok);
	JAssertStringsEqual("abcd" "\xC3\x86", buf.GetText());
}

void TestItemStartEnd(const JUtf8Byte* s)
{
	StyledText buf;
	buf.SetText(JString(s, 0, kJFalse));

	JAssertEqual(1, buf.GetWordStart(0, 0));
	JAssertEqual(1, buf.GetWordStart(1, 1));
	JAssertEqual(1, buf.GetWordStart(3, 3));
	JAssertEqual(1, buf.GetWordStart(5, 5));
	JAssertEqual(6, buf.GetWordStart(6, 6));
	JAssertEqual(6, buf.GetWordStart(8, 8));
	JAssertEqual(6, buf.GetWordStart(10, 11));

	JAssertEqual(4, buf.GetWordEnd(1, 1));
	JAssertEqual(4, buf.GetWordEnd(3, 3));
	JAssertEqual(4, buf.GetWordEnd(4, 4));
	JAssertEqual(10, buf.GetWordEnd(5, 5));
	JAssertEqual(10, buf.GetWordEnd(8, 8));
	JAssertEqual(10, buf.GetWordEnd(9, 10));
	JAssertEqual(buf.GetText().GetCharacterCount(), buf.GetWordEnd(buf.GetText().GetCharacterCount()+1, buf.GetText().GetByteCount()+1));

	JAssertEqual(6, buf.GetWordStart(11, 12));
	JAssertEqual(6, buf.GetWordStart(12, 13));
	JAssertEqual(6, buf.GetWordStart(13, 14));

	JAssertEqual(16, buf.GetWordEnd(11, 12));
	JAssertEqual(16, buf.GetWordEnd(12, 13));
	JAssertEqual(16, buf.GetWordEnd(13, 14));

	const TextIndex first(1,1);

	JBoolean wrapped;
	const JStringMatch m1 = buf.SearchForward(first, JRegex("PRoP\xC3\xB8s1tion"), kJFalse, kJFalse, &wrapped);
	JAssertFalse(m1.IsEmpty());
	JAssertEqual(11, m1.GetCharacterCount());

	const JCharacterRange charRange = m1.GetCharacterRange();
	const JUtf8ByteRange  byteRange = m1.GetUtf8ByteRange();
	JAssertEqual(charRange.first, buf.GetWordStart(charRange.first+3, byteRange.first+3));
	JAssertEqual(charRange.last, buf.GetWordEnd(charRange.first+3, byteRange.first+3));

	JAssertEqual(1, buf.GetPartialWordStart(0, 0));
	JAssertEqual(charRange.last-3, buf.GetPartialWordStart(charRange.last, byteRange.last));
	JAssertEqual(charRange.last-3, buf.GetPartialWordStart(charRange.last-3, byteRange.last-3));
	JAssertEqual(charRange.last-4, buf.GetPartialWordStart(charRange.last-4, byteRange.last-4));
	JAssertEqual(charRange.last-7, buf.GetPartialWordStart(charRange.last-5, byteRange.last-5));
	JAssertEqual(charRange.last-9, buf.GetPartialWordStart(charRange.last-8, byteRange.last-9));
	JAssertEqual(charRange.last-10, buf.GetPartialWordStart(charRange.last-9, byteRange.last-10));

	JAssertEqual(charRange.first, buf.GetPartialWordEnd(charRange.first, byteRange.first));
	JAssertEqual(charRange.first+2, buf.GetPartialWordEnd(charRange.first+1, byteRange.first+1));
	JAssertEqual(charRange.first+5, buf.GetPartialWordEnd(charRange.first+3, byteRange.first+3));
	JAssertEqual(charRange.first+5, buf.GetPartialWordEnd(charRange.first+4, byteRange.first+4));
	JAssertEqual(charRange.first+6, buf.GetPartialWordEnd(charRange.first+6, byteRange.first+7));
	JAssertEqual(charRange.first+10, buf.GetPartialWordEnd(charRange.first+7, byteRange.first+8));
	JAssertEqual(buf.GetText().GetCharacterCount(), buf.GetPartialWordEnd(buf.GetText().GetCharacterCount()+1, buf.GetText().GetByteCount()+1));

	const JStringMatch m2 = buf.SearchForward(first, JRegex("and dedicated"), kJFalse, kJFalse, &wrapped);
	JAssertFalse(m1.IsEmpty());

	JAssertEqual(1, buf.GetParagraphStart(0, 0));
	JAssertEqual(1, buf.GetParagraphStart(13, 14));
	JAssertEqual(m2.GetCharacterRange().first, buf.GetParagraphStart(m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first));

	JAssertEqual(37, buf.GetParagraphEnd(1, 1));
	JAssertEqual(37, buf.GetParagraphEnd(13, 14));
	JAssertEqual(buf.GetText().GetCharacterCount(), buf.GetParagraphEnd(m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first));
	JAssertEqual(buf.GetText().GetCharacterCount(), buf.GetParagraphEnd(buf.GetText().GetCharacterCount()+1, buf.GetText().GetByteCount()+1));
}

JTEST(ItemStartEnd)
{
	TestItemStartEnd(
		"Four sc\xC3\xB8" "re - and seven years ago our\n"
		"fathers brought forth on this continent,\n"
		"a new nation, conceived in Liberty,\n"
		"and dedicated to the PRoP\xC3\xB8" "s1tion that all men are created equal.");

	TestItemStartEnd(
		"Four sc\xC3\xB8" "re - and seven years ago our\n"
		"fathers brought forth on this continent,\n"
		"a new nation, conceived in Liberty,\n"
		"and dedicated to the PRoP\xC3\xB8" "s1tion that all men are created equal.\n");
}

JTEST(ColumnForChar)
{
	StyledText buf;
	buf.SetText(JString("\xC3\xA1" "bcd\n\t1234\n  \twxzy", 0, kJFalse));

	JIndex col = buf.GetColumnForChar(TextIndex(1,1), TextIndex(1,1));
	JAssertEqual(1, col);

	col = buf.GetColumnForChar(TextIndex(1,1), TextIndex(2,3));
	JAssertEqual(2, col);

	col = buf.GetColumnForChar(TextIndex(6,7), TextIndex(6,7));
	JAssertEqual(1, col);

	col = buf.GetColumnForChar(TextIndex(6,7), TextIndex(8,9));
	JAssertEqual(10, col);

	col = buf.GetColumnForChar(TextIndex(12,13), TextIndex(12,13));
	JAssertEqual(1, col);

	col = buf.GetColumnForChar(TextIndex(12,13), TextIndex(16,17));
	JAssertEqual(10, col);
}

JTEST(ReadPlainText)
{
	StyledText buf;

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextSet);

	JStyledText::PlainTextFormat format;
	JBoolean ok = buf.ReadPlainText(JString("data/test_ReadPlainUNIXText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledText::kUNIXText, format);
	JAssertTrue(buf.EndsWithNewline());
	JAssertEqual(47, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	const JString s = buf.GetText();

	bcastTest.Expect(JStyledText::kWillBeBusy);
	bcastTest.Expect(JStyledText::kTextSet);

	ok = buf.ReadPlainText(JString("data/test_ReadPlainDOSText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledText::kDOSText, format);
	JAssertStringsEqual(s, buf.GetText());
	JAssertEqual(47, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	bcastTest.Expect(JStyledText::kTextSet);

	ok = buf.ReadPlainText(JString("data/test_ReadPlainMacText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledText::kMacintoshText, format);
	JAssertStringsEqual(s, buf.GetText());
	JAssertEqual(47, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	bcastTest.Expect(JStyledText::kTextSet);

	ok = buf.ReadPlainText(JString("data/test_ReadPlainBinaryText.txt", 0, kJFalse), &format);
	JAssertFalse(ok);
	JAssertEqual(JStyledText::kUNIXText, format);
	JAssertStringsEqual(s, buf.GetText());
	JAssertEqual(47, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	ok = buf.ReadPlainText(JString("data/test_ReadPlainBinaryText.txt", 0, kJFalse), &format, kJFalse);
	JAssertFalse(ok);
}

JTEST(WritePlainText)
{
	StyledText buf;

	JStyledText::PlainTextFormat format;
	JBoolean ok = buf.ReadPlainText(JString("data/test_ReadPlainUNIXText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledText::kUNIXText, format);

	JString fileName;
	JSize fileSize;
	JString s;
	JAssertOK(JCreateTempFile(&fileName));

	buf.WritePlainText(fileName, JStyledText::kUNIXText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(buf.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\n"));
	JAssertFalse(s.Contains("\r"));

	buf.WritePlainText(fileName, JStyledText::kDOSText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(buf.GetText().GetByteCount()+2, fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r\n"));

	buf.WritePlainText(fileName, JStyledText::kMacintoshText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(buf.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r"));
	JAssertFalse(s.Contains("\n"));

	JRemoveFile(fileName);
}

JTEST(ReadWritePrivateFormat)
{
	StyledText buf;
	buf.SetText(JString("\xC3\x86" "bcd", 0, kJFalse));
	buf.SetFontName(TextRange(JCharacterRange(2,2), JUtf8ByteRange(3,3)), JString("foo", 0, kJFalse), kJTrue);
	buf.SetFontSize(TextRange(JCharacterRange(2,3), JUtf8ByteRange(3,4)), 2*JGetDefaultFontSize(), kJTrue);
	buf.SetFontBold(TextRange(JCharacterRange(1,2), JUtf8ByteRange(1,3)), kJTrue, kJTrue);
	JAssertEqual(4, buf.GetStyles().GetRunCount());

	JString fileName;
	JAssertOK(JCreateTempFile(&fileName));

	{
	std::ofstream output(fileName.GetBytes());
	buf.WritePrivateFormat(output);
	}

	StyledText buf2;
	{
	JBroadcastTester bcastTest(&buf2);
	bcastTest.Expect(JStyledText::kWillBeBusy);
	bcastTest.Expect(JStyledText::kTextSet);

	std::ifstream input(fileName.GetBytes());
	buf2.ReadPrivateFormat(input);
	}

	JAssertStringsEqual("\xC3\x86" "bcd", buf2.GetText());
	JAssertEqual(4, buf2.GetStyles().GetRunCount());

	JFont f = buf2.GetFont(1);
	JAssertStringsEqual(JGetDefaultFontName(), f.GetName());
	JAssertEqual(JGetDefaultFontSize(), f.GetSize());
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = buf2.GetFont(2);
	JAssertStringsEqual("foo", f.GetName());
	JAssertEqual(2*JGetDefaultFontSize(), f.GetSize());
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = buf2.GetFont(3);
	JAssertStringsEqual(JGetDefaultFontName(), f.GetName());
	JAssertEqual(2*JGetDefaultFontSize(), f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = buf2.GetFont(4);
	JAssertStringsEqual(JGetDefaultFontName(), f.GetName());
	JAssertEqual(JGetDefaultFontSize(), f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
}

JTEST(SearchTextForward)
{
	StyledText buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	const TextIndex first(1,1);

	// entire word, no wrapping

	JBoolean wrapped;
	const JStringMatch m1 = buf.SearchForward(first, JRegex("sc" "\xC3\xB8" "re"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m2 = buf.SearchForward(first, JRegex("sc" "\xC3\xB8" "re"), kJFalse, kJFalse, &wrapped);
	JAssertStringsEqual("sc" "\xC3\xB8" "re", m2.GetString());
	JAssertEqual(JCharacterRange(5, 9), m2.GetCharacterRange());
	JAssertFalse(wrapped);

	// partial word, no wrapping

	TextIndex start(m2.GetCharacterRange().last, m2.GetUtf8ByteRange().last);

	const JStringMatch m3 = buf.SearchForward(start, JRegex("\xC3\xB8" "re and"), kJFalse, kJFalse, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = buf.SearchForward(start, JRegex("\xC3\xB8" "re and"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("\xC3\xB8" "re and", m4.GetString());
	JAssertEqual(JCharacterRange(7, 13), m4.GetCharacterRange());
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = buf.SearchForward(start, JRegex("year"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertFalse(wrapped);

	const JStringMatch m7 = buf.SearchForward(start, JRegex("years"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("years", m7.GetString());
	JAssertFalse(wrapped);

	// last characters

	start.charIndex = buf.GetText().GetCharacterCount();
	start.byteIndex = buf.GetText().GetByteCount();
	start = buf.AdjustTextIndex(start, -2);

	const JStringMatch m5 = buf.SearchForward(start, JRegex("\\.\\.\\."), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("...", m5.GetString());
	JAssertFalse(wrapped);
}

JTEST(SearchTextBackward)
{
	StyledText buf;
	buf.SetText(JString("Fourscore and seve" "\xC3\xB1" " years ago...", 0, kJFalse));

	const TextIndex last(buf.GetText().GetCharacterCount() + 1, buf.GetText().GetByteCount() + 1);

	// entire word, no wrapping

	JBoolean wrapped;
	const JStringMatch m1 = buf.SearchBackward(last, JRegex("year"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(wrapped);

	const JStringMatch m2 = buf.SearchBackward(last, JRegex("years"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("years", m2.GetString());
	JAssertEqual(JCharacterRange(21, 25), m2.GetCharacterRange());
	JAssertFalse(wrapped);

	// partial word, no wrapping

	TextIndex start(17,17);

	const JStringMatch m3 = buf.SearchBackward(start, JRegex("e" "\xC3\xB1" " ye"), kJFalse, kJFalse, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = buf.SearchBackward(start, JRegex("e" "\xC3\xB1" " ye"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("e" "\xC3\xB1" " ye", m4.GetString());
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = buf.SearchBackward(start, JRegex("an"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertFalse(wrapped);

	const JStringMatch m7 = buf.SearchBackward(start, JRegex("and"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("and", m7.GetString());
	JAssertFalse(wrapped);

	// first characters

	start.charIndex = 5;
	start.byteIndex = 5;

	const JStringMatch m5 = buf.SearchBackward(start, JRegex("Four"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("Four", m5.GetString());
	JAssertEqual(JCharacterRange(1, 4), m5.GetCharacterRange());
	JAssertFalse(wrapped);
}

JTEST(ReplaceMatch)
{
	StyledText buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);

	const TextIndex first(1,1);

	JBoolean wrapped;
	const JStringMatch m1 = buf.SearchForward(first, JRegex("sc" "(\xC3\xB8)" "re"), kJFalse, kJFalse, &wrapped);
	JAssertStringsEqual("sc" "\xC3\xB8" "re", m1.GetString());
	JAssertEqual(JCharacterRange(5, 9), m1.GetCharacterRange());
	JAssertFalse(wrapped);

	JInterpolate interp;

	const TextCount count =
		buf.ReplaceMatch(m1, JString("FL$1UR", 0, kJFalse), kJTrue, interp, kJTrue);

	JAssertEqual(5, count.charCount);
	JAssertEqual(6, count.byteCount);
	JAssertStringsEqual("Fourfl" "\xC3\xB8" "ur and seven years ago...", buf.GetText());
}

JTEST(ReplaceAllInRange)
{
	StyledText buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);

	JInterpolate interp;

	buf.ReplaceAllInRange(TextRange(
		JCharacterRange(1, buf.GetText().GetCharacterCount()),
		JUtf8ByteRange(1, buf.GetText().GetByteCount())),
		JRegex("e"), kJFalse, JString("\xC3\xA9", 0, kJFalse), kJFalse, interp, kJFalse);

	JAssertStringsEqual("Foursc" "\xC3\xB8" "r" "\xC3\xA9" " and s" "\xC3\xA9" "v" "\xC3\xA9" "n y" "\xC3\xA9" "ars ago...", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	JBoolean canUndo, canRedo;
	JAssertTrue(buf.HasSingleUndo());
	JAssertFalse(buf.HasMultipleUndo(&canUndo, &canRedo));

	buf.Undo();

	JAssertTrue(buf.HasSingleUndo());
	JAssertFalse(buf.HasMultipleUndo(&canUndo, &canRedo));

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.ReplaceAllInRange(TextRange(
		JCharacterRange(11, 21),
		JUtf8ByteRange(12, 22)),
		JRegex("e"), kJFalse, JString("\xC3\xA9", 0, kJFalse), kJFalse, interp, kJFalse);

	JAssertStringsEqual("Foursc" "\xC3\xB8" "re and s" "\xC3\xA9" "v" "\xC3\xA9" "n years ago...", buf.GetText());
}

JTEST(IsEntireWord)
{
	StyledText buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	JBoolean is = buf.IsEntireWord(JStyledText::TextRange(
		JCharacterRange(1,9),
		JUtf8ByteRange(1,10)));
	JAssertTrue(is);

	is = buf.IsEntireWord(TextRange(
		JCharacterRange(3,9),
		JUtf8ByteRange(3,10)));
	JAssertFalse(is);

	is = buf.IsEntireWord(TextRange(
		JCharacterRange(11,13),
		JUtf8ByteRange(12,14)));
	JAssertTrue(is);

	is = buf.IsEntireWord(TextRange(
		JCharacterRange(11,16),
		JUtf8ByteRange(12,17)));
	JAssertFalse(is);
}

class BigFontMatch : public JStyledText::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return JI2B( f.GetSize() == 20 );
	}
};

class BoldFontMatch : public JStyledText::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return f.GetStyle().bold;
	}
};

class ItalicFontMatch : public JStyledText::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return f.GetStyle().italic;
	}
};

class UnderlineFontMatch : public JStyledText::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return JI2B( f.GetStyle().underlineCount > 0 );
	}
};

JTEST(SearchStyle)
{
	StyledText buf;
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, kJFalse);
	buf.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), kJTrue, kJFalse);
	buf.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, kJFalse);

	const TextIndex first(1,1);
	TextRange r;

	// forward

	JBoolean wrapped;
	JBoolean found = buf.SearchForward(ItalicFontMatch(), first, kJFalse, &wrapped, &r);
	JAssertFalse(found);
	JAssertFalse(wrapped);

	found = buf.SearchForward(BoldFontMatch(), first, kJTrue, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(4, 7), r.charRange);
	JAssertFalse(wrapped);

	found = buf.SearchForward(UnderlineFontMatch(), first, kJTrue, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(14, 29), r.charRange);
	JAssertFalse(wrapped);

	TextIndex start(29, 31);

	found = buf.SearchForward(BoldFontMatch(), start, kJFalse, &wrapped, &r);
	JAssertFalse(found);
	JAssertFalse(wrapped);

	found = buf.SearchForward(BigFontMatch(), start, kJTrue, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(1, 3), r.charRange);
	JAssertTrue(wrapped);

	// backward

	start.charIndex = 24;
	start.byteIndex = 26;

	found = buf.SearchBackward(BigFontMatch(), start, kJTrue, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(1, 3), r.charRange);
	JAssertFalse(wrapped);

	start.charIndex = 1;
	start.byteIndex = 1;

	found = buf.SearchBackward(BoldFontMatch(), start, kJFalse, &wrapped, &r);
	JAssertFalse(found);
	JAssertFalse(wrapped);

	found = buf.SearchBackward(BoldFontMatch(), start, kJTrue, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(4, 7), r.charRange);
	JAssertTrue(wrapped);

	start.charIndex = r.charRange.first;
	start.byteIndex = r.byteRange.first;

	buf.SearchBackward(BigFontMatch(), start, kJTrue, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(1, 3), r.charRange);
	JAssertFalse(wrapped);
}

JTEST(SetAllFontNameAndSize)
{
	StyledText buf;
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, kJFalse);
	buf.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), kJTrue, kJFalse);
	buf.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, kJFalse);

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.SetAllFontNameAndSize(JString("foo", 0, kJFalse), 24, kJFalse);

	JAssertEqual(4, buf.GetStyles().GetRunCount());
	JAssertEqual(24, buf.GetFont(2).GetSize());
	JAssertTrue(buf.GetFont(5).GetStyle().bold);
	JAssertFalse(buf.GetFont(10).GetStyle().bold);
	JAssertEqual(2, buf.GetFont(16).GetStyle().underlineCount);
}

JTEST(CalcInsertionFont)
{
	StyledText buf;
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, kJFalse);
	buf.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), kJTrue, kJFalse);
	buf.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, kJFalse);

	JAssertEqual(20, buf.CalcInsertionFont(TextIndex(2,2)).GetSize());
	JAssertTrue(buf.CalcInsertionFont(TextIndex(5,6)).GetStyle().bold);
	JAssertEqual(0, buf.CalcInsertionFont(TextIndex(11,13)).GetStyle().underlineCount);
	JAssertEqual(2, buf.CalcInsertionFont(TextIndex(16,18)).GetStyle().underlineCount);
}

JTEST(CopyPaste)
{
	StyledText buf;
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, kJFalse);
	buf.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), kJTrue, kJFalse);
	buf.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, kJFalse);

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);

	JString s;
	JRunArray<JFont> style;

	JBoolean ok = buf.Copy(TextRange(
		JCharacterRange(2,6), JUtf8ByteRange(2,8)), &s, &style);

	JAssertTrue(ok);
	JAssertStringsEqual("\xC3\xAE" "g" "b" "\xC3\xB8" "l", s);
	JAssertEqual(2, style.GetRunCount());

	buf.Paste(TextRange(
		JCharacterRange(10,11), JUtf8ByteRange(12,13)), s, &style);

	JAssertStringsEqual("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "no" "\xC3\xAE" "g" "b" "\xC3\xB8" "l" "al" "double underline", buf.GetText());
	JAssertEqual(7, buf.GetStyles().GetRunCount());
	JAssertTrue(buf.GetFont(13).GetStyle().bold);
}

JTEST(DeleteText)
{
	StyledText buf;
	buf.UseMultipleUndo();
	buf.SetText(JString("b" "\xC3\xAE" "g\n" "b" "\xC3\xB8" "ld\n" "\t   normal\n" "double underline", 0, kJFalse));
	buf.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, kJFalse);
	buf.SetFontBold(TextRange(JCharacterRange(5,8), JUtf8ByteRange(6,10)), kJTrue, kJFalse);
	buf.SetFontUnderline(TextRange(JCharacterRange(16,31), JUtf8ByteRange(18,33)), 2, kJTrue);

	buf.DeleteText(TextRange(JCharacterRange(2,6), JUtf8ByteRange(2,8)));
	JAssertStringsEqual("b" "ld\n" "\t   normal\n" "double underline", buf.GetText());

	buf.DeleteText(TextRange(JCharacterRange(10,18), JUtf8ByteRange(10,18)));
	JAssertStringsEqual("b" "ld\n" "\t   nble underline", buf.GetText());

	JAssertEqual(2, buf.GetStyles().GetElement(13).GetStyle().underlineCount);

	JBoolean canUndo, canRedo;
	JAssertFalse(buf.HasSingleUndo());
	JAssertTrue(buf.HasMultipleUndo(&canUndo, &canRedo));
	JAssertTrue(canUndo);
	JAssertFalse(canRedo);

	buf.Undo();

	JAssertTrue(buf.HasMultipleUndo(&canUndo, &canRedo));
	JAssertTrue(canUndo);
	JAssertTrue(canRedo);

	buf.Undo();

	JAssertTrue(buf.HasMultipleUndo(&canUndo, &canRedo));
	JAssertFalse(canUndo);
	JAssertTrue(canRedo);

	JAssertEqual(20, buf.GetStyles().GetElement(2).GetSize());
	JAssertTrue(buf.GetStyles().GetElement(6).GetStyle().bold);
	JAssertEqual(2, buf.GetStyles().GetElement(21).GetStyle().underlineCount);
}

JTEST(BackwardDelete)
{
	StyledText buf;
	buf.UseMultipleUndo();
	buf.SetText(JString("b" "\xC3\xAE" "g\n" "b" "\xC3\xB8" "ld\n" "\t   normal\n" "double underline", 0, kJFalse));
	buf.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, kJFalse);
	buf.SetFontBold(TextRange(JCharacterRange(5,8), JUtf8ByteRange(6,10)), kJTrue, kJFalse);
	buf.SetFontUnderline(TextRange(JCharacterRange(16,31), JUtf8ByteRange(18,33)), 2, kJFalse);

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);

	TextIndex caretIndex = buf.BackwardDelete(TextIndex(5,6), TextIndex(8,10), kJFalse);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "\xC3\xB8" "d\n" "\t   normal\n" "double underline", buf.GetText());
	JAssertEqual(7, caretIndex.charIndex);
	JAssertEqual(9, caretIndex.byteIndex);

	bcastTest.Expect(JStyledText::kTextChanged);

	JString returnText;
	JRunArray<JFont> returnStyle;
	buf.BackwardDelete(TextIndex(5,6), TextIndex(7,9), kJFalse, &returnText, &returnStyle);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\t   normal\n" "double underline", buf.GetText());
	JAssertStringsEqual("\xC3\xB8", returnText);
	JAssertEqual(1, returnStyle.GetElementCount());
	JAssertTrue(returnStyle.GetFirstElement().GetStyle().bold);

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.BackwardDelete(TextIndex(8,9), TextIndex(11,12), kJFalse);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\t  normal\n" "double underline", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Undo();

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.BackwardDelete(TextIndex(8,9), TextIndex(11,12), kJTrue);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\t  normal\n" "double underline", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Paste(TextRange(
		JCharacterRange(9,8), JUtf8ByteRange(10,9)),
		JString("      ", 0, kJFalse));
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\t        normal\n" "double underline", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.BackwardDelete(TextIndex(8,9), TextIndex(17,18), kJTrue);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\tnormal\n" "double underline", buf.GetText());
}

JTEST(ForwardDelete)
{
	StyledText buf;
	buf.UseMultipleUndo();
	buf.SetText(JString("b" "\xC3\xAE" "g\n" "b" "\xC3\xB8" "ld\n" "\t   normal\n" "double underline", 0, kJFalse));
	buf.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, kJFalse);
	buf.SetFontBold(TextRange(JCharacterRange(5,8), JUtf8ByteRange(6,10)), kJTrue, kJFalse);
	buf.SetFontUnderline(TextRange(JCharacterRange(16,31), JUtf8ByteRange(18,33)), 2, kJFalse);

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.ForwardDelete(TextIndex(5,6), TextIndex(5,6), kJFalse);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "\xC3\xB8" "ld\n" "\t   normal\n" "double underline", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	JString returnText;
	JRunArray<JFont> returnStyle;
	buf.ForwardDelete(TextIndex(5,6), TextIndex(5,6), kJFalse, &returnText, &returnStyle);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\t   normal\n" "double underline", buf.GetText());
	JAssertStringsEqual("\xC3\xB8", returnText);
	JAssertEqual(1, returnStyle.GetElementCount());
	JAssertTrue(returnStyle.GetFirstElement().GetStyle().bold);

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.ForwardDelete(TextIndex(8,9), TextIndex(9,10), kJFalse);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\t  normal\n" "double underline", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Undo();

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.ForwardDelete(TextIndex(8,9), TextIndex(9,10), kJTrue);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\t  normal\n" "double underline", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Paste(TextRange(
		JCharacterRange(9,8), JUtf8ByteRange(10,9)),
		JString("      ", 0, kJFalse));
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\t        normal\n" "double underline", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.ForwardDelete(TextIndex(8,9), TextIndex(9,10), kJTrue);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\tnormal\n" "double underline", buf.GetText());
}

JTEST(Move)
{
	StyledText buf;
	buf.UseMultipleUndo();
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, kJFalse);
	buf.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), kJTrue, kJFalse);
	buf.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, kJFalse);

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);

	TextRange r = buf.MoveText(
		TextRange(
			JCharacterRange(5,7),
			JUtf8ByteRange(6,9)),
		TextIndex(11,13),
		kJFalse);

	JAssertFalse(r.IsEmpty());
	JAssertStringsEqual("b" "\xC3\xAE" "g" "b" "nor" "\xC3\xB8" "ld" "mal" "double underline", buf.GetText());
	JAssertTrue(buf.GetFont(4).GetStyle().bold);
	JAssertFalse(buf.GetFont(7).GetStyle().bold);
	JAssertTrue(buf.GetFont(8).GetStyle().bold);

	bcastTest.Expect(JStyledText::kTextChanged);

	r = buf.MoveText(
		TextRange(
			JCharacterRange(14,19),
			JUtf8ByteRange(16,21)),
		TextIndex(1,1),
		kJTrue);

	JAssertFalse(r.IsEmpty());
	JAssertStringsEqual("double" "b" "\xC3\xAE" "g" "b" "nor" "\xC3\xB8" "ld" "mal" "double underline", buf.GetText());
	JAssertEqual(2, buf.GetFont(2).GetStyle().underlineCount);

	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Undo();
	buf.Undo();

	JAssertStringsEqual("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", buf.GetText());
	JAssertEqual(0, buf.GetFont(1).GetStyle().underlineCount);
	JAssertTrue(buf.GetFont(5).GetStyle().bold);
	JAssertFalse(buf.GetFont(10).GetStyle().bold);
	JAssertFalse(buf.GetFont(11).GetStyle().bold);
}

JTEST(TabSelection)
{
	StyledText buf;
	buf.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", 0, kJFalse));
	buf.UseMultipleUndo();

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Indent(TextRange(JCharacterRange(1,1), JUtf8ByteRange(1,2)), 2);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	buf.Outdent(TextRange(JCharacterRange(8,13), JUtf8ByteRange(9,14)), 3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Indent(TextRange(JCharacterRange(8,13), JUtf8ByteRange(9,14)), 3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Undo();
	buf.Undo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());
	buf.Redo();
	buf.Redo();

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Outdent(TextRange(JCharacterRange(1,22), JUtf8ByteRange(1,23)), 1);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Undo();
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", buf.GetText());
	buf.Redo();

	buf.Outdent(TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 2);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Outdent(TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 2, kJTrue);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());
}

JTEST(TabSelectionMixed)
{
	StyledText buf;
	buf.SetText(JString("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Outdent(TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Undo();
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", buf.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Undo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());


	bcastTest.Expect(JStyledText::kTextSet);

	buf.SetText(JString("  " "\xC3\xA1" "bcd\n   1234\n    wxzy", 0, kJFalse));

	bcastTest.Expect(JStyledText::kTextChanged);

	buf.Outdent(TextRange(JCharacterRange(1,23), JUtf8ByteRange(1,24)), 1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n 1234\n  wxzy", buf.GetText());
}

JTEST(CleanWhitespaceTabs)
{
	StyledText buf;
	buf.TabShouldInsertSpaces(kJFalse);
	buf.SetCRMTabCharCount(4);

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	TextRange r =
		buf.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJFalse);
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,18), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJTrue);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,17), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(TextRange(
							JCharacterRange(9,12),
							JUtf8ByteRange(10,13)), kJTrue);
	buf.Undo();
	buf.Undo();
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(8,13), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n   1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJTrue);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,17), r.charRange);
}

JTEST(CleanWhitespaceSpaces)
{
	StyledText buf;
	buf.TabShouldInsertSpaces(kJTrue);
	buf.SetCRMTabCharCount(4);

	JBroadcastTester bcastTest(&buf);
	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	TextRange r =
		buf.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJFalse);
	JAssertStringsEqual("     " "\xC3\xA1" "bcd\n    1234\n    wxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,27), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJTrue);
	JAssertStringsEqual("    " "\xC3\xA1" "bcd\n    1234\n    wxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,26), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(TextRange(
							JCharacterRange(9,12),
							JUtf8ByteRange(10,13)), kJTrue);
	buf.Undo();
	buf.Redo();
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n    1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(8,16), r.charRange);
}

JTEST(AdjustTextIndex)
{
	StyledText buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	TextIndex index(5,5);
	index = buf.AdjustTextIndex(index, 6);
	JAssertEqual(11, index.charIndex);
	JAssertEqual(12, index.byteIndex);

	index = buf.AdjustTextIndex(index, -7);
	JAssertEqual(4, index.charIndex);
	JAssertEqual(4, index.byteIndex);
}
