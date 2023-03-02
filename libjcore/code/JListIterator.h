/******************************************************************************
 JListIterator.h

	Interface for JListIterator class template

	Copyright (C) 1994-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JListIterator
#define _H_JListIterator

#include "JCollection.h"
#include <functional>

// must forward declare to avoid #include loop
// (We do it here so JList includes JListIterator for convenience.)
template <class T> class JList;

template <class T>
class JListIterator
{
	friend class JList<T>;

public:

	JListIterator(const JList<T>& theList,
				  const JIteratorPosition start = kJIteratorStartAtBeginning,
				  const JIndex index = 0);
	JListIterator(JList<T>* theList,
				  const JIteratorPosition start = kJIteratorStartAtBeginning,
				  const JIndex index = 0);

	virtual ~JListIterator();

	const JList<T>*	GetList() const;
	bool			GetList(JList<T>** obj) const;

	virtual bool	Prev(T* data, const JIteratorAction move = kJIteratorMove) = 0;
	virtual bool	Next(T* data, const JIteratorAction move = kJIteratorMove) = 0;

	bool			Prev(std::function<bool(const T&)> match, T* item);
	bool			Next(std::function<bool(const T&)> match, T* item);

	virtual void	SkipPrev(const JSize count = 1) = 0;
	virtual void	SkipNext(const JSize count = 1) = 0;

	virtual void	MoveTo(const JIteratorPosition newPosition, const JIndex index);
	bool			AtBeginning() const;
	bool			AtEnd() const;
	JIndex			GetPrevElementIndex() const;		// asserts
	JIndex			GetNextElementIndex() const;		// asserts
	bool			GetPrevElementIndex(JIndex* i) const;
	bool			GetNextElementIndex(JIndex* i) const;

	// only allowed if constructed from non-const JList<T>*

	virtual bool	SetPrev(const T& data, const JIteratorAction move = kJIteratorMove) = 0;
	virtual bool	SetNext(const T& data, const JIteratorAction move = kJIteratorMove) = 0;

	virtual bool	RemovePrev(const JSize count = 1) = 0;
	virtual bool	RemoveNext(const JSize count = 1) = 0;

	virtual bool	Insert(const T& data) = 0;

	// range-based for loop

	T					operator*() const;
	JListIterator<T>&	operator++();
	bool				operator!=(const JListIterator<T>& it) const;

protected:

	JListIterator(const JListIterator<T>& source);

	JCursorPosition	GetCursor() const;
	void			SetCursor(const JCursorPosition pos);

	virtual void	ListChanged(const JBroadcaster::Message& message);

private:

	const JList<T>*		itsConstList;		// JList that is being iterated over
	JList<T>*			itsList;			// nullptr if we were passed a const object
	JCursorPosition		itsCursorPosition;	// Current iterator position

	JListIterator<T>*	itsNextIterator;	// Next iterator in linked list

private:

	void	JListIteratorX(const JIteratorPosition start, const JIndex index);

	void	AddToIteratorList();
	void	RemoveFromIteratorList();

	// not allowed

	JListIterator<T>& operator=(const JListIterator<T>&) = delete;
};

#endif
