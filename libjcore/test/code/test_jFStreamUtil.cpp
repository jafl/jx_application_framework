#include <jFStreamUtil.h>
#include <JString.h>
#include <stdio.h>
#include <jGlobals.h>
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

	std::ofstream file0(fileName);
	file0 << "0123456789";
	file0.close();		// force write to disk
	std::fstream file1(fileName, kJTextFile);
	std::cout << "Length of file (10): " << JGetFStreamLength(file1) << std::endl;

	std::fstream* file2 = JSetFStreamLength(fileName, file1, 20, kJTextFile);
	std::cout << "Old std::fstream open? (0) " << (file1.rdbuf())->is_open() << std::endl;
	std::cout << "Length of file (20): " << JGetFStreamLength(*file2) << std::endl;

	std::fstream* file3 = JSetFStreamLength(fileName, *file2, 5, kJTextFile);
	std::cout << "Old std::fstream open? (0) " << (file2->rdbuf())->is_open() << std::endl;
	std::cout << "Length of file (5): " << JGetFStreamLength(*file3) << std::endl;

	file3->close();

	std::ofstream file4(fileName);
	file4.close();

	file1.open(fileName, kJTextFile);
	std::cout << "default open of std::ofstream should erase file" << std::endl;
	std::cout << "Length of file (0): " << JGetFStreamLength(file1) << std::endl;
	file1.close();

	remove(fileName);

	return 0;
}
