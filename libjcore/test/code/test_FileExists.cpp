#include <JString.h>
#include <JPtrArray.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jCommandLine.h>
#include <stdio.h>
#include <jAssert.h>

int main
	(
	int argc,
	char** argv
	)
{
	JString testFileName;
	JError err = JCreateTempFile(&testFileName);
	if (!err.OK())
		{
		std::cout << err.GetMessage() << std::endl;
		}

	remove(testFileName);
	std::cout << "File exists   (F): " << JFileExists(testFileName) << std::endl;
	std::cout << "File writable (F): " << JFileWritable(testFileName) << std::endl;

	std::ofstream file(testFileName);
	file << "abc";
	file.close();
	std::cout << "File exists   (T): " << JFileExists(testFileName) << std::endl;
	std::cout << "File writable (T): " << JFileWritable(testFileName) << std::endl;

	remove(testFileName);

	JString dirName;
	std::cout << "Enter a directory name: ";
	std::cin >> dirName;
	JInputFinished();

	std::cout << "Directory exists  : " << JDirectoryExists(dirName) << std::endl;
	std::cout << "Directory writable: " << JDirectoryWritable(dirName) << std::endl;

	std::cout << std::endl;
	std::cout << "Creating directory 'junk'" << std::endl;
	err = JCreateDirectory("junk");
	if (!err.OK())
		{
		std::cout << err.GetMessage() << std::endl;
		}
	std::cout << "Renaming directory to 'junk2'" << std::endl;
	err = JRenameDirectory("junk", "junk2");
	if (!err.OK())
		{
		std::cout << err.GetMessage() << std::endl;
		}
	std::cout << "Deleting directory 'junk2'" << std::endl;
	err = JRemoveDirectory("junk2");
	if (!err.OK())
		{
		std::cout << err.GetMessage() << std::endl;
		std::cout << "Killing directory 'junk2'" << std::endl;
		const JBoolean ok = JKillDirectory("junk2");
		std::cout << "Kill succeeded: " << ok << std::endl;
		}

	JString path;
	const JBoolean ok = JGetTempDirectory(&path);
	assert( ok );

	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=200; i++)
		{
		JString* fileName = jnew JString();
		assert( fileName != NULL );

		err = JCreateTempFile(path, NULL, fileName);
		if (err.OK())
			{
			fileList.Append(fileName);
			}
		else
			{
			std::cout << err.GetMessage() << std::endl;
			}
		}
	std::cout << std::endl << "Contents of " << path << ":" << std::endl << std::endl;
#ifdef WIN32
	const JString cmd = "dir " + path;
#else
	const JString cmd = "ls " + path;
#endif
	system(cmd);
	while (!fileList.IsEmpty())
		{
		remove(*(fileList.FirstElement()));
		fileList.DeleteElement(1);
		}

	return 0;
}
