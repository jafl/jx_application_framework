/******************************************************************************
 test_JStyledText.cpp

	Program to test JStyledText class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JBroadcastTester.h>
#include "StyledText.h"
#include <JStringIterator.h>
#include <JInterpolate.h>
#include <JRegex.h>
#include <JFontManager.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

typedef JStyledText::TextIndex TextIndex;
typedef JStyledText::TextCount TextCount;
typedef JStyledText::TextRange TextRange;

int main()
{
	return JTestManager::Execute();
}

JTEST(SetText)
{
	StyledText text;
	JAssertTrue(text.IsEmpty());

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextSet);

	bool ok = text.SetText(JString("abcd" "\xC3\x86", JString::kNoCopy));
	JAssertTrue(ok);
	JAssertStringsEqual("abcd" "\xC3\x86", text.GetText());
	JAssertEqual(5, text.GetStyles().GetElementCount());
	JAssertEqual(1, text.GetStyles().GetRunCount());

	bcastTest.Expect(JStyledText::kTextSet);

	ok = text.SetText(JString("a\vb\ac\bd" "\xC3\x86", JString::kNoCopy));
	JAssertFalse(ok);
	JAssertStringsEqual("abcd" "\xC3\x86", text.GetText());
}

void TestItemStartEnd(const JUtf8Byte* s)
{
	StyledText text;
	text.SetText(JString(s, JString::kNoCopy));

	JAssertEqual(1, text.GetWordStart(0, 0).charIndex);
	JAssertEqual(1, text.GetWordStart(1, 1).charIndex);
	JAssertEqual(1, text.GetWordStart(3, 3).charIndex);
	JAssertEqual(1, text.GetWordStart(5, 5).charIndex);
	JAssertEqual(6, text.GetWordStart(6, 6).charIndex);
	JAssertEqual(6, text.GetWordStart(8, 8).charIndex);
	JAssertEqual(6, text.GetWordStart(10, 11).charIndex);

	JAssertEqual(4, text.GetWordEnd(1, 1).charIndex);
	JAssertEqual(4, text.GetWordEnd(3, 3).charIndex);
	JAssertEqual(4, text.GetWordEnd(4, 4).charIndex);
	JAssertEqual(10, text.GetWordEnd(5, 5).charIndex);
	JAssertEqual(10, text.GetWordEnd(8, 8).charIndex);
	JAssertEqual(10, text.GetWordEnd(9, 10).charIndex);
	JAssertEqual(text.GetText().GetCharacterCount(), text.GetWordEnd(text.GetText().GetCharacterCount()+1, text.GetText().GetByteCount()+1).charIndex);

	JAssertEqual(6, text.GetWordStart(11, 12).charIndex);
	JAssertEqual(6, text.GetWordStart(12, 13).charIndex);
	JAssertEqual(6, text.GetWordStart(13, 14).charIndex);

	JAssertEqual(16, text.GetWordEnd(11, 12).charIndex);
	JAssertEqual(16, text.GetWordEnd(12, 13).charIndex);
	JAssertEqual(16, text.GetWordEnd(13, 14).charIndex);

	const TextIndex first(1,1);

	bool wrapped;
	const JStringMatch m1 = text.SearchForward(first, JRegex("PRoP\xC3\xB8s1tion"), false, false, &wrapped);
	JAssertFalse(m1.IsEmpty());
	JAssertEqual(11, m1.GetCharacterCount());

	const JCharacterRange charRange = m1.GetCharacterRange();
	const JUtf8ByteRange  byteRange = m1.GetUtf8ByteRange();
	JAssertEqual(charRange.first, text.GetWordStart(charRange.first+3, byteRange.first+3).charIndex);
	JAssertEqual(charRange.last, text.GetWordEnd(charRange.first+3, byteRange.first+3).charIndex);

	JAssertEqual(1, text.GetPartialWordStart(0, 0).charIndex);
	JAssertEqual(charRange.last-3, text.GetPartialWordStart(charRange.last, byteRange.last).charIndex);
	JAssertEqual(charRange.last-3, text.GetPartialWordStart(charRange.last-3, byteRange.last-3).charIndex);
	JAssertEqual(charRange.last-4, text.GetPartialWordStart(charRange.last-4, byteRange.last-4).charIndex);
	JAssertEqual(charRange.last-7, text.GetPartialWordStart(charRange.last-5, byteRange.last-5).charIndex);
	JAssertEqual(charRange.last-9, text.GetPartialWordStart(charRange.last-8, byteRange.last-9).charIndex);
	JAssertEqual(charRange.last-10, text.GetPartialWordStart(charRange.last-9, byteRange.last-10).charIndex);

	JAssertEqual(charRange.first, text.GetPartialWordEnd(charRange.first, byteRange.first).charIndex);
	JAssertEqual(charRange.first+2, text.GetPartialWordEnd(charRange.first+1, byteRange.first+1).charIndex);
	JAssertEqual(charRange.first+5, text.GetPartialWordEnd(charRange.first+3, byteRange.first+3).charIndex);
	JAssertEqual(charRange.first+5, text.GetPartialWordEnd(charRange.first+4, byteRange.first+4).charIndex);
	JAssertEqual(charRange.first+6, text.GetPartialWordEnd(charRange.first+6, byteRange.first+7).charIndex);
	JAssertEqual(charRange.first+10, text.GetPartialWordEnd(charRange.first+7, byteRange.first+8).charIndex);
	JAssertEqual(text.GetText().GetCharacterCount(), text.GetPartialWordEnd(text.GetText().GetCharacterCount()+1, text.GetText().GetByteCount()+1).charIndex);

	const JStringMatch m2 = text.SearchForward(first, JRegex("and dedicated"), false, false, &wrapped);
	JAssertFalse(m1.IsEmpty());

	JAssertEqual(1, text.GetParagraphStart(0, 0).charIndex);
	JAssertEqual(1, text.GetParagraphStart(13, 14).charIndex);
	JAssertEqual(m2.GetCharacterRange().first, text.GetParagraphStart(m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first).charIndex);

	JAssertEqual(37, text.GetParagraphEnd(1, 1).charIndex);
	JAssertEqual(37, text.GetParagraphEnd(13, 14).charIndex);
	JAssertEqual(text.GetText().GetCharacterCount(), text.GetParagraphEnd(m1.GetCharacterRange().first, m1.GetUtf8ByteRange().first).charIndex);
	JAssertEqual(text.GetText().GetCharacterCount(), text.GetParagraphEnd(text.GetText().GetCharacterCount()+1, text.GetText().GetByteCount()+1).charIndex);
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

	StyledText text;
	text.SetText(JString(
		"\xC3\x91""\xC3\xAE""\xC3\xA7""\xC3\xA9"
		"\xC3\x91""\xC3\xA5""\xC5\x93""\xC3\xB8", JString::kNoCopy));

	TextIndex i = text.GetWordStart(3,5);
	JAssertEqual(1, i.byteIndex);

	i = text.GetWordEnd(3,5);
	JAssertEqual(15, i.byteIndex);

	i = text.GetPartialWordStart(3,5);
	JAssertEqual(1, i.byteIndex);

	i = text.GetPartialWordStart(7,15);
	JAssertEqual(9, i.byteIndex);

	i = text.GetPartialWordEnd(3,5);
	JAssertEqual(7, i.byteIndex);

	i = text.GetPartialWordEnd(7,15);
	JAssertEqual(15, i.byteIndex);

	i = text.GetParagraphStart(3,5);
	JAssertEqual(1, i.byteIndex);

	i = text.GetParagraphEnd(3,5);
	JAssertEqual(15, i.byteIndex);
}

JTEST(ColumnForChar)
{
	StyledText text;
	text.SetText(JString("\xC3\xA1" "bcd\n\t1234\n  \twxzy", JString::kNoCopy));

	JIndex col = text.GetColumnForChar(TextIndex(1,1), TextIndex(1,1));
	JAssertEqual(1, col);

	col = text.GetColumnForChar(TextIndex(1,1), TextIndex(2,3));
	JAssertEqual(2, col);

	col = text.GetColumnForChar(TextIndex(6,7), TextIndex(6,7));
	JAssertEqual(1, col);

	col = text.GetColumnForChar(TextIndex(6,7), TextIndex(8,9));
	JAssertEqual(10, col);

	col = text.GetColumnForChar(TextIndex(12,13), TextIndex(12,13));
	JAssertEqual(1, col);

	col = text.GetColumnForChar(TextIndex(12,13), TextIndex(16,17));
	JAssertEqual(10, col);

	col = text.GetColumnForChar(TextIndex(12,13), TextIndex(19,20));
}

JTEST(ReadPlainText)
{
	StyledText text;

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextSet);

	JStyledText::PlainTextFormat format;
	bool ok = text.ReadPlainText(JString("data/test_ReadPlainUNIXText.txt", JString::kNoCopy), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledText::kUNIXText, format);
	JAssertTrue(text.EndsWithNewline());
	JAssertEqual(47, text.GetStyles().GetElementCount());
	JAssertEqual(1, text.GetStyles().GetRunCount());

	const JString s = text.GetText();

	bcastTest.Expect(JStyledText::kWillBeBusy);
	bcastTest.Expect(JStyledText::kTextSet);

	ok = text.ReadPlainText(JString("data/test_ReadPlainDOSText.txt", JString::kNoCopy), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledText::kDOSText, format);
	JAssertStringsEqual(s, text.GetText());
	JAssertEqual(47, text.GetStyles().GetElementCount());
	JAssertEqual(1, text.GetStyles().GetRunCount());

	bcastTest.Expect(JStyledText::kTextSet);

	ok = text.ReadPlainText(JString("data/test_ReadPlainMacText.txt", JString::kNoCopy), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledText::kMacintoshText, format);
	JAssertStringsEqual(s, text.GetText());
	JAssertEqual(47, text.GetStyles().GetElementCount());
	JAssertEqual(1, text.GetStyles().GetRunCount());

	bcastTest.Expect(JStyledText::kTextSet);

	ok = text.ReadPlainText(JString("data/test_ReadPlainBinaryText.txt", JString::kNoCopy), &format);
	JAssertFalse(ok);
	JAssertEqual(JStyledText::kUNIXText, format);
	JAssertStringsEqual(s, text.GetText());
	JAssertEqual(47, text.GetStyles().GetElementCount());
	JAssertEqual(1, text.GetStyles().GetRunCount());

	ok = text.ReadPlainText(JString("data/test_ReadPlainBinaryText.txt", JString::kNoCopy), &format, false);
	JAssertFalse(ok);
}

JTEST(WritePlainText)
{
	StyledText text;

	JStyledText::PlainTextFormat format;
	bool ok = text.ReadPlainText(JString("data/test_ReadPlainUNIXText.txt", JString::kNoCopy), &format);
	JAssertTrue(ok);
	JAssertEqual(JStyledText::kUNIXText, format);

	JString fileName;
	JSize fileSize;
	JString s;
	JAssertOK(JCreateTempFile(&fileName));

	text.WritePlainText(fileName, JStyledText::kUNIXText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(text.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\n"));
	JAssertFalse(s.Contains("\r"));

	text.WritePlainText(fileName, JStyledText::kDOSText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(text.GetText().GetByteCount()+2, fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r\n"));

	text.WritePlainText(fileName, JStyledText::kMacintoshText);
	JAssertOK(JGetFileLength(fileName, &fileSize));
	JAssertEqual(text.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r"));
	JAssertFalse(s.Contains("\n"));

	JRemoveFile(fileName);
}

void
TestReadPrivateFormat
	(
	const JString& fileName
	)
{
	StyledText text;
	{
	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kWillBeBusy);
	bcastTest.Expect(JStyledText::kTextSet);

	std::ifstream input(fileName.GetBytes());
	text.ReadPrivateFormat(input);
	}

	JAssertStringsEqual("\xC3\x86" "bcd", text.GetText());
	JAssertEqual(4, text.GetStyles().GetRunCount());

	JFont f = text.GetFont(1);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());
	JAssertEqual(JFontManager::GetDefaultFontSize(), f.GetSize());
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = text.GetFont(2);
	JAssertStringsEqual("foo", f.GetName());
	JAssertEqual(2*JFontManager::GetDefaultFontSize(), f.GetSize());
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = text.GetFont(3);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());
	JAssertEqual(2*JFontManager::GetDefaultFontSize(), f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = text.GetFont(4);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());
	JAssertEqual(JFontManager::GetDefaultFontSize(), f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
}

JTEST(ReadWritePrivateFormat)
{
	StyledText text1;
	text1.SetText(JString("\xC3\x86" "bcd", JString::kNoCopy));
	text1.SetFontName(TextRange(JCharacterRange(2,2), JUtf8ByteRange(3,3)), JString("foo", JString::kNoCopy), true);
	text1.SetFontSize(TextRange(JCharacterRange(2,3), JUtf8ByteRange(3,4)), 2*JFontManager::GetDefaultFontSize(), true);
	text1.SetFontBold(TextRange(JCharacterRange(1,2), JUtf8ByteRange(1,3)), true, true);
	JAssertEqual(4, text1.GetStyles().GetRunCount());

	JString fileName;
	JAssertOK(JCreateTempFile(&fileName));

	{
	std::ofstream output(fileName.GetBytes());
	text1.WritePrivateFormat(output);
	}
	TestReadPrivateFormat(fileName);

	{
	std::ofstream output(fileName.GetBytes());
	text1.WritePrivateFormat(output, 1, text1.GetText(), text1.GetStyles(),
							 JCharacterRange(1,4));
	}
	TestReadPrivateFormat(fileName);

	StyledText text2;
	text2.SetText(JString("zing\xC3\xA5\xE2\x9C\x94 \xC3\x86" "bcd goop", JString::kNoCopy));
	text2.SetFontName(TextRange(JCharacterRange(9,9), JUtf8ByteRange(13,13)), JString("foo", JString::kNoCopy), true);
	text2.SetFontSize(TextRange(JCharacterRange(9,10), JUtf8ByteRange(13,14)), 2*JFontManager::GetDefaultFontSize(), true);
	text2.SetFontBold(TextRange(JCharacterRange(8,9), JUtf8ByteRange(11,13)), true, true);
	JAssertEqual(5, text2.GetStyles().GetRunCount());

	{
	std::ofstream output(fileName.GetBytes());
	text2.WritePrivateFormat(output, 1, text2.GetText(), text2.GetStyles(),
							 JCharacterRange(8,11));
	}
	TestReadPrivateFormat(fileName);

	JRemoveFile(fileName);
}

JTEST(SearchTextForward)
{
	StyledText text;
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", JString::kNoCopy));

	const TextIndex first(1,1);

	// entire word, no wrapping

	bool wrapped;
	const JStringMatch m1 = text.SearchForward(first, JRegex("sc" "\xC3\xB8" "re"), true, false, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m2 = text.SearchForward(first, JRegex("sc" "\xC3\xB8" "re"), false, false, &wrapped);
	JAssertStringsEqual("sc" "\xC3\xB8" "re", m2.GetString());
	JAssertEqual(JCharacterRange(5, 9), m2.GetCharacterRange());
	JAssertFalse(wrapped);

	// partial word, no wrapping

	TextIndex start(m2.GetCharacterRange().last, m2.GetUtf8ByteRange().last);

	const JStringMatch m3 = text.SearchForward(start, JRegex("\xC3\xB8" "re and"), false, false, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = text.SearchForward(start, JRegex("\xC3\xB8" "re and"), false, true, &wrapped);
	JAssertStringsEqual("\xC3\xB8" "re and", m4.GetString());
	JAssertEqual(JCharacterRange(7, 13), m4.GetCharacterRange());
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = text.SearchForward(start, JRegex("year"), true, false, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertFalse(wrapped);

	const JStringMatch m7 = text.SearchForward(start, JRegex("years"), true, false, &wrapped);
	JAssertStringsEqual("years", m7.GetString());
	JAssertFalse(wrapped);

	// last characters

	start.charIndex = text.GetText().GetCharacterCount();
	start.byteIndex = text.GetText().GetByteCount();
	start = text.AdjustTextIndex(start, -2);

	const JStringMatch m5 = text.SearchForward(start, JRegex("\\.\\.\\."), false, true, &wrapped);
	JAssertStringsEqual("...", m5.GetString());
	JAssertFalse(wrapped);
}

JTEST(SearchTextBackward)
{
	StyledText text;
	text.SetText(JString("Fourscore and seve" "\xC3\xB1" " years ago...", JString::kNoCopy));

	const TextIndex last = text.GetBeyondEnd();

	// entire word, no wrapping

	bool wrapped;
	const JStringMatch m1 = text.SearchBackward(last, JRegex("year"), true, false, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(wrapped);

	const JStringMatch m2 = text.SearchBackward(last, JRegex("years"), true, false, &wrapped);
	JAssertStringsEqual("years", m2.GetString());
	JAssertEqual(JCharacterRange(21, 25), m2.GetCharacterRange());
	JAssertFalse(wrapped);

	// partial word, no wrapping

	TextIndex start(17,17);

	const JStringMatch m3 = text.SearchBackward(start, JRegex("e" "\xC3\xB1" " ye"), false, false, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = text.SearchBackward(start, JRegex("e" "\xC3\xB1" " ye"), false, true, &wrapped);
	JAssertStringsEqual("e" "\xC3\xB1" " ye", m4.GetString());
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = text.SearchBackward(start, JRegex("an"), true, false, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertFalse(wrapped);

	const JStringMatch m7 = text.SearchBackward(start, JRegex("and"), true, false, &wrapped);
	JAssertStringsEqual("and", m7.GetString());
	JAssertFalse(wrapped);

	// first characters

	start.charIndex = 5;
	start.byteIndex = 5;

	const JStringMatch m5 = text.SearchBackward(start, JRegex("Four"), false, true, &wrapped);
	JAssertStringsEqual("Four", m5.GetString());
	JAssertEqual(JCharacterRange(1, 4), m5.GetCharacterRange());
	JAssertFalse(wrapped);
}

JTEST(ReplaceMatch)
{
	StyledText text;
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", JString::kNoCopy));

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(5, tc->GetRange().charRange.first);
			JAssertEqual(9, tc->GetRange().charRange.last);
			JAssertEqual(5, tc->GetRange().byteRange.first);
			JAssertEqual(10, tc->GetRange().byteRange.last);
			JAssertTrue(tc->GetRedrawRange().charRange.IsNothing());
			JAssertTrue(tc->GetRedrawRange().byteRange.IsNothing());
			JAssertEqual(0, tc->GetCharDelta());
			JAssertEqual(0, tc->GetByteDelta());
		});

	const TextIndex first(1,1);

	bool wrapped;
	const JStringMatch m1 = text.SearchForward(first, JRegex("sc" "(\xC3\xB8)" "re"), false, false, &wrapped);
	JAssertStringsEqual("sc" "\xC3\xB8" "re", m1.GetString());
	JAssertEqual(JCharacterRange(5, 9), m1.GetCharacterRange());
	JAssertFalse(wrapped);

	JInterpolate interp;

	const TextCount count =
		text.ReplaceMatch(m1, JString("FL$1UR", JString::kNoCopy), &interp, true);

	JAssertEqual(5, count.charCount);
	JAssertEqual(6, count.byteCount);
	JAssertStringsEqual("Fourfl" "\xC3\xB8" "ur and seven years ago...", text.GetText());
}

JTEST(ReplaceAllInRange)
{
	StyledText text;
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", JString::kNoCopy));

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[&text] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(text.GetText().GetCharacterCount(), tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(text.GetText().GetByteCount(), tc->GetRange().byteRange.last);
			JAssertEqual(0, tc->GetCharDelta());
			JAssertEqual(4, tc->GetByteDelta());
		});

	JInterpolate interp;

	text.ReplaceAllInRange(text.SelectAll(),
		JRegex("e"), false, JString("\xC3\xA9", JString::kNoCopy), nullptr, false);

	JAssertStringsEqual("Foursc" "\xC3\xB8" "r" "\xC3\xA9" " and s" "\xC3\xA9" "v" "\xC3\xA9" "n y" "\xC3\xA9" "ars ago...", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[&text] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(text.GetText().GetCharacterCount(), tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(text.GetText().GetByteCount(), tc->GetRange().byteRange.last);
			JAssertEqual(0, tc->GetCharDelta());
			JAssertEqual(-4, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	bool canUndo, canRedo;
	JAssertTrue(text.HasSingleUndo());
	JAssertFalse(text.HasMultipleUndo(&canUndo, &canRedo));

	text.Undo();

	JAssertTrue(text.HasSingleUndo());
	JAssertFalse(text.HasMultipleUndo(&canUndo, &canRedo));

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(11, tc->GetRange().charRange.first);
			JAssertEqual(21, tc->GetRange().charRange.last);
			JAssertEqual(12, tc->GetRange().byteRange.first);
			JAssertEqual(24, tc->GetRange().byteRange.last);
			JAssertEqual(0, tc->GetCharDelta());
			JAssertEqual(2, tc->GetByteDelta());
		});

	text.ReplaceAllInRange(TextRange(
		JCharacterRange(11, 21),
		JUtf8ByteRange(12, 22)),
		JRegex("e"), false, JString("\xC3\xA9", JString::kNoCopy), nullptr, false);

	JAssertStringsEqual("Foursc" "\xC3\xB8" "re and s" "\xC3\xA9" "v" "\xC3\xA9" "n years ago...", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[&text] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(text.GetText().GetCharacterCount(), tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(text.GetText().GetByteCount(), tc->GetRange().byteRange.last);
			JAssertEqual(12, tc->GetCharDelta());
			JAssertEqual(12, tc->GetByteDelta());
		});

	text.ReplaceAllInRange(text.SelectAll(),
		JRegex(" "), false, JString("abcd", JString::kNoCopy), nullptr, false);

	JAssertStringsEqual("Foursc" "\xC3\xB8" "reabcdandabcds" "\xC3\xA9" "v" "\xC3\xA9" "nabcdyearsabcdago...", text.GetText());
}

JTEST(IsEntireWord)
{
	StyledText text;
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", JString::kNoCopy));

	bool is = text.IsEntireWord(JStyledText::TextRange(
		JCharacterRange(1,9),
		JUtf8ByteRange(1,10)));
	JAssertTrue(is);

	is = text.IsEntireWord(TextRange(
		JCharacterRange(3,9),
		JUtf8ByteRange(3,10)));
	JAssertFalse(is);

	is = text.IsEntireWord(TextRange(
		JCharacterRange(11,13),
		JUtf8ByteRange(12,14)));
	JAssertTrue(is);

	is = text.IsEntireWord(TextRange(
		JCharacterRange(11,16),
		JUtf8ByteRange(12,17)));
	JAssertFalse(is);
}

bool bigFontMatch(const JFont& f)
{
	return f.GetSize() == 20;
}

bool boldFontMatch(const JFont& f)
{
	return f.GetStyle().bold;
}

JTEST(SearchStyle)
{
	StyledText text;
	text.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, false);

	const TextIndex first(1,1);
	TextRange r;

	// forward

	bool wrapped;
	bool found = text.SearchForward([] (const JFont& f)
		{
		return f.GetStyle().italic;
		},
		first, false, &wrapped, &r);

	JAssertFalse(found);
	JAssertFalse(wrapped);

	found = text.SearchForward(boldFontMatch, first, true, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(4, 7), r.charRange);
	JAssertEqual(JUtf8ByteRange(5, 9), r.byteRange);
	JAssertFalse(wrapped);

	found = text.SearchForward([] (const JFont& f)
		{
		return f.GetStyle().underlineCount > 0;
		},
		first, true, &wrapped, &r);

	JAssertTrue(found);
	JAssertEqual(JCharacterRange(14, 29), r.charRange);
	JAssertEqual(JUtf8ByteRange(16, 31), r.byteRange);
	JAssertFalse(wrapped);

	TextIndex start(29, 31);

	found = text.SearchForward(boldFontMatch, start, false, &wrapped, &r);
	JAssertFalse(found);
	JAssertFalse(wrapped);

	found = text.SearchForward(bigFontMatch, start, true, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(1, 3), r.charRange);
	JAssertEqual(JUtf8ByteRange(1, 4), r.byteRange);
	JAssertTrue(wrapped);

	// backward

	start.charIndex = 24;
	start.byteIndex = 26;

	found = text.SearchBackward(bigFontMatch, start, true, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(1, 3), r.charRange);
	JAssertEqual(JUtf8ByteRange(1, 4), r.byteRange);
	JAssertFalse(wrapped);

	start.charIndex = 1;
	start.byteIndex = 1;

	found = text.SearchBackward(boldFontMatch, start, false, &wrapped, &r);
	JAssertFalse(found);
	JAssertFalse(wrapped);

	found = text.SearchBackward(boldFontMatch, start, true, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(4, 7), r.charRange);
	JAssertEqual(JUtf8ByteRange(5, 9), r.byteRange);
	JAssertTrue(wrapped);

	start.charIndex = r.charRange.first;
	start.byteIndex = r.byteRange.first;

	text.SearchBackward(bigFontMatch, start, true, &wrapped, &r);
	JAssertTrue(found);
	JAssertEqual(JCharacterRange(1, 3), r.charRange);
	JAssertEqual(JUtf8ByteRange(1, 4), r.byteRange);
	JAssertFalse(wrapped);
}

JTEST(SetAllFontNameAndSize)
{
	StyledText text;
	text.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, false);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[&text] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(text.GetText().GetCharacterCount(), tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(text.GetText().GetByteCount(), tc->GetRange().byteRange.last);
			JAssertEqual(0, tc->GetCharDelta());
			JAssertEqual(0, tc->GetByteDelta());
		});

	text.SetAllFontNameAndSize(JString("foo", JString::kNoCopy), 24, false);

	JAssertEqual(4, text.GetStyles().GetRunCount());
	JAssertEqual(24, text.GetFont(2).GetSize());
	JAssertTrue(text.GetFont(5).GetStyle().bold);
	JAssertFalse(text.GetFont(10).GetStyle().bold);
	JAssertEqual(2, text.GetFont(16).GetStyle().underlineCount);
}

JTEST(CalcInsertionFont)
{
	StyledText text;
	text.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, false);

	JAssertEqual(20, text.CalcInsertionFont(TextIndex(2,2)).GetSize());
	JAssertTrue(text.CalcInsertionFont(TextIndex(5,6)).GetStyle().bold);
	JAssertEqual(0, text.CalcInsertionFont(TextIndex(11,13)).GetStyle().underlineCount);
	JAssertEqual(2, text.CalcInsertionFont(TextIndex(16,18)).GetStyle().underlineCount);
}

JTEST(InsertCharacter)
{
	StyledText text(true);
	text.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, false);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(3, tc->GetRange().charRange.first);
			JAssertEqual(3, tc->GetRange().charRange.last);
			JAssertEqual(4, tc->GetRange().byteRange.first);
			JAssertEqual(4, tc->GetRange().byteRange.last);
			JAssertEqual(1, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	text.InsertCharacter(TextRange(JCharacterRange(3,0), JUtf8ByteRange(4,0)), JUtf8Character('x'), JFontManager::GetDefaultFont());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(4, tc->GetRange().charRange.first);
			JAssertEqual(4, tc->GetRange().charRange.last);
			JAssertEqual(5, tc->GetRange().byteRange.first);
			JAssertEqual(5, tc->GetRange().byteRange.last);
			JAssertEqual(1, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	text.InsertCharacter(TextRange(JCharacterRange(4,0), JUtf8ByteRange(5,0)), JUtf8Character('y'), JFontManager::GetDefaultFont());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(5, tc->GetRange().charRange.first);
			JAssertEqual(5, tc->GetRange().charRange.last);
			JAssertEqual(6, tc->GetRange().byteRange.first);
			JAssertEqual(6, tc->GetRange().byteRange.last);
			JAssertEqual(1, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	text.InsertCharacter(TextRange(JCharacterRange(5,0), JUtf8ByteRange(6,0)), JUtf8Character('z'), JFontManager::GetDefaultFont());
	JAssertStringsEqual("b" "\xC3\xAE" "xyzg" "b" "\xC3\xB8" "ld" "normal" "double underline", text.GetText());

	JRunArrayIterator<JFont> iter(text.GetStyles());
	JFont f;
	iter.SkipNext();
	iter.Next(&f);
	JAssertEqual(20, f.GetSize());
	iter.Next(&f);
	JAssertEqual(JFontManager::GetDefaultFontSize(), f.GetSize());
	iter.Next(&f);
	JAssertEqual(JFontManager::GetDefaultFontSize(), f.GetSize());
	iter.Next(&f);
	JAssertEqual(JFontManager::GetDefaultFontSize(), f.GetSize());
	iter.Next(&f);
	JAssertEqual(20, f.GetSize());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(8, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(9, tc->GetRange().byteRange.last);
			JAssertEqual(1, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	text.InsertCharacter(TextRange(JCharacterRange(8,0), JUtf8ByteRange(9,0)), JUtf8Character('1'), JFontManager::GetDefaultFont());
	JAssertStringsEqual("b" "\xC3\xAE" "xyzg" "b1" "\xC3\xB8" "ld" "normal" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(13, tc->GetRange().charRange.first);
			JAssertEqual(13, tc->GetRange().charRange.last);
			JAssertEqual(15, tc->GetRange().byteRange.first);
			JAssertEqual(16, tc->GetRange().byteRange.last);
			JAssertEqual(-2, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	text.InsertCharacter(TextRange(JCharacterRange(13,15), JUtf8ByteRange(15,17)), JUtf8Character("\xC3\xA9"), JFontManager::GetDefaultFont());
	JAssertStringsEqual("b" "\xC3\xAE" "xyzg" "b1" "\xC3\xB8" "ld" "n" "\xC3\xA9" "al" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(13, tc->GetRange().charRange.first);
			JAssertEqual(15, tc->GetRange().charRange.last);
			JAssertEqual(15, tc->GetRange().byteRange.first);
			JAssertEqual(17, tc->GetRange().byteRange.last);
			JAssertEqual(2, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(7, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(8, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(3, tc->GetRange().charRange.first);
			JAssertEqual(2, tc->GetRange().charRange.last);
			JAssertEqual(4, tc->GetRange().byteRange.first);
			JAssertEqual(3, tc->GetRange().byteRange.last);
			JAssertEqual(-3, tc->GetCharDelta());
			JAssertEqual(-3, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();

	JAssertStringsEqual("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", text.GetText());
}

JTEST(CopyPaste)
{
	StyledText text;
	text.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, false);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(10, tc->GetRange().charRange.first);
			JAssertEqual(14, tc->GetRange().charRange.last);
			JAssertEqual(12, tc->GetRange().byteRange.first);
			JAssertEqual(18, tc->GetRange().byteRange.last);
			JAssertEqual(3, tc->GetCharDelta());
			JAssertEqual(5, tc->GetByteDelta());
		});

	JString s;
	JRunArray<JFont> style;

	bool ok = text.Copy(TextRange(
		JCharacterRange(2,6), JUtf8ByteRange(2,8)), &s, &style);

	JAssertTrue(ok);
	JAssertStringsEqual("\xC3\xAE" "g" "b" "\xC3\xB8" "l", s);
	JAssertEqual(2, style.GetRunCount());

	text.Paste(TextRange(
		JCharacterRange(10,11), JUtf8ByteRange(12,13)), s, &style);

	JAssertStringsEqual("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "no" "\xC3\xAE" "g" "b" "\xC3\xB8" "l" "al" "double underline", text.GetText());
	JAssertEqual(7, text.GetStyles().GetRunCount());
	JAssertTrue(text.GetFont(13).GetStyle().bold);
}

JTEST(DeleteText)
{
	StyledText text(true);
	text.SetText(JString("b" "\xC3\xAE" "g\n" "b" "\xC3\xB8" "ld\n" "\t   normal\n" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(5,8), JUtf8ByteRange(6,10)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(16,31), JUtf8ByteRange(18,33)), 2, true);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(2, tc->GetRange().charRange.first);
			JAssertEqual(1, tc->GetRange().charRange.last);
			JAssertEqual(2, tc->GetRange().byteRange.first);
			JAssertEqual(1, tc->GetRange().byteRange.last);
			JAssertEqual(-5, tc->GetCharDelta());
			JAssertEqual(-7, tc->GetByteDelta());
		});

	text.DeleteText(TextRange(JCharacterRange(2,6), JUtf8ByteRange(2,8)));
	JAssertStringsEqual("b" "ld\n" "\t   normal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(10, tc->GetRange().charRange.first);
			JAssertEqual(9, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(9, tc->GetRange().byteRange.last);
			JAssertEqual(-9, tc->GetCharDelta());
			JAssertEqual(-9, tc->GetByteDelta());
		});

	text.DeleteText(TextRange(JCharacterRange(10,18), JUtf8ByteRange(10,18)));
	JAssertStringsEqual("b" "ld\n" "\t   nble underline", text.GetText());

	JRunArrayIterator<JFont> iter(text.GetStyles());
	iter.MoveTo(kJIteratorStartBefore, 13);
	JFont f;
	iter.Next(&f);
	JAssertEqual(2, f.GetStyle().underlineCount);

	bool canUndo, canRedo;
	JAssertFalse(text.HasSingleUndo());
	JAssertTrue(text.HasMultipleUndo(&canUndo, &canRedo));
	JAssertTrue(canUndo);
	JAssertFalse(canRedo);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(10, tc->GetRange().charRange.first);
			JAssertEqual(18, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(18, tc->GetRange().byteRange.last);
			JAssertEqual(9, tc->GetCharDelta());
			JAssertEqual(9, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();

	JAssertTrue(text.HasMultipleUndo(&canUndo, &canRedo));
	JAssertTrue(canUndo);
	JAssertTrue(canRedo);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(2, tc->GetRange().charRange.first);
			JAssertEqual(6, tc->GetRange().charRange.last);
			JAssertEqual(2, tc->GetRange().byteRange.first);
			JAssertEqual(8, tc->GetRange().byteRange.last);
			JAssertEqual(5, tc->GetCharDelta());
			JAssertEqual(7, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();

	JAssertTrue(text.HasMultipleUndo(&canUndo, &canRedo));
	JAssertFalse(canUndo);
	JAssertTrue(canRedo);

	iter.MoveTo(kJIteratorStartBefore, 2);
	iter.Next(&f);
	JAssertEqual(20, f.GetSize());

	iter.MoveTo(kJIteratorStartBefore, 6);
	iter.Next(&f);
	JAssertTrue(f.GetStyle().bold);

	iter.MoveTo(kJIteratorStartBefore, 21);
	iter.Next(&f);
	JAssertEqual(2, f.GetStyle().underlineCount);
}

JTEST(BackwardDelete)
{
	StyledText text(true);
	text.SetText(JString("b" "\xC3\xAE" "g\n" "b" "\xC3\xB8" "ld\n" "\t   normal\n" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(5,8), JUtf8ByteRange(6,10)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(16,31), JUtf8ByteRange(18,33)), 2, false);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(7, tc->GetRange().charRange.first);
			JAssertEqual(6, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(8, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	TextIndex caretIndex = text.BackwardDelete(TextIndex(5,6), TextIndex(8,10), false);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "\xC3\xB8" "d\n" "\t   normal\n" "double underline", text.GetText());
	JAssertEqual(7, caretIndex.charIndex);
	JAssertEqual(9, caretIndex.byteIndex);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(6, tc->GetRange().charRange.first);
			JAssertEqual(5, tc->GetRange().charRange.last);
			JAssertEqual(7, tc->GetRange().byteRange.first);
			JAssertEqual(6, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-2, tc->GetByteDelta());
		});

	JString returnText;
	JRunArray<JFont> returnStyle;
	text.BackwardDelete(TextIndex(5,6), TextIndex(7,9), false, &returnText, &returnStyle);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\t   normal\n" "double underline", text.GetText());
	JAssertStringsEqual("\xC3\xB8", returnText);
	JAssertEqual(1, returnStyle.GetElementCount());
	JAssertTrue(returnStyle.GetFirstElement().GetStyle().bold);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(10, tc->GetRange().charRange.first);
			JAssertEqual(9, tc->GetRange().charRange.last);
			JAssertEqual(11, tc->GetRange().byteRange.first);
			JAssertEqual(10, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	text.BackwardDelete(TextIndex(8,9), TextIndex(11,12), false);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\t  normal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(10, tc->GetRange().charRange.first);
			JAssertEqual(10, tc->GetRange().charRange.last);
			JAssertEqual(11, tc->GetRange().byteRange.first);
			JAssertEqual(11, tc->GetRange().byteRange.last);
			JAssertEqual(1, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(10, tc->GetRange().charRange.first);
			JAssertEqual(9, tc->GetRange().charRange.last);
			JAssertEqual(11, tc->GetRange().byteRange.first);
			JAssertEqual(10, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	text.BackwardDelete(TextIndex(8,9), TextIndex(11,12), true);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\t  normal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(9, tc->GetRange().charRange.first);
			JAssertEqual(14, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(15, tc->GetRange().byteRange.last);
			JAssertEqual(6, tc->GetCharDelta());
			JAssertEqual(6, tc->GetByteDelta());
		});

	text.Paste(TextRange(
		JCharacterRange(9,8), JUtf8ByteRange(10,9)),
		JString("      ", JString::kNoCopy));
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\t        normal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(9, tc->GetRange().charRange.first);
			JAssertEqual(8, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(9, tc->GetRange().byteRange.last);
			JAssertEqual(-8, tc->GetCharDelta());
			JAssertEqual(-8, tc->GetByteDelta());
		});

	text.BackwardDelete(TextIndex(8,9), TextIndex(17,18), true);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\tnormal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(31, tc->GetRange().charRange.first);
			JAssertEqual(30, tc->GetRange().charRange.last);
			JAssertEqual(32, tc->GetRange().byteRange.first);
			JAssertEqual(31, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	text.BackwardDelete(TextIndex(16,17), TextIndex(32,33), true);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "b" "d\n" "\tnormal\n" "double underlin", text.GetText());

	// edge cases

	bcastTest.Expect(JStyledText::kTextSet);

	text.SetText(JString("abc\t", JString::kNoCopy));

	bcastTest.Expect(JStyledText::kTextChanged);
	text.BackwardDelete(TextIndex(1,1), TextIndex(5,5), true);
	JAssertStringsEqual("abc", text.GetText());
}

JTEST(ForwardDelete)
{
	StyledText text(true);
	text.SetText(JString("b" "\xC3\xAE" "g\n" "b" "\xC3\xB8" "ld\n" "\t   normal\n" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(5,8), JUtf8ByteRange(6,10)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(16,31), JUtf8ByteRange(18,33)), 2, false);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(5, tc->GetRange().charRange.first);
			JAssertEqual(4, tc->GetRange().charRange.last);
			JAssertEqual(6, tc->GetRange().byteRange.first);
			JAssertEqual(5, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	text.ForwardDelete(TextIndex(5,6), TextIndex(5,6), false);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "\xC3\xB8" "ld\n" "\t   normal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(5, tc->GetRange().charRange.first);
			JAssertEqual(4, tc->GetRange().charRange.last);
			JAssertEqual(6, tc->GetRange().byteRange.first);
			JAssertEqual(5, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-2, tc->GetByteDelta());
		});

	JString returnText;
	JRunArray<JFont> returnStyle;
	text.ForwardDelete(TextIndex(5,6), TextIndex(5,6), false, &returnText, &returnStyle);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\t   normal\n" "double underline", text.GetText());
	JAssertStringsEqual("\xC3\xB8", returnText);
	JAssertEqual(1, returnStyle.GetElementCount());
	JAssertTrue(returnStyle.GetFirstElement().GetStyle().bold);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(9, tc->GetRange().charRange.first);
			JAssertEqual(8, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(9, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	text.ForwardDelete(TextIndex(8,9), TextIndex(9,10), false);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\t  normal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(9, tc->GetRange().charRange.first);
			JAssertEqual(9, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(10, tc->GetRange().byteRange.last);
			JAssertEqual(1, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(9, tc->GetRange().charRange.first);
			JAssertEqual(8, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(9, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	text.ForwardDelete(TextIndex(8,9), TextIndex(9,10), true);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\t  normal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(9, tc->GetRange().charRange.first);
			JAssertEqual(14, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(15, tc->GetRange().byteRange.last);
			JAssertEqual(6, tc->GetCharDelta());
			JAssertEqual(6, tc->GetByteDelta());
		});

	text.Paste(TextRange(
		JCharacterRange(9,8), JUtf8ByteRange(10,9)),
		JString("      ", JString::kNoCopy));
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\t        normal\n" "double underline", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(9, tc->GetRange().charRange.first);
			JAssertEqual(8, tc->GetRange().charRange.last);
			JAssertEqual(10, tc->GetRange().byteRange.first);
			JAssertEqual(9, tc->GetRange().byteRange.last);
			JAssertEqual(-8, tc->GetCharDelta());
			JAssertEqual(-8, tc->GetByteDelta());
		});

	text.ForwardDelete(TextIndex(8,9), TextIndex(9,10), true);
	JAssertStringsEqual("b" "\xC3\xAE" "g\n" "ld\n" "\tnormal\n" "double underline", text.GetText());
}

JTEST(Move)
{
	StyledText text(true);
	text.SetText(JString("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", JString::kNoCopy));
	text.SetFontSize(TextRange(JCharacterRange(1,3), JUtf8ByteRange(1,4)), 20, false);
	text.SetFontBold(TextRange(JCharacterRange(4,7), JUtf8ByteRange(5,9)), true, false);
	text.SetFontUnderline(TextRange(JCharacterRange(14,29), JUtf8ByteRange(16,31)), 2, false);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(5, tc->GetRange().charRange.first);
			JAssertEqual(4, tc->GetRange().charRange.last);
			JAssertEqual(6, tc->GetRange().byteRange.first);
			JAssertEqual(5, tc->GetRange().byteRange.last);
			JAssertEqual(-3, tc->GetCharDelta());
			JAssertEqual(-4, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(10, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(12, tc->GetRange().byteRange.last);
			JAssertEqual(3, tc->GetCharDelta());
			JAssertEqual(4, tc->GetByteDelta());
		});

	TextRange r;
	bool ok = text.MoveText(
		TextRange(
			JCharacterRange(5,7),
			JUtf8ByteRange(6,9)),
		TextIndex(11,13),
		false, &r);

	JAssertTrue(ok);
	JAssertFalse(r.IsEmpty());
	JAssertStringsEqual("b" "\xC3\xAE" "g" "b" "nor" "\xC3\xB8" "ld" "mal" "double underline", text.GetText());
	JAssertTrue(text.GetFont(4).GetStyle().bold);
	JAssertFalse(text.GetFont(7).GetStyle().bold);
	JAssertTrue(text.GetFont(8).GetStyle().bold);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(6, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(6, tc->GetRange().byteRange.last);
			JAssertEqual(6, tc->GetCharDelta());
			JAssertEqual(6, tc->GetByteDelta());
		});

	ok = text.MoveText(
		TextRange(
			JCharacterRange(14,19),
			JUtf8ByteRange(16,21)),
		TextIndex(1,1),
		true, &r);

	JAssertTrue(ok);
	JAssertFalse(r.IsEmpty());
	JAssertStringsEqual("double" "b" "\xC3\xAE" "g" "b" "nor" "\xC3\xB8" "ld" "mal" "double underline", text.GetText());
	JAssertEqual(2, text.GetFont(2).GetStyle().underlineCount);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(0, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(0, tc->GetRange().byteRange.last);
			JAssertEqual(-6, tc->GetCharDelta());
			JAssertEqual(-6, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(7, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(8, tc->GetRange().byteRange.last);
			JAssertEqual(-3, tc->GetCharDelta());
			JAssertEqual(-4, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(5, tc->GetRange().charRange.first);
			JAssertEqual(7, tc->GetRange().charRange.last);
			JAssertEqual(6, tc->GetRange().byteRange.first);
			JAssertEqual(9, tc->GetRange().byteRange.last);
			JAssertEqual(3, tc->GetCharDelta());
			JAssertEqual(4, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();
	text.Undo();

	JAssertStringsEqual("b" "\xC3\xAE" "g" "b" "\xC3\xB8" "ld" "normal" "double underline", text.GetText());
	JAssertEqual(0, text.GetFont(1).GetStyle().underlineCount);
	JAssertTrue(text.GetFont(5).GetStyle().bold);
	JAssertFalse(text.GetFont(10).GetStyle().bold);
	JAssertFalse(text.GetFont(11).GetStyle().bold);
}

JTEST(TabSelection)
{
	StyledText text(true);
	text.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", JString::kNoCopy));

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(7, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(8, tc->GetRange().byteRange.last);
			JAssertEqual(2, tc->GetCharDelta());
			JAssertEqual(2, tc->GetByteDelta());
		});

	text.Indent(TextRange(JCharacterRange(1,1), JUtf8ByteRange(1,2)), 2);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	text.Outdent(TextRange(JCharacterRange(8,13), JUtf8ByteRange(9,14)), 3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(22, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(23, tc->GetRange().byteRange.last);
			JAssertEqual(6, tc->GetCharDelta());
			JAssertEqual(6, tc->GetByteDelta());
		});

	text.Indent(TextRange(JCharacterRange(8,13), JUtf8ByteRange(9,14)), 3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(16, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(17, tc->GetRange().byteRange.last);
			JAssertEqual(-6, tc->GetCharDelta());
			JAssertEqual(-6, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(5, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(6, tc->GetRange().byteRange.last);
			JAssertEqual(-2, tc->GetCharDelta());
			JAssertEqual(-2, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();
	text.Undo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(7, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(8, tc->GetRange().byteRange.last);
			JAssertEqual(2, tc->GetCharDelta());
			JAssertEqual(2, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(22, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(23, tc->GetRange().byteRange.last);
			JAssertEqual(6, tc->GetCharDelta());
			JAssertEqual(6, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Redo();
	text.Redo();

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(19, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(20, tc->GetRange().byteRange.last);
			JAssertEqual(-3, tc->GetCharDelta());
			JAssertEqual(-3, tc->GetByteDelta());
		});

	text.Outdent(TextRange(JCharacterRange(1,22), JUtf8ByteRange(1,23)), 1);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(22, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(23, tc->GetRange().byteRange.last);
			JAssertEqual(3, tc->GetCharDelta());
			JAssertEqual(3, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(19, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(20, tc->GetRange().byteRange.last);
			JAssertEqual(-3, tc->GetCharDelta());
			JAssertEqual(-3, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Redo();

	text.Outdent(TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 2);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(14, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(15, tc->GetRange().byteRange.last);
			JAssertEqual(-5, tc->GetCharDelta());
			JAssertEqual(-5, tc->GetByteDelta());
		});

	text.Outdent(TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 2, true);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	// edge cases

	bcastTest.Expect(JStyledText::kTextSet);

	text.SetText(JString("\tabcd\n", JString::kNoCopy));

	bcastTest.Expect(JStyledText::kTextChanged);
	text.Outdent(TextRange(JCharacterRange(1,6), JUtf8ByteRange(1,6)), 1);
	JAssertStringsEqual("abcd\n", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged);
	text.Indent(TextRange(JCharacterRange(1,5), JUtf8ByteRange(1,5)), 1);
	JAssertStringsEqual("\tabcd\n", text.GetText());
}

JTEST(TabSelectionMixed)
{
	StyledText text;
	text.SetText(JString("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", JString::kNoCopy));

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(14, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(15, tc->GetRange().byteRange.last);
			JAssertEqual(-5, tc->GetCharDelta());
			JAssertEqual(-5, tc->GetByteDelta());
		});

	text.Outdent(TextRange(JCharacterRange(1,19), JUtf8ByteRange(1,20)), 1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(19, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(20, tc->GetRange().byteRange.last);
			JAssertEqual(5, tc->GetCharDelta());
			JAssertEqual(5, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", text.GetText());

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(14, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(15, tc->GetRange().byteRange.last);
			JAssertEqual(-5, tc->GetCharDelta());
			JAssertEqual(-5, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.Undo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());


	bcastTest.Expect(JStyledText::kTextSet);

	text.SetText(JString("  " "\xC3\xA1" "bcd\n   1234\n    wxzy", JString::kNoCopy));

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(17, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(18, tc->GetRange().byteRange.last);
			JAssertEqual(-6, tc->GetCharDelta());
			JAssertEqual(-6, tc->GetByteDelta());
		});

	text.Outdent(TextRange(JCharacterRange(1,23), JUtf8ByteRange(1,24)), 1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n 1234\n  wxzy", text.GetText());
}

JTEST(CleanWhitespaceTabs)
{
	StyledText text;
	text.TabShouldInsertSpaces(false);
	text.SetCRMTabCharCount(4);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(18, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(19, tc->GetRange().byteRange.last);
			JAssertEqual(-2, tc->GetCharDelta());
			JAssertEqual(-2, tc->GetByteDelta());
		});

	text.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", JString::kNoCopy));
	TextRange r =
		text.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), false);
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n\t1234\n\twxzy", text.GetText());
	JAssertEqual(JCharacterRange(1,18), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(17, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(18, tc->GetRange().byteRange.last);
			JAssertEqual(-3, tc->GetCharDelta());
			JAssertEqual(-3, tc->GetByteDelta());
		});

	text.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", JString::kNoCopy));
	r = text.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), true);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t1234\n\twxzy", text.GetText());
	JAssertEqual(JCharacterRange(1,17), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(13, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(14, tc->GetRange().byteRange.last);
			JAssertEqual(-2, tc->GetCharDelta());
			JAssertEqual(-2, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(15, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(16, tc->GetRange().byteRange.last);
			JAssertEqual(2, tc->GetCharDelta());
			JAssertEqual(2, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(13, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(14, tc->GetRange().byteRange.last);
			JAssertEqual(-2, tc->GetCharDelta());
			JAssertEqual(-2, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", JString::kNoCopy));
	r = text.CleanWhitespace(TextRange(
							JCharacterRange(9,12),
							JUtf8ByteRange(10,13)), true);
	text.Undo();
	text.Undo();
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n\t1234\n\twxzy", text.GetText());
	JAssertEqual(JCharacterRange(8,13), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(17, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(18, tc->GetRange().byteRange.last);
			JAssertEqual(-3, tc->GetCharDelta());
			JAssertEqual(-3, tc->GetByteDelta());
		});

	text.SetText(JString("\t " "\xC3\xA1" "bcd\n   1234\n\twxzy", JString::kNoCopy));
	r = text.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), true);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t1234\n\twxzy", text.GetText());
	JAssertEqual(JCharacterRange(1,17), r.charRange);

	// edge cases

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);
	bcastTest.Expect(JStyledText::kTextChanged);

	text.SetText(JString("    abcd", JString::kNoCopy));
	text.SetFontUnderline(TextRange(JCharacterRange(1,4), JUtf8ByteRange(1,4)), 2, false);
	text.SetFontBold(TextRange(JCharacterRange(5,8), JUtf8ByteRange(5,8)), true, false);

	text.CleanWhitespace(TextRange(JCharacterRange(1,8), JUtf8ByteRange(1,8)), false);
	JAssertStringsEqual("\tabcd", text.GetText());

	JFont f = text.GetFont(1);
	JAssertFalse(f.GetStyle().bold);
	JAssertEqual(2, f.GetStyle().underlineCount);
	f = text.GetFont(2);
	JAssertTrue(f.GetStyle().bold);
	JAssertEqual(0, f.GetStyle().underlineCount);
}

JTEST(CleanWhitespaceSpaces)
{
	StyledText text;
	text.TabShouldInsertSpaces(true);
	text.SetCRMTabCharCount(4);

	JBroadcastTester bcastTest(&text);
	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(27, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(28, tc->GetRange().byteRange.last);
			JAssertEqual(7, tc->GetCharDelta());
			JAssertEqual(7, tc->GetByteDelta());
		});

	text.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", JString::kNoCopy));
	TextRange r =
		text.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), false);
	JAssertStringsEqual("     " "\xC3\xA1" "bcd\n    1234\n    wxzy", text.GetText());
	JAssertEqual(JCharacterRange(1,27), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(1, tc->GetRange().charRange.first);
			JAssertEqual(26, tc->GetRange().charRange.last);
			JAssertEqual(1, tc->GetRange().byteRange.first);
			JAssertEqual(27, tc->GetRange().byteRange.last);
			JAssertEqual(6, tc->GetCharDelta());
			JAssertEqual(6, tc->GetByteDelta());
		});

	text.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", JString::kNoCopy));
	r = text.CleanWhitespace(TextRange(
							JCharacterRange(1,20),
							JUtf8ByteRange(1,21)), true);
	JAssertStringsEqual("    " "\xC3\xA1" "bcd\n    1234\n    wxzy", text.GetText());
	JAssertEqual(JCharacterRange(1,26), r.charRange);

	bcastTest.Expect(JStyledText::kTextSet);
	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(16, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(17, tc->GetRange().byteRange.last);
			JAssertEqual(1, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(15, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(16, tc->GetRange().byteRange.last);
			JAssertEqual(-1, tc->GetCharDelta());
			JAssertEqual(-1, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	bcastTest.Expect(JStyledText::kTextChanged,
		[] (const JBroadcaster::Message& m)
		{
			const JStyledText::TextChanged* tc =
				dynamic_cast<const JStyledText::TextChanged*>(&m);
			JAssertNotNull(tc);
			JAssertEqual(8, tc->GetRange().charRange.first);
			JAssertEqual(16, tc->GetRange().charRange.last);
			JAssertEqual(9, tc->GetRange().byteRange.first);
			JAssertEqual(17, tc->GetRange().byteRange.last);
			JAssertEqual(1, tc->GetCharDelta());
			JAssertEqual(1, tc->GetByteDelta());
		});

	bcastTest.Expect(JStyledText::kUndoFinished);

	text.SetText(JString("\t " "\xC3\xA1" "bcd\n  \t1234\n\twxzy", JString::kNoCopy));
	r = text.CleanWhitespace(TextRange(
							JCharacterRange(9,12),
							JUtf8ByteRange(10,13)), true);
	text.Undo();
	text.Redo();
	JAssertStringsEqual("\t " "\xC3\xA1" "bcd\n    1234\n\twxzy", text.GetText());
	JAssertEqual(JCharacterRange(8,16), r.charRange);
}

JTEST(AdjustTextIndex)
{
	StyledText text;
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", JString::kNoCopy));

	TextIndex index(5,5);
	index = text.AdjustTextIndex(index, 6);
	JAssertEqual(11, index.charIndex);
	JAssertEqual(12, index.byteIndex);

	index = text.AdjustTextIndex(index, -7);
	JAssertEqual(4, index.charIndex);
	JAssertEqual(4, index.byteIndex);
}

// generates link errors - prevents accidentally writing out JStyledText* instead of JString
/*
JTEST(Streaming)
{
	StyledText text;
	std::fstream f;
	f << &text;
	f >> &text;
}
*/
