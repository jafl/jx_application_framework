/******************************************************************************
 test_JTextEditor.cc

	Program to test JTextEditor class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <TextEditor.h>
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
	TextEditor te;
	JAssertTrue(te.IsEmpty());

	JBoolean cleaned = te.SetText(JString("abcd" "\xC3\x86", 0, kJFalse));
	JAssertFalse(cleaned);
	JAssertStringsEqual("abcd" "\xC3\x86", te.GetText());
	JAssertEqual(5, te.GetStyles().GetElementCount());
	JAssertEqual(1, te.GetStyles().GetRunCount());

	cleaned = te.SetText(JString("a\vb\ac\bd" "\xC3\x86", 0, kJFalse));
	JAssertTrue(cleaned);
	JAssertStringsEqual("abcd" "\xC3\x86", te.GetText());
}

JTEST(ReadPlainText)
{
	TextEditor te;

	JTextEditor::PlainTextFormat format;
	JBoolean ok = te.ReadPlainText(JString("data/test_ReadPlainUNIXText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kUNIXText, format);
	JAssertTrue(te.EndsWithNewline());
	JAssertEqual(47, te.GetStyles().GetElementCount());
	JAssertEqual(1, te.GetStyles().GetRunCount());

	const JString s = te.GetText();

	ok = te.ReadPlainText(JString("data/test_ReadPlainDOSText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kDOSText, format);
	JAssertStringsEqual(s, te.GetText());
	JAssertEqual(47, te.GetStyles().GetElementCount());
	JAssertEqual(1, te.GetStyles().GetRunCount());

	ok = te.ReadPlainText(JString("data/test_ReadPlainMacText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kMacintoshText, format);
	JAssertStringsEqual(s, te.GetText());
	JAssertEqual(47, te.GetStyles().GetElementCount());
	JAssertEqual(1, te.GetStyles().GetRunCount());

	ok = te.ReadPlainText(JString("data/test_ReadPlainBinaryText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kUNIXText, format);
	JAssertStringsEqual(s, te.GetText());
	JAssertEqual(47, te.GetStyles().GetElementCount());
	JAssertEqual(1, te.GetStyles().GetRunCount());

	ok = te.ReadPlainText(JString("data/test_ReadPlainBinaryText.txt", 0, kJFalse), &format, kJFalse);
	JAssertFalse(ok);
}

JTEST(WritePlainText)
{
	TextEditor te;

	JTextEditor::PlainTextFormat format;
	JBoolean ok = te.ReadPlainText(JString("data/test_ReadPlainUNIXText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kUNIXText, format);

	JString fileName;
	JSize fileSize;
	JString s;
	JAssertOK(JCreateTempFile(&fileName));

	te.WritePlainText(fileName, JTextEditor::kUNIXText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(te.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\n"));
	JAssertFalse(s.Contains("\r"));

	te.WritePlainText(fileName, JTextEditor::kDOSText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(te.GetText().GetByteCount() + te.GetLineCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r\n"));

	te.WritePlainText(fileName, JTextEditor::kMacintoshText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(te.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r"));
	JAssertFalse(s.Contains("\n"));

	JRemoveFile(fileName);
}

JTEST(ReadWritePrivateFormat)
{
	TextEditor te;
	te.SetText(JString("\xC3\x86" "bcd", 0, kJFalse));
	te.SetFontName(2, 2, JString("foo", 0, kJFalse), kJTrue);
	te.SetFontSize(2, 3, 2*kJDefaultFontSize, kJTrue);
	te.SetFontBold(1, 2, kJTrue, kJTrue);
	JAssertEqual(4, te.GetStyles().GetRunCount());

	JString fileName;
	JAssertOK(JCreateTempFile(&fileName));

	{
	std::ofstream output(fileName.GetBytes());
	te.WritePrivateFormat(output);
	}

	TextEditor te2;
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
	TextEditor te;
	te.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	// entire word, no wrapping

	JBoolean wrapped;
	const JStringMatch m1 = te.SearchForward(JRegex("sc" "\xC3\xB8" "re"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(te.HasSelection());	// caret still at beginning
	JAssertFalse(wrapped);

	// partial word, no wrapping

	JString s;
	JCharacterRange r;
	const JStringMatch m2 = te.SearchForward(JRegex("sc" "\xC3\xB8" "re"), kJFalse, kJFalse, &wrapped);
	JAssertStringsEqual("score", m2.GetString());
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(5, 9), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("score", s);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m3 = te.SearchForward(JRegex("\xC3\xB8" "re and"), kJFalse, kJFalse, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(5, 9), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("score", s);
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = te.SearchForward(JRegex("\xC3\xB8" "re and"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("ore and", m4.GetString());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(7, 13), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("ore and", s);
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = te.SearchForward(JRegex("year"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(7, 13), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("ore and", s);
	JAssertFalse(wrapped);

	const JStringMatch m7 = te.SearchForward(JRegex("years"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("years", m7.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// last characters

	te.SetCaretLocation(te.GetText().GetCharacterCount() - 2);
	JAssertFalse(te.HasSelection());
	const JStringMatch m5 = te.SearchForward(JRegex("\\.\\.\\."), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("...", m5.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("...", s);
	JAssertFalse(wrapped);
}

JTEST(SearchTextBackward)
{
	TextEditor te;
	te.SetText(JString("Fourscore and seve" "\xC3\xB1" " years ago...", 0, kJFalse));
	te.SetCaretLocation(te.GetText().GetCharacterCount() + 1);

	// entire word, no wrapping

	JBoolean wrapped;
	const JStringMatch m1 = te.SearchBackward(JRegex("year"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(te.HasSelection());	// caret still at end
	JAssertFalse(wrapped);

	JString s;
	JCharacterRange r;
	const JStringMatch m2 = te.SearchBackward(JRegex("years"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("years", m2.GetString());
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(21, 25), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m3 = te.SearchBackward(JRegex("e" "\xC3\xB1" " ye"), kJFalse, kJFalse, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(21, 25), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = te.SearchBackward(JRegex("e" "\xC3\xB1" " ye"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("en ye", m4.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("en ye", s);
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = te.SearchBackward(JRegex("an"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertTrue(te.GetSelection(&s));		// selection maintained
	JAssertStringsEqual("en ye", s);
	JAssertFalse(wrapped);

	const JStringMatch m7 = te.SearchBackward(JRegex("and"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("and", m7.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("and", s);
	JAssertFalse(wrapped);

	// first characters

	te.SetCaretLocation(5);
	JAssertFalse(te.HasSelection());
	const JStringMatch m5 = te.SearchBackward(JRegex("Four"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("Four", m5.GetString());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 4), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("Four", s);
	JAssertFalse(wrapped);
}

JTEST(ReplaceAllForward)
{
	TextEditor te;
	te.SetText(JString("Fourscore and seven years ago...", 0, kJFalse));
	const JSize charCount = te.GetText().GetCharacterCount();

	JBoolean found = te.ReplaceAllForward(
		JRegex("e"), kJTrue, kJTrue,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertFalse(found);
	JAssertStringsEqual("Fourscore and seven years ago...", te.GetText());

	found = te.ReplaceAllForward(
		JRegex("e"), kJFalse, kJTrue,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fourscor\xC3\xA9 and s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", te.GetText());
	JAssertEqual(charCount, te.GetText().GetCharacterCount());

	JRegex r1 = "four";
	r1.SetCaseSensitive(kJFalse);
	found = te.ReplaceAllForward(
		r1, kJFalse, kJFalse,
		JString("five", 0, kJFalse), kJFalse, kJTrue);
	JAssertFalse(found);	// caret at end

	found = te.ReplaceAllForward(
		r1, kJFalse, kJTrue,
		JString("five", 0, kJFalse), kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 and s" "\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", te.GetText());

	found = te.ReplaceAllForward(
		JRegex("a([^\\s]+)"), kJFalse, kJTrue,
		JString("fou$1", 0, kJFalse), kJTrue, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", te.GetText());

	te.SetSelection(JCharacterRange(15, 22));
	found = te.ReplaceAllForward(
		JRegex("\xC3\xA9"), kJFalse, kJFalse,
		JString("e", 0, kJFalse), kJFalse, kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found seven y\xC3\xA9" "ars ago...", te.GetText());

	te.SetCaretLocation(te.GetText().GetCharacterCount() - 5);
	found = te.ReplaceAllForward(
		JRegex("o"), kJFalse, kJFalse,
		JString("\xC3\xB8", 0, kJFalse), kJFalse, kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found seven y\xC3\xA9" "ars ag\xC3\xB8...", te.GetText());
}

JTEST(ReplaceAllBackward)
{
	TextEditor te;
	te.SetText(JString("Fourscore and seven years ago...", 0, kJFalse));
	const JSize charCount = te.GetText().GetCharacterCount();

	JBoolean found = te.ReplaceAllBackward(
		JRegex("e"), kJTrue, kJTrue,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertFalse(found);
	JAssertStringsEqual("Fourscore and seven years ago...", te.GetText());

	found = te.ReplaceAllBackward(
		JRegex("e"), kJFalse, kJFalse,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertFalse(found);
	JAssertStringsEqual("Fourscore and seven years ago...", te.GetText());

	found = te.ReplaceAllBackward(
		JRegex("e"), kJFalse, kJTrue,
		JString("\xC3\xA9", 0, kJFalse), kJFalse, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fourscor\xC3\xA9 and s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", te.GetText());
	JAssertEqual(charCount, te.GetText().GetCharacterCount());

	JRegex r1 = "four";
	r1.SetCaseSensitive(kJFalse);
	found = te.ReplaceAllBackward(
		r1, kJFalse, kJTrue,
		JString("five", 0, kJFalse), kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 and s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", te.GetText());

	found = te.ReplaceAllBackward(
		JRegex("a([^\\s]+)"), kJFalse, kJTrue,
		JString("fou$1", 0, kJFalse), kJTrue, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", te.GetText());

	te.SetCaretLocation(10);
	found = te.ReplaceAllBackward(
		JRegex("o"), kJFalse, kJFalse,
		JString("\xC3\xB8", 0, kJFalse), kJFalse, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fivesc\xC3\xB8" "r\xC3\xA9 found seven y\xC3\xA9" "ars ago...", te.GetText());
}

class BigFontMatch : public JTextEditor::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return JI2B( f.GetSize() == 20 );
	}
};

class BoldFontMatch : public JTextEditor::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return f.GetStyle().bold;
	}
};

class ItalicFontMatch : public JTextEditor::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return f.GetStyle().italic;
	}
};

class UnderlineFontMatch : public JTextEditor::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return JI2B( f.GetStyle().underlineCount > 0 );
	}
};

JTEST(SearchStyle)
{
	TextEditor te;
	te.SetCurrentFontSize(20);
	te.Paste(JString("b" "\xC3\xAE" "g", 0, kJFalse));
	te.SetCurrentFontSize(kJDefaultFontSize);
	te.SetCurrentFontBold(kJTrue);
	te.Paste(JString("b" "\xC3\xB8" "ld", 0, kJFalse));
	te.SetCurrentFontBold(kJFalse);
	te.Paste(JString("normal", 0, kJFalse));
	te.SetCurrentFontUnderline(2);
	te.Paste(JString("double underline", 0, kJFalse));
	te.SetCurrentFontUnderline(0);

	// forward

	JBoolean wrapped;
	JBoolean found = te.SearchForward(ItalicFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertFalse(te.HasSelection());	// caret still at beginning
	JAssertFalse(wrapped);

	JString s;
	JCharacterRange r;
	found = te.SearchForward(BoldFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(4, 7), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xB8" "ld", s);
	JAssertFalse(wrapped);

	found = te.SearchForward(UnderlineFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("double underline", s);
	JAssertFalse(wrapped);

	found = te.SearchForward(BoldFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertTrue(te.GetSelection(&s));		// selection maintained
	JAssertStringsEqual("double underline", s);
	JAssertFalse(wrapped);

	found = te.SearchForward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertTrue(wrapped);

	te.SetCaretLocation(te.GetText().GetCharacterCount() - 5);
	found = te.SearchForward(UnderlineFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("erline", s);
	JAssertFalse(wrapped);

	// backward

	te.SearchBackward(UnderlineFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("double und", s);
	JAssertFalse(wrapped);

	te.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	found = te.SearchBackward(BoldFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	found = te.SearchBackward(BoldFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(4, 7), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xB8" "ld", s);
	JAssertTrue(wrapped);

	te.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	te.SetCaretLocation(4);
	te.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);
}

void TestItemStartEnd(const JUtf8Byte* s)
{
	TextEditor te;
	te.SetText(JString(s, 0, kJFalse));

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
	JAssertEqual(te.GetText().GetCharacterCount(), GetWordEnd(te.GetText().GetCharacterCount()+1, te.GetText().GetByteCount()+1));

	JAssertEqual(6, GetWordStart(11, 12));
	JAssertEqual(6, GetWordStart(12, 13));
	JAssertEqual(6, GetWordStart(13, 14));

	JAssertEqual(16, GetWordEnd(11, 12));
	JAssertEqual(16, GetWordEnd(12, 13));
	JAssertEqual(16, GetWordEnd(13, 14));

	JBoolean wrapped;
	const JStringMatch m1 = te.SearchForward(JRegex("PRoP\xC3\xB8s1tion"), kJFalse, kJFalse, &wrapped);
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
	JAssertEqual(te.GetText().GetCharacterCount(), GetPartialWordEnd(te.GetText().GetCharacterCount()+1, te.GetText().GetByteCount()+1));

	const JStringMatch m2 = te.SearchForward(JRegex("and dedicated"), kJFalse, kJFalse, &wrapped);
	JAssertFalse(m1.IsEmpty());

	JAssertEqual(1, GetParagraphStart(0, 0));
	JAssertEqual(1, GetParagraphStart(13, 14));
	JAssertEqual(m2.GetCharacterRange().first, GetParagraphStart(m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first));

	JAssertEqual(37, GetParagraphEnd(1, 1));
	JAssertEqual(37, GetParagraphEnd(13, 14));
	JAssertEqual(te.GetText().GetCharacterCount(), GetParagraphEnd(m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first));
	JAssertEqual(te.GetText().GetCharacterCount(), GetParagraphEnd(te.GetText().GetCharacterCount()+1, te.GetText().GetByteCount()+1));
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
	TextEditor te;
	te.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", 0, kJFalse));

	te.SetSelection(1, 5);
	te.Cut();

	JAssertStringsEqual("1234\nwxzy", te.GetText());

	te.SetCaretLocation(10);
	te.Paste(JString("\n", 0, kJFalse));
	te.Paste();

	JAssertStringsEqual("1234\nwxzy\n" "\xC3\xA1" "bcd\n", te.GetText());

	te.SetSelection(6, 10);
	te.DeleteSelection();

	JAssertStringsEqual("1234\n" "\xC3\xA1" "bcd\n", te.GetText());

	te.SelectAll();
	te.DeleteSelection();

	JAssertEqual(0, te.GetText().GetCharacterCount());
}

JTEST(TabSelection)
{
	TextEditor te;
	te.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", 0, kJFalse));

	te.TabSelectionRight(2);

	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", te.GetText());

	te.SetSelection(10, 13);
	te.TabSelectionLeft(3);

	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", te.GetText());

	te.TabSelectionRight(3);

	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", te.GetText());

	te.SelectAll();
	te.TabSelectionLeft(1);

	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", te.GetText());

	te.TabSelectionLeft(2);

	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", te.GetText());

	te.TabSelectionLeft(2, kJTrue);

	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", te.GetText());
}

JTEST(TabSelectionMixed)
{
	TextEditor te;
	te.SetText(JString("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));

	te.TabSelectionLeft(1);

	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", te.GetText());

	te.Undo();

	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", te.GetText());

	te.Redo();

	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", te.GetText());


	te.SetText(JString("  " "\xC3\xA1" "bcd\n   1234\n    wxzy", 0, kJFalse));

	te.TabSelectionLeft(1);

	JAssertStringsEqual("\xC3\xA1" "bcd\n 1234\n  wxzy", te.GetText());
}
