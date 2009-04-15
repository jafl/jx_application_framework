#include <jFStreamUtil.h>
#include <JString.h>
#include <stdio.h>
#include <jGlobals.h>
#include <ace/OS.h>
#include <jAssert.h>

int
main
	(
	int argc,
	char** argv
	)
{
	JInitCore();

	JString fileName;
	if (!(JGetChooseSaveFile())->SaveFile("Save file as:", NULL, "junk", &fileName))
		{
		return 0;
		}

	ofstream file0(fileName);
	file0 << "0123456789";
	file0.close();		// force write to disk
	fstream file1(fileName, kJTextFile);
	cout << "Length of file (10): " << JGetFStreamLength(file1) << endl;

	fstream* file2 = JSetFStreamLength(fileName, file1, 20, kJTextFile);
	cout << "Old fstream open? (0) " << (file1.rdbuf())->is_open() << endl;
	cout << "Length of file (20): " << JGetFStreamLength(*file2) << endl;

	fstream* file3 = JSetFStreamLength(fileName, *file2, 5, kJTextFile);
	cout << "Old fstream open? (0) " << (file2->rdbuf())->is_open() << endl;
	cout << "Length of file (5): " << JGetFStreamLength(*file3) << endl;

	file3->close();

	ofstream file4(fileName);
	file4.close();

	file1.open(fileName, kJTextFile);
	cout << "default open of ofstream should erase file" << endl;
	cout << "Length of file (0): " << JGetFStreamLength(file1) << endl;
	file1.close();

	remove(fileName);

	return 0;
}
