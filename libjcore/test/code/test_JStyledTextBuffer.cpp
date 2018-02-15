/******************************************************************************
 test_JStyledTextBuffer.cc

	Program to test JStyledTextBuffer class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <StyledTextBuffer.h>
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

	JBoolean cleaned = buf.SetText(JString("abcd" "\xC3\x86", 0, kJFalse));
	JAssertFalse(cleaned);
	JAssertStringsEqual("abcd" "\xC3\x86", buf.GetText());
	JAssertEqual(5, buf.GetStyles().GetElementCount());
	JAssertEqual(1, buf.GetStyles().GetRunCount());

	cleaned = buf.SetText(JString("a\vb\ac\bd" "\xC3\x86", 0, kJFalse));
	JAssertTrue(cleaned);
	JAssertStringsEqual("abcd" "\xC3\x86", buf.GetText());
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
	JAssertTrue(ok);
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
	JAssertEqual(buf.GetText().GetByteCount() + buf.GetLineCount(), fileSize);

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
	buf.SetFontSize(2, 3, 2*kJDefaultFontSize, kJTrue);
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
	te2.ReadPrivateFormat(input);
	}

	JAssertStringsEqual("\xC3\x86" "bcd", te2.GetText());
	JAssertEqual(4, te2.GetStyles().GetRunCount());

	JFont f = te2.GetFont(1);
	JAssertStringsEqual(JGetDefaultFontName(), f.GetName());
	JAssertEqual(kJDefaultFontSize, f.GetSize());
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = te2.GetFont(2);
	JAssertStringsEqual("foo", f.GetName());
	JAssertEqual(2*kJDefaultFontSize, f.GetSize());
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = te2.GetFont(3);
	JAssertStringsEqual(JGetDefaultFontName(), f.GetName());
	JAssertEqual(2*kJDefaultFontSize, f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = te2.GetFont(4);
	JAssertStringsEqual(JGetDefaultFontName(), f.GetName());
	JAssertEqual(kJDefaultFontSize, f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
}

JTEST(SearchTextForward)
{
	StyledTextBuffer buf;
	buf.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	// entire word, no wrapping

	JBoolean wrapped;
	const JStringMatch m1 = buf.SearchForward(JRegex("sc" "\xC3\xB8" "re"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(buf.HasSelection());	// caret still at beginning
	JAssertFalse(wrapped);

	// partial word, no wrapping

	JString s;
	JCharacterRange r;
	const JStringMatch m2 = buf.SearchForward(JRegex("sc" "\xC3\xB8" "re"), kJFalse, kJFalse, &wrapped);
	JAssertStringsEqual("score", m2.GetString());
	JAssertTrue(buf.HasSelection());
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(5, 9), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("score", s);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m3 = buf.SearchForward(JRegex("\xC3\xB8" "re and"), kJFalse, kJFalse, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertTrue(buf.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(5, 9), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("score", s);
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = buf.SearchForward(JRegex("\xC3\xB8" "re and"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("ore and", m4.GetString());
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(7, 13), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("ore and", s);
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = buf.SearchForward(JRegex("year"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertTrue(buf.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(7, 13), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("ore and", s);
	JAssertFalse(wrapped);

	const JStringMatch m7 = buf.SearchForward(JRegex("years"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("years", m7.GetString());
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// last characters

	buf.SetCaretLocation(buf.GetText().GetCharacterCount() - 2);
	JAssertFalse(buf.HasSelection());
	const JStringMatch m5 = buf.SearchForward(JRegex("\\.\\.\\."), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("...", m5.GetString());
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("...", s);
	JAssertFalse(wrapped);
}

JTEST(SearchTextBackward)
{
	StyledTextBuffer buf;
	buf.SetText(JString("Fourscore and seve" "\xC3\xB1" " years ago...", 0, kJFalse));
	buf.SetCaretLocation(buf.GetText().GetCharacterCount() + 1);

	// entire word, no wrapping

	JBoolean wrapped;
	const JStringMatch m1 = buf.SearchBackward(JRegex("year"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(buf.HasSelection());	// caret still at end
	JAssertFalse(wrapped);

	JString s;
	JCharacterRange r;
	const JStringMatch m2 = buf.SearchBackward(JRegex("years"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("years", m2.GetString());
	JAssertTrue(buf.HasSelection());
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(21, 25), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m3 = buf.SearchBackward(JRegex("e" "\xC3\xB1" " ye"), kJFalse, kJFalse, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertTrue(buf.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(21, 25), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = buf.SearchBackward(JRegex("e" "\xC3\xB1" " ye"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("en ye", m4.GetString());
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("en ye", s);
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = buf.SearchBackward(JRegex("an"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertTrue(buf.GetSelection(&s));		// selection maintained
	JAssertStringsEqual("en ye", s);
	JAssertFalse(wrapped);

	const JStringMatch m7 = buf.SearchBackward(JRegex("and"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("and", m7.GetString());
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("and", s);
	JAssertFalse(wrapped);

	// first characters

	buf.SetCaretLocation(5);
	JAssertFalse(buf.HasSelection());
	const JStringMatch m5 = buf.SearchBackward(JRegex("Four"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("Four", m5.GetString());
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 4), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("Four", s);
	JAssertFalse(wrapped);
}

JTEST(ReplaceAllForward)
{
	StyledTextBuffer buf;
	buf.SetText(JString("Fourscore and seven years ago...", 0, kJFalse));
	const JSize charCount = buf.GetText().GetCharacterCount();

	JBoolean found = buf.ReplaceAllForward(
		JRegex("e"), kJTrue, kJTrue,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertFalse(found);
	JAssertStringsEqual("Fourscore and seven years ago...", buf.GetText());

	found = buf.ReplaceAllForward(
		JRegex("e"), kJFalse, kJTrue,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fourscor\xC3\xA9 and s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", buf.GetText());
	JAssertEqual(charCount, buf.GetText().GetCharacterCount());

	JRegex r1 = "four";
	r1.SetCaseSensitive(kJFalse);
	found = buf.ReplaceAllForward(
		r1, kJFalse, kJFalse,
		JString("five", 0, kJFalse), kJFalse, kJTrue);
	JAssertFalse(found);	// caret at end

	found = buf.ReplaceAllForward(
		r1, kJFalse, kJTrue,
		JString("five", 0, kJFalse), kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 and s" "\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", buf.GetText());

	found = buf.ReplaceAllForward(
		JRegex("a([^\\s]+)"), kJFalse, kJTrue,
		JString("fou$1", 0, kJFalse), kJTrue, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", buf.GetText());

	buf.SetSelection(JCharacterRange(15, 22));
	found = buf.ReplaceAllForward(
		JRegex("\xC3\xA9"), kJFalse, kJFalse,
		JString("e", 0, kJFalse), kJFalse, kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found seven y\xC3\xA9" "ars ago...", buf.GetText());

	buf.SetCaretLocation(buf.GetText().GetCharacterCount() - 5);
	found = buf.ReplaceAllForward(
		JRegex("o"), kJFalse, kJFalse,
		JString("\xC3\xB8", 0, kJFalse), kJFalse, kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found seven y\xC3\xA9" "ars ag\xC3\xB8...", buf.GetText());
}

JTEST(ReplaceAllBackward)
{
	StyledTextBuffer buf;
	buf.SetText(JString("Fourscore and seven years ago...", 0, kJFalse));
	const JSize charCount = buf.GetText().GetCharacterCount();

	JBoolean found = buf.ReplaceAllBackward(
		JRegex("e"), kJTrue, kJTrue,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertFalse(found);
	JAssertStringsEqual("Fourscore and seven years ago...", buf.GetText());

	found = buf.ReplaceAllBackward(
		JRegex("e"), kJFalse, kJFalse,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertFalse(found);
	JAssertStringsEqual("Fourscore and seven years ago...", buf.GetText());

	found = buf.ReplaceAllBackward(
		JRegex("e"), kJFalse, kJTrue,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fourscor\xC3\xA9 and s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", buf.GetText());
	JAssertEqual(charCount, buf.GetText().GetCharacterCount());

	JRegex r1 = "four";
	r1.SetCaseSensitive(kJFalse);
	found = buf.ReplaceAllBackward(
		r1, kJFalse, kJTrue,
		JString("five", 0, kJFalse), kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 and s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", buf.GetText());

	found = buf.ReplaceAllBackward(
		JRegex("a([^\\s]+)"), kJFalse, kJTrue,
		JString("fou$1", 0, kJFalse), kJTrue, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", buf.GetText());

	buf.SetCaretLocation(10);
	found = buf.ReplaceAllBackward(
		JRegex("o"), kJFalse, kJFalse,
		JString("\xC3\xB8", 0, kJFalse), kJFalse, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fivesc\xC3\xB8" "r\xC3\xA9 found seven y\xC3\xA9" "ars ago...", buf.GetText());
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
	buf.SetCurrentFontSize(20);
	buf.Paste(JString("b" "\xC3\xAE" "g", 0, kJFalse));
	buf.SetCurrentFontSize(kJDefaultFontSize);
	buf.SetCurrentFontBold(kJTrue);
	buf.Paste(JString("b" "\xC3\xB8" "ld", 0, kJFalse));
	buf.SetCurrentFontBold(kJFalse);
	buf.Paste(JString("normal", 0, kJFalse));
	buf.SetCurrentFontUnderline(2);
	buf.Paste(JString("double underline", 0, kJFalse));
	buf.SetCurrentFontUnderline(0);

	// forward

	JBoolean wrapped;
	JBoolean found = buf.SearchForward(ItalicFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertFalse(buf.HasSelection());	// caret still at beginning
	JAssertFalse(wrapped);

	JString s;
	JCharacterRange r;
	found = buf.SearchForward(BoldFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.HasSelection());
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(4, 7), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xB8" "ld", s);
	JAssertFalse(wrapped);

	found = buf.SearchForward(UnderlineFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("double underline", s);
	JAssertFalse(wrapped);

	found = buf.SearchForward(BoldFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertTrue(buf.GetSelection(&s));		// selection maintained
	JAssertStringsEqual("double underline", s);
	JAssertFalse(wrapped);

	found = buf.SearchForward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertTrue(wrapped);

	buf.SetCaretLocation(buf.GetText().GetCharacterCount() - 5);
	found = buf.SearchForward(UnderlineFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("erline", s);
	JAssertFalse(wrapped);

	// backward

	buf.SearchBackward(UnderlineFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("double und", s);
	JAssertFalse(wrapped);

	buf.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	found = buf.SearchBackward(BoldFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertTrue(buf.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	found = buf.SearchBackward(BoldFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(4, 7), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xB8" "ld", s);
	JAssertTrue(wrapped);

	buf.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	buf.SetCaretLocation(4);
	buf.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(buf.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(buf.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);
}

void TestItemStartEnd(const JUtf8Byte* s)
{
	StyledTextBuffer buf;
	buf.SetText(JString(s, 0, kJFalse));

	JAssertEqual(1, GetWordStart(0, 0));
	JAssertEqual(1, GetWordStart(1, 1));
	JAssertEqual(1, GetWordStart(3, 3));
	JAssertEqual(1, GetWordStart(5, 5));
	JAssertEqual(6, GetWordStart(6, 6));
	JAssertEqual(6, GetWordStart(8, 8));
	JAssertEqual(6, GetWordStart(10, 11));

	JAssertEqual(4, GetWordEnd(1, 1));
	JAssertEqual(4, GetWordEnd(3, 3));
	JAssertEqual(4, GetWordEnd(4, 4));
	JAssertEqual(10, GetWordEnd(5, 5));
	JAssertEqual(10, GetWordEnd(8, 8));
	JAssertEqual(10, GetWordEnd(9, 10));
	JAssertEqual(buf.GetText().GetCharacterCount(), GetWordEnd(buf.GetText().GetCharacterCount()+1, buf.GetText().GetByteCount()+1));

	JAssertEqual(6, GetWordStart(11, 12));
	JAssertEqual(6, GetWordStart(12, 13));
	JAssertEqual(6, GetWordStart(13, 14));

	JAssertEqual(16, GetWordEnd(11, 12));
	JAssertEqual(16, GetWordEnd(12, 13));
	JAssertEqual(16, GetWordEnd(13, 14));

	JBoolean wrapped;
	const JStringMatch m1 = buf.SearchForward(JRegex("PRoP\xC3\xB8s1tion"), kJFalse, kJFalse, &wrapped);
	JAssertFalse(m1.IsEmpty());
	JAssertEqual(11, m1.GetCharacterCount());

	JCharacterRange charRange = m1.GetCharacterRange();
	JUtf8ByteRange  byteRange = m1.GetUtf8ByteRange();
	JAssertEqual(charRange.first, GetWordStart(charRange.first+3, byteRange.first+3));
	JAssertEqual(charRange.last, GetWordEnd(charRange.first+3, byteRange.first+3));

	JAssertEqual(1, GetPartialWordStart(0, 0));
	JAssertEqual(charRange.last-3, GetPartialWordStart(charRange.last, byteRange.last));
	JAssertEqual(charRange.last-4, GetPartialWordStart(charRange.last-3, byteRange.last-3));
	JAssertEqual(charRange.last-7, GetPartialWordStart(charRange.last-4, byteRange.last-4));
	JAssertEqual(charRange.last-9, GetPartialWordStart(charRange.last-7, byteRange.last-8));
	JAssertEqual(charRange.last-10, GetPartialWordStart(charRange.last-9, byteRange.last-10));

	JAssertEqual(charRange.first, GetPartialWordEnd(charRange.first, byteRange.first));
	JAssertEqual(charRange.first+2, GetPartialWordEnd(charRange.first+1, byteRange.first+1));
	JAssertEqual(charRange.first+5, GetPartialWordEnd(charRange.first+3, byteRange.first+3));
	JAssertEqual(charRange.first+5, GetPartialWordEnd(charRange.first+4, byteRange.first+4));
	JAssertEqual(charRange.first+6, GetPartialWordEnd(charRange.first+6, byteRange.first+7));
	JAssertEqual(charRange.first+10, GetPartialWordEnd(charRange.first+7, byteRange.first+8));
	JAssertEqual(buf.GetText().GetCharacterCount(), GetPartialWordEnd(buf.GetText().GetCharacterCount()+1, buf.GetText().GetByteCount()+1));

	const JStringMatch m2 = buf.SearchForward(JRegex("and dedicated"), kJFalse, kJFalse, &wrapped);
	JAssertFalse(m1.IsEmpty());

	JAssertEqual(1, GetParagraphStart(0, 0));
	JAssertEqual(1, GetParagraphStart(13, 14));
	JAssertEqual(m2.GetCharacterRange().first, GetParagraphStart(m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first));

	JAssertEqual(37, GetParagraphEnd(1, 1));
	JAssertEqual(37, GetParagraphEnd(13, 14));
	JAssertEqual(buf.GetText().GetCharacterCount(), GetParagraphEnd(m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first));
	JAssertEqual(buf.GetText().GetCharacterCount(), GetParagraphEnd(buf.GetText().GetCharacterCount()+1, buf.GetText().GetByteCount()+1));
}

JTEST(ItemStartEnd)
{
	TestItemStartEnd(
		"Four sc\xC3\xB8" "re - and seven years ago our\n"
		"fathers brought forth on this continent,\n"
		"a new nation, conceived in Liberty,\n"
		"and dedicated to the PRoP\xC3\xB8" "s1tion that all men are created equal.");

	TestItemStartEnd(
		"Four sc\xC3\xB8" "re and seven years ago our\n"
		"fathers brought forth on this continent,\n"
		"a new nation, conceived in Liberty,\n"
		"and dedicated to the PRoP\xC3\xB8" "s1tion that all men are created equal.\n");
}

JTEST(Selection)
{
	StyledTextBuffer buf;
	buf.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", 0, kJFalse));

	buf.SetSelection(1, 5);
	buf.Cut();
	JAssertStringsEqual("1234\nwxzy", buf.GetText());

	buf.SetCaretLocation(10);
	buf.Paste(JString("\n", 0, kJFalse));
	buf.Paste();
	JAssertStringsEqual("1234\nwxzy\n" "\xC3\xA1" "bcd\n", buf.GetText());

	buf.SetSelection(6, 10);
	buf.DeleteSelection();
	JAssertStringsEqual("1234\n" "\xC3\xA1" "bcd\n", buf.GetText());

	buf.SelectAll();
	buf.DeleteSelection();
	JAssertEqual(0, buf.GetText().GetCharacterCount());
}

JTEST(TabSelection)
{
	StyledTextBuffer buf;
	buf.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", 0, kJFalse));

	buf.TabSelectionRight(2);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	buf.SetSelection(10, 13);
	buf.TabSelectionLeft(3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	buf.TabSelectionRight(3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", buf.GetText());

	buf.SelectAll();
	buf.TabSelectionLeft(1);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", buf.GetText());

	buf.TabSelectionLeft(2);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", buf.GetText());

	buf.TabSelectionLeft(2, kJTrue);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());
}

JTEST(TabSelectionMixed)
{
	StyledTextBuffer buf;
	buf.SetText(JString("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));

	buf.TabSelectionLeft(1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());

	buf.Undo();
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", buf.GetText());

	buf.Redo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", buf.GetText());


	buf.SetText(JString("  " "\xC3\xA1" "bcd\n   1234\n    wxzy", 0, kJFalse));

	buf.TabSelectionLeft(1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n 1234\n  wxzy", buf.GetText());
}

JTEST(CleanWhitespaceTabs)
{
	StyledTextBuffer buf;
	buf.TabShouldInsertSpaces(kJTrue);
	buf.SetCRMTabCharCount(4);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	buf.CleanAllWhitespace(kJFalse);
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	buf.CleanAllWhitespace(kJTrue);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	buf.CleanWhitespace(JCharacterRange(9, 12), kJTrue);
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n\t1234\n\twxzy", buf.GetText());

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n    \t1234\n\twxzy", 0, kJFalse));
	buf.CleanAllWhitespace(kJTrue);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\twxzy", buf.GetText());
}

JTEST(CleanWhitespaceSpaces)
{
	StyledTextBuffer buf;
	buf.TabShouldInsertSpaces(kJFalse);
	buf.SetCRMTabCharCount(4);

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	buf.CleanAllWhitespace(kJFalse);
	JAssertStringsEqual("     " "\xC3\xA1" "bcd\n    1234\n    wxzy", buf.GetText());

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	buf.CleanAllWhitespace(kJTrue);
	JAssertStringsEqual("    " "\xC3\xA1" "bcd\n    1234\n    wxzy", buf.GetText());

	buf.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));
	buf.CleanWhitespace(JCharacterRange(9, 12), kJTrue);
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n    1234\n\twxzy", buf.GetText());
}
