/******************************************************************************
 test_JPrefsFile.cpp

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
	std::cout << std::endl << '.' << kTestFileName << ".pref created in home directory" << std::endl;

	JPrefsFile* prefsFile;
	const JError err =
		JPrefsFile::Create(kTestFileName, &prefsFile);
	assert( err.OK() );
	gPrefsFile = prefsFile;
	assert( gPrefsFile != nullptr );

	long i;
	std::cout << "Enter 0 for testing new file, 1 for testing existing file: ";
	std::cin >> i;
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
		std::cout << "arf!";
		}

	jdelete gPrefsFile;

	return 0;
}



void NewFileTest()
{
	std::cout << "itemCount should be 0" << std::endl;
	std::cout << "itemCount = " << gPrefsFile->GetElementCount() << std::endl << std::endl;

	JFileVersion vers = gPrefsFile->GetVersion();

	if (vers == kVersionOfExistingFile)
		{
		std::cout << "You asked to test a new file." << std::endl;
		std::cout << "Please delete the existing file first." << std::endl;
		return;
		}

	gPrefsFile->SetVersion(kVersionOfExistingFile);

	std::cout << "array version = " << gPrefsFile->GetVersion() << std::endl;

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

	std::cout << "itemCount should be 2" << std::endl;
	std::cout << "itemCount = " << gPrefsFile->GetElementCount() << std::endl << std::endl;

	{
	std::string data;
	gPrefsFile->GetData(7, &data);

	std::istringstream dataStream(data);
	JString elementData;
	dataStream >> elementData;
	std::cout << "Element with id 7 is: " << elementData << std::endl;
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
	std::cout << "itemCount = " << gPrefsFile->GetElementCount() << std::endl << std::endl;

	JFileVersion vers = gPrefsFile->GetVersion();

	if (vers != kVersionOfExistingFile)
		{
		std::cout << "You asked to test an existing file." << std::endl;
		std::cout << "Please run the new file test first." << std::endl;
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
		std::cout << "Element " << i << " has id = " << id.GetID() << std::endl;
		std::cout << "Element " << i << " is: " << elementData << std::endl << std::endl;
		}
}
