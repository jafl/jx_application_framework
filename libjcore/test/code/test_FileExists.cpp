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
		cout << err.GetMessage() << endl;
		}

	remove(testFileName);
	cout << "File exists   (F): " << JFileExists(testFileName) << endl;
	cout << "File writable (F): " << JFileWritable(testFileName) << endl;

	ofstream file(testFileName);
	file << "abc";
	file.close();
	cout << "File exists   (T): " << JFileExists(testFileName) << endl;
	cout << "File writable (T): " << JFileWritable(testFileName) << endl;

	remove(testFileName);

	JString dirName;
	cout << "Enter a directory name: ";
	cin >> dirName;
	JInputFinished();

	cout << "Directory exists  : " << JDirectoryExists(dirName) << endl;
	cout << "Directory writable: " << JDirectoryWritable(dirName) << endl;

	cout << endl;
	cout << "Creating directory 'junk'" << endl;
	err = JCreateDirectory("junk");
	if (!err.OK())
		{
		cout << err.GetMessage() << endl;
		}
	cout << "Renaming directory to 'junk2'" << endl;
	err = JRenameDirectory("junk", "junk2");
	if (!err.OK())
		{
		cout << err.GetMessage() << endl;
		}
	cout << "Deleting directory 'junk2'" << endl;
	err = JRemoveDirectory("junk2");
	if (!err.OK())
		{
		cout << err.GetMessage() << endl;
		cout << "Killing directory 'junk2'" << endl;
		const JBoolean ok = JKillDirectory("junk2");
		cout << "Kill succeeded: " << ok << endl;
		}

	JString path;
	const JBoolean ok = JGetTempDirectory(&path);
	assert( ok );

	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=200; i++)
		{
		JString* fileName = new JString();
		assert( fileName != NULL );

		err = JCreateTempFile(path, NULL, fileName);
		if (err.OK())
			{
			fileList.Append(fileName);
			}
		else
			{
			cout << err.GetMessage() << endl;
			}
		}
	cout << endl << "Contents of " << path << ":" << endl << endl;
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
