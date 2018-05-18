/******************************************************************************
 test_jFStreamUtil.cc

	Program to test fstream utilities.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <jFStreamUtil.h>
#include <stdio.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	JString fileName("test_jFStreamUtil_tmp_file", 0);

	std::ofstream file0(fileName.GetBytes());
	file0 << "0123456789";
	file0.close();		// force write to disk
	std::fstream file1(fileName.GetBytes(), kJTextFile);
	JAssertEqual(10, JGetFStreamLength(file1));

	std::fstream* file2 = JSetFStreamLength(fileName, file1, 20, kJTextFile);
	JAssertFalse((file1.rdbuf())->is_open());
	JAssertEqual(20, JGetFStreamLength(*file2));

	std::fstream* file3 = JSetFStreamLength(fileName, *file2, 5, kJTextFile);
	JAssertFalse((file2->rdbuf())->is_open());
	JAssertEqual(5, JGetFStreamLength(*file3));

	file3->close();

	std::ofstream file4(fileName.GetBytes());
	file4.close();

	file1.open(fileName.GetBytes(), kJTextFile);
	JAssertEqual(0, JGetFStreamLength(file1));
	file1.close();

	remove(fileName.GetBytes());
}
