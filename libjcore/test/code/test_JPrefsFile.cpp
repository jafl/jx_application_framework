/******************************************************************************
 test_JPrefsFile.cc

	Program to test JPrefsFile classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JPrefsFile.h>
#include <JString.h>

#include <jCommandLine.h>

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <jAssert.h>

JPrefsFile*	gPrefsFile;

const JFileVersion kVersionOfExistingFile = 5;

const JCharacter* kTestFileName = "PrefsFileTest";

void NewFileTest();
void OldFileTest();

int main()
{
	cout << endl << '.' << kTestFileName << ".pref created in home directory" << endl;

	JPrefsFile* prefsFile;
	const JError err =
		JPrefsFile::Create(kTestFileName, &prefsFile);
	assert( err.OK() );
	gPrefsFile = prefsFile;
	assert( gPrefsFile != NULL );

	long i;
	cout << "Enter 0 for testing new file, 1 for testing existing file: ";
	cin >> i;
	JInputFinished();

	// run the requested tests

	if (i==0)
		{
		NewFileTest();
		}
	else if (i==1)
		{
		OldFileTest();
		}
	else
		{
		cout << "arf!";
		}

	delete gPrefsFile;

	return 0;
}



void NewFileTest()
{
	cout << "itemCount should be 0" << endl;
	cout << "itemCount = " << gPrefsFile->GetElementCount() << endl << endl;

	JFileVersion vers = gPrefsFile->GetVersion();

	if (vers == kVersionOfExistingFile)
		{
		cout << "You asked to test a new file." << endl;
		cout << "Please delete the existing file first." << endl;
		return;
		}

	gPrefsFile->SetVersion(kVersionOfExistingFile);

	cout << "array version = " << gPrefsFile->GetVersion() << endl;

	{
	std::ostringstream dataStream;
	JString    elementData = "This was the first element that was added to the file.";

	dataStream << elementData;
	gPrefsFile->SetData(3, dataStream);
	}

	{
	std::ostringstream dataStream;
	JString    elementData = "This was the second element that was added to the file.";

	dataStream << elementData;
	gPrefsFile->SetData(5, dataStream);
	}

	gPrefsFile->RemoveData(3);

	{
	std::ostringstream dataStream;
	JString    elementData = "This was the third element that was added to the file";

	dataStream << elementData;
	gPrefsFile->SetData(7, dataStream);
	}

	cout << "itemCount should be 2" << endl;
	cout << "itemCount = " << gPrefsFile->GetElementCount() << endl << endl;

	{
	std::string data;
	gPrefsFile->GetData(7, &data);

	std::istringstream dataStream(data);
	JString elementData;
	dataStream >> elementData;
	cout << "Element with id 7 is: " << elementData << endl;
	}

	{
	std::ostringstream dataStream;
	JString    elementData = "1st element shortended";
	dataStream << elementData;
	gPrefsFile->SetData(5, dataStream);
	}

	{
	std::ostringstream dataStream;
	JString    elementData = "2nd element shortended";
	dataStream << elementData;
	gPrefsFile->SetData(7, dataStream);
	}

	{
	std::ostringstream dataStream;
	JString    elementData = "Now the first element is really, really long!!!";
	dataStream << elementData;
	gPrefsFile->SetData(5, dataStream);
	}

	{
	std::ostringstream dataStream;
	JString    elementData = "Now the second element is also very much longer!!!";
	dataStream << elementData;
	gPrefsFile->SetData(7, dataStream);
	}
}



void OldFileTest()
{
	cout << "itemCount = " << gPrefsFile->GetElementCount() << endl << endl;

	JFileVersion vers = gPrefsFile->GetVersion();

	if (vers != kVersionOfExistingFile)
		{
		cout << "You asked to test an existing file." << endl;
		cout << "Please run the new file test first." << endl;
		return;
		}

	const JSize elementCount = gPrefsFile->GetElementCount();

	JString elementData;

	for (JIndex i=1; i<=elementCount; i++)
		{
		JPrefID id;
		const JBoolean ok = gPrefsFile->IndexToID(i, &id);
		assert( ok );

		std::string data;
		gPrefsFile->GetData(id, &data);

		std::istringstream dataStream(data);
		dataStream >> elementData;
		cout << "Element " << i << " has id = " << id.GetID() << endl;
		cout << "Element " << i << " is: " << elementData << endl << endl;
		}
}
