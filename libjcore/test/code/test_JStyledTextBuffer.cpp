/******************************************************************************
 test_JStyledTextBuffer.cc

	Program to test JStyledTextBuffer class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <StyledTextBuffer.h>
#include <JStringIterator.h>
#include <JInterpolate.h>
#include <JRegex.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(SetText)
{
	StyledTextBuffer buf;
	JAssertTrue(buf.IsEmpty());

	JBoolean ok = buf.SetText(JString("abcd" "\xC3\x86", 0, kJFalse));
	JAssertTrue(ok);
	JAssertStringsEqual("abcd" "\xC3\x86", buf.GetText());
	JAssertEqual(5, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	ok = buf.SetText(JString("a\vb\ac\bd" "\xC3\x86", 0, kJFalse));
	JAssertFalse(ok);
	JAssertStringsEqual("abcd" "\xC3\x86", buf.GetText());
}

void TestItemStartEnd(const JUtf8Byte* s)
{
	StyledTextBuffer buf;
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

	const JStyledTextBuffer::TextIndex first(1,1);

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
	StyledTextBuffer buf;
	buf.SetText(JString("\xC3\xA1" "bcd\n\t1234\n  \twxzy", 0, kJFalse));

	JIndex col = buf.GetColumnForChar(
		JStyledTextBuffer::TextIndex(1,1),
		JStyledTextBuffer::TextIndex(1,1));
	JAssertEqual(1, col);

	col = buf.GetColumnForChar(
		JStyledTextBuffer::TextIndex(1,1),
		JStyledTextBuffer::TextIndex(2,3));
	JAssertEqual(2, col);

	col = buf.GetColumnForChar(
		JStyledTextBuffer::TextIndex(6,7),
		JStyledTextBuffer::TextIndex(6,7));
	JAssertEqual(1, col);

	col = buf.GetColumnForChar(
		JStyledTextBuffer::TextIndex(6,7),
		JStyledTextBuffer::TextIndex(8,9));
	JAssertEqual(10, col);

	col = buf.GetColumnForChar(
		JStyledTextBuffer::TextIndex(12,13),
		JStyledTextBuffer::TextIndex(12,13));
	JAssertEqual(1, col);

	col = buf.GetColumnForChar(
		JStyledTextBuffer::TextIndex(12,13),
		JStyledTextBuffer::TextIndex(16,17));
	JAssertEqual(10, col);
}

JTEST(ReadPlainText)
{
	StyledTextBuffer buf;

	JStyledTextBuffer::PlainTextFormat format;
	JBoolean ok = buf.ReadPlainText(JString("data/test_ReadPlainUNIXText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledTextBuffer::kUNIXText, format);
	JAssertTrue(buf.EndsWithNewline());
	JAssertEqual(47, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	const JString s = buf.GetText();

	ok = buf.ReadPlainText(JString("data/test_ReadPlainDOSText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledTextBuffer::kDOSText, format);
	JAssertStringsEqual(s, buf.GetText());
	JAssertEqual(47, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	ok = buf.ReadPlainText(JString("data/test_ReadPlainMacText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledTextBuffer::kMacintoshText, format);
	JAssertStringsEqual(s, buf.GetText());
	JAssertEqual(47, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	ok = buf.ReadPlainText(JString("data/test_ReadPlainBinaryText.txt", 0, kJFalse), &format);
	JAssertFalse(ok);
	JAssertEqual(JStyledTextBuffer::kUNIXText, format);
	JAssertStringsEqual(s, buf.GetText());
	JAssertEqual(47, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	ok = buf.ReadPlainText(JString("data/test_ReadPlainBinaryText.txt", 0, kJFalse), &format, kJFalse);
	JAssertFalse(ok);
}

JTEST(WritePlainText)
{
	StyledTextBuffer buf;

	JStyledTextBuffer::PlainTextFormat format;
	JBoolean ok = buf.ReadPlainText(JString("data/test_ReadPlainUNIXText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledTextBuffer::kUNIXText, format);

	JString fileName;
	JSize fileSize;
	JString s;
	JAssertOK(JCreateTempFile(&fileName));

	buf.WritePlainText(fileName, JStyledTextBuffer::kUNIXText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(buf.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\n"));
	JAssertFalse(s.Contains("\r"));

	buf.WritePlainText(fileName, JStyledTextBuffer::kDOSText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(buf.GetText().GetByteCount()+2, fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r\n"));

	buf.WritePlainText(fileName, JStyledTextBuffer::kMacintoshText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(buf.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r"));
	JAssertFalse(s.Contains("\n"));

	JRemoveFile(fileName);
}

JTEST(ReadWritePrivateFormat)
{
	StyledTextBuffer buf;
	buf.SetText(JString("\xC3\x86" "bcd", 0, kJFalse));
	buf.SetFontName(2, 2, JString("foo", 0, kJFalse), kJTrue);
	buf.SetFontSize(2, 3, 2*JGetDefaultFontSize(), kJTrue);
	buf.SetFontBold(1, 2, kJTrue, kJTrue);
	JAssertEqual(4, buf.GetStyles().GetRunCount());

	JString fileName;
	JAssertOK(JCreateTempFile(&fileName));

	{
	std::ofstream output(fileName.GetBytes());
	buf.WritePrivateFormat(output);
	}

	StyledTextBuffer buf2;
	{
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
	StyledTextBuffer buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	const JStyledTextBuffer::TextIndex first(1,1);

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

	JStyledTextBuffer::TextIndex start(m2.GetCharacterRange().last, m2.GetUtf8ByteRange().last);

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
	StyledTextBuffer buf;
	buf.SetText(JString("Fourscore and seve" "\xC3\xB1" " years ago...", 0, kJFalse));

	const JStyledTextBuffer::TextIndex last(buf.GetText().GetCharacterCount() + 1, buf.GetText().GetByteCount() + 1);

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

	JStyledTextBuffer::TextIndex start(17,17);

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

JTEST(ReplaceRange)
{
	StyledTextBuffer buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	const JStyledTextBuffer::TextIndex first(1,1);

	JBoolean wrapped;
	const JStringMatch m1 = buf.SearchForward(first, JRegex("sc" "(\xC3\xB8)" "re"), kJFalse, kJFalse, &wrapped);
	JAssertStringsEqual("sc" "\xC3\xB8" "re", m1.GetString());
	JAssertEqual(JCharacterRange(5, 9), m1.GetCharacterRange());
	JAssertFalse(wrapped);

	JStringIterator iter(const_cast<JString*>(&(buf.GetText())));
	iter.UnsafeMoveTo(kJIteratorStartBefore, m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first);

	JRunArray<JFont> styles(buf.GetStyles());
	JInterpolate interp;

	buf.ReplaceRange(&iter, &styles, m1, JString("FL$1UR", 0, kJFalse), kJTrue, interp, kJTrue);

	JAssertStringsEqual("Fourfl" "\xC3\xB8" "ur and seven years ago...", buf.GetText());
}

JTEST(IsEntireWord)
{
	StyledTextBuffer buf;
	const JString s("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse);

	JBoolean is = buf.IsEntireWord(s, JStyledTextBuffer::TextRange(
		JCharacterRange(1,9),
		JUtf8ByteRange(1,10)));
	JAssertTrue(is);

	is = buf.IsEntireWord(s, JStyledTextBuffer::TextRange(
		JCharacterRange(3,9),
		JUtf8ByteRange(3,10)));
	JAssertFalse(is);

	is = buf.IsEntireWord(s, JStyledTextBuffer::TextRange(
		JCharacterRange(11,13),
		JUtf8ByteRange(12,14)));
	JAssertTrue(is);

	is = buf.IsEntireWord(s, JStyledTextBuffer::TextRange(
		JCharacterRange(11,16),
		JUtf8ByteRange(12,17)));
	JAssertFalse(is);
}

class BigFontMatch : public JStyledTextBuffer::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return JI2B( f.GetSize() == 20 );
	}
};

class BoldFontMatch : public JStyledTextBuffer::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return f.GetStyle().bold;
	}
};

class ItalicFontMatch : public JStyledTextBuffer::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return f.GetStyle().italic;
	}
};

class UnderlineFontMatch : public JStyledTextBuffer::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return JI2B( f.GetStyle().underlineCount > 0 );
	}
};

JTEST(SearchStyle)
{
	StyledTextBuffer buf;
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(1, 3, 20, kJFalse);
	buf.SetFontBold(4, 7, kJTrue, kJFalse);
	buf.SetFontUnderline(14, 29, 2, kJFalse);

	const JStyledTextBuffer::TextIndex first(1,1);
	JStyledTextBuffer::TextRange r;

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

	JStyledTextBuffer::TextIndex start(29, 31);

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
	StyledTextBuffer buf;
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(1, 3, 20, kJFalse);
	buf.SetFontBold(4, 7, kJTrue, kJFalse);
	buf.SetFontUnderline(14, 29, 2, kJFalse);

	buf.SetAllFontNameAndSize(JString("foo", 0, kJFalse), 24, kJFalse);

	JAssertEqual(4, buf.GetStyles().GetRunCount());
	JAssertEqual(24, buf.GetFont(2).GetSize());
	JAssertTrue(buf.GetFont(5).GetStyle().bold);
	JAssertFalse(buf.GetFont(10).GetStyle().bold);
	JAssertEqual(2, buf.GetFont(16).GetStyle().underlineCount);
}

JTEST(CalcInsertionFont)
{
	StyledTextBuffer buf;
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(1, 3, 20, kJFalse);
	buf.SetFontBold(4, 7, kJTrue, kJFalse);
	buf.SetFontUnderline(14, 29, 2, kJFalse);

	JAssertEqual(20, buf.CalcInsertionFont(JStyledTextBuffer::TextIndex(2,2)).GetSize());
	JAssertTrue(buf.CalcInsertionFont(JStyledTextBuffer::TextIndex(5,6)).GetStyle().bold);
	JAssertEqual(0, buf.CalcInsertionFont(JStyledTextBuffer::TextIndex(11,13)).GetStyle().underlineCount);
	JAssertEqual(2, buf.CalcInsertionFont(JStyledTextBuffer::TextIndex(16,18)).GetStyle().underlineCount);
}

JTEST(CopyPaste)
{
	StyledTextBuffer buf;
	buf.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", 0, kJFalse));
	buf.SetFontSize(1, 3, 20, kJFalse);
	buf.SetFontBold(4, 7, kJTrue, kJFalse);
	buf.SetFontUnderline(14, 29, 2, kJFalse);

	JString s;
	JRunArray<JFont> style;

	JBoolean ok = buf.Copy(JStyledTextBuffer::TextRange(
		JCharacterRange(2,6), JUtf8ByteRange(2,8)), &s, &style);

	JAssertTrue(ok);
	JAssertStringsEqual("\xC3\xAE" "g" "b" "\xC3\xB8" "l", s);
	JAssertEqual(2, style.GetRunCount());

	buf.Paste(JStyledTextBuffer::TextRange(
		JCharacterRange(10,11), JUtf8ByteRange(12,13)), s, &style);

	JAssertStringsEqual("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "no" "\xC3\xAE" "g" "b" "\xC3\xB8" "l" "al" "double underline", buf.GetText());
	JAssertEqual(7, buf.GetStyles().GetRunCount());
	JAssertTrue(buf.GetFont(13).GetStyle().bold);
}

JTEST(ForwardBackwardDelete)
{}

JTEST(Move)
{}

JTEST(TabSelection)
{
	StyledTextBuffer buf;
	buf.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", 0, kJFalse));
	buf.UseMultipleUndo();

	buf.Indent(JStyledTextBuffer::TextRange(JCharacterRange(1,1), JUtf8ByteRange(1,1)), 2);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	buf.Outdent(JStyledTextBuffer::TextRange(JCharacterRange(8,13), JUtf8ByteRange(9,14)), 3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	buf.Indent(JStyledTextBuffer::TextRange(JCharacterRange(8,13), JUtf8ByteRange(9,14)), 3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", buf.GetText());

	buf.Undo();
	buf.Undo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());
	buf.Redo();
	buf.Redo();

	buf.Outdent(JStyledTextBuffer::TextRange(JCharacterRange(1,22), JUtf8ByteRange(1,23)), 1);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", buf.GetText());

	buf.Undo();
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", buf.GetText());
	buf.Redo();

	buf.Outdent(JStyledTextBuffer::TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 2);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", buf.GetText());

	buf.Outdent(JStyledTextBuffer::TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 2, kJTrue);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());
}

JTEST(TabSelectionMixed)
{
	StyledTextBuffer buf;
	buf.SetText(JString("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));

	buf.Outdent(JStyledTextBuffer::TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	buf.Undo();
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", buf.GetText());

	buf.Undo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());


	buf.SetText(JString("  " "\xC3\xA1" "bcd\n   1234\n    wxzy", 0, kJFalse));

	buf.Outdent(JStyledTextBuffer::TextRange(JCharacterRange(1,23), JUtf8ByteRange(1,24)), 1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n 1234\n  wxzy", buf.GetText());
}

JTEST(CleanWhitespaceTabs)
{
	StyledTextBuffer buf;
	buf.TabShouldInsertSpaces(kJFalse);
	buf.SetCRMTabCharCount(4);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	JStyledTextBuffer::TextRange r =
		buf.CleanWhitespace(JStyledTextBuffer::TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJFalse);
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,18), r.charRange);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(JStyledTextBuffer::TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJTrue);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,17), r.charRange);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(JStyledTextBuffer::TextRange(
							JCharacterRange(9,12),
							JUtf8ByteRange(10,13)), kJTrue);
	buf.Undo();
	buf.Undo();
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(8,13), r.charRange);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n   1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(JStyledTextBuffer::TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJTrue);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,17), r.charRange);
}

JTEST(CleanWhitespaceSpaces)
{
	StyledTextBuffer buf;
	buf.TabShouldInsertSpaces(kJTrue);
	buf.SetCRMTabCharCount(4);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	JStyledTextBuffer::TextRange r =
		buf.CleanWhitespace(JStyledTextBuffer::TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJFalse);
	JAssertStringsEqual("     " "\xC3\xA1" "bcd\n    1234\n    wxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,27), r.charRange);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(JStyledTextBuffer::TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), kJTrue);
	JAssertStringsEqual("    " "\xC3\xA1" "bcd\n    1234\n    wxzy", buf.GetText());
	JAssertEqual(JCharacterRange(1,26), r.charRange);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	r = buf.CleanWhitespace(JStyledTextBuffer::TextRange(
							JCharacterRange(9,12),
							JUtf8ByteRange(10,13)), kJTrue);
	buf.Undo();
	buf.Redo();
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n    1234\n\twxzy", buf.GetText());
	JAssertEqual(JCharacterRange(8,16), r.charRange);
}

JTEST(AdjustTextIndex)
{
	StyledTextBuffer buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	JStyledTextBuffer::TextIndex index(5,5);
	index = buf.AdjustTextIndex(index, 6);
	JAssertEqual(11, index.charIndex);
	JAssertEqual(12, index.byteIndex);

	index = buf.AdjustTextIndex(index, -7);
	JAssertEqual(4, index.charIndex);
	JAssertEqual(4, index.byteIndex);
}
