/******************************************************************************
 JFileArrayIndex.h

	Interface for JFileArrayIndex class (used by JFileArray class)

	Copyright (C) 1998 John Lindal.

 ******************************************************************************/

#ifndef _H_JFileArrayIndex
#define _H_JFileArrayIndex

#include "jx-af/jcore/JContainer.h"
#include "jx-af/jcore/JFAIndex.h"
#include "jx-af/jcore/JFAID.h"
#include "jx-af/jcore/JArray.h"
#include "jx-af/jcore/jFStreamUtil.h"

class JFileArray;

class JFileArrayIndex : public JContainer
{
public:

	enum ElementType
	{
		kData         = 0,
		kEmbeddedFile = 1
	};

public:

	JFileArrayIndex();

	~JFileArrayIndex();

	void	InsertElementAtIndex(const JFAIndex& index, const JUnsignedOffset offset,
								 const JFAID& id);
	void	RemoveElement(const JFAIndex& index, const JSize elementSize);

	void	MoveElementToIndex(const JFAIndex& currentIndex, const JFAIndex& newIndex);
	void	SwapElements(const JFAIndex& index1, const JFAIndex& index2);

	JUnsignedOffset	GetElementOffset(const JFAIndex& index) const;
	void			ElementSizeChanged(const JFAIndex& index,
									   const JInteger changeInElementSize);

	JFAID	GetElementID(const JFAIndex& index) const;
	void	SetElementID(const JFAIndex& index, const JFAID& id);

	bool	GetElementIndexFromID(const JFAID& id, JFAIndex* index) const;
	JFAID	GetUniqueID() const;

	void	SetToEmbeddedFile(const JFAIndex& index);
	bool	IsEmbeddedFile(const JFAIndex& index) const;

	void	EmbeddedFileOpened(const JFAIndex& index, JFileArray* theEmbeddedFile);
	void	EmbeddedFileClosed(const JFAIndex& index);

	bool	EmbeddedFileIsClosed(const JFAIndex& index) const;
	bool	AllEmbeddedFilesAreClosed() const;

	void	ReplaceEmbeddedFileStreams(std::fstream* newStream);

	void	ReadIndex(const JSize elementCount, std::istream& input);
	void	WriteIndex(std::ostream& output);
	JSize	GetIndexLength() const;

private:

	struct ElementInfo
	{
		JUnsignedOffset	offset;				// offset of data from start of file
		JFAID			id;					// unique id for element
		ElementType		type;				// data or embedded file
		JFileArray*		theEmbeddedFile;	// nullptr or pointer to open JFileArray object

		ElementInfo()
			:
			offset( 0 ),
			id( JFAID::kInvalidID ),
			type( kData ),
			theEmbeddedFile( nullptr )
			{ };

		ElementInfo
			(
			const JUnsignedOffset	anOffset,
			const JFAID&			anID,
			const ElementType		aType
			)
			:
			offset( anOffset ),
			id( anID ),
			type( aType ),
			theEmbeddedFile( nullptr )
			{ };
	};

private:

	JArray<ElementInfo>*	itsArray;

private:

	// not allowed

	JFileArrayIndex(const JFileArrayIndex& source);
	JFileArrayIndex& operator=(const JFileArrayIndex& source);
};

/******************************************************************************
 MoveElementToIndex

 ******************************************************************************/

inline void
JFileArrayIndex::MoveElementToIndex
	(
	const JFAIndex& currentIndex,
	const JFAIndex& newIndex
	)
{
	itsArray->MoveElementToIndex(currentIndex.GetIndex(), newIndex.GetIndex());
}

/******************************************************************************
 SwapElements

 ******************************************************************************/

inline void
JFileArrayIndex::SwapElements
	(
	const JFAIndex& index1,
	const JFAIndex& index2
	)
{
	itsArray->SwapElements(index1.GetIndex(), index2.GetIndex());
}

#endif
