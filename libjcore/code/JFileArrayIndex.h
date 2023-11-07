/******************************************************************************
 JFileArrayIndex.h

	Interface for JFileArrayIndex class (used by JFileArray class)

	Copyright (C) 1998 John Lindal.

 ******************************************************************************/

#ifndef _H_JFileArrayIndex
#define _H_JFileArrayIndex

#include "JContainer.h"
#include "JFAIndex.h"
#include "JFAID.h"
#include "JArray.h"
#include "jFStreamUtil.h"

class JFileArray;

class JFileArrayIndex : public JContainer
{
public:

	enum ItemType
	{
		kData         = 0,
		kEmbeddedFile = 1
	};

public:

	JFileArrayIndex();

	~JFileArrayIndex() override;

	void	InsertItemAtIndex(const JFAIndex& index, const JUnsignedOffset offset,
								 const JFAID& id);
	void	RemoveItem(const JFAIndex& index, const JSize itemSize);

	void	MoveItemToIndex(const JFAIndex& currentIndex, const JFAIndex& newIndex);
	void	SwapItems(const JFAIndex& index1, const JFAIndex& index2);

	JUnsignedOffset	GetItemOffset(const JFAIndex& index) const;
	void			ItemSizeChanged(const JFAIndex& index,
									const JInteger changeInItemSize);

	JFAID	GetItemID(const JFAIndex& index) const;
	void	SetItemID(const JFAIndex& index, const JFAID& id);

	bool	GetItemIndexFromID(const JFAID& id, JFAIndex* index) const;
	JFAID	GetUniqueID() const;

	void	SetToEmbeddedFile(const JFAIndex& index);
	bool	IsEmbeddedFile(const JFAIndex& index) const;

	void	EmbeddedFileOpened(const JFAIndex& index, JFileArray* theEmbeddedFile);
	void	EmbeddedFileClosed(const JFAIndex& index);

	bool	EmbeddedFileIsClosed(const JFAIndex& index) const;
	bool	AllEmbeddedFilesAreClosed() const;

	void	ReplaceEmbeddedFileStreams(std::fstream* newStream);

	void	ReadIndex(const JSize itemCount, std::istream& input);
	void	WriteIndex(std::ostream& output);
	JSize	GetIndexLength() const;

private:

	struct ItemInfo
	{
		JUnsignedOffset	offset;				// offset of data from start of file
		JFAID			id;					// unique id for item
		ItemType		type;				// data or embedded file
		JFileArray*		theEmbeddedFile;	// nullptr or pointer to open JFileArray object

		ItemInfo()
			:
			offset( 0 ),
			id( JFAID::kInvalidID ),
			type( kData ),
			theEmbeddedFile( nullptr )
			{ };

		ItemInfo
			(
			const JUnsignedOffset	anOffset,
			const JFAID&			anID,
			const ItemType		aType
			)
			:
			offset( anOffset ),
			id( anID ),
			type( aType ),
			theEmbeddedFile( nullptr )
			{ };
	};

private:

	JArray<ItemInfo>*	itsArray;

private:

	// not allowed

	JFileArrayIndex(const JFileArrayIndex& source);
	JFileArrayIndex& operator=(const JFileArrayIndex& source);
};

/******************************************************************************
 MoveItemToIndex

 ******************************************************************************/

inline void
JFileArrayIndex::MoveItemToIndex
	(
	const JFAIndex& currentIndex,
	const JFAIndex& newIndex
	)
{
	itsArray->MoveItemToIndex(currentIndex.GetIndex(), newIndex.GetIndex());
}

/******************************************************************************
 SwapItems

 ******************************************************************************/

inline void
JFileArrayIndex::SwapItems
	(
	const JFAIndex& index1,
	const JFAIndex& index2
	)
{
	itsArray->SwapItems(index1.GetIndex(), index2.GetIndex());
}

#endif
