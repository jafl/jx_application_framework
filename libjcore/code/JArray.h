/******************************************************************************
 JArray.h

	Interface for JArray class

	Copyright (C) 1994-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArray
#define _H_JArray

#include "JList.h"
#include "JArrayIterator.h"

template <class T>
class JArray : public JList<T>
{
public:

	JArray(const JSize blockSize = 10);
	JArray(const JArray<T>& source);
	JArray(JArray<T>&& dyingSource);

	virtual ~JArray();

	const JArray<T>& operator=(const JArray<T>& source);

	explicit operator const T*() const;

	const T*	GetCArray() const;
	T*			AllocateCArray() const;		// client must call delete [] when finished with it

	T			GetElement(const JIndex index) const;
	void		SetElement(const JIndex index, const T& data);

	T			GetElementFromEnd(const JIndex index) const;
	void		SetElementFromEnd(const JIndex index, const T& data);

	virtual T	GetFirstElement() const override;
	virtual T	GetLastElement() const override;

	void			InsertElementAtIndex(const JIndex index, const T& data);
	virtual void	PrependElement(const T& data) override;
	virtual void	AppendElement(const T& data) override;

	void			RemoveElement(const JIndex index);
	void			RemoveNextElements(const JIndex firstIndex, const JSize count);
	void			RemovePrevElements(const JIndex lastIndex, const JSize count);
	void			RemoveElements(const JIndexRange& range);
	void			RemoveElements(const JListT::ElementsRemoved& info);
	virtual void	RemoveAll() override;

	void	MoveElementToIndex(const JIndex currentIndex, const JIndex newIndex);
	void	MoveElementToIndex(const JListT::ElementMoved& info);
	void	SwapElements(const JIndex index1, const JIndex index2);
	void	SwapElements(const JListT::ElementsSwapped& info);

	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) override;
	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) const override;

	JSize	GetBlockSize() const;
	void	SetBlockSize(const JSize newBlockSize);

	// sorting functions

	void		Sort();

	JBoolean	InsertSorted(const T& data, const JBoolean insertIfDuplicate = kJTrue,
							 JIndex* index = nullptr);
	JIndex		GetInsertionSortIndex(const T& data, JBoolean* isDuplicate = nullptr) const;

	JBoolean	SearchSorted(const T& target, const JListT::SearchReturn which,
							 JIndex* index) const;
	JIndex		SearchSorted1(const T& target,
							  const JListT::SearchReturn which,
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

#include "JArray.tmpl"
#include "JArrayIterator.tmpl"

#endif
