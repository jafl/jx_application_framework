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
#include <jassert_simple.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(File)
{
	JString testFileName;
	JError err = JCreateTempFile(&testFileName);
	JAssertOK(err);

	err = JRemoveFile(testFileName);
	JAssertOK(err);
	JAssertFalse(JFileExists(testFileName));
	JAssertFalse(JFileWritable(testFileName));

	ofstream file(testFileName);
	file << "abc";
	file.close();
	JAssertTrue(JFileExists(testFileName));
	JAssertTrue(JFileWritable(testFileName));

	err = JRemoveFile(testFileName);
	JAssertOK(err);
}

JTEST(Directory)
{
	JString dirName = "code";
	JAssertTrue(JDirectoryExists(dirName));
	JAssertTrue(JDirectoryWritable(dirName));

	err = JCreateDirectory("junk");
	JAssertOK(err);

	err = JRenameDirectory("junk", "junk2");
	JAssertOK(err);

	err = JRemoveDirectory("junk2");
	JAssertOK(err);

	JString path;
	JAssertTrue(JGetTempDirectory(&path));
}

JTEST(Files)
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=200; i++)
		{
		JString* fileName = jnew JString();
		assert( fileName != NULL );

		err = JCreateTempFile(path, NULL, fileName);
		JAssertOK(err);
		fileList.Append(fileName);
		}

	cout << endl << "Contents of " << path << ":" << endl << endl;
	const JString cmd = "ls " + path;
	system(cmd);

	JAssertTrue(JKillDirectory("junk"));
}
