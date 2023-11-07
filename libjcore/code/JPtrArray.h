/******************************************************************************
 JPtrArray.h

	Interface for JPtrArray class

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPtrArray
#define _H_JPtrArray

#include "JArray.h"
#include "JArrayIterator.h"
#include "JPtrArrayT.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

template <class T>
class JPtrArray : public JArray<T*>
{
public:

	JPtrArray(const JPtrArrayT::CleanUpAction action, const JSize aBlockSize = 10);
	JPtrArray(const JPtrArray<T>& source, const JPtrArrayT::CleanUpAction action);
	JPtrArray(JPtrArray<T>&& dyingSource) noexcept;

	~JPtrArray() override;

	// we use different names so the insert-copy versions don't shadow the JArray functions

	const JPtrArray<T>& CopyPointers(const JPtrArray<T>& source,
									 const JPtrArrayT::CleanUpAction action,
									 const bool append);

	void	InsertAtIndex(const JIndex index, T* dataPtr);
	void	Prepend(T* dataPtr);
	void	Append(T* dataPtr);
	void	InsertBefore(const T* beforePtr, T* dataPtr);
	void	InsertAfter(const T* afterPtr, T* dataPtr);
	void	SetItem(const JIndex index, T* dataPtr,
					   const JPtrArrayT::SetAction action);
	void	SetToNull(const JIndex index, const JPtrArrayT::SetAction action);

	// these insert a *copy* of the object into the array
	// (only available if template instantiated with #define JPtrArrayCopy)

	const JPtrArray<T>& CopyObjects(const JPtrArray<T>& source,
									const JPtrArrayT::CleanUpAction action,
									const bool append);

	void	InsertAtIndex(const JIndex index, const T& data);
	void	Prepend(const T& data);
	void	Append(const T& data);
	void	InsertBefore(const T* beforePtr, const T& data);
	void	InsertAfter(const T* afterPtr, const T& data);
	void	SetItem(const JIndex index, const T& data,
					   const JPtrArrayT::SetAction action);

	bool	Remove(const T* dataPtr);
	void	DeleteItem(const JIndex index);
	void	DeleteAll();
	void	DeleteItemAsArray(const JIndex index);
	void	DeleteAllAsArrays();

	bool	Includes(const T* dataPtr) const;
	bool	Find(const T* dataPtr, JIndex* itemIndex) const;

	JPtrArrayT::CleanUpAction	GetCleanUpAction() const;
	void						SetCleanUpAction(const JPtrArrayT::CleanUpAction action);
	void						CleanOut();

private:

	JPtrArrayT::CleanUpAction	itsCleanUpAction;

private:

	// This is generally fatal for JPtrArrays

	void	QuickSort(int (*compare)(const void*, const void*));

	// These are not safe because the CleanUpAction must not be kDeleteAllAs*
	// for both the original and the copy.

	JPtrArray(const JPtrArray<T>&) = delete;
	JPtrArray<T>& operator=(const JPtrArray<T>&) = delete;
};

// "deep copy constructor" version of JPtrArray

template <class T>
class JDCCPtrArray : public JPtrArray<T>
{
public:

	JDCCPtrArray(const JPtrArrayT::CleanUpAction action, const JSize aBlockSize = 10);
	JDCCPtrArray(const JPtrArray<T>& source, const JPtrArrayT::CleanUpAction action);
};

// JPtrArrayIterator.h can't include JPtrArray.h because that would create
// a loop.  JPtrArrayIterator.h would have to include JPtrArray.h because
// it needs SetAction.

template <class T>
class JPtrArrayIterator : public JArrayIterator<T*>
{
public:

	JPtrArrayIterator(const JPtrArray<T>& theArray,
					  const JListT::Position start = JListT::kStartAtBeginning,
					  const JIndex index = 0);
	JPtrArrayIterator(JPtrArray<T>* theArray,
					  const JListT::Position start = JListT::kStartAtBeginning,
					  const JIndex index = 0);

	// only allowed if constructed from non-const JList<T>*

	bool	SetPrev(T* dataPtr, const JPtrArrayT::SetAction action,
					const JListT::Action move = JListT::kMove);
	bool	SetNext(T* dataPtr, const JPtrArrayT::SetAction action,
					const JListT::Action move = JListT::kMove);

	bool	DeletePrev();
	bool	DeleteNext();

	bool	DeletePrevAsArray();
	bool	DeleteNextAsArray();
};

#pragma GCC diagnostic pop

#endif
