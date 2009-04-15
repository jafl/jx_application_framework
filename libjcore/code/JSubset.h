/******************************************************************************
 JSubset.h

	Interface for the JSubset class

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JSubset
#define _H_JSubset

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JCollection.h>
#include <JPtrArray.h>

typedef JOrderedSetIterator<JIndex>	JSubsetIterator;

class JSubset : public JCollection
{
	friend istream& operator>>(istream& input, JSubset& aSubset);
	friend ostream& operator<<(ostream& output, const JSubset& aSubset);

	friend JSubset JIntersection(const JSubset& s1, const JSubset& s2);

	friend int operator==(const JSubset& s1, const JSubset& s2);

public:

	JSubset(const JSize setSize);
	JSubset(const JSubset& source);

	virtual ~JSubset();

	const JSubset& operator=(const JSubset& source);

	JSubset& operator+=(const JSubset& s);
	JSubset& operator-=(const JSubset& s);

	JSubset	Complement() const;

	JSize		GetOriginalSetSize() const;
	void		SetOriginalSetSize(const JSize size);
	JBoolean	Contains(const JIndex indexToFind) const;

	void	Add(const JIndex indexToAdd);
	void	AddRange(const JIndex startIndex, const JIndex endIndex);
	void	AddAll();
	void	Remove(const JIndex indexToRemove);
	void	RemoveRange(const JIndex startIndex, const JIndex endIndex);
	void	RemoveAll();

	JBoolean	GetRandomSample(JSubset* sample, const JSize sampleSize) const;
	JBoolean	GetRandomDisjointSamples(JPtrArray<JSubset>* sampleList,
										 const JArray<JSize>& sampleSizeList) const;

	JSubsetIterator	NewIterator(
		const JIteratorPosition start = kJIteratorStartAtBeginning,
		const JIndex index = 0);
	JSubsetIterator	NewIterator(
		const JIteratorPosition start = kJIteratorStartAtBeginning,
		const JIndex index = 0) const;

	JSize			GetDataStreamLength() const;
	static JSize	GetDataStreamLength(const JSize origSetSize);

	JBoolean	FullSetIndexValid(const JIndex index) const;

private:

	JSize			itsOriginalSetSize;
	JArray<JIndex>*	itsIndices;
};

// declarations of non-friend global functions for dealing with subsets

JBoolean JGetRandomSample(JSubset* sample, const JSize sampleSize,
						  const JIndex firstIndex = 1, const JIndex lastIndex = 0);

void JAddToSubsetInStream(iostream& theStream, const JIndex indexToAdd);
void JRemoveFromSubsetInStream(iostream& theStream, const JIndex indexToRemove);

// inlined functions

inline int
operator!=
	(
	const JSubset& subset1,
	const JSubset& subset2
	)
{
	return !(subset1 == subset2);
}

inline JSubset
operator+
	(
	const JSubset& subset1,
	const JSubset& subset2
	)
{
	JSubset sum = subset1;
	sum += subset2;
	return sum;
}

inline JSubset
operator-
	(
	const JSubset& subset1,
	const JSubset& subset2
	)
{
	JSubset diff = subset1;
	diff -= subset2;
	return diff;
}

inline JSubset
JUnion
	(
	const JSubset& subset1,
	const JSubset& subset2
	)
{
	return (subset1 + subset2);
}

/******************************************************************************
 GetOriginalSetSize

 ******************************************************************************/

inline JSize
JSubset::GetOriginalSetSize()
	const
{
	return itsOriginalSetSize;
}

/******************************************************************************
 Contains

	Returns kJTrue if our subset contains the specified element.

 ******************************************************************************/

inline JBoolean
JSubset::Contains
	(
	const JIndex indexToFind
	)
	const
{
	JIndex i;
	return itsIndices->SearchSorted(indexToFind, JOrderedSetT::kAnyMatch, &i);
}

/******************************************************************************
 NewIterator

	Returns a new iterator for use with this subset.

 ******************************************************************************/

inline JSubsetIterator
JSubset::NewIterator
	(
	const JIteratorPosition	start,
	const JIndex			index
	)
{
	return JSubsetIterator(itsIndices, start, index);
}

inline JSubsetIterator
JSubset::NewIterator
	(
	const JIteratorPosition	start,
	const JIndex			index
	)
	const
{
	return JSubsetIterator(*itsIndices, start, index);
}

/******************************************************************************
 FullSetIndexValid

 ******************************************************************************/

inline JBoolean
JSubset::FullSetIndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= itsOriginalSetSize );
}

#endif
