/******************************************************************************
 JFileArray.cpp

							The Array-in-a-File Class

	An array implemented as a file.  All data is stored as text and is
	transferred via stringstream objects. Each element can have an
	arbitrary size.

	A JFileArray can be embedded within other JFileArray by storing all the
	embedded file's data inside one element of the enclosing file.
	The single base file object is initialized with the file specifications.
	An embedded file object is initialized with the file object that
	contains it and the ID of the element that contains it.

	The embedding can theoretically go on forever.  In practice, deep
	embedding can be hard to keep track of.

	*** Embedded files must be opened after the base file and
		must be closed before the base file.

	Storage details:

	The first item is the file signature (arbitrary length).
	The second item is the version.
	The third item is the number of elements in the array.
	The fourth item is the offset into the file where the index starts.

	Each element is preceded by its length.
	(This makes it easier to recover the data directly from the file.)

	The positions and id's of the elements in the file are stored
	at the end of the file.  This is handled by JFileArrayIndex.

	Since both the data and index sections have to change size,
	the index was placed at the end because it is usually
	significantly smaller and only needs to be written out when
	the file is closed.

	sig  header      elements (length+data)            index
	+--+--+--+--+-------------------------------+-------------------+
	|  |  |  |  | ++---++---+- - - -++---++---+   +-+-+- - - -+-+-+
	|  |  |  |  | ||   ||   |       ||   ||   |   | | |       | | |
	|  |  |  |  | ||   ||   |       ||   ||   |   | | |       | | |
	|  |  |  |  | ++---++---+- - - -++---++---+   +-+-+- - - -+-+-+
	+--+--+--+--+-------------------------------+-------------------+

	Implementation details:

	Only the base file opens the stream.  All embedded files simply get
	a pointer to the stream so they can use it.

	***	This means that one should never rely on the position of
		the read or write mark unless the previous statement explicitly set it.

	***	Embedded files should not delete itsStream.

	The file's header and index are read in when the file is opened and is
	maintained in memory.  However, the header and index are also written
	to the file whenever they change.  This provides safety in the event of
	program and system crashes.  It also slows the program down, however.
	One could use a flag to tell whether to always update the file or just
	update it when it is closed.

	*** While the file is open, do not rely on the data in the header or
		index sections of the file.  Use the data members and itsFileIndex.
		The information in the space allocated for the header and index is
		undefined.

	*** While the file is open, space for the index is still maintained at
		the end of the file to avoid having to adjust the allocation when
		the index is written out.

	Since some information is only written out when the file is closed,
	only one process at a time can open the disk file.  To guarantee this,
	the high bit of the elementCount is set when the file is opened.
	This does not conflict with other uses because one will run out of
	RAM and disk space long before one needs the high bit in elementCount.
	(If you're not out of space, switch to a real database system!)
	If this bit is set, JFileArray::Create() will return false.

	BASE CLASS = JCollection

	Copyright (C) 1993-98 John Lindal.

 ******************************************************************************/

#include "JFileArray.h"
#include "JFileArrayIndex.h"
#include "JString.h"
#include "JListUtil.h"
#include "jFStreamUtil.h"
#include "jStreamUtil.h"
#include "jFileUtil.h"
#include "jDirUtil.h"
#include "JMinMax.h"
#include "jAssert.h"

const JFileVersion JFileArray::kInitialVersion = 0;
const JIndex JFAIndex::kInvalidIndex           = 0;
const JFAID_t JFAID::kInvalidID                = 0;
const JFAID_t JFAID::kMinID                    = 1;
const JFAID_t JFAID::kMaxID                    = kJUInt32Max - 1;	// avoid rollover

// JBroadcaster message types

const JUtf8Byte* JFileArray::kElementInserted = "ElementInserted::JFileArray";
const JUtf8Byte* JFileArray::kElementRemoved  = "ElementRemoved::JFileArray";
const JUtf8Byte* JFileArray::kElementMoved    = "ElementMoved::JFileArray";
const JUtf8Byte* JFileArray::kElementsSwapped = "ElementsSwapped::JFileArray";
const JUtf8Byte* JFileArray::kElementChanged  = "ElementChanged::JFileArray";

// JError types

const JUtf8Byte* JFileArray::kFileNotWritable = "FileNotWritable::JFileArray";
const JUtf8Byte* JFileArray::kFileAlreadyOpen = "FileAlreadyOpen::JFileArray";
const JUtf8Byte* JFileArray::kWrongSignature  = "WrongSignature::JFileArray";
const JUtf8Byte* JFileArray::kNotEmbeddedFile = "NotEmbeddedFile::JFileArray";

// maximum amount of temporary memory to allocate while
// inserting/deleting space in the file

const JSize maxTempMem = 2000;

// mode for opening std::fstream

const JFStreamOpenMode kFileOpenMode = kJBinaryFile;

// mask for checking lock bit

const unsigned long kFileLockedMask = 0x80000000;

// info for blocking until file is available

const JSize kMaxAttemptCount = 50;
const JFloat kBlockDelay     = 0.25;	// seconds

// size of various data items stored in the file

enum
	{
	// stored at front of file

	kVersionLength       = JFileArray::kUnsignedLongLength,
	kElementCountLength  = JFileArray::kUnsignedLongLength,
	kIndexOffsetLength   = JFileArray::kUnsignedLongLength,

	// these do -not- include the length of the file signature

	kVersionOffset       = 0,
	kElementCountOffset  = kVersionOffset + kVersionLength,
	kIndexOffsetOffset   = kElementCountOffset + kElementCountLength,

	kFileHeaderLength    = kIndexOffsetOffset + kIndexOffsetLength,

	// at the front of each element

	kElementSizeLength   = JFileArray::kUnsignedLongLength
	};

/******************************************************************************
 Constructor function (base file) (static)

	By forcing everyone to use this function, we avoid having to worry
	about errors within the class itself.

	Derived classes must define their own constructor functions
	and check OKToCreateBase() before creating the object.

	Be *very* careful when using kIgnoreIfOpen.  If you always call
	ShouldFlushChanges(true) when you open the file, it should be safe to
	specify kIgnoreIfOpen, but only if JFileArray itself did not cause the
	crash.

	kTryWaitUntilCanOpen does *not* guarantee that Create() will return
	true.  If somebody else never closes the file, Create() will fail.
	This is safer than blocking forever.

	The code for kTryWaitUntilCanOpen and kDeleteIfWaitTimeout suffers from
	a race condition.  If it ever becomes a problem, it will need to use
	lockfile instead.

 ******************************************************************************/

JError
JFileArray::Create
	(
	const JString&		fileName,
	const JUtf8Byte*	fileSignature,
	JFileArray**		obj,
	const CreateAction	action
	)
{
	const JError err = OKToCreateBase(fileName, fileSignature, action);
	if (err.OK())
		{
		*obj = jnew JFileArray(fileName, fileSignature, action);
		assert( *obj != nullptr );
		}
	else
		{
		*obj = nullptr;
		}
	return err;
}

JError
JFileArray::OKToCreateBase
	(
	const JString&		fileName,
	const JUtf8Byte*	fileSignature,
	const CreateAction	action
	)
{
	if (JFileExists(fileName))
		{
		if (!JFileWritable(fileName))
			{
			return FileNotWritable(fileName);
			}

		std::ifstream input(fileName.GetBytes());

		// check file signature

		const JSize sigLength   = strlen(fileSignature);
		const JString signature = JRead(input, sigLength);
		if (signature != fileSignature)
			{
			return WrongSignature(fileName);
			}

		// check open flag

		if (action == kIgnoreIfOpen || !FileIsOpen(input, sigLength))
			{
			return JNoError();
			}
		else if (action == kFailIfOpen)
			{
			return FileAlreadyOpen(fileName);
			}
		else if (action == kDeleteIfOpen)
			{
			return JRemoveFile(fileName);
			}
		else
			{
			for (JIndex i=1; i<=kMaxAttemptCount; i++)
				{
				input.close();
				JWait(kBlockDelay);
				input.open(fileName.GetBytes());	// avoid buffering problems
				if (!FileIsOpen(input, sigLength))
					{
					return JNoError();
					}
				}

			if (action == kDeleteIfWaitTimeout)
				{
				return JRemoveFile(fileName);
				}
			else
				{
				return FileAlreadyOpen(fileName);
				}
			}
		}
	else
		{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);
		if (!JDirectoryWritable(path))
			{
			return JAccessDenied(path);
			}
		}

	return JNoError();
}

/******************************************************************************
 Constructor (base file) (protected)

	Initialize a JFileArray object as the base file.
	Since we are the base class, we create and open the std::fstream.

 ******************************************************************************/

JFileArray::JFileArray
	(
	const JString&		fileName,
	const JUtf8Byte*	fileSignature,
	const CreateAction	action
	)
	:
	JCollection(),
	itsEnclosingFile( nullptr ),
	itsEnclosureElementID( JFAID::kInvalidID )
{
	assert( sizeof(JSize)           >= kUnsignedLongLength &&
			sizeof(JUnsignedOffset) >= kUnsignedLongLength &&
			sizeof(JFileVersion)    >= kUnsignedLongLength );

	// check whether the file already exists on disk

	const bool isNew = !JFileExists(fileName);

	assert( isNew || OKToCreateBase(fileName, fileSignature, action) == kJNoError );

	// open the file for use

	if (isNew)		// hack: gcc 3.2.x doesn't allow std::ios::in if file doesn't exist!
		{
		std::ofstream temp(fileName.GetBytes());
		}

	itsStream = jnew std::fstream;
	assert( itsStream != nullptr );

	itsStream->open(fileName.GetBytes(), kFileOpenMode);
	assert( itsStream->good() );

	itsFileName = jnew JString;
	assert( itsFileName != nullptr );
	const bool ok = JGetTrueName(fileName, itsFileName);
	assert( ok );

	// common initialization

	FileArrayX(isNew, fileSignature);
}

/******************************************************************************
 Constructor function (embedded file) (static)

	By forcing everyone to use this function, we avoid having to worry
	about errors within the class itself.

	Derived classes must define their own constructor functions
	and check OKToCreateEmbedded() before creating the object.

 ******************************************************************************/

JError
JFileArray::Create
	(
	JFileArray*		theEnclosingFile,
	const JFAID&	enclosureElementID,
	JFileArray**	obj
	)
{
	const JError err =
		OKToCreateEmbedded(theEnclosingFile, enclosureElementID);
	if (err.OK())
		{
		*obj = jnew JFileArray(theEnclosingFile, enclosureElementID);
		assert( *obj != nullptr );
		}
	else
		{
		*obj = nullptr;
		}
	return err;
}

JError
JFileArray::OKToCreateEmbedded
	(
	JFileArray*		theEnclosingFile,
	const JFAID&	enclosureElementID
	)
{
	JFAIndex index;
	if (theEnclosingFile->IDToIndex(enclosureElementID, &index))
		{
		JFileArrayIndex* fileIndex = theEnclosingFile->GetFileArrayIndex();
		if (!fileIndex->IsEmbeddedFile(index))
			{
			return NotEmbeddedFile(index.GetIndex());
			}
		else if (!fileIndex->EmbeddedFileIsClosed(index))
			{
			return FileAlreadyOpen(JString("embedded file", JString::kNoCopy));
			}
		}

	return JNoError();
}

/******************************************************************************
 Constructor (embedded file) (protected)

	Initialize a JFileArray object as a nested part of the given JFileArray.

 ******************************************************************************/

JFileArray::JFileArray
	(
	JFileArray*		theEnclosingFile,
	const JFAID&	enclosureElementID
	)
	:
	JCollection(),
	itsEnclosingFile( theEnclosingFile ),
	itsEnclosureElementID( enclosureElementID )
{
	// get the std::fstream and whether we are new from our enclosing file

	bool isNew;
	itsFileName = nullptr;
	itsStream   = theEnclosingFile->OpenEmbeddedFile(this, enclosureElementID, &isNew);
	assert( itsStream != nullptr );

	// common initialization

	FileArrayX(isNew, "");
}

/******************************************************************************
 FileArrayX (private)

	Extra initialization required by all constructors.

 ******************************************************************************/

void
JFileArray::FileArrayX
	(
	const bool		isNew,
	const JUtf8Byte*	fileSignature
	)
{
	itsFileIndex = jnew JFileArrayIndex;
	assert( itsFileIndex != nullptr);

	itsFileSignatureByteCount = strlen(fileSignature);

	itsIsOpenFlag       = false;
	itsFlushChangesFlag = false;

	if (isNew)
		{
		itsVersion = kInitialVersion;
		SetElementCount(0);
		itsIndexOffset = itsFileSignatureByteCount + kFileHeaderLength;

		SetFileLength(itsIndexOffset);

		if (itsFileSignatureByteCount > 0)
			{
			SetReadWriteMark(0, kFromFileStart);
			itsStream->write(fileSignature, itsFileSignatureByteCount);
			}

		WriteVersion();
		WriteElementCount();
		WriteIndexOffset();
		// no index to write out
		}
	else
		{
		ReadVersion();
		ReadElementCount();
		ReadIndexOffset();
		ReadIndex(GetElementCount());
		}

	// set high bit to lock file

	itsIsOpenFlag = true;
	WriteElementCount();
	itsStream->flush();
}

/******************************************************************************
 Destructor

	*** All embedded files must be closed before calling this!

	We could use a Message to notify the owners of embedded files that
	the files have been closed, but it is much easier and safer to simply
	write client code that closes the embedded files first.

 ******************************************************************************/

JFileArray::~JFileArray()
{
	// make sure that all embedded files have been closed

	assert( itsFileIndex->AllEmbeddedFilesAreClosed() );

	// update the file and throw out itsFileIndex

	itsIsOpenFlag       = false;
	itsFlushChangesFlag = true;
	FlushChanges();

	jdelete itsFileIndex;
	itsFileIndex = nullptr;

	// base file deletes std::fstream
	// embedded file notifies enclosing file

	if (itsEnclosingFile != nullptr)
		{
		itsEnclosingFile->EmbeddedFileClosed(itsEnclosureElementID);
		itsEnclosingFile = nullptr;
		}
	else
		{
		itsStream->close();
		jdelete itsStream;
		jdelete itsFileName;
		}

	itsStream   = nullptr;
	itsFileName = nullptr;
}

/******************************************************************************
 OpenEmbeddedFile (private)

	Receive notification of a jnew embedded file.

	Returns a copy of itsStream so embedded file can use it.
	isNew is set to TRUE if the given id was not already in the file.

 ******************************************************************************/

std::fstream*
JFileArray::OpenEmbeddedFile
	(
	JFileArray*		theEmbeddedFile,
	const JFAID&	id,
	bool*		isNew
	)
{
	*isNew = false;

	JFAIndex index;
	if (!IDToIndex(id, &index))
		{
		*isNew = true;

		std::ostringstream emptyStream;
		AppendElement(emptyStream);

		index.SetIndex(GetElementCount());

		itsFileIndex->SetElementID(index, id);
		itsFileIndex->SetToEmbeddedFile(index);
		}

	itsFileIndex->EmbeddedFileOpened(index, theEmbeddedFile);

	return itsStream;
}

/******************************************************************************
 EmbeddedFileClosed (private)

	Receive notification that an embedded file has been closed.

 ******************************************************************************/

void
JFileArray::EmbeddedFileClosed
	(
	const JFAID& id
	)
{
	JFAIndex index;
	const bool ok = IDToIndex(id, &index);
	assert( ok );
	itsFileIndex->EmbeddedFileClosed(index);
}

/******************************************************************************
 GetFileName

	Returns the name of the file on disk.

 ******************************************************************************/

const JString&
JFileArray::GetFileName()
	const
{
	if (itsEnclosingFile != nullptr)
		{
		return itsEnclosingFile->GetFileName();
		}
	else
		{
		assert( itsFileName != nullptr );
		return *itsFileName;
		}
}

/******************************************************************************
 GetElement

	Fills the given string with a copy of the data for the specified
	element.

 ******************************************************************************/

void
JFileArray::GetElement
	(
	const JFAIndex&	index,
	std::string*	elementData
	)
	const
{
	assert( IndexValid(index) );
	assert( !itsFileIndex->IsEmbeddedFile(index) );

	// move the read mark to the start of the element

	GoToElement(index);

	// read in the length of the element

	const JSize length = ReadElementSize();

	// allocate temporary space to hold the element's data

	JUtf8Byte* data = jnew JUtf8Byte [ length ];
	assert( data != nullptr );

	// stuff the element's data into elementData

	itsStream->read(data, length);
	elementData->assign(data);

	// clean up

	jdelete [] data;
}

/******************************************************************************
 GetElement

 ******************************************************************************/

void
JFileArray::GetElement
	(
	const JFAID&	id,
	std::string*	elementData
	)
	const
{
	JFAIndex index;
	const bool ok = IDToIndex(id, &index);
	assert( ok );
	GetElement(index, elementData);
}

/******************************************************************************
 SetElement

	Write out the contents of the ostringstream to the specified element.

 ******************************************************************************/

void
JFileArray::SetElement
	(
	const JFAIndex&		index,
	std::ostringstream&	dataStream
	)
{
	const std::string data = dataStream.str();
	SetElement(index, JString(data.c_str(), data.length(), JString::kNoCopy));
}

void
JFileArray::SetElement
	(
	const JFAIndex&	index,
	const JString&	data
	)
{
	assert( IndexValid(index) );
	assert( !itsFileIndex->IsEmbeddedFile(index) );

	const JSize length = data.GetByteCount() + 1;		// include termination
	SetElementSize(index, length);

	GoToElement(index);
	WriteElementSize(length);
	itsStream->write(data.GetBytes(), length);

	// make sure all the data in the file is correct

	FlushChanges();

	// notify the world that the element data changed

	Broadcast(ElementChanged(index));
}

/******************************************************************************
 SetElement

 ******************************************************************************/

void
JFileArray::SetElement
	(
	const JFAID&		id,
	std::ostringstream&	dataStream
	)
{
	JFAIndex index;
	const bool ok = IDToIndex(id, &index);
	assert( ok );
	SetElement(index, dataStream);
}

void
JFileArray::SetElement
	(
	const JFAID&	id,
	const JString&	data
	)
{
	JFAIndex index;
	const bool ok = IDToIndex(id, &index);
	assert( ok );
	SetElement(index, data);
}

/******************************************************************************
 InsertElementAtIndex

	Insert an element into the array at the specified index.

	If index is any value greater than the current number of elements,
		then the element is appended to the end of the array.

	The actual data is appended to the data section of the file.
	The index entry for the element is inserted into the file's index
		at the specified index.

 ******************************************************************************/

void
JFileArray::InsertElementAtIndex
	(
	const JFAIndex&		index,
	std::ostringstream&	dataStream
	)
{
	const std::string data = dataStream.str();
	InsertElementAtIndex(index, JString(data.c_str(), data.length(), JString::kNoCopy));
}

void
JFileArray::InsertElementAtIndex
	(
	const JFAIndex&	index,
	const JString&	newElementData
	)
{
	assert( index.IsValid() );

	// if the specified index is too large, set it to after the last element

	const JSize elementCount = GetElementCount();

	JFAIndex trueIndex = index;
	if (trueIndex.GetIndex() > elementCount)
		{
		trueIndex.SetIndex(elementCount + 1);
		}

	// get information about the new element

	const JUnsignedOffset newElementOffset = itsIndexOffset;		// end of data section
	const JFAID           newElementID     = itsFileIndex->GetUniqueID();

	// update the index first so its new length will be included when we
	// expand the allocation for the file

	itsFileIndex->InsertElementAtIndex(trueIndex, newElementOffset, newElementID);

	// now expand the allocation for the file

	const JSize newElementSize = newElementData.GetByteCount() + 1;	// include termination

	itsIndexOffset += kElementSizeLength + newElementSize;
	SetFileLength(itsIndexOffset + itsFileIndex->GetIndexLength());

	// write new element's length + data

	SetReadWriteMark(newElementOffset, kFromFileStart);
	WriteElementSize(newElementSize);
	itsStream->write(newElementData.GetBytes(), newElementSize);

	// notify JCollection base class

	ElementAdded();

	// make sure all the data in the file is correct

	FlushChanges();

	// notify the rest of the world

	Broadcast(ElementInserted(trueIndex));
}

/******************************************************************************
 RemoveElement

	Remove the specified element from the file.

	If it is an embedded file, it must already be closed.

 ******************************************************************************/

void
JFileArray::RemoveElement
	(
	const JFAIndex& index
	)
{
	assert( IndexValid(index) );

	// make sure that the embedded file is closed

	assert( itsFileIndex->EmbeddedFileIsClosed(index) );

	// get information about element

	const JUnsignedOffset elementOffset = itsFileIndex->GetElementOffset(index);
	const JSize           elementSize   = kElementSizeLength + GetElementSize(index);

	// Shift the rest of the data down to remove the element's data.
	// This updates itsIndexOffset, which means that there is extra
	// empty space at the end of the file.

	CompactData(elementOffset, elementSize);

	// remove the element from the index

	itsFileIndex->RemoveElement(index, elementSize);

	// now shrink the file to remove the empty space at the end

	SetFileLength(itsIndexOffset + itsFileIndex->GetIndexLength());

	// notify JCollection base class

	JCollection::ElementRemoved();

	// make sure all the data in the file is correct

	FlushChanges();

	// notify the rest of the world

	Broadcast(ElementRemoved(index));
}

/******************************************************************************
 RemoveElement

 ******************************************************************************/

void
JFileArray::RemoveElement
	(
	const JFAID& id
	)
{
	JFAIndex index;
	const bool ok = IDToIndex(id, &index);
	assert( ok );
	RemoveElement(index);
}

/******************************************************************************
 MoveElementToIndex

	Move the specified element to a different index in the file.

 ******************************************************************************/

void
JFileArray::MoveElementToIndex
	(
	const JFAIndex&	currentIndex,
	const JFAIndex&	newIndex
	)
{
	assert( IndexValid(currentIndex) );
	assert( IndexValid(newIndex) );

	if (currentIndex.GetIndex() == newIndex.GetIndex())
		{
		return;
		}

	// we only have to rearrange the information in the index

	itsFileIndex->MoveElementToIndex(currentIndex, newIndex);

	// make sure all the data in the file is correct

	FlushChanges();

	// notify the rest of the world

	Broadcast(ElementMoved(currentIndex, newIndex));
}

/******************************************************************************
 SwapElements

	Interchange the specified elements in the file.

 ******************************************************************************/

void
JFileArray::SwapElements
	(
	const JFAIndex&	index1,
	const JFAIndex&	index2
	)
{
	assert( IndexValid(index1) );
	assert( IndexValid(index2) );

	if (index1.GetIndex() == index2.GetIndex())
		{
		return;
		}

	// we only have to rearrange the information in the index

	itsFileIndex->SwapElements(index1, index2);

	// make sure all the data in the file is correct

	FlushChanges();

	// notify the rest of the world

	Broadcast(ElementsSwapped(index1, index2));
}

/******************************************************************************
 IndexToID

	Return the ID of the specified element.

	This is not inline because JFileArrayIndex is forward declared.

 ******************************************************************************/

bool
JFileArray::IndexToID
	(
	const JFAIndex&	index,
	JFAID*			id
	)
	const
{
	if (IndexValid(index))
		{
		*id = itsFileIndex->GetElementID(index);
		return true;
		}
	else
		{
		id->SetID(JFAID::kInvalidID);
		return false;
		}
}

/******************************************************************************
 IDToIndex

	Return the index of the element with the specified ID.
	Returns an invalid JFAIndex if there is no element with the specified ID.

	This is not inline because JFileArrayIndex is forward declared.

 ******************************************************************************/

bool
JFileArray::IDToIndex
	(
	const JFAID&	id,
	JFAIndex*		index
	)
	const
{
	return itsFileIndex->GetElementIndexFromID(id, index);
}

/******************************************************************************
 GetElementSize (private)

 ******************************************************************************/

JSize
JFileArray::GetElementSize
	(
	const JFAIndex& index
	)
	const
{
	GoToElement(index);
	return ReadElementSize();
}

/******************************************************************************
 SetElementSize (private)

	Set the size of the specified element.
	Element offsets are updated appropriately.

 ******************************************************************************/

void
JFileArray::SetElementSize
	(
	const JFAIndex&	index,
	const JSize		newSize
	)
{
	// calculate the change in the element's size

	const JSize oldSize     = GetElementSize(index);
	const long changeInSize = newSize - oldSize;

	const JUnsignedOffset elementOffset = itsFileIndex->GetElementOffset(index);

	// expand or shrink the file as necessary

	if (changeInSize > 0)
		{
		SetFileLength(GetFileLength() + changeInSize);
		ExpandData(elementOffset + kElementSizeLength + oldSize, changeInSize);
		}
	else if (changeInSize < 0)
		{
		CompactData(elementOffset + kElementSizeLength + newSize, -changeInSize);
		SetFileLength(GetFileLength() + changeInSize);
		}
	else
		{
		return;
		}

	// write new element size

	SetReadWriteMark(elementOffset, kFromFileStart);
	WriteElementSize(newSize);

	// adjust offsets of other elements in index

	itsFileIndex->ElementSizeChanged(index, changeInSize);
}

/******************************************************************************
 ExpandData (private)

	Allocate extra space at a particular point in the file.

	offset specifies start of spaceNeeded

	*** Caller must already have expanded the file's total allocation.
		If we did it here, the caller might be forced to expand the file's
		total allocation more than once, which is very inefficient.

	*** Since the index is actually stored in memory while the file is open,
		we don't waste time moving the undefined bytes within its allocation.

 ******************************************************************************/

void
JFileArray::ExpandData
	(
	const JUnsignedOffset	offset,
	const JSize				spaceNeeded
	)
{
	if (spaceNeeded == 0)
		{
		return;
		}

	const JSize totalLength = itsIndexOffset;
	assert( offset <= totalLength );

	// adjust itsIndexOffset to reflect the larger amount of data

	itsIndexOffset += spaceNeeded;

	// if the space is needed at the end of the data allocation, we are already done

	if (offset == totalLength)
		{
		return;
		}

	// allocate temporary memory for transfer of data

	JSize       dataSize = JMin(maxTempMem, totalLength - offset);
	JUtf8Byte* data      = jnew JUtf8Byte [ dataSize ];
	assert( data != nullptr );

	// start at end of data and work towards front of file

	JUnsignedOffset mark = totalLength - dataSize;

	// transfer data in blocks

	while (1)
		{
		SetReadWriteMark(mark, kFromFileStart);
		itsStream->read(data, dataSize);

		SetReadWriteMark(mark + spaceNeeded, kFromFileStart);
		itsStream->write(data, dataSize);

		if (mark <= offset)				// we're done
			{
			break;
			}

		if (mark >= dataSize)
			{
			mark -= dataSize;
			if (mark < offset)				// catch leftovers
				{
				dataSize -= (offset - mark);
				mark = offset;
				}
			}
		else	// mark < dataSize
			{
			dataSize = mark - offset;
			mark     = offset;
			}
		}

	// throw out temporary allocation

	jdelete [] data;
}

/******************************************************************************
 CompactData (private)

	Remove extra space at a particular point in the data portion of the file.
	(The change in the index portion of the file should be included in
	 the call to SetFileLength.)

	offset specifies start of blankSize.

	*** Caller must shrink the file's total allocation after calling us.
		If we did it here, the caller might be forced to shrink the file's
		total allocation more than once, which is very inefficient.

	*** Since the index is actually stored in memory while the file is open,
		we don't waste time moving the undefined bytes within its allocation.

 ******************************************************************************/

void
JFileArray::CompactData
	(
	const JUnsignedOffset	offset,
	const JSize				blankSize
	)
{
	if (blankSize == 0)
		{
		return;
		}

	const JSize totalLength = itsIndexOffset;
	assert( offset < totalLength );
	assert( offset + blankSize <= totalLength );

	// adjust itsIndexOffset to reflect the smaller amount of data

	itsIndexOffset -= blankSize;

	// if the unneeded space is at the end of the data allocation, we are already done

	if (offset + blankSize == totalLength)
		{
		return;
		}

	// allocate temporary memory for transfer of data

	JSize       dataSize = JMin(maxTempMem, totalLength - offset - blankSize);
	JUtf8Byte* data      = jnew JUtf8Byte [ dataSize ];
	assert( data != nullptr );

	// start in front of unneeded space and work towards end of file

	JUnsignedOffset mark = offset + blankSize;

	// transfer data in blocks

	while (1)
		{
		SetReadWriteMark(mark, kFromFileStart);
		itsStream->read(data, dataSize);

		SetReadWriteMark(mark - blankSize, kFromFileStart);
		itsStream->write(data, dataSize);

		mark += dataSize;
		if (mark >= totalLength)
			{
			break;
			}

		if (mark + dataSize > totalLength)		// catch leftovers
			{
			dataSize = totalLength - mark;
			}
		}

	// throw out temporary allocation

	jdelete [] data;
}

/******************************************************************************
 GoToElement (private)

	Set the file mark to the start of the specified element.
 ******************************************************************************/

void
JFileArray::GoToElement
	(
	const JFAIndex& index
	)
	const
{
	const JUnsignedOffset elementOffset = itsFileIndex->GetElementOffset(index);
	SetReadWriteMark(elementOffset, kFromFileStart);
}

/******************************************************************************
 FlushChanges

	Write out the header information and the file's index.

	This is called automatically if ShouldFlushChanges(true) was called.
	You can optimize by calling ShouldFlushChanges(false) and then calling
	FlushChanges() after making a sequence of changes.

 ******************************************************************************/

void
JFileArray::FlushChanges()
{
	if (itsFlushChangesFlag)
		{
		WriteVersion();
		WriteElementCount();
		WriteIndexOffset();
		WriteIndex();
		itsStream->flush();
		}
}

/******************************************************************************
 ReadVersion (private)

 ******************************************************************************/

void
JFileArray::ReadVersion()
{
	SetReadWriteMark(itsFileSignatureByteCount + kVersionOffset, kFromFileStart);
	itsVersion = ReadUnsignedLong(*itsStream);
}

/******************************************************************************
 WriteVersion (private)

 ******************************************************************************/

void
JFileArray::WriteVersion()
{
	SetReadWriteMark(itsFileSignatureByteCount + kVersionOffset, kFromFileStart);
	WriteUnsignedLong(*itsStream, itsVersion);
}

/******************************************************************************
 ReadElementCount (private)

 ******************************************************************************/

void
JFileArray::ReadElementCount()
{
	SetReadWriteMark(itsFileSignatureByteCount + kElementCountOffset, kFromFileStart);
	const JSize count = ReadUnsignedLong(*itsStream) & (~kFileLockedMask);
	SetElementCount(count);
}

/******************************************************************************
 WriteElementCount (private)

 ******************************************************************************/

void
JFileArray::WriteElementCount()
{
	SetReadWriteMark(itsFileSignatureByteCount + kElementCountOffset, kFromFileStart);

	JSize count = GetElementCount();
	if (itsIsOpenFlag)
		{
		count |= kFileLockedMask;
		}

	WriteUnsignedLong(*itsStream, count);
}

/******************************************************************************
 FileIsOpen (static private)

 ******************************************************************************/

bool
JFileArray::FileIsOpen
	(
	std::ifstream&	file,
	const JSize	sigLength
	)
{
	JSeekg(file, sigLength + kElementCountOffset);
	const JSize count = ReadUnsignedLong(file);
	return (count & kFileLockedMask) != 0L;
}

/******************************************************************************
 ReadIndexOffset (private)

 ******************************************************************************/

void
JFileArray::ReadIndexOffset()
{
	SetReadWriteMark(itsFileSignatureByteCount + kIndexOffsetOffset, kFromFileStart);
	itsIndexOffset = ReadUnsignedLong(*itsStream);
}

/******************************************************************************
 WriteIndexOffset (private)

 ******************************************************************************/

void
JFileArray::WriteIndexOffset()
{
	SetReadWriteMark(itsFileSignatureByteCount + kIndexOffsetOffset, kFromFileStart);
	WriteUnsignedLong(*itsStream, itsIndexOffset);
}

/******************************************************************************
 ReadElementSize (private)

	*** Caller must set read mark appropriately.

 ******************************************************************************/

JSize
JFileArray::ReadElementSize()
	const
{
	return ReadUnsignedLong(*itsStream);
}

/******************************************************************************
 WriteElementSize (private)

	*** Caller must set write mark appropriately.

 ******************************************************************************/

void
JFileArray::WriteElementSize
	(
	const JSize elementSize
	)
{
	WriteUnsignedLong(*itsStream, elementSize);
}

/******************************************************************************
 ReadUnsignedLong (static private)

	Reads a 4-byte unsigned integer stored in Little Endian format.

	We use Little Endian format because that is what Linux uses,
	and Linux is the only system that we have to provide backward
	compatibility on as of 10/27/96.

 ******************************************************************************/

unsigned long
JFileArray::ReadUnsignedLong
	(
	std::istream& input
	)
{
	assert( sizeof(unsigned long) >= kUnsignedLongLength );

	unsigned long result = 0;
	for (JUnsignedOffset i=0; i<kUnsignedLongLength; i++)
		{
		char c;
		input.read(&c,1);
		unsigned long x = (unsigned char) c;
		result += (x << (8*i));
		}

	return result;
}

/******************************************************************************
 WriteUnsignedLong (static private)

	Writes a 4-byte unsigned integer in Little Endian format.

 ******************************************************************************/

void
JFileArray::WriteUnsignedLong
	(
	std::ostream&			output,
	const unsigned long	value
	)
{
	assert( sizeof(unsigned long) >= kUnsignedLongLength );

	for (JUnsignedOffset i=0; i<kUnsignedLongLength; i++)
		{
		const char c = (char) ((value >> (8*i)) & 0x000000FF);
		output.write(&c,1);
		}
}

/******************************************************************************
 ReadIndex (private)

	Tell itsFileIndex to read in the index information.
	Used once when the file is opened.

	We require the number of elements to be passed in to force the caller
	to read in itsElementCount first.

 ******************************************************************************/

void
JFileArray::ReadIndex
	(
	const JSize elementCount
	)
	const
{
	SetReadWriteMark(itsIndexOffset, kFromFileStart);
	itsFileIndex->ReadIndex(elementCount, *itsStream);

	// Since the index is stored at the end of the file, the std::ios::eof
	// bit will be set when we are done reading the index.
	// We must therefore clear this in order to continue normal operation.
	// First we must check that no other error bits are set, however.

	assert( ! itsStream->fail() );
	itsStream->clear();
}

/******************************************************************************
 WriteIndex (private)

	Tell itsFileIndex to write out the index information.

	Since the total file allocation always includes space for the index,
	we can simply start writing at itsIndexOffset.

	Used once when the file is closed.

 ******************************************************************************/

void
JFileArray::WriteIndex()
{
	SetReadWriteMark(itsIndexOffset, kFromFileStart);
	itsFileIndex->WriteIndex(*itsStream);

	// unlike reading, writing does not cause the std::ios::eof bit to be set
}

/******************************************************************************
 GetStartOfFile (private)

 ******************************************************************************/

JUnsignedOffset
JFileArray::GetStartOfFile()
	const
{
	if (itsEnclosingFile != nullptr)
		{
		JFAIndex enclosureElementIndex;
		const bool ok =
			itsEnclosingFile->IDToIndex(itsEnclosureElementID, &enclosureElementIndex);
		assert( ok );

		const JUnsignedOffset enclosureOffset =
			(itsEnclosingFile->itsFileIndex)->GetElementOffset(enclosureElementIndex);
		return enclosureOffset + kElementSizeLength;
		}
	else
		{
		return 0;
		}
}

/******************************************************************************
 GetFileLength (private)

	Return the total amount of space allocated to the file.

 ******************************************************************************/

JSize
JFileArray::GetFileLength()
	const
{
	if (itsEnclosingFile != nullptr)
		{
		JFAIndex enclosureElementIndex;
		const bool ok =
			itsEnclosingFile->IDToIndex(itsEnclosureElementID, &enclosureElementIndex);
		assert( ok );

		return itsEnclosingFile->GetElementSize(enclosureElementIndex);
		}
	else
		{
		return JGetFStreamLength(*itsStream);
		}
}

/******************************************************************************
 SetFileLength (private)

	Set the total amount of space allocated to the file.
 ******************************************************************************/

void
JFileArray::SetFileLength
	(
	const JSize newLength
	)
{
	if (itsEnclosingFile != nullptr)
		{
		JFAIndex enclosureElementIndex;
		const bool ok =
			itsEnclosingFile->IDToIndex(itsEnclosureElementID, &enclosureElementIndex);
		assert( ok );

		itsEnclosingFile->SetElementSize(enclosureElementIndex, newLength);
		}
	else
		{
		// closes old std::fstream, changes file length, returns new std::fstream

		std::fstream* newStream =
			JSetFStreamLength(*itsFileName, *itsStream,
							  newLength, kFileOpenMode);

		// deletes the old std::fstream and notifies embedded files of new one

		ReplaceStream(newStream);
		}
}

/******************************************************************************
 ReplaceStream (private)

	Delete our current std::fstream, replace it with the given new one, and
	notify all the embedded files (recursive).

	This is used by SetFileLength because the old stream has to be
	thrown out.  If std::fstream's weren't so stupid, we wouldn't need this.

 ******************************************************************************/

void
JFileArray::ReplaceStream
	(
	std::fstream* newStream
	)
{
	// replace our stream with the new stream

	if (itsEnclosingFile == nullptr)
		{
		jdelete itsStream;
		}
	itsStream = newStream;

	// notify all embedded files -- this makes us recursive

	itsFileIndex->ReplaceEmbeddedFileStreams(newStream);
}

/******************************************************************************
 GetReadWriteMark (private)

	Get the position of the read and write marks.

 ******************************************************************************/

JUnsignedOffset
JFileArray::GetReadWriteMark()
	const
{
	return (std::streamoff(itsStream->tellg()) - GetStartOfFile());
}

/******************************************************************************
 SetReadWriteMark (private)

	Set the position of the read and write marks for the stream.

	fstreams don't support independent read and write marks,
	so we have to put up with the lowest common denominator.

 ******************************************************************************/

void
JFileArray::SetReadWriteMark
	(
	const JSignedOffset	howFar,
	const SetMarkMode	fromWhere
	)
	const
{
	const JUnsignedOffset startOfFile = GetStartOfFile();
	const JSize           fileLength  = GetFileLength();

	if (fromWhere == kFromFileStart)
		{
		assert( howFar >= 0 && ((JSize) howFar) <= fileLength );

		itsStream->seekg(startOfFile + howFar);
		itsStream->seekp(startOfFile + howFar);
		}
	else if (fromWhere == kFromFileEnd)
		{
		assert( -howFar >= 0 && ((JSize) -howFar) <= fileLength );

		itsStream->seekg((startOfFile + fileLength) + howFar);
		itsStream->seekp((startOfFile + fileLength) + howFar);
		}
}

/******************************************************************************
 AdjustIndex

 ******************************************************************************/

void
JFileArray::ElementInserted::AdjustIndex
	(
	JFAIndex* index
	)
	const
{
	JIndex i = index->GetIndex();
	JAdjustIndexAfterInsert(GetIndex().GetIndex(), 1, &i);
	index->SetIndex(i);
}

bool
JFileArray::ElementRemoved::AdjustIndex
	(
	JFAIndex* index
	)
	const
{
	JIndex i = index->GetIndex();
	const bool ok = JAdjustIndexAfterRemove(GetIndex().GetIndex(), 1, &i);
	index->SetIndex(i);
	return ok;
}

void
JFileArray::ElementMoved::AdjustIndex
	(
	JFAIndex* index
	)
	const
{
	JIndex i = index->GetIndex();
	JAdjustIndexAfterMove(itsOrigIndex.GetIndex(), itsNewIndex.GetIndex(), &i);
	index->SetIndex(i);
}

void
JFileArray::ElementsSwapped::AdjustIndex
	(
	JFAIndex* index
	)
	const
{
	JIndex i = index->GetIndex();
	JAdjustIndexAfterSwap(itsIndex1.GetIndex(), itsIndex2.GetIndex(), &i);
	index->SetIndex(i);
}

/******************************************************************************
 JError constructors

 ******************************************************************************/

JFileArray::FileNotWritable::FileNotWritable
	(
	const JString& fileName
	)
	:
	JError(JFileArray::kFileNotWritable)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JFileArray::FileAlreadyOpen::FileAlreadyOpen
	(
	const JString& fileName
	)
	:
	JError(JFileArray::kFileAlreadyOpen)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JFileArray::WrongSignature::WrongSignature
	(
	const JString& fileName
	)
	:
	JError(JFileArray::kWrongSignature)
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JFileArray::NotEmbeddedFile::NotEmbeddedFile
	(
	const JIndex index
	)
	:
	JError(JFileArray::kNotEmbeddedFile)
{
	const JString s((JUInt64) index);
	static const JUtf8Byte* map[] =
	{
		"index", s.GetBytes()
	};
	SetMessage(map, sizeof(map));
}
