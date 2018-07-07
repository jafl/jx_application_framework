/******************************************************************************
 JArray.h

	Interface for JArray class

	Copyright (C) 1994-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArray
#define _H_JArray

#include "JList.h"

template <class T>
class JArray : public JList<T>
{
public:

	JArray(const JSize aBlockSize = 5);
	JArray(const JArray<T>& source);
	JArray(JArray<T>&& dyingSource);

	virtual ~JArray();

	const JArray<T>& operator=(const JArray<T>& source);

	explicit operator const T*() const;

	const T*	GetCArray() const;
	T*			AllocateCArray() const;		// client must call delete [] when finished with it

	virtual void	InsertElementAtIndex(const JIndex index, const T& data) override;

	virtual void	RemoveNextElements(const JIndex firstIndex, const JSize count) override;
	virtual void	RemoveAll() override;

	virtual T		GetElement(const JIndex index) const override;
	virtual void	SetElement(const JIndex index, const T& data) override;

	virtual void	MoveElementToIndex(const JIndex currentIndex, const JIndex newIndex) override;
	virtual void	SwapElements(const JIndex index1, const JIndex index2) override;

	JSize	GetBlockSize() const;
	void	SetBlockSize(const JSize newBlockSize);

	// optimized for O(1) lookup time

	virtual void	Sort() override;

	virtual JIndex	SearchSorted1(const T& target,
								  const JListT::SearchReturn which,
								  JBoolean* found) const override;

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

#include "JArray.tmpl"

#endif
