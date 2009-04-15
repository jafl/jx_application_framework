/******************************************************************************
 JFileHashTable.h

	Interface for JFileHashTable class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFileHashTable
#define _H_JFileHashTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class iostream;

template <class T>
class JFileHashTable
{
public:

	JFileHashTable();

	virtual ~JFileHashTable();

	virtual void	UpdateFile();

protected:

	JBoolean	OpenFile(const JCharacter* fileName, const JSize fileHeaderLength,
						 const JSize hashTableSize, const JSize identifierLength,
						 const JSize dataLength);

	JBoolean	FindElement(const T& theIdentifier, const JBoolean canAdd = kJFalse);
	void		RemoveElement(const T& theIdentifier);
	JBoolean	MoveToNextElement(JIndex* slotIndex) const;

	iostream*	GetFile() const;
	JSize		GetHashTableSize() const;
	JSize		GetIndentifierLength() const;
	JSize		GetDataLength() const;

	virtual JIndex		Hash(const T& theIdentifier) const = 0;
	virtual JBoolean	CompareFromFile(const T& theIdentifier) const = 0;
	virtual void		InitializeElement(const T& theIdentifier) = 0;

private:

	JSize		itsReservedSpaceLength;	// length of reserved space at front of file

	JSize		itsHashTableCount;		// number of hash tables
	JSize		itsHashTableSize;		// number of LHS's in a hash table
	JSize		itsIdentifierLength;	// length of identifier stored by derived class
	JSize		itsDataLength;			// length of data stored by derived class

	JSize		itsMaxSearchDistance;	// max distance to search in hash table

	iostream*	itsStream;				// file that stores hash table

private:

	void	CloseFile();

	void	AllocateOneHashTable();
	JSize	GetFileHeaderLength() const;
	JSize	GetElementLength() const;
	JSize	GetFileLength() const;

	void	AddElement(const JIndex tableIndex, const JIndex slotIndex,
					   const T& theIdentifier);

	void	ReadHashTableSize();
	void	WriteHashTableSize();

	void	ReadHashTableCount();
	void	WriteHashTableCount();

	void	ReadSearchDistance();
	void	WriteSearchDistance();

	// not allowed

	JFileHashTable(const JFileHashTable<T>& source);
	const JFileHashTable<T>& operator=(const JFileHashTable<T>& source);
};

#endif
