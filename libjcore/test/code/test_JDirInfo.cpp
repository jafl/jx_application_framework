/******************************************************************************
 test_JDirInfo.cpp

	Program to test JDirInfo & JDirEntry classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	const JString path("/tmp/test_JDirInfo/", kJFalse);
	JAssertOK(JCreateDirectory(path));
	JAssertOK(JChangeDirectory(path));

	system("touch foobar");
	system("touch foobaz");
	system("touch fooshug");

	JDirInfo* info;
	JAssertTrue(JDirInfo::Create(path, &info));
	JAssertTrue(info->DirectoryExists());
	JAssertStringsEqual(path, info->GetDirectory());
	JAssertTrue(info->IsWritable());

	JAssertEqual(3, info->GetEntryCount());

	system("mkdir bar");
	system("mkdir baz");

	JAssertTrue(JDirInfo::Empty(JString("bar", kJFalse)));

	info->ForceUpdate();
	JAssertEqual(5, info->GetEntryCount());

	info->ShowDirs(kJFalse);
	JAssertEqual(3, info->GetEntryCount());
	info->ShowDirs(kJTrue);

	info->ShowFiles(kJFalse);
	JAssertEqual(2, info->GetEntryCount());
	info->ShowFiles(kJTrue);

	info->SetWildcardFilter(JString("*oob*", kJFalse));
	JAssertTrue(info->HasWildcardFilter());
	JAssertEqual(4, info->GetEntryCount());		// 2 files + 2 dirs
	info->ShouldApplyWildcardFilterToDirs();
	JAssertEqual(2, info->GetEntryCount());		// 2 files
	info->ClearWildcardFilter();
	JAssertFalse(info->HasWildcardFilter());
	info->ShouldApplyWildcardFilterToDirs(kJFalse);

	JAssertOK(info->GoDown(JString("bar", kJFalse)));
	JAssertTrue(info->DirectoryExists());
	JAssertEqual(0, info->GetEntryCount());
	JAssertOK(info->GoUp());
	JAssertStringsEqual(path, info->GetDirectory());

	system("ln -s foobaz zaboof");
	info->ForceUpdate();
	JAssertEqual(6, info->GetEntryCount());

	info->SetWildcardFilter(JString("*oof*", kJFalse));
	JIndex i;
	JAssertTrue(info->FindEntry(JString("zaboof", kJFalse), &i));
	const JDirEntry& e1 = info->GetEntry(i);
	JAssertTrue(e1.IsFile());
	JAssertTrue(e1.IsLink());
	JAssertTrue(e1.IsReadable());
	JAssertTrue(e1.IsWritable());
	JAssertFalse(e1.IsExecutable());
	const JDirEntry e2 = e1.FollowLink();
	JAssertTrue(e2.IsFile());
	JAssertFalse(e2.IsLink());
	JAssertStringsEqual("foobaz", e2.GetName());
	JAssertTrue(e2.IsReadable());
	JAssertTrue(e2.IsWritable());
	JAssertFalse(e2.IsExecutable());
	JAssertEqual(0, e2.GetSize());
	info->ClearWildcardFilter();

	JAssertTrue(info->ClosestMatch(JString("za", kJFalse), &i));
	const JDirEntry& e3 = info->GetEntry(i);
	JAssertStringsEqual("zaboof", e3.GetName());

	JAssertTrue(info->ClosestMatch(JString("dx", kJFalse), &i));
	const JDirEntry& e4 = info->GetEntry(i);
	JAssertStringsEqual("foobar", e4.GetName());

	JAssertTrue(info->ClosestMatch(JString("fook", kJFalse), &i));
	const JDirEntry& e5 = info->GetEntry(i);
	JAssertStringsEqual("fooshug", e5.GetName());

	JAssertTrue(JKillDirectory(path));
}

JTEST(WildcardFilter)
{
	JString s1, s2;
	JAssertFalse(JDirInfo::BuildRegexFromWildcardFilter(s1, &s2));

	s1 = "*.cc *.h";
	JAssertTrue(JDirInfo::BuildRegexFromWildcardFilter(s1, &s2));
	JAssertStringsEqual("^.*\\.cc$|^.*\\.h$", s2);
}
