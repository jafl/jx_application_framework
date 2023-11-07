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
	JArray(JArray<T>&& dyingSource) noexcept;

	~JArray() override;

	JArray<T>& operator=(const JArray<T>& source);

	explicit operator const T*() const;

	const T*	GetCArray() const;
	T*			AllocateCArray() const;		// client must call delete [] when finished with it

	T		GetItem(const JIndex index) const;
	void	SetItem(const JIndex index, const T& data);

	T		GetItemFromEnd(const JIndex index) const;
	void	SetItemFromEnd(const JIndex index, const T& data);

	T		GetFirstItem() const override;
	T		GetLastItem() const override;

	void	InsertItemAtIndex(const JIndex index, const T& data);
	void	PrependItem(const T& data) override;
	void	AppendItem(const T& data) override;

	void	RemoveItem(const JIndex index);
	void	RemoveNextItems(const JIndex firstIndex, const JSize count);
	void	RemovePrevItems(const JIndex lastIndex, const JSize count);
	void	RemoveItems(const JIndexRange& range);
	void	RemoveItems(const JListT::ItemsRemoved& info);
	void	RemoveAll() override;

	void	MoveItemToIndex(const JIndex currentIndex, const JIndex newIndex);
	void	MoveItemToIndex(const JListT::ItemMoved& info);
	void	SwapItems(const JIndex index1, const JIndex index2);
	void	SwapItems(const JListT::ItemsSwapped& info);

	JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) override;
	JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) const override;

	JSize	GetBlockSize() const;
	void	SetBlockSize(const JSize newBlockSize);

	// sorting functions

	void	Sort();

	bool	InsertSorted(const T& data, const bool insertIfDuplicate = true,
						 JIndex* index = nullptr);
	JIndex	GetInsertionSortIndex(const T& data, bool* isDuplicate = nullptr) const;

	bool	SearchSorted(const T& target, const JListT::SearchReturn which,
						 JIndex* index) const;
	JIndex	SearchSortedOTI(const T& target,
							const JListT::SearchReturn which,
							bool* found) const;

	// unrelated, fast sort -- broadcasts Sorted
	// (Do NOT use on JPtrArray!)

	void	QuickSort(int (*compare)(const void*, const void*));

	// std::ranges support

	const T* begin() const { return itsItems; };
	const T* end() const   { return (itsItems + this->GetItemCount()); };

protected:

	const T&	ProtectedGetItem(const JIndex index) const;

private:

	T*		itsItems;		// Items in the array
	JSize	itsSlotCount;	// Total number of slots allocated
	JSize	itsBlockSize;	// Number of slots to allocate for more space

private:

	void	CopyArray(const JArray<T>& source);

	JIndex	CreateItem(const JIndex index);
	void	StoreItem(const JIndex index, const T& dataPtr);

	T*		GetItemPtr(const JIndex index);

	void	AddSlots();
	void	RemoveSlots();
	void	ResizeMemoryAllocation(const JSize newSlotCount);
};

#endif
