/******************************************************************************
 test_jFileUtils.cpp

	Program to test file & directory utils.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "jFileUtil.h"
#include "jDirUtil.h"
#include "jFStreamUtil.h"
#include "JPtrArray-JString.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(File)
{
	JString testFileName;
	JAssertTrue(JCreateTempFile(&testFileName));

	JAssertTrue(JNameUsed(testFileName));
	JAssertTrue(JFileExists(testFileName));
	JAssertTrue(JFileReadable(testFileName));
	JAssertTrue(JFileWritable(testFileName));
	JAssertFalse(JFileExecutable(testFileName));
	JAssertFalse(JDirectoryExists(testFileName));

	JSize size;
	JAssertTrue(JGetFileLength(testFileName, &size));
	JAssertEqual(0, size);

	JAssertTrue(JRemoveFile(testFileName));
	JAssertFalse(JFileExists(testFileName));
	JAssertFalse(JFileWritable(testFileName));

	std::ofstream file(testFileName.GetBytes());
	file << "abc";
	file.close();
	JAssertTrue(JFileExists(testFileName));
	JAssertTrue(JFileWritable(testFileName));

	JAssertTrue(JGetFileLength(testFileName, &size));
	JAssertEqual(3, size);

	JString path, name;
	JAssertTrue(JSplitPathAndName(testFileName, &path, &name));

	JString s = JGetUniqueDirEntryName(path, JString("test_j_file_util_", JString::kNoCopy));
	JAssertTrue(JRenameFile(testFileName, s));
	JAssertFalse(JFileExists(testFileName));
	JAssertTrue(JFileExists(s));

	JAssertTrue(JRemoveFile(s));
}

JTEST(Symlink)
{
	JString f1("data/test_JString.txt", JString::kNoCopy);
	JString f2("test_jFileUtil.dat", JString::kNoCopy);

	JAssertTrue(JFileExists(f1));
	if (JFileExists(f2))
	{
		JAssertTrue(JRemoveFile(f2));
	}
	JAssertTrue(JCreateSymbolicLink(f1, f2));
	JAssertTrue(JSameDirEntry(f1, f2));

	JString f3;
	JAssertTrue(JGetSymbolicLinkTarget(f2, &f3));
	JAssertTrue(JSameDirEntry(f1, f3));

	JAssertTrue(JRemoveFile(f2));
}

JTEST(Permissions)
{
	JAssertStringsEqual("rwxr-xr-x", JGetPermissionsString(0755));
}

JTEST(Directory)
{
	JString dirName("code", JString::kNoCopy);
	JAssertFalse(JFileExists(dirName));
	JAssertTrue(JDirectoryExists(dirName));
	JAssertTrue(JDirectoryWritable(dirName));
	JAssertTrue(JCanEnterDirectory(dirName));

	JString d1("junk", JString::kNoCopy);
	JAssertTrue(JCreateDirectory(d1));

	JString d2("junk2", JString::kNoCopy);
	JAssertTrue(JRenameDirectory(d1, d2));

	JAssertTrue(JRemoveDirectory(d2));

	JString path;
	JAssertTrue(JCreateTempDirectory(nullptr, nullptr, &path));
	JAssertTrue(JRemoveDirectory(path));

	JAssertTrue(JGetTempDirectory(&path));

	JAssertTrue(JSearchSubdirs(
		JString(".", JString::kNoCopy),
		JString("test_JFileUtil.cpp", JString::kNoCopy),
		true, JString::kIgnoreCase, &path));
}

JTEST(Files)
{
	const JString path("/tmp/junk/junk2/junk3", JString::kNoCopy);
	JAssertTrue(JCreateDirectory(path));

	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=10; i++)
	{
		auto* fileName = jnew JString();

		JAssertTrue(JCreateTempFile(&path, nullptr, fileName));
		fileList.Append(fileName);
	}

	JAssertTrue(JRenameFile(*(fileList.GetFirstItem()), *(fileList.GetLastItem()), true));
	JAssertTrue(JKillDirectory(JString("/tmp/junk", JString::kNoCopy)));
}

JTEST(Path)
{
	JString s = JCombinePathAndName(
		JString("/usr", JString::kNoCopy), JString("local", JString::kNoCopy));
	JAssertStringsEqual("/usr/local", s);

	JAppendDirSeparator(&s);
	JAppendDirSeparator(&s);
	JAssertStringsEqual("/usr/local/", s);

	JString path, name;
	JAssertTrue(JSplitPathAndName(s, &path, &name));
	JAssertStringsEqual("/usr/", path);
	JAssertStringsEqual("local", name);

	JStripTrailingDirSeparator(&s);
	JAssertStringsEqual("/usr/local", s);

	s += "///";
	JStripTrailingDirSeparator(&s);
	JAssertStringsEqual("/usr/local", s);

	s = "//usr/./local/../local/";
	JAssertTrue(JGetTrueName(s, &name));
	JAssertStringsEqual("/usr/local/", name);

	JCleanPath(&s);
	JAssertStringsEqual("/usr/local/../local/", s);
	JAssertTrue(JIsAbsolutePath(s));

	s = "/usr/lib/../..";
	JAssertTrue(JGetTrueName(s, &name));
	JAssertStringsEqual("/", name);

	s = JGetRootDirectory();
	JAssertTrue(JIsRootDirectory(s));

	s = "./code";
	JAssertTrue(JIsRelativePath(s));

	JAssertTrue(JConvertToAbsolutePath(s, JString::empty, &path));
	s = JConvertToRelativePath(path, JGetCurrentDirectory());
	JAssertStringsEqual("./code", s);

	s = JConvertToRelativePath(
		JString("/usr/include", JString::kNoCopy),
		JString("/usr/local", JString::kNoCopy));
	JAssertStringsEqual("../include", s);

	s = "/usr";
	JAssertTrue(JConvertToAbsolutePath(s, JString::empty, &path));
	JAssertStringsEqual("/usr", path);

	s = JGetClosestDirectory(
		JString("/usr/local/zzz/junk/foo/bar/baz.cc", JString::kNoCopy));
	JAssertStringsEqual("/usr/local/", s);

	s = "./test_j_file_util_test_directory";
	JAssertTrue(JCreateDirectory(s));
	s = JGetClosestDirectory(
		JString("./test_j_file_util_test_directory/foo/bar/baz", JString::kNoCopy));
	JAssertStringsEqual("test_j_file_util_test_directory/", s);
	JAssertTrue(JRemoveDirectory(s));
}

JTEST(ExtractFileAndLine)
{
	JString s;
	JIndex start, end;
	JExtractFileAndLine(JString("foo.cpp:5", JString::kNoCopy), &s, &start);
	JAssertEqual("foo.cpp", s);
	JAssertEqual(5, start);

	JExtractFileAndLine(JString("foo.cpp:7-12", JString::kNoCopy), &s, &start, &end);
	JAssertEqual("foo.cpp", s);
	JAssertEqual(7, start);
	JAssertEqual(12, end);
}

JTEST(RootSuffix)
{
	JString s("foo.cpp", JString::kNoCopy);
	JString root, suffix;
	JAssertTrue(JSplitRootAndSuffix(s, &root, &suffix));
	JAssertStringsEqual("foo", root);
	JAssertStringsEqual("cpp", suffix);

	s = JCombineRootAndSuffix(root, suffix);
	JAssertStringsEqual("foo.cpp", s);

	s = "foo.bar.baz";
	JAssertTrue(JSplitRootAndSuffix(s, &root, &suffix));
	JAssertStringsEqual("foo.bar", root);
	JAssertStringsEqual("baz", suffix);

	s = "foo.5";
	JAssertFalse(JSplitRootAndSuffix(s, &root, &suffix));
	JAssertStringsEqual("foo.5", root);
	JAssertTrue(suffix.IsEmpty());

	s = "foo.";
	JAssertFalse(JSplitRootAndSuffix(s, &root, &suffix));
	JAssertStringsEqual("foo.", root);
	JAssertTrue(suffix.IsEmpty());
}

JTEST(Url)
{
	JString s("./test_jFileUtil", JString::kNoCopy), fullName;
	JAssertTrue(JConvertToAbsolutePath(s, JString::empty, &fullName));

	s = JFileNameToURL(fullName);

	JString s2;
	JAssertTrue(JURLToFileName(s, &s2));
	JAssertStringsEqual(fullName, s2);
}

JTEST(HomeDirShortcut)
{
	JString home;
	JAssertTrue(JGetHomeDirectory(&home));

	const JString path = JCombinePathAndName(home, JString("test_j_file_util_test_directory", JString::kNoCopy));
	JAssertTrue(JCreateDirectory(path));

	JString s = JConvertToHomeDirShortcut(path);
	JAssertStringsEqual("~/test_j_file_util_test_directory", s);

	JString s2;
	JAssertTrue(JConvertToAbsolutePath(s, JString::empty, &s2));
	JAssertStringsEqual(path, s2);

	s = JGetClosestDirectory(
		JString("~/test_j_file_util_test_directory/foo/bar/baz", JString::kNoCopy),
		true);
	JAssertStringsEqual("~/test_j_file_util_test_directory/", s);

	JAssertTrue(JRemoveDirectory(path));
}
