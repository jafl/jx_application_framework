/******************************************************************************
 JArray.h

	Interface for JArray class

	Copyright (C) 1994-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArray
#define _H_JArray

#include <JOrderedSet.h>

template <class T>
class JArray : public JOrderedSet<T>
{
public:

	JArray(const JSize aBlockSize = 5);
	JArray(const JArray<T>& source);

	virtual ~JArray();

	const JArray<T>& operator=(const JArray<T>& source);

	operator const T*() const;

	const T*	GetCArray() const;
	T*			AllocateCArray() const;		// client must call delete [] when finished with it

	virtual void	InsertElementAtIndex(const JIndex index, const T& data);

	virtual void	RemoveNextElements(const JIndex firstIndex, const JSize count);
	virtual void	RemoveAll();

	virtual T		GetElement(const JIndex index) const;
	virtual void	SetElement(const JIndex index, const T& data);

	virtual void	MoveElementToIndex(const JIndex currentIndex, const JIndex newIndex);
	virtual void	SwapElements(const JIndex index1, const JIndex index2);

	JSize	GetBlockSize() const;
	void	SetBlockSize(const JSize newBlockSize);

	// optimized for O(1) lookup time

	virtual void	Sort();

	virtual JIndex	SearchSorted1(const T& target,
								  const JOrderedSetT::SearchReturn which,
								  JBoolean* found) const;

	// unrelated, fast sort -- broadcasts Sorted
	// (Do NOT use on JPtrArray!)

	void	QuickSort(int (*compare)(const void*, const void*));

protected:

	const T&	ProtectedGetElement(const JIndex index) const;

private:

	T*		itsElements;		// Items in the array
	JSize	itsSlotCount;		// Total number of slots allocated
	JSize	itsBlockSize;		// Number of slots to allocate for more space

private:

	void	CopyArray(const JArray<T>& source);

	JIndex	CreateElement(const JIndex index);
	void	StoreElement(const JIndex index, const T& dataPtr);

	T*		GetElementPtr(const JIndex index);

	void	AddSlots();
	void	RemoveSlots();
	void	ResizeMemoryAllocation(const JSize newSlotCount);
};

#include <JArray.tmpl>

#endif
