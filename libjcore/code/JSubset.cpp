/******************************************************************************
 JSubset.cpp

							The JSubset class

	This class represents a subset of a given set.  It stores a sorted list of
	indices of the elements that are in the subset.  The indices are the indices
	of the elements in the enclosing set.

	GetElementCount returns the number of elements in the -subset-.

	NewIterator() returns iterators for iterating over all the elements in
	the subset.  Since we keep the indices sorted in our array, we create
	an iterator directly for our array of indices.

	In order to greatly simplify the code, we often temporarily append the
	index kMaxIndex to our list of indices.  It can only be temporary, however,
	since the iterators don't understand this trick.

	BASE CLASS = JCollection

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JSubset.h>
#include <JOrderedSetUtil.h>
#include <JKLRand.h>
#include <limits.h>
#include <jAssert.h>

const JIndex kMaxIndex = ULONG_MAX;

const JSize kBlockSize = 50;

enum
{
	kOrigSetSizeLength = 10,
	kElementLength     = sizeof(JCharacter)
};

static JKLRand theRNG;

/******************************************************************************
 Constructor

 ******************************************************************************/

JSubset::JSubset
	(
	const JSize setSize
	)
	:
	JCollection()
{
	// make sure that the original set was non-empty and that we can
	// append an index larger than any legal index

	assert( 0 < setSize && setSize < kMaxIndex );

	itsOriginalSetSize = setSize;

	itsIndices = new JArray<JIndex>(kBlockSize);
	assert( itsIndices != NULL );
	itsIndices->SetCompareFunction(JCompareIndices);
	itsIndices->SetSortOrder(JOrderedSetT::kSortAscending);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JSubset::JSubset
	(
	const JSubset& source
	)
	:
	JCollection(source)
{
	itsOriginalSetSize = source.itsOriginalSetSize;

	itsIndices = new JArray<JIndex>(*(source.itsIndices));
	assert( itsIndices != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSubset::~JSubset()
{
	delete itsIndices;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JSubset&
JSubset::operator=
	(
	const JSubset& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	JCollection::operator=(source);

	itsOriginalSetSize = source.itsOriginalSetSize;
	*itsIndices        = *(source.itsIndices);

	// allow chaining

	return *this;
}

/******************************************************************************
 SetOriginalSetSize

 ******************************************************************************/

void
JSubset::SetOriginalSetSize
	(
	const JSize size
	)
{
	assert( 0 < size && size < kMaxIndex );
	assert( itsIndices->IsEmpty() || itsIndices->GetLastElement() <= size );
	itsOriginalSetSize = size;
}

/******************************************************************************
 Complement

 ******************************************************************************/

JSubset
JSubset::Complement()
	const
{
	// create the subset to contain the complement

	const JSize setSize = GetOriginalSetSize();
	JSubset complement(setSize);

	// by appending this index, we know we will cover all the elements
	// up to and including setSize

	itsIndices->AppendElement(setSize+1);

	// add the elements between the elements of the original subset

	const JSize count = itsIndices->GetElementCount();
	JIndex index, prevIndex = 0;
	for (JIndex j=1; j<=count; j++)
		{
		index = itsIndices->GetElement(j);
		for (JIndex i=prevIndex+1; i<index; i++)
			{
			(complement.itsIndices)->AppendElement(i);
			complement.ElementAdded();
			}
		prevIndex = index;
		}

	// remove the extra index

	itsIndices->RemoveElement(count);

	return complement;
}

/******************************************************************************
 Addition

	This is equivalent to logical OR.

 ******************************************************************************/

JSubset&
JSubset::operator+=
	(
	const JSubset& subset
	)
{
	// make sure we are dealing with the same set size

	assert( itsOriginalSetSize == subset.itsOriginalSetSize );

	// if the subset is empty, we can quit now

	if (subset.IsEmpty())
		{
		return *this;
		}

	// make sure that we will stay in the loop until all the elements
	// from the subset have been added

	itsIndices->AppendElement(kMaxIndex);

	// add the elements in the other subset to ourselves

	const JArray<JIndex>* otherIndices = subset.itsIndices;

	JSize ourCount = itsIndices->GetElementCount();
	const JSize otherCount = otherIndices->GetElementCount();

	JIndex j = 1;
	JIndex indexToAdd = otherIndices->GetFirstElement();

	JBoolean found;
	JIndex i = itsIndices->SearchSorted1(indexToAdd, JOrderedSetT::kAnyMatch, &found);

//	JIndex i = 1;
	JIndex ourIndex = itsIndices->GetElement(i);

	while (1)
		{
		if (ourIndex == indexToAdd)
			{
			// If the index is in both sets,
			// just get the next index in each set.

			if (i < ourCount && j < otherCount)
				{
				i++;
				ourIndex = itsIndices->GetElement(i);
				j++;
				indexToAdd = otherIndices->GetElement(j);
				}
			else
				{
				break;
				}
			}
		else if (ourIndex < indexToAdd)
			{
			// let ourIndex catch up to indexToAdd

			if (i < ourCount)
				{
				i++;
				ourIndex = itsIndices->GetElement(i);
				}
			else
				{
				break;
				}
			}
		else if (indexToAdd < ourIndex)
			{
			// add the new index to ourselves and get the
			// next index to add

			itsIndices->InsertElementAtIndex(i, indexToAdd);
			ElementAdded();
			i++;
			ourCount++;

			if (j < otherCount)
				{
				j++;
				indexToAdd = otherIndices->GetElement(j);
				}
			else
				{
				break;
				}
			}
		}

	// remove the spurious element that we added initially

	itsIndices->RemoveElement(ourCount);

	// allow chaining

	return *this;
}

/******************************************************************************
 Subtraction

	Remove the elements in the given subset from our own subset.

 ******************************************************************************/

JSubset&
JSubset::operator-=
	(
	const JSubset& subset
	)
{
	// make sure we are dealing with the same set size

	assert( itsOriginalSetSize == subset.itsOriginalSetSize );

	// if we are empty or the subset is empty, we can quit now

	if (IsEmpty() || subset.IsEmpty())
		{
		return *this;
		}

	// remove the elements in the other subset from ourselves

	const JArray<JIndex>* otherIndices = subset.itsIndices;

	JSize ourCount = itsIndices->GetElementCount();
	const JSize otherCount = otherIndices->GetElementCount();

	JIndex j = 1;
	JIndex indexToRemove = otherIndices->GetFirstElement();

	JBoolean found;
	JIndex i = itsIndices->SearchSorted1(indexToRemove, JOrderedSetT::kAnyMatch, &found);

//	JIndex i = 1;
	JIndex ourIndex = itsIndices->GetElement(i);

	while (1)
		{
		if (ourIndex == indexToRemove)
			{
			// If the index is in both sets, remove it from our set and
			// get the next index in each set.

			itsIndices->RemoveElement(i);
			ElementRemoved();
			ourCount--;

			if (i <= ourCount && j < otherCount)
				{
				ourIndex = itsIndices->GetElement(i);
				j++;
				indexToRemove = otherIndices->GetElement(j);
				}
			else
				{
				break;
				}
			}
		else if (ourIndex < indexToRemove)
			{
			// let ourIndex catch up to indexToRemove

			if (i < ourCount)
				{
				i++;
				ourIndex = itsIndices->GetElement(i);
				}
			else
				{
				break;
				}
			}
		else if (indexToRemove < ourIndex)
			{
			// let indexToRemove catch up to ourIndex

			if (j < otherCount)
				{
				j++;
				indexToRemove = otherIndices->GetElement(j);
				}
			else
				{
				break;
				}
			}
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 Add

	Add the specified element to our subset.  We insert the index into the
	list to keep the indices sorted.

 ******************************************************************************/

void
JSubset::Add
	(
	const JIndex indexToAdd
	)
{
	assert( FullSetIndexValid(indexToAdd) );

	if (itsIndices->InsertSorted(indexToAdd, kJFalse))
		{
		ElementAdded();
		}
}

/******************************************************************************
 AddRange

	Add all elements in the given range to our subset.

 ******************************************************************************/

void
JSubset::AddRange
	(
	const JIndex startIndex,
	const JIndex endIndex
	)
{
	assert( FullSetIndexValid(startIndex) );
	assert( FullSetIndexValid(endIndex) );
	assert( startIndex <= endIndex );

	// make sure that we will stay in the loop until all the elements
	// from the subset have been added

	itsIndices->AppendElement(kMaxIndex);

	// add all the elements in the range to ourselves

	JSize count = itsIndices->GetElementCount();

	JIndex indexToAdd = startIndex;

	JBoolean found;
	JIndex i = itsIndices->SearchSorted1(indexToAdd, JOrderedSetT::kAnyMatch, &found);

//	JIndex i = 1;
	JIndex ourIndex = itsIndices->GetElement(i);

	while (1)
		{
		if (ourIndex == indexToAdd)
			{
			// If the index is in both sets,
			// just get the next index in each set.

			if (i < count)
				{
				i++;
				ourIndex = itsIndices->GetElement(i);
				}
			else
				{
				break;
				}

			if (indexToAdd < endIndex)
				{
				indexToAdd++;
				}
			else
				{
				break;
				}
			}
		else if (ourIndex < indexToAdd)
			{
			// let ourIndex catch up to indexToAdd

			if (i < count)
				{
				i++;
				ourIndex = itsIndices->GetElement(i);
				}
			else
				{
				break;
				}
			}
		else if (indexToAdd < ourIndex)
			{
			// add the new index to ourselves and get the
			// next index to add

			itsIndices->InsertElementAtIndex(i, indexToAdd);
			ElementAdded();
			i++;
			count++;

			if (indexToAdd < endIndex)
				{
				indexToAdd++;
				}
			else
				{
				break;
				}
			}
		}

	// remove the spurious element that we added initially

	itsIndices->RemoveElement(count);
}

/******************************************************************************
 AddAll

	Add all elements to our subset.

 ******************************************************************************/

void
JSubset::AddAll()
{
	if (itsIndices->GetElementCount() < itsOriginalSetSize)
		{
		itsIndices->RemoveAll();

		for (JIndex i=1; i<=itsOriginalSetSize; i++)
			{
			itsIndices->AppendElement(i);
			}

		SetElementCount(itsOriginalSetSize);
		}
}

/******************************************************************************
 Remove

	Remove the specified element from our subset.

 ******************************************************************************/

void
JSubset::Remove
	(
	const JIndex indexToRemove
	)
{
	JIndex i;
	if (itsIndices->SearchSorted(indexToRemove, JOrderedSetT::kAnyMatch, &i))
		{
		itsIndices->RemoveElement(i);
		ElementRemoved();
		}
}

/******************************************************************************
 RemoveRange

	Remove all elements in the given range from our subset.

 ******************************************************************************/

void
JSubset::RemoveRange
	(
	const JIndex startIndex,
	const JIndex endIndex
	)
{
	assert( FullSetIndexValid(startIndex) );
	assert( FullSetIndexValid(endIndex) );
	assert( startIndex <= endIndex );

	JBoolean foundFirst, foundLast;
	const JIndex i1 =
		itsIndices->SearchSorted1(startIndex, JOrderedSetT::kAnyMatch, &foundFirst);
	JIndex i2 =
		itsIndices->SearchSorted1(endIndex, JOrderedSetT::kAnyMatch, &foundLast);
	if (!foundLast)
		{
		i2--;
		}

	for (JIndex i=i1; i<=i2; i++)
		{
		itsIndices->RemoveElement(i1);
		ElementRemoved();
		}
}

/******************************************************************************
 RemoveAll

	Remove all elements from our subset.

 ******************************************************************************/

void
JSubset::RemoveAll()
{
	if (!IsEmpty())
		{
		itsIndices->RemoveAll();
		SetElementCount(0);
		}
}

/******************************************************************************
 GetRandomSample

	Randomly select sampleSize number of elements from the elements that we
	contain and add to sample.  *sample must have the same original set size
	as we do.  Returns kJTrue if the operation was successful.

 ******************************************************************************/

JBoolean
JSubset::GetRandomSample
	(
	JSubset*	sample,
	const JSize	sampleSize
	)
	const
{
	sample->RemoveAll();

	JSize count = GetElementCount();

	if (itsOriginalSetSize != sample->GetOriginalSetSize() ||
		sampleSize > count)
		{
		return kJFalse;
		}
	else if (sampleSize == count)
		{
		*sample = *this;
		return kJTrue;
		}
	else
		{
		JSize remaining = sampleSize;
		for (JIndex i=GetElementCount(); i>=1 && remaining>0; i--)
			{
			if (theRNG.UniformULong(1,i) <= remaining)
				{
				sample->Add(itsIndices->GetElement(i));
				remaining--;
				}
			}

		return kJTrue;
		}
}

/******************************************************************************
 GetRandomDisjointSamples

	If *sampleList is empty, allocates JSubsets to match sampleSizeList.
	Then fills them with disjoint subsets of the elements that we contain.
	The sum of the sample sizes must not exceed the number of elements
	that we contain.

	Returns kJTrue if the operation was successful.

 ******************************************************************************/

JBoolean
JSubset::GetRandomDisjointSamples
	(
	JPtrArray<JSubset>*		sampleList,
	const JArray<JSize>&	origSampleSizeList
	)
	const
{
JIndex i;

	// check the sample sizes

	const JSize sampleCount = origSampleSizeList.GetElementCount();
	const JSize* sizes      = origSampleSizeList.GetCArray();
	JSize sum               = 0;
	for (i=0; i<sampleCount; i++)
		{
		sum += sizes[i];
		}
	if (sum > GetElementCount())
		{
		return kJFalse;
		}

	// allocate the subsets

	if (sampleList->IsEmpty())
		{
		for (i=1; i<=sampleCount; i++)
			{
			JSubset* sample = new JSubset(itsOriginalSetSize);
			assert( sample != NULL );
			sampleList->Append(sample);
			}
		}
	else
		{
		assert( sampleList->GetElementCount() ==
				origSampleSizeList.GetElementCount() );

		for (i=1; i<=sampleCount; i++)
			{
			JSubset* sample = sampleList->NthElement(i);
			assert( sample != NULL );
			sample->RemoveAll();
			}
		}

	// partition our indices

	JArray<JSize> sampleSizeList = origSampleSizeList;
	JSize* remaining             = const_cast<JSize*>(sampleSizeList.GetCArray());

	JArray<JSize> countList(sampleCount);
	sum = GetElementCount();
	for (i=0; i<sampleCount; i++)
		{
		countList.AppendElement(sum);
		sum -= remaining[i];
		}
	JSize* count = const_cast<JSize*>(countList.GetCArray());

	for (i=GetElementCount(); i>=1; i--)
		{
		JBoolean done = kJTrue;
		for (JIndex j=0; j<sampleCount; j++)
			{
			assert( count[j] > 0 );
			count[j]--;		// must decrement, even when break
			if (theRNG.UniformULong(1,count[j]+1) <= remaining[j])
				{
				(sampleList->NthElement(j+1))->Add(itsIndices->GetElement(i));
				remaining[j]--;
				done = kJFalse;
				break;
				}
			if (remaining[j] > 0)
				{
				done = kJFalse;
				}
			}
		if (done)
			{
			break;
			}
		}

	return kJTrue;
}

/******************************************************************************
 GetDataStreamLength

	Since we write fixed length data, we are willing to tell others how
	long we are or will be.

	Not inlined because private constants should not be defined in header.

 ******************************************************************************/

JSize
JSubset::GetDataStreamLength()
	const
{
	return kOrigSetSizeLength + itsOriginalSetSize * kElementLength;
}

// static

JSize
JSubset::GetDataStreamLength
	(
	const JSize origSetSize
	)
{
	return kOrigSetSizeLength + origSetSize * kElementLength;
}

/******************************************************************************
 Global functions for JSubset class

 ******************************************************************************/

/******************************************************************************
 JIntersection

 ******************************************************************************/

JSubset
JIntersection
	(
	const JSubset& subset1,
	const JSubset& subset2
	)
{
	// make sure we are dealing with the same set size

	assert( subset1.GetOriginalSetSize() == subset2.GetOriginalSetSize() );

	// create the subset to contain the intersection

	JSubset intersection(subset1.GetOriginalSetSize());

	// if either one is empty, return an empty subset

	const JArray<JIndex>* indices1 = subset1.itsIndices;
	const JArray<JIndex>* indices2 = subset2.itsIndices;

	const JSize count1 = indices1->GetElementCount();
	const JSize count2 = indices2->GetElementCount();

	if (count1 == 0 || count2 == 0)
		{
		return intersection;
		}

	// we find the intersection by comparing the elements in the
	// two subsets

	JIndex i = 1;
	JIndex j = 1;
	JIndex index1 = indices1->GetFirstElement();
	JIndex index2 = indices2->GetFirstElement();

	JArray<JIndex>* intersectIndices = intersection.itsIndices;

	while (1)
		{
		if (index1 == index2)
			{
			// If the index is in both sets, add it to the intersection
			// and get the next index in each set.

			intersectIndices->AppendElement(index1);
			intersection.ElementAdded();

			if (i < count1 && j < count2)
				{
				i++;
				index1 = indices1->GetElement(i);
				j++;
				index2 = indices2->GetElement(j);
				}
			else
				{
				break;
				}
			}
		else if (index1 < index2)
			{
			// let index1 catch up to index2

			if (i < count1)
				{
				i++;
				index1 = indices1->GetElement(i);
				}
			else
				{
				break;
				}
			}
		else if (index2 < index1)
			{
			// let index2 catch up to index1

			if (j < count2)
				{
				j++;
				index2 = indices2->GetElement(j);
				}
			else
				{
				break;
				}
			}
		}

	return intersection;
}

/******************************************************************************
 Comparison

 ******************************************************************************/

int
operator==
	(
	const JSubset& subset1,
	const JSubset& subset2
	)
{
	// make sure we are dealing with the same set size

	const JSize setSize = subset1.GetOriginalSetSize();
	assert( setSize == subset2.GetOriginalSetSize() );

	// check that there are the same number of elements in each subset

	const JSize count = subset1.GetElementCount();
	if (count != subset2.GetElementCount())
		{
		return 0;
		}

	// compare the indices in the two subsets

	const JArray<JIndex>* indices1 = subset1.itsIndices;
	const JArray<JIndex>* indices2 = subset2.itsIndices;

	int equal = 1;
	for (JIndex i=1; i<=count; i++)
		{
		if (indices1->GetElement(i) != indices2->GetElement(i))
			{
			equal = 0;
			break;
			}
		}

	return equal;
}

/******************************************************************************
 JGetRandomSample

	Randomly select sampleSize number of elements between firstIndex and
	lastIndex and add to sample.

	If lastIndex == 0, it is set to the original set size of *sample.

	Returns kJTrue if the operation was successful.

 ******************************************************************************/

JBoolean
JGetRandomSample
	(
	JSubset*		sample,
	const JSize		sampleSize,
	const JIndex	firstIndex,
	const JIndex	origLastIndex
	)
{
	assert( sample != NULL && firstIndex > 0 );

	sample->RemoveAll();
	const JSize setSize = sample->GetOriginalSetSize();

	JIndex lastIndex = origLastIndex;
	if (lastIndex == 0)
		{
		lastIndex = setSize;
		}
	if (lastIndex > setSize || lastIndex < firstIndex)
		{
		return kJFalse;
		}

	const JSize rangeSize = lastIndex - firstIndex + 1;

	if (sampleSize > rangeSize)
		{
		return kJFalse;
		}
	else if (sampleSize == rangeSize)
		{
		sample->AddRange(firstIndex, lastIndex);
		return kJTrue;
		}
	else
		{
		JSize remaining = sampleSize;
		for (JIndex i=rangeSize; i>=1 && remaining>0; i--)
			{
			if (theRNG.UniformULong(1,i) <= remaining)
				{
				sample->Add(firstIndex + i-1);
				remaining--;
				}
			}

		return kJTrue;
		}
}

/******************************************************************************
 Stream operators

	We write fixed length data by writing out a stream of boolean values.
	(This saves space in the case of almost full subsets.)

 ******************************************************************************/

const JCharacter kInSet    = 'T';
const JCharacter kNotInSet = 'F';

istream&
operator>>
	(
	istream&	input,
	JSubset&	aSubset
	)
{
	// throw out old data

	aSubset.RemoveAll();

	// get size of set

	input >> aSubset.itsOriginalSetSize;
	if (input.eof() || input.fail())
		{
		return input;
		}

	// allocate space for the data

	JCharacter* data = new JCharacter [ aSubset.itsOriginalSetSize ];
	assert( data != NULL );

	// read the data all at once

	input.read(data, aSubset.itsOriginalSetSize);

	// add the elements that are in the set

	JArray<JIndex>* theIndices = aSubset.itsIndices;
	const JSize count = aSubset.itsOriginalSetSize;
	for (JIndex i=1; i<=count; i++)
		{
		if (data[ i-1 ] == kInSet)
			{
			theIndices->AppendElement(i);
			aSubset.ElementAdded();
			}
		}

	// clean up

	delete [] data;

	// allow chaining

	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const JSubset&	aSubset
	)
{
	// write the size of the set

	const JSize setSize = aSubset.GetOriginalSetSize();
	output.width(kOrigSetSizeLength);
	output.setf(ios::right, ios::adjustfield);
	output << setSize;

	// allocate space for the data

	JCharacter* data = new JCharacter[ setSize ];
	assert( data != NULL );

	// mark all elements as not in set
	{
	for (JIndex i=0; i<setSize; i++)
		{
		data[i] = kNotInSet;
		}
	}

	// mark elements that are in set
	{
	const JArray<JIndex>* theIndices = aSubset.itsIndices;
	const JSize count = theIndices->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JIndex index = theIndices->GetElement(i);
		data[ index-1 ] = kInSet;
		}
	}

	// write out the data

	output.write(data, setSize);
	delete data;

	// allow chaining

	return output;
}

/******************************************************************************
 More stream operators

	Since we write fixed length data, we can perform simple manipulations
	on the subset data without reading it all in.

 ******************************************************************************/

void JFindPositionOfElementInStream(iostream& theStream, const JIndex indexToAdd);

/******************************************************************************
 JAddToSubsetInStream

	Add the specified element to the subset in the given stream.

	*** The read mark must be set to the start of the data.

 ******************************************************************************/

void
JAddToSubsetInStream
	(
	iostream&		theStream,
	const JIndex	indexToAdd
	)
{
	JFindPositionOfElementInStream(theStream, indexToAdd);
	theStream << kInSet;
}

/******************************************************************************
 JRemoveFromSubsetInStream

	Remove the specified element from the subset in the given stream.

	*** The read mark must be set to the start of the data.

 ******************************************************************************/

void
JRemoveFromSubsetInStream
	(
	iostream&		theStream,
	const JIndex	indexToRemove
	)
{
	JFindPositionOfElementInStream(theStream, indexToRemove);
	theStream << kNotInSet;
}

/******************************************************************************
 JFindPositionOfElementInStream

	Find the specified element from the subset in the given stream.

	*** The read mark must be set to the start of the data.

 ******************************************************************************/

void
JFindPositionOfElementInStream
	(
	iostream&		theStream,
	const JIndex	indexToAdd
	)
{
	JSize setSize;
	theStream >> setSize;

	assert( 1 <= indexToAdd && indexToAdd <= setSize );

	streamoff offset = theStream.tellg();
	theStream.seekp(offset + indexToAdd - 1);
}

#define JTemplateType JSubset
#include <JPtrArray.tmpls>
#undef JTemplateType
