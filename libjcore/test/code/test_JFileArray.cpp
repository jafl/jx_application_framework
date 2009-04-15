/******************************************************************************
 test_JFileArray.cc

	Program to test JFileArray class.

	Written by John Lindal.

 ******************************************************************************/

#include <JFileArray.h>
#include <JString.h>
#include <jCommandLine.h>
#include <JBroadcastSnooper.h>
#include <stdio.h>
#include <stdlib.h>
#include <jAssert.h>

const JFileVersion	kVersionOfExistingFile = 5;
const JFAID_t		kEmbeddedFileID        = 10;

static const JCharacter* kTestFileName      = "file_array_test.dat";
static const JCharacter* kTestFileSignature = "jfilearray_test_sig";

void NewFileTest(JFileArray& a, long objectIndex, long embeddedFileCount);
void OldFileTest(JFileArray& a);

int main()
{
	long i;

	cout << "Enter 0 for testing new file, 1 for testing existing file: ";
	cin >> i;
	JInputFinished();

	{																// constructor
	JFileArray* a1;
	JError createErr = JFileArray::Create(kTestFileName, kTestFileSignature, &a1);
	if (createErr == JFileArray::kWrongSignature)
		{
		cerr << "The file has the wrong signature." << endl;
		return 1;
		}
	else if (createErr == JFileArray::kFileAlreadyOpen)
		{
		cerr << "The file has already been opened by another program." << endl;
		return 1;
		}
	assert( createErr.OK() );
	delete a1;
	}

JFileArray* a1;											// constructor on empty file
JError createErr = JFileArray::Create(kTestFileName, kTestFileSignature, &a1);
assert( createErr.OK() );

	cout << endl << "file array a1 created" << endl;

JBroadcastSnooper snoop1(a1);

	cout << "a1 address: " << (void*) a1 << endl << endl;

JFileArray* a2;														// constructor
createErr = JFileArray::Create(a1, kEmbeddedFileID, &a2);
assert( createErr.OK() );

	cout << endl << "file array a2 created" << endl;

JBroadcastSnooper snoop2(a2);

	cout << "a2 address: " << (void*) a2 << endl << endl;

	JWaitForReturn();

	// run the requested tests

	if (i==0)
		{
		NewFileTest(*a2,2,0);	// test the embedded file first
		JWaitForReturn();
		NewFileTest(*a1,1,1);
		}
	else if (i==1)
		{
		OldFileTest(*a1);
		JWaitForReturn();
		OldFileTest(*a2);
		}
	else
		{
		cout << "arf!";
		}

	delete a2;	// delete embedded first
	delete a1;

	return 0;
}



void NewFileTest
	(
	JFileArray&	a,
	long		objectIndex,
	long		embeddedFileCount
	)
{
	cout << "array itemCount should be " << embeddedFileCount << endl;
	cout << "array itemCount = " << a.GetElementCount() << endl << endl;

	JFileVersion vers = a.GetVersion();

	if (vers == kVersionOfExistingFile)
		{
		cout << "You asked to test a new file." << endl;
		cout << "Please delete the existing file first." << endl;
		return;
		}

	a.SetVersion(kVersionOfExistingFile);

	cout << "array version = " << a.GetVersion() << endl;

	{
	std::ostringstream dataStream;
	JString    elementData = "This was the first element that was added to the file.";

	dataStream << elementData;
	a.AppendElement(dataStream);
	}

	{
	std::ostringstream dataStream;
	JString    elementData = "This was the second element that was added to the file.";

	dataStream << elementData;
	a.PrependElement(dataStream);
	}

	{
	// we can't remove the embedded file, and we can't remove a non-existent element

	JFAIndex index = 3 - objectIndex + 1;
	a.RemoveElement(index);
	}

	{
	std::ostringstream dataStream;
	JString    elementData = "This was the third element that was added to the file";

	dataStream << elementData;
	a.PrependElement(dataStream);
	}

	cout << "array itemCount should be " << 2+embeddedFileCount << endl;
	cout << "array itemCount = " << a.GetElementCount() << endl << endl;

	{
	JFAID id = 3;
	std::string data;
	a.GetElement(id, &data);

	std::istringstream dataStream(data);
	JString elementData;
	dataStream >> elementData;
	cout << "Element with id 3 is: " << elementData << endl;
	}

	a.MoveElementToIndex(1,2);

	{
	JFAIndex index = 1;

	std::ostringstream dataStream;
	JString    elementData = "1st element shortended";

	dataStream << elementData;
	a.SetElement(index,dataStream);
	}

	{
	JFAIndex index = 2;

	std::ostringstream dataStream;
	JString    elementData = "2nd element shortended";

	dataStream << elementData;
	a.SetElement(index,dataStream);
	}

	{
	JFAIndex index = 1;

	std::ostringstream dataStream;
	JString    elementData = "Now the first element is really, really long!!!";

	dataStream << elementData;
	a.SetElement(index,dataStream);
	}

	{
	JFAIndex index = 2;

	std::ostringstream dataStream;
	JString    elementData = "Now the second element is also very much longer!!!";

	dataStream << elementData;
	a.SetElement(index,dataStream);
	}

	a.SwapElements(1,2);
}



void OldFileTest
	(
	JFileArray& a
	)
{
	cout << "array itemCount = " << a.GetElementCount() << endl << endl;

	JFileVersion vers = a.GetVersion();

	if (vers != kVersionOfExistingFile)
		{
		cout << "You asked to test an existing file." << endl;
		cout << "Please run the new file test first." << endl;
		return;
		}

	{
	const JSize elementCount = a.GetElementCount();

	JString elementData;

	for (JIndex i=1; i<=elementCount; i++)
		{
		JFAIndex index = i;
		JFAID    id;
		const JBoolean ok = a.IndexToID(index, &id);
		assert( ok );

		if (id.GetID() != kEmbeddedFileID)
			{
			std::string data;
			a.GetElement(index, &data);

			std::istringstream dataStream(data);
			dataStream >> elementData;
			cout << "Element " << i << " has id = " << id.GetID() << endl;
			cout << "Element " << i << " is: " << elementData << endl << endl;
			}
		else
			{
			cout << "Element " << i << " is an embedded file." << endl;
			}
		}
	}
}
