/******************************************************************************
 JFileArrayIndex.cpp

				The Index-of-Items for a JFileArray Class

	This class stores and index of the items in a JFileArray.

	BASE CLASS = public JCollection

	Copyright (C) 1993-98 John Lindal.

 ******************************************************************************/

#include "JFileArrayIndex.h"
#include "JFileArray.h"
#include <algorithm>
#include "jAssert.h"

// size of data stored in index section of file

enum
{
	kItemOffsetLength = JFileArray::kUnsignedLongLength,
	kItemIDLength     = JFileArray::kUnsignedLongLength,
	kItemTypeLength   = JFileArray::kUnsignedLongLength,

	kIndexItemLength  = kItemOffsetLength + kItemIDLength + kItemTypeLength
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

	itsArray = jnew JArray<ItemInfo>;
	assert( itsArray != nullptr );

	InstallCollection(itsArray);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFileArrayIndex::~JFileArrayIndex()
{
	jdelete itsArray;
}

/******************************************************************************
 InsertItemAtIndex

	Insert a new item into the index.  Since the storage is appended to the
	file, we don't have to adjust any item offsets.

	If the item will be an embedded file, the caller must set it separately.

 ******************************************************************************/

void
JFileArrayIndex::InsertItemAtIndex
	(
	const JFAIndex&			index,
	const JUnsignedOffset	offset,
	const JFAID&			id
	)
{
	const ItemInfo itemInfo(offset, id, kData);
	itsArray->InsertItemAtIndex(index.GetIndex(), itemInfo);
}

/******************************************************************************
 RemoveItem

	Remove the specified item and adjust the offsets of the other items.

 ******************************************************************************/

void
JFileArrayIndex::RemoveItem
	(
	const JFAIndex&	index,
	const JSize		itemSize
	)
{
	ItemSizeChanged(index, -(long) itemSize);
	itsArray->RemoveItem(index.GetIndex());
}

/******************************************************************************
 GetItemOffset

 ******************************************************************************/

JUnsignedOffset
JFileArrayIndex::GetItemOffset
	(
	const JFAIndex& index
	)
	const
{
	const ItemInfo itemInfo = itsArray->GetItem(index.GetIndex());
	return itemInfo.offset;
}

/******************************************************************************
 ItemSizeChanged

	Adjust the offsets of the other items.
	This is the only way item offsets change.

 ******************************************************************************/

void
JFileArrayIndex::ItemSizeChanged
	(
	const JFAIndex&	index,
	const JInteger	changeInItemSize
	)
{
	if (changeInItemSize == 0)
	{
		return;
	}

	const JUnsignedOffset itemOffset = GetItemOffset(index);
	const JSize itemCount            = GetItemCount();

	for (JIndex i=1; i<=itemCount; i++)
	{
		ItemInfo itemInfo = itsArray->GetItem(i);
		if (itemInfo.offset > itemOffset)
		{
			// make sure that itemOffset won't try to go negative

			assert( changeInItemSize > 0 ||
					((JSize) -changeInItemSize) <= itemInfo.offset );

			itemInfo.offset += changeInItemSize;
			itsArray->SetItem(i, itemInfo);
		}
	}
}

/******************************************************************************
 GetItemID

 ******************************************************************************/

JFAID
JFileArrayIndex::GetItemID
	(
	const JFAIndex& index
	)
	const
{
	const ItemInfo itemInfo = itsArray->GetItem(index.GetIndex());
	return itemInfo.id;
}

/******************************************************************************
 SetItemID

	Set the id of the specified item.  We scream if the id has already been used.

 ******************************************************************************/

void
JFileArrayIndex::SetItemID
	(
	const JFAIndex&	index,
	const JFAID&	newID
	)
{
	ItemInfo itemInfo = itsArray->GetItem(index.GetIndex());

	if (itemInfo.id.GetID() != newID.GetID())
	{
		// we scream if newID has already been used

		JFAIndex origIndex;
		const bool ok = GetItemIndexFromID(newID, &origIndex);
		assert( !ok );

		// store the new ID

		itemInfo.id = newID;
		itsArray->SetItem(index.GetIndex(), itemInfo);
	}
}

/******************************************************************************
 GetItemIndexFromID

	Return the index of the item with the specified ID.
	Returns false if there is no item with the specified ID.

 ******************************************************************************/

bool
JFileArrayIndex::GetItemIndexFromID
	(
	const JFAID&	id,
	JFAIndex*		index
	)
	const
{
	const JSize itemCount = GetItemCount();
	for (JIndex i=1; i<=itemCount; i++)
	{
		const ItemInfo itemInfo = itsArray->GetItem(i);
		if (itemInfo.id.GetID() == id.GetID())
		{
			index->SetIndex(i);
			return true;
		}
	}

	index->SetIndex(JFAIndex::kInvalidIndex);
	return false;
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

	const JSize itemCount = GetItemCount();

	// this is relevant to the outmost do-while loop

	enum Range
	{
		kAboveItemCount,
		kBelowItemCount,
		kEmpty
	};
	Range idRange = kAboveItemCount;

	do
	{
		// try the larger Id's first

		JFAID_t firstId, lastId;

		if (idRange == kAboveItemCount && itemCount < JFAID::kMaxID)
		{
			firstId = itemCount + 1;
			lastId  = JFAID::kMaxID;
		}
		else if (idRange == kAboveItemCount)
		{
			idRange = kBelowItemCount;
			firstId = JFAID::kMinID;
			lastId  = JFAID::kMaxID;
		}
		else
		{
			assert( idRange == kBelowItemCount );
			firstId = JFAID::kMinID;
			lastId  = itemCount;
		}

		// try all possible id's in the given range

		JFAID id;
		JFAIndex index;
		for (JFAID_t anID=firstId; anID<=lastId; anID++)
		{
			id.SetID(anID);
			if (!GetItemIndexFromID(id, &index))
			{
				return id;
			}
		}

		if (idRange == kAboveItemCount)
		{
			idRange = kBelowItemCount;
		}
		else if (idRange == kBelowItemCount)
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

	Specify that the specified item contains an embedded file.
	This is irreversible because the data (an entire JFileArray) is not
	usable by anyone else.  They should remove the item instead.

 ******************************************************************************/

void
JFileArrayIndex::SetToEmbeddedFile
	(
	const JFAIndex& index
	)
{
	ItemInfo itemInfo = itsArray->GetItem(index.GetIndex());

	if (itemInfo.type != kEmbeddedFile)
	{
		itemInfo.type = kEmbeddedFile;
		itsArray->SetItem(index.GetIndex(), itemInfo);
	}
}

/******************************************************************************
 IsEmbeddedFile

	Returns TRUE if the specified item contains an embedded file.

 ******************************************************************************/

bool
JFileArrayIndex::IsEmbeddedFile
	(
	const JFAIndex& index
	)
	const
{
	const ItemInfo itemInfo = itsArray->GetItem(index.GetIndex());
	return itemInfo.type == kEmbeddedFile;
}

/******************************************************************************
 EmbeddedFileOpened

	Notify us that an embedded file has been opened.
	We scream if the item isn't an embedded file or if the item is already open.

 ******************************************************************************/

void
JFileArrayIndex::EmbeddedFileOpened
	(
	const JFAIndex&	index,
	JFileArray*		theEmbeddedFile
	)
{
	ItemInfo itemInfo = itsArray->GetItem(index.GetIndex());

	// we scream if the item isn't an embedded file or if the item is already open

	assert( itemInfo.type == kEmbeddedFile );
	assert( itemInfo.theEmbeddedFile == nullptr );

	// save the reference to the opened file

	itemInfo.theEmbeddedFile = theEmbeddedFile;
	itsArray->SetItem(index.GetIndex(), itemInfo);
}

/******************************************************************************
 EmbeddedFileClosed

	Tell us that the specified item (an embedded file) has been closed.
	We scream if the item isn't an embedded file or if the item isn't open.

 ******************************************************************************/

void
JFileArrayIndex::EmbeddedFileClosed
	(
	const JFAIndex& index
	)
{
	ItemInfo itemInfo = itsArray->GetItem(index.GetIndex());

	// we scream if the item isn't an embedded file or if the item isn't open

	assert( itemInfo.type == kEmbeddedFile );
	assert( itemInfo.theEmbeddedFile != nullptr );

	// clear the reference to the file

	itemInfo.theEmbeddedFile = nullptr;
	itsArray->SetItem(index.GetIndex(), itemInfo);
}

/******************************************************************************
 EmbeddedFileIsClosed

	Returns TRUE if the specified item is not an embedded file or
	if it is a closed embedded file.

 ******************************************************************************/

bool
JFileArrayIndex::EmbeddedFileIsClosed
	(
	const JFAIndex& index
	)
	const
{
	const ItemInfo itemInfo = itsArray->GetItem(index.GetIndex());
	return itemInfo.theEmbeddedFile == nullptr;
}

/******************************************************************************
 AllEmbeddedFilesAreClosed

	Returns true if all embedded files are closed.

 ******************************************************************************/

bool
JFileArrayIndex::AllEmbeddedFilesAreClosed()
	const
{
	return std::all_of(begin(*itsArray), end(*itsArray),
			[] (ItemInfo e) { return e.theEmbeddedFile == nullptr; });
}

/******************************************************************************
 ReplaceEmbeddedFileStreams

	Notify all embedded files that their enclosing file has a new std::fstream.

 ******************************************************************************/

void
JFileArrayIndex::ReplaceEmbeddedFileStreams
	(
	std::fstream* newStream
	)
{
	for (const auto& e : *itsArray)
	{
		if (e.theEmbeddedFile != nullptr)
		{
			e.theEmbeddedFile->ReplaceStream(newStream);
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
	const JSize		itemCount,
	std::istream&	input
	)
{
	// we should only be called when the file is opened, but it doesn't hurt to be sure

	itsArray->RemoveAll();

	// read in the information on each item

	ItemInfo itemInfo;
	for (JIndex i=1; i<=itemCount; i++)
	{
		itemInfo.offset = JFileArray::ReadUnsignedLong(input);

		JFAID_t id;
		id = JFileArray::ReadUnsignedLong(input);
		itemInfo.id.SetID(id);

		unsigned long type;
		type = JFileArray::ReadUnsignedLong(input);
		if (type == kData)
		{
			itemInfo.type = kData;
		}
		else if (type == kEmbeddedFile)
		{
			itemInfo.type = kEmbeddedFile;
		}
		else
		{
			assert( 0 );
		}

		itsArray->InsertItemAtIndex(i, itemInfo);
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
	std::ostream& output
	)
{
	for (const auto& e : *itsArray)
	{
		JFileArray::WriteUnsignedLong(output, e.offset);

		const JFAID_t id = e.id.GetID();
		JFileArray::WriteUnsignedLong(output, id);

		const unsigned long type = e.type;
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
	return GetItemCount() * kIndexItemLength;
}
