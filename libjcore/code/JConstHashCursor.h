#ifndef _H_JConstHashCursor
#define _H_JConstHashCursor

/******************************************************************************
 JConstHashCursor.h

	Interface for the JConstHashCursor class.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "jx-af/jcore/jTypes.h"

#include "jx-af/jcore/jHashFunctions.h"
#include "jx-af/jcore/JHashRecord.h"

	// Inclusion would cause a circular dependancy
	template <class V> class JHashTable;

template <class V>
class JConstHashCursor
{
public:

	JConstHashCursor(const JHashTable<V>* table);
	JConstHashCursor(const JHashTable<V>* table, const JHashValue hash);
//	JConstHashCursor(const JHashTable<V>* table, const K& key);
	virtual ~JConstHashCursor();

	// Accept built-in forms
	//	JConstHashCursor(const JConstHashCursor& source);
	//	const JConstHashCursor& operator=(const JConstHashCursor& source);

// Fundamental operations
	bool Next();
	bool Next(bool (*NextRecordType)(const JHashRecord<V>&),
				  const bool stopOnEmpty);

	bool NextFull();
	bool NextOpen();
	bool NextHash(const bool allowEmpty = false);
	bool NextHashOrOpen();

	bool NextKey(const bool allowEmpty = false);
	bool NextKeyOrOpen();

// Shorthands for common iterator constructs
	bool NextMapInsertHash();
	bool NextMapInsertKey();

	void Reset(const bool clear = false);
	void ResetHash(const JHashValue hash);
	void ResetKey(const V& value);

	// Useful for avoiding extra calls to the hash function
	JHashValue GetCursorHashValue() const;

	bool Equal(const V& value1, const V& value2) const;
	JHashValue Hash(const V& value1) const;

// Get... functions--may only be called after Next() has returned true;
// otherwise the result is undefined (but isn't likely to be good).

	const JHashRecord<V>& GetRecord() const;

	JHashRecordT::State GetState() const;
	bool            IsEmpty() const;
	bool            IsDeleted() const;
	bool            IsFull() const;
	JHashValue          GetHashValue() const;

	const V&              GetValue() const;

protected:

	const JHashTable<V>*  GetTable() const;

	JSize GetIndex() const;

	static JDualHashValue DualHash(const JHashValue hash);

	void Step();

private:

	const JHashTable<V>* itsTable;

	const V*       itsValue;
	JHashValue     itsHashValue;
	JSize          itsInitialIndex;
	JSize          itsIndex;
	JDualHashValue itsIncrement; // Needed for double hashing

	JSize itsInitialCount;
};

/******************************************************************************
 GetRecord

 *****************************************************************************/

template <class V>
inline const JHashRecord<V>&
JConstHashCursor<V>::GetRecord() const
{
	return itsTable->GetRecord(itsIndex);
}

/******************************************************************************
 GetState

 *****************************************************************************/

template <class V>
inline JHashRecordT::State
JConstHashCursor<V>::GetState() const
{
	return itsTable->GetState(itsIndex);
}

/******************************************************************************
 IsEmpty

 *****************************************************************************/

template <class V>
inline bool
JConstHashCursor<V>::IsEmpty() const
{
	return itsTable->IsEmpty(itsIndex);
}

/******************************************************************************
 IsDeleted

 *****************************************************************************/

template <class V>
inline bool
JConstHashCursor<V>::IsDeleted() const
{
	return itsTable->IsDeleted(itsIndex);
}

/******************************************************************************
 IsFull

 *****************************************************************************/

template <class V>
inline bool
JConstHashCursor<V>::IsFull() const
{
	return itsTable->IsFull(itsIndex);
}

/******************************************************************************
 GetCursorHashValue

	Returns the hash value that the cursor is currently using.  This can be
	useful for avoiding extra calls to the hash function.

 *****************************************************************************/

template <class V>
inline JHashValue
JConstHashCursor<V>::GetCursorHashValue() const
{
	return itsHashValue;
}

/******************************************************************************
 Equal

 *****************************************************************************/

template <class V>
inline bool
JConstHashCursor<V>::Equal
	(
	const V& value1,
	const V& value2
	)
	const
{
	return itsTable->itsEqual(value1, value2);
}

/******************************************************************************
 Hash

 *****************************************************************************/

template <class V>
inline JHashValue
JConstHashCursor<V>::Hash
	(
	const V& value
	)
	const
{
	return itsTable->itsHashFunction(value);
}

/******************************************************************************
 GetHashValue

 *****************************************************************************/

template <class V>
inline JHashValue
JConstHashCursor<V>::GetHashValue() const
{
	return itsTable->GetHashValue(itsIndex);
}

/******************************************************************************
 GetValue

 *****************************************************************************/

template <class V>
inline const V&
JConstHashCursor<V>::GetValue() const
{
	return itsTable->GetValue(itsIndex);
}

/******************************************************************************
 GetTable (protected)

 *****************************************************************************/

template <class V>
inline const JHashTable<V>*
JConstHashCursor<V>::GetTable() const
{
	return itsTable;
}

/******************************************************************************
 GetIndex (protected)

 *****************************************************************************/

template <class V>
inline JSize
JConstHashCursor<V>::GetIndex() const
{
	return itsIndex;
}

/******************************************************************************
 DualHash (static protected)

	The secondary hash function which allows double-hashing.

 *****************************************************************************/

template <class V>
inline JDualHashValue
JConstHashCursor<V>::DualHash
	(
	const JHashValue hashValue
	)
{
	return JDualHash(hashValue);
}

/******************************************************************************
 Step (protected)

	Advances the index by the increment, wrapping to the tablesize.  No safety
	of any kind.

 *****************************************************************************/

template <class V>
inline void
JConstHashCursor<V>::Step()
{
	itsIndex = itsTable->HashToIndex(itsIndex+itsIncrement);
}

#endif
