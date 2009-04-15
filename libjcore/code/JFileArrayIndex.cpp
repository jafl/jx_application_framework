/******************************************************************************
 JFileArrayIndex.cpp

				The Index-of-Elements for a JFileArray Class

	This class stores and index of the elements in a JFileArray.

	BASE CLASS = public JCollection

	Copyright © 1993-98 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JFileArrayIndex.h>
#include <JFileArray.h>
#include <jAssert.h>

// size of data stored in index section of file

enum
	{
	kElementOffsetLength = JFileArray::kUnsignedLongLength,
	kElementIDLength     = JFileArray::kUnsignedLongLength,
	kElementTypeLength   = JFileArray::kUnsignedLongLength,

	kIndexElementLength  = kElementOffsetLength + kElementIDLength +
						   kElementTypeLength
	};

/******************************************************************************
 Constructor

 ******************************************************************************/

JFileArrayIndex::JFileArrayIndex()
	:
	JContainer()
{
	assert( sizeof(JUnsignedOffset) >= JFileArray::kUnsignedLongLength &&
			sizeof(JFAID_t)         >= JFileArray::kUnsignedLongLength );

	itsArray = new JArray<ElementInfo>;
	assert( itsArray != NULL );

	InstallOrderedSet(itsArray);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFileArrayIndex::~JFileArrayIndex()
{
	delete itsArray;
}

/******************************************************************************
 InsertElementAtIndex

	Insert a new element into the index.  Since the storage is appended to the
	file, we don't have to adjust any element offsets.

	If the element will be an embedded file, the caller must set it separately.

 ******************************************************************************/

void
JFileArrayIndex::InsertElementAtIndex
	(
	const JFAIndex&			index,
	const JUnsignedOffset	offset,
	const JFAID&			id
	)
{
	const ElementInfo elementInfo(offset, id, kData);
	itsArray->InsertElementAtIndex(index.GetIndex(), elementInfo);
}

/******************************************************************************
 RemoveElement

	Remove the specified element and adjust the offsets of the other elements.

 ******************************************************************************/

void
JFileArrayIndex::RemoveElement
	(
	const JFAIndex&	index,
	const JSize		elementSize
	)
{
	ElementSizeChanged(index, -(long) elementSize);
	itsArray->RemoveElement(index.GetIndex());
}

/******************************************************************************
 GetElementOffset

 ******************************************************************************/

JUnsignedOffset
JFileArrayIndex::GetElementOffset
	(
	const JFAIndex& index
	)
	const
{
	const ElementInfo elementInfo = itsArray->GetElement(index.GetIndex());
	return elementInfo.offset;
}

/******************************************************************************
 ElementSizeChanged

	Adjust the offsets of the other elements.
	This is the only way element offsets change.

 ******************************************************************************/

void
JFileArrayIndex::ElementSizeChanged
	(
	const JFAIndex&	index,
	const JInteger	changeInElementSize
	)
{
	if (changeInElementSize == 0)
		{
		return;
		}

	const JUnsignedOffset elementOffset = GetElementOffset(index);
	const JSize elementCount            = GetElementCount();

	for (JIndex i=1; i<=elementCount; i++)
		{
		ElementInfo elementInfo = itsArray->GetElement(i);
		if (elementInfo.offset > elementOffset)
			{
			// make sure that elementOffset won't try to go negative

			assert( changeInElementSize > 0 ||
					((JSize) -changeInElementSize) <= elementInfo.offset );

			elementInfo.offset += changeInElementSize;
			itsArray->SetElement(i, elementInfo);
			}
		}
}

/******************************************************************************
 GetElementID

 ******************************************************************************/

JFAID
JFileArrayIndex::GetElementID
	(
	const JFAIndex& index
	)
	const
{
	const ElementInfo elementInfo = itsArray->GetElement(index.GetIndex());
	return elementInfo.id;
}

/******************************************************************************
 SetElementID

	Set the id of the specified element.  We scream if the id has already been used.

 ******************************************************************************/

void
JFileArrayIndex::SetElementID
	(
	const JFAIndex&	index,
	const JFAID&	newID
	)
{
	ElementInfo elementInfo = itsArray->GetElement(index.GetIndex());

	if ((elementInfo.id).GetID() != newID.GetID())
		{
		// we scream if newID has already been used

		JFAIndex origIndex;
		const JBoolean ok = GetElementIndexFromID(newID, &origIndex);
		assert( !ok );

		// store the new ID

		elementInfo.id = newID;
		itsArray->SetElement(index.GetIndex(), elementInfo);
		}
}

/******************************************************************************
 GetElementIndexFromID

	Return the index of the element with the specified ID.
	Returns kJFalse if there is no element with the specified ID.

 ******************************************************************************/

JBoolean
JFileArrayIndex::GetElementIndexFromID
	(
	const JFAID&	id,
	JFAIndex*		index
	)
	const
{
	const JSize elementCount = GetElementCount();
	for (JIndex i=1; i<=elementCount; i++)
		{
		const ElementInfo elementInfo = itsArray->GetElement(i);
		if ((elementInfo.id).GetID() == id.GetID())
			{
			index->SetIndex(i);
			return kJTrue;
			}
		}

	index->SetIndex(JFAIndex::kInvalidIndex);
	return kJFalse;
}

/******************************************************************************
 GetUniqueID

	Return an unused ID.  Return zero if all ID's have been used.

 ******************************************************************************/

JFAID
JFileArrayIndex::GetUniqueID()
	const
{
	if (IsEmpty())
		{
		return JFAID(JFAID::kMinID);
		}

	const JSize elementCount = GetElementCount();

	// this is relevant to the outmost do-while loop

	enum Range
	{
		kAboveElementCount,
		kBelowElementCount,
		kEmpty
	};
	Range idRange = kAboveElementCount;

	do
		{
		// try the larger Id's first

		JFAID_t firstId, lastId;

		if (idRange == kAboveElementCount && elementCount < JFAID::kMaxID)
			{
			firstId = elementCount + 1;
			lastId  = JFAID::kMaxID;
			}
		else if (idRange == kAboveElementCount)
			{
			idRange = kBelowElementCount;
			firstId = JFAID::kMinID;
			lastId  = JFAID::kMaxID;
			}
		else
			{
			assert( idRange == kBelowElementCount );
			firstId = JFAID::kMinID;
			lastId  = elementCount;
			}

		// try all possible id's in the given range

		JFAID id;
		JFAIndex index;
		for (JFAID_t anID=firstId; anID<=lastId; anID++)
			{
			id.SetID(anID);
			if (!GetElementIndexFromID(id, &index))
				{
				return id;
				}
			}

		if (idRange == kAboveElementCount)
			{
			idRange = kBelowElementCount;
			}
		else if (idRange == kBelowElementCount)
			{
			idRange = kEmpty;
			}
		}
		while (idRange != kEmpty);

	// We've tried everything.  It's time to give up.

	return JFAID(JFAID::kInvalidID);
}

/******************************************************************************
 SetToEmbeddedFile

	Specify that the specified element contains an embedded file.
	This is irreversible because the data (an entire JFileArray) is not
	usable by anyone else.  They should remove the element instead.

 ******************************************************************************/

void
JFileArrayIndex::SetToEmbeddedFile
	(
	const JFAIndex& index
	)
{
	ElementInfo elementInfo = itsArray->GetElement(index.GetIndex());

	if (elementInfo.type != kEmbeddedFile)
		{
		elementInfo.type = kEmbeddedFile;
		itsArray->SetElement(index.GetIndex(), elementInfo);
		}
}

/******************************************************************************
 IsEmbeddedFile

	Returns TRUE if the specified element contains an embedded file.

 ******************************************************************************/

JBoolean
JFileArrayIndex::IsEmbeddedFile
	(
	const JFAIndex& index
	)
	const
{
	const ElementInfo elementInfo = itsArray->GetElement(index.GetIndex());
	return JConvertToBoolean( elementInfo.type == kEmbeddedFile );
}

/******************************************************************************
 EmbeddedFileOpened

	Notify us that an embedded file has been opened.
	We scream if the element isn't an embedded file or if the element is already open.

 ******************************************************************************/

void
JFileArrayIndex::EmbeddedFileOpened
	(
	const JFAIndex&	index,
	JFileArray*		theEmbeddedFile
	)
{
	ElementInfo elementInfo = itsArray->GetElement(index.GetIndex());

	// we scream if the element isn't an embedded file or if the element is already open

	assert( elementInfo.type == kEmbeddedFile );
	assert( elementInfo.theEmbeddedFile == NULL );

	// save the reference to the opened file

	elementInfo.theEmbeddedFile = theEmbeddedFile;
	itsArray->SetElement(index.GetIndex(), elementInfo);
}

/******************************************************************************
 EmbeddedFileClosed

	Tell us that the specified element (an embedded file) has been closed.
	We scream if the element isn't an embedded file or if the element isn't open.

 ******************************************************************************/

void
JFileArrayIndex::EmbeddedFileClosed
	(
	const JFAIndex& index
	)
{
	ElementInfo elementInfo = itsArray->GetElement(index.GetIndex());

	// we scream if the element isn't an embedded file or if the element isn't open

	assert( elementInfo.type == kEmbeddedFile );
	assert( elementInfo.theEmbeddedFile != NULL );

	// clear the reference to the file

	elementInfo.theEmbeddedFile = NULL;
	itsArray->SetElement(index.GetIndex(), elementInfo);
}

/******************************************************************************
 EmbeddedFileIsClosed

	Returns TRUE if the specified element is not an embedded file or
	if it is a closed embedded file.

 ******************************************************************************/

JBoolean
JFileArrayIndex::EmbeddedFileIsClosed
	(
	const JFAIndex& index
	)
	const
{
	const ElementInfo elementInfo = itsArray->GetElement(index.GetIndex());
	return JConvertToBoolean( elementInfo.theEmbeddedFile == NULL );
}

/******************************************************************************
 AllEmbeddedFilesAreClosed

	Returns true if all embedded files are closed.

 ******************************************************************************/

JBoolean
JFileArrayIndex::AllEmbeddedFilesAreClosed()
	const
{
	const JSize elementCount = GetElementCount();
	for (JIndex i=1; i<=elementCount; i++)
		{
		const ElementInfo elementInfo = itsArray->GetElement(i);
		if (elementInfo.theEmbeddedFile != NULL)
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 ReplaceEmbeddedFileStreams

	Notify all embedded files that their enclosing file has a new fstream.

 ******************************************************************************/

void
JFileArrayIndex::ReplaceEmbeddedFileStreams
	(
	fstream* newStream
	)
{
	const JSize elementCount = GetElementCount();
	for (JIndex i=1; i<=elementCount; i++)
		{
		const ElementInfo elementInfo = itsArray->GetElement(i);
		if (elementInfo.theEmbeddedFile != NULL)
			{
			(elementInfo.theEmbeddedFile)->ReplaceStream(newStream);
			}
		}
}

/******************************************************************************
 ReadIndex (private)

	Read the file's index.

	*** Caller must position read mark before calling.

 ******************************************************************************/

void
JFileArrayIndex::ReadIndex
	(
	const JSize	elementCount,
	istream&	input
	)
{
	// we should only be called when the file is opened, but it doesn't hurt to be sure

	itsArray->RemoveAll();

	// read in the information on each element

	ElementInfo elementInfo;
	for (JIndex i=1; i<=elementCount; i++)
		{
		elementInfo.offset = JFileArray::ReadUnsignedLong(input);

		JFAID_t id;
		id = JFileArray::ReadUnsignedLong(input);
		(elementInfo.id).SetID(id);

		unsigned long type;
		type = JFileArray::ReadUnsignedLong(input);
		if (type == kData)
			{
			elementInfo.type = kData;
			}
		else if (type == kEmbeddedFile)
			{
			elementInfo.type = kEmbeddedFile;
			}
		else
			{
			assert( 0 );
			}

		itsArray->InsertElementAtIndex(i, elementInfo);
		}
}

/******************************************************************************
 WriteIndex (private)

	Write the file's index.

	*** Caller must position write mark before calling.

 ******************************************************************************/

void
JFileArrayIndex::WriteIndex
	(
	ostream& output
	)
{
	const JSize elementCount = GetElementCount();
	for (JIndex i=1; i<=elementCount; i++)
		{
		ElementInfo elementInfo = itsArray->GetElement(i);

		JFileArray::WriteUnsignedLong(output, elementInfo.offset);

		const JFAID_t id = (elementInfo.id).GetID();
		JFileArray::WriteUnsignedLong(output, id);

		const unsigned long type = elementInfo.type;
		JFileArray::WriteUnsignedLong(output, type);
		}
}

/******************************************************************************
 GetIndexLength

	Return the amount of space required to store the index.
	Not inline because constants shouldn't be part of class declaration.

 ******************************************************************************/

JSize
JFileArrayIndex::GetIndexLength()
	const
{
	return GetElementCount() * kIndexElementLength;
}

#define JTemplateType JFileArrayIndex::ElementInfo
#include <JArray.tmpls>
#undef JTemplateType
