/******************************************************************************
 test_jFileUtils.cc

	Program to test file & directory utils.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jFStreamUtil.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(File)
{
	JString testFileName;
	JAssertOK(JCreateTempFile(&testFileName));

	JAssertTrue(JNameUsed(testFileName));

	JAssertOK(JRemoveFile(testFileName));
	JAssertFalse(JFileExists(testFileName));
	JAssertFalse(JFileWritable(testFileName));

	std::ofstream file(testFileName.GetBytes());
	file << "abc";
	file.close();
	JAssertTrue(JFileExists(testFileName));
	JAssertTrue(JFileWritable(testFileName));

	JAssertOK(JRemoveFile(testFileName));
}

JTEST(Symlink)
{
	JString f1("test_JString.dat", 0, kJFalse);
	JString f2("test_JFileUtil.dat", 0, kJFalse);

	JAssertOK(JCreateSymbolicLink(f1, f2));
	JAssertTrue(JSameDirEntry(f1, f2));

	JString f3;
	JAssertOK(JGetSymbolicLinkTarget(f2, &f3));
	JAssertTrue(JSameDirEntry(f1, f3));

	JAssertOK(JRemoveFile(f2));
}

JTEST(Permissions)
{
	JAssertStringsEqual("rwxr-xr-x", JGetPermissionsString(0755));
}

JTEST(Directory)
{
	JString dirName("code", 0, kJFalse);
	JAssertTrue(JDirectoryExists(dirName));
	JAssertTrue(JDirectoryWritable(dirName));
	JAssertTrue(JCanEnterDirectory(dirName));

	JString d1("junk", 0, kJFalse);
	JAssertOK(JCreateDirectory(d1));

	JString d2("junk2", 0, kJFalse);
	JAssertOK(JRenameDirectory(d1, d2));

	JAssertOK(JRemoveDirectory(d2));

	JString path;
	JAssertOK(JCreateTempDirectory(NULL, NULL, &path));
	JAssertOK(JRemoveDirectory(path));

	JAssertTrue(JGetTempDirectory(&path));

	JAssertTrue(JSearchSubdirs(
		JString(".", 0, kJFalse),
		JString("test_JFileUtil.cpp", 0, kJFalse),
		kJTrue, kJFalse, &path));
}

JTEST(Files)
{
	JString path("/tmp/junk", 0);

	JError err = JCreateDirectory(path);
	JAssertOK(err);

	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=10; i++)
		{
		JString* fileName = jnew JString();
		assert( fileName != NULL );

		const JError err = JCreateTempFile(&path, NULL, fileName);
		JAssertOK(err);
		fileList.Append(fileName);
		}

	std::cout << std::endl << "Contents of " << path << ":" << std::endl << std::endl;
	const JString cmd = "ls " + path;
	system(cmd.GetBytes());

	JAssertTrue(JKillDirectory(path));
}

JTEST(Path)
{
	JString s = JCombinePathAndName(
		JString("/usr", 0, kJFalse), JString("local", 0, kJFalse));
	JAssertStringsEqual("/usr/local", s);

	JAppendDirSeparator(&s);
	JAppendDirSeparator(&s);
	JAssertStringsEqual("/usr/local/", s);

	JString path, name;
	JAssertTrue(JSplitPathAndName(s, &path, &name));
	JAssertStringsEqual("/usr", path);
	JAssertStringsEqual("local", name);

	JStripTrailingDirSeparator(&s);
	JAssertStringsEqual("/usr/local", s);

	s = "//usr/./local/../local/";
	JAssertTrue(JGetTrueName(s, &name));
	JAssertStringsEqual("/usr/local/", name);

	JCleanPath(&s);
	JAssertStringsEqual("/usr/local/../local/", s);
	JAssertTrue(JIsAbsolutePath(s));

	s = JGetRootDirectory();
	JAssertTrue(JIsRootDirectory(s));

	s = "./code";
	JAssertTrue(JIsRelativePath(s));

	JAssertTrue(JConvertToAbsolutePath(s, NULL, &path));
	s = JConvertToRelativePath(path, JGetCurrentDirectory());
	JAssertStringsEqual("./code", s);

	s = JGetClosestDirectory(
		JString("/usr/include/zzz/junk/foo/bar/baz.cc", 0, kJFalse));
	JAssertStringsEqual("/usr/include", s);
}
