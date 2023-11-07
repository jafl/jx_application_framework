/******************************************************************************
 test_JFileArray.cpp

	Program to test JFileArray class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JFileArray.h"
#include "JBroadcastTester.h"
#include "jFileUtil.h"
#include "jAssert.h"

const JFileVersion	kVersionOfExistingFile = 5;
const JFAID_t		kEmbeddedFileID        = 10;

static const JString kTestFileName("/tmp/file_array_test.dat", JString::kNoCopy);
static const JUtf8Byte* kTestFileSignature = "jfilearray_test_sig";

int main()
{
	return JTestManager::Execute();
}

void NewFileTest
	(
	JFileArray&	a,
	const long	removeIndex,
	const long	embeddedFileCount
	)
{
	JBroadcastTester snoop(&a);

	JAssertEqual(embeddedFileCount, a.GetItemCount());
	JAssertFalse(a.GetVersion() == kVersionOfExistingFile);

	a.SetVersion(kVersionOfExistingFile);
	JAssertTrue(a.GetVersion() == kVersionOfExistingFile);


	snoop.Expect(JFileArray::kItemInserted);
{
	std::ostringstream dataStream;
	JString elementData("This was the first element that was added to the file.", JString::kNoCopy);

	dataStream << elementData;
	a.AppendItem(dataStream);
}
	JAssertEqual(1+embeddedFileCount, a.GetItemCount());


	snoop.Expect(JFileArray::kItemInserted);
{
	std::ostringstream dataStream;
	JString elementData("This was the second element that was added to the file.", JString::kNoCopy);

	dataStream << elementData;
	a.PrependItem(dataStream);
}
	JAssertEqual(2+embeddedFileCount, a.GetItemCount());


	snoop.Expect(JFileArray::kItemRemoved);
	a.RemoveItem(JFAIndex(removeIndex));
	JAssertEqual(1+embeddedFileCount, a.GetItemCount());


	snoop.Expect(JFileArray::kItemInserted);
{
	std::ostringstream dataStream;
	JString elementData("This was the third element that was added to the file", JString::kNoCopy);

	dataStream << elementData;
	a.PrependItem(dataStream);
}
	JAssertEqual(2+embeddedFileCount, a.GetItemCount());


{
	JFAID id = 3;
	std::string data;
	a.GetItem(id, &data);

	std::istringstream dataStream(data);
	JString elementData;
	dataStream >> elementData;
	JAssertStringsEqual(
		embeddedFileCount > 0 ?
			"This was the second element that was added to the file." :
			"This was the third element that was added to the file",
		elementData);
}


	snoop.Expect(JFileArray::kItemMoved);
	a.MoveItemToIndex(1,2);


	snoop.Expect(JFileArray::kItemChanged);
{
	JFAIndex index = 1;

	std::ostringstream dataStream;
	JString elementData("1st element shortended", JString::kNoCopy);

	dataStream << elementData;
	a.SetItem(index,dataStream);
}


	snoop.Expect(JFileArray::kItemChanged);
{
	JFAIndex index = 2;

	std::ostringstream dataStream;
	JString elementData("2nd element shortended", JString::kNoCopy);

	dataStream << elementData;
	a.SetItem(index,dataStream);
}


	snoop.Expect(JFileArray::kItemChanged);
{
	JFAIndex index = 1;

	std::ostringstream dataStream;
	JString elementData("Now the first element is really, really long!!!", JString::kNoCopy);

	dataStream << elementData;
	a.SetItem(index,dataStream);
}


	snoop.Expect(JFileArray::kItemChanged);
{
	JFAIndex index = 2;

	std::ostringstream dataStream;
	JString elementData("Now the second element is also very much longer!!!", JString::kNoCopy);

	dataStream << elementData;
	a.SetItem(index,dataStream);
}


	snoop.Expect(JFileArray::kItemsSwapped);
	a.SwapItems(1,2);
}

void OldFileTest
	(
	const JFileArray&	a,
	const long			embeddedFileCount
	)
{
	JAssertEqual(2+embeddedFileCount, a.GetItemCount());
	JAssertTrue(a.GetVersion() == kVersionOfExistingFile);

	JString elementData;

	for (JIndex i : { 1,2 })
	{
		JFAIndex index = i;
		JFAID    id;
		JAssertTrue(a.IndexToID(index, &id));
		JAssertEqual(4-i+embeddedFileCount, id.GetID());

		std::string data;
		a.GetItem(index, &data);

		std::istringstream dataStream(data);
		dataStream >> elementData;

		JAssertEqual(
			i == 1 ?
				"Now the second element is also very much longer!!!" :
				"Now the first element is really, really long!!!",
			elementData);
	}
}

JTEST(Exercise)
{
	JRemoveFile(kTestFileName);

	// constructor creating file

	JFileArray* a1;
	JError err = JFileArray::Create(kTestFileName, kTestFileSignature, &a1);
	JAssertOK(err);
	jdelete a1;

	// constructor with empty file

	err = JFileArray::Create(kTestFileName, kTestFileSignature, &a1);
	JAssertOK(err);

	// embedded file

	JFileArray* a2;
	err = JFileArray::Create(a1, kEmbeddedFileID, &a2);
	JAssertOK(err);

	// exercise both files

	NewFileTest(*a2,2,0);	// test the embedded file first
	NewFileTest(*a1,3,1);

	OldFileTest(*a1, 1);
	OldFileTest(*a2, 0);

	jdelete a2;				// delete embedded first
	jdelete a1;

	JRemoveFile(kTestFileName);
}
