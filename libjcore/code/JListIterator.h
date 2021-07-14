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
	JListIterator(const JListIterator<T>& source);

	virtual ~JListIterator();

	const JList<T>*	GetList() const;
	JBoolean		GetList(JList<T>** obj) const;

	virtual JBoolean	Prev(T* data, const JBoolean move = kJTrue) = 0;
	virtual JBoolean	Next(T* data, const JBoolean move = kJTrue) = 0;

	JBoolean			Prev(std::function<JBoolean(const T&)> match, T* item);
	JBoolean			Next(std::function<JBoolean(const T&)> match, T* item);

	virtual void		SkipPrev(const JSize count = 1) = 0;
	virtual void		SkipNext(const JSize count = 1) = 0;

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index);
	JBoolean			AtBeginning() const;
	JBoolean			AtEnd() const;
	JIndex				GetPrevElementIndex() const;		// asserts
	JIndex				GetNextElementIndex() const;		// asserts
	JBoolean			GetPrevElementIndex(JIndex* i) const;
	JBoolean			GetNextElementIndex(JIndex* i) const;

	// only allowed if constructed from non-const JList<T>*

	virtual JBoolean	SetPrev(const T& data, const JBoolean move = kJTrue) = 0;
	virtual JBoolean	SetNext(const T& data, const JBoolean move = kJTrue) = 0;

	virtual JBoolean	RemovePrev(const JSize count = 1) = 0;
	virtual JBoolean	RemoveNext(const JSize count = 1) = 0;

	virtual JBoolean	Insert(const T& data) = 0;

	// range-based for loop

	T					operator*() const;
	JListIterator<T>&	operator++();
	bool				operator!=(const JListIterator<T>& it) const;

protected:

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

	const JListIterator<T>& operator=(const JListIterator<T>& source);
};

#endif
