/******************************************************************************
 test_JXTextMenuData.cpp

	Test text menu parsing functions.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <TestFontManager.h>
#include <JXDisplay_mock.h>
#include <JXTextMenu_mock.h>
#include <jx-af/jx/JXTextMenuData.h>		// after mocks
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class TestTextMenuData : public JXTextMenuData
{
public:

	TestTextMenuData(JXTextMenu* menu)
		:
		JXTextMenuData(menu)
{};

	bool
	TestItemID(const JIndex index, const JString** id) const
{
		return GetItemID(index, id);
}
};

JTEST(ParseMacOS)
{
	JXMenu::SetDisplayStyle(JXMenu::kMacintoshStyle);

	TestFontManager fontManager;
	JXDisplay display(true);
	JXTextMenu menu(&display, &fontManager);

	TestTextMenuData data(&menu);
	data.AppendMenuItems(
		"    Undo       %k Ctrl-Z       %iundo"
		"  | Redo       %k Ctrl-Shift-Z %iredo"
		"%l| Select all %k Ctrl-Hyper-A %iselectall"
		"%l| Underline  %b"
		"  | Strike     %b %d"
		"%l| Black      %r"
		"  | Gray       %r");

	const JString* s;

	JAssertTrue(data.IsEnabled(1));
	JAssertEqual(JXMenu::ItemType::kPlainType, data.GetType(1));
	JAssertStringsEqual("Undo", data.GetText(1));
	JAssertTrue(data.GetNMShortcut(1, &s));
	JAssertStringsEqual("\xE2\x8C\x98" "Z", *s);
	JAssertTrue(data.TestItemID(1, &s));
	JAssertStringsEqual("undo", *s);

	JAssertStringsEqual("Redo", data.GetText(2));
	JAssertTrue(data.GetNMShortcut(2, &s));
	JAssertStringsEqual("\xE2\x87\xA7" "\xE2\x8C\x98" "Z", *s);
	JAssertTrue(data.TestItemID(2, &s));
	JAssertStringsEqual("redo", *s);

	JAssertTrue(data.HasSeparator(2));

	JAssertTrue(data.GetNMShortcut(3, &s));
	JAssertStringsEqual("Hyper-" "\xE2\x8C\x98" "A", *s);

	JAssertTrue(data.IsEnabled(4));
	JAssertEqual(JXMenu::ItemType::kCheckboxType, data.GetType(4));
	JAssertStringsEqual("Underline", data.GetText(4));
	JAssertFalse(data.GetNMShortcut(4, &s));
	JAssertFalse(data.TestItemID(4, &s));

	JAssertFalse(data.IsEnabled(5));
	JAssertEqual(JXMenu::ItemType::kCheckboxType, data.GetType(5));
	JAssertStringsEqual("Strike", data.GetText(5));
	JAssertFalse(data.GetNMShortcut(5, &s));
	JAssertFalse(data.TestItemID(5, &s));

	JAssertEqual(JXMenu::ItemType::kRadioType, data.GetType(6));
	JAssertStringsEqual("Black", data.GetText(6));

	JAssertTrue(data.HasCheckboxes());
	JAssertFalse(data.HasSubmenus());
}

JTEST(ParseLinux)
{
	JXMenu::SetDisplayStyle(JXMenu::kMacintoshStyle);

	TestFontManager fontManager;
	JXDisplay display(false);
	JXTextMenu menu(&display, &fontManager);

	TestTextMenuData data(&menu);
	data.AppendMenuItems(
		"    Undo       %k Ctrl-Z       %iundo"
		"  | Redo       %k Ctrl-Shift-Z %iredo"
		"%l| Select all %k Ctrl-Hyper-A %h a %iselectall");

	const JString* s;

	JAssertTrue(data.IsEnabled(1));
	JAssertEqual(JXMenu::ItemType::kPlainType, data.GetType(1));
	JAssertStringsEqual("Undo", data.GetText(1));
	JAssertTrue(data.GetNMShortcut(1, &s));
	JAssertStringsEqual("Meta-Z", *s);
	JAssertTrue(data.TestItemID(1, &s));
	JAssertStringsEqual("undo", *s);

	JAssertStringsEqual("Redo", data.GetText(2));
	JAssertTrue(data.GetNMShortcut(2, &s));
	JAssertStringsEqual("Meta-Shift-Z", *s);
	JAssertTrue(data.TestItemID(2, &s));
	JAssertStringsEqual("redo", *s);

	JAssertTrue(data.HasSeparator(2));

	JAssertTrue(data.GetNMShortcut(3, &s));
	JAssertStringsEqual("Meta-Hyper-A", *s);

	JIndex i;
	JAssertFalse(data.ShortcutToIndex(JUtf8Character('a'), &i));

	JAssertFalse(data.HasCheckboxes());
	JAssertFalse(data.HasSubmenus());
}

JTEST(ParseWindows)
{
	JXMenu::SetDisplayStyle(JXMenu::kWindowsStyle);

	TestFontManager fontManager;
	JXDisplay display(false);
	JXTextMenu menu(&display, &fontManager);

	TestTextMenuData data(&menu);
	data.AppendMenuItems(
		"    Undo       %k Ctrl-Z       %iundo"
		"  | Redo       %k Ctrl-Shift-Z %iredo"
		"%l| Select all %k Ctrl-Hyper-A %h a %iselectall");

	const JString* s;

	JAssertTrue(data.IsEnabled(1));
	JAssertEqual(JXMenu::ItemType::kPlainType, data.GetType(1));
	JAssertStringsEqual("Undo", data.GetText(1));
	JAssertTrue(data.GetNMShortcut(1, &s));
	JAssertStringsEqual("Ctrl-Z", *s);
	JAssertTrue(data.TestItemID(1, &s));
	JAssertStringsEqual("undo", *s);

	JAssertStringsEqual("Redo", data.GetText(2));
	JAssertTrue(data.GetNMShortcut(2, &s));
	JAssertStringsEqual("Ctrl-Shift-Z", *s);
	JAssertTrue(data.TestItemID(2, &s));
	JAssertStringsEqual("redo", *s);

	JAssertTrue(data.HasSeparator(2));

	JAssertTrue(data.GetNMShortcut(3, &s));
	JAssertStringsEqual("Ctrl-Hyper-A", *s);

	JIndex i;
	JAssertTrue(data.ShortcutToIndex(JUtf8Character('a'), &i));
	JAssertEqual(3, i);

	JAssertFalse(data.HasCheckboxes());
	JAssertFalse(data.HasSubmenus());
}
