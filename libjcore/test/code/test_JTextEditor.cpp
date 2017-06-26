/******************************************************************************
 test_JTextEditor.cc

	Program to test JTextEditor class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <TextEditor.h>
#include <jFileUtil.h>
#include <fstream>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(SetText)
{
	TextEditor te;
	JAssertTrue(te.IsEmpty());

	JBoolean cleaned = te.SetText(JString("abcd", 0, kJFalse));
	JAssertFalse(cleaned);
	JAssertStringsEqual("abcd", te.GetText());
	JAssertEqual(4, te.GetStyle().GetElementCount());
	JAssertEqual(1, te.GetStyle().GetRunCount());

	cleaned = te.SetText(JString("a\vb\ac\bd", 0, kJFalse));
	JAssertTrue(cleaned);
	JAssertStringsEqual("abcd", te.GetText());
}

JTEST(ReadPlainText)
{
	TextEditor te;

	JTextEditor::PlainTextFormat format;
	JBoolean ok = te.ReadPlainText(JString("test_ReadPlainUNIXText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kUNIXText, format);
	JAssertTrue(te.EndsWithNewline());
	JAssertEqual(45, te.GetStyle().GetElementCount());
	JAssertEqual(1, te.GetStyle().GetRunCount());

	const JString s = te.GetText();

	ok = te.ReadPlainText(JString("test_ReadPlainDOSText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kDOSText, format);
	JAssertStringsEqual(s, te.GetText());
	JAssertEqual(45, te.GetStyle().GetElementCount());
	JAssertEqual(1, te.GetStyle().GetRunCount());

	ok = te.ReadPlainText(JString("test_ReadPlainMacText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kMacintoshText, format);
	JAssertStringsEqual(s, te.GetText());
	JAssertEqual(45, te.GetStyle().GetElementCount());
	JAssertEqual(1, te.GetStyle().GetRunCount());

	ok = te.ReadPlainText(JString("test_ReadPlainBinaryText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kUNIXText, format);
	JAssertStringsEqual(s, te.GetText());
	JAssertEqual(45, te.GetStyle().GetElementCount());
	JAssertEqual(1, te.GetStyle().GetRunCount());

	ok = te.ReadPlainText(JString("test_ReadPlainBinaryText.txt", 0, kJFalse), &format, kJFalse);
	JAssertFalse(ok);
}

JTEST(WritePlainText)
{
	TextEditor te;

	JTextEditor::PlainTextFormat format;
	JBoolean ok = te.ReadPlainText(JString("test_ReadPlainUNIXText.txt", 0, kJFalse), &format);
	JAssertTrue(ok);
	JAssertEqual(JTextEditor::kUNIXText, format);

	JString fileName;
	JSize fileSize;
	JString s;
	JAssertOK(JCreateTempFile(&fileName));

	te.WritePlainText(fileName, JTextEditor::kUNIXText);
	JAssertOK(JGetFileLength(fileName, &fileSize);
	JAssertEqual(te.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\n"));
	JAssertFalse(s.Contains("\r"));

	te.WritePlainText(fileName, JTextEditor::kDOSText);
	JAssertOK(JGetFileLength(fileName, &fileSize);
	JAssertEqual(te.GetText().GetByteCount() + te.GetLineCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r\n"));

	te.WritePlainText(fileName, JTextEditor::kMacintoshText);
	JAssertOK(JGetFileLength(fileName, &fileSize);
	JAssertEqual(te.GetText().GetByteCount(), fileSize);

	JReadFile(fileName, &s);
	JAssertTrue(s.Contains("\r"));
	JAssertFalse(s.Contains("\n"));

	JRemoveFile(fileName);
}

JTEST(ReadWritePrivateFormat)
{
	TextEditor te;
	te.SetText(JString("\xC3\x86bcd", 0, kJFalse));
	te.SetFontName(2, 2, "foo", kJTrue);
	te.SetFontSize(2, 3, 2*kJDefaultFontSize, kJTrue);
	te.SetFontBold(1, 2, kJTrue, kJTrue);
	JAssertEqual(4, te.GetStyles().GetRunCount());

	JString fileName;
	JAssertOK(JCreateTempFile(&fileName));

	{
	std::ofstream output(fileName);
	te.WritePrivateFormat(output);
	}

	TextEditor te2;
	{
	std::ifstream input(fileName);
	te2.ReadPrivateFormat(input);
	}

	JAssertStringsEqual("\xC3\x86bcd", te2.GetText());
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
