/******************************************************************************
 test_jFStreamUtil.cc

	Program to test fstream utilities.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <jFStreamUtil.h>
#include <stdio.h>
#include <jassert_simple.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(Exercise)
{
	const JUtf8Byte* fileName = "test_jFStreamUtil_tmp_file";

	JString fName;
	fName = fileName;

	ofstream file0(fileName);
	file0 << "0123456789";
	file0.close();		// force write to disk
	fstream file1(fileName, kJTextFile);
	JAssertEqual(10, JGetFStreamLength(file1));

	fstream* file2 = JSetFStreamLength(fName, file1, 20, kJTextFile);
	JAssertFalse((file1.rdbuf())->is_open());
	JAssertEqual(20, JGetFStreamLength(*file2));

	fstream* file3 = JSetFStreamLength(fName, *file2, 5, kJTextFile);
	JAssertFalse((file2->rdbuf())->is_open());
	JAssertEqual(5, JGetFStreamLength(*file3));

	file3->close();

	ofstream file4(fileName);
	file4.close();

	file1.open(fileName, kJTextFile);
	JAssertEqual(0, JGetFStreamLength(file1));
	file1.close();

	remove(fileName);
}
