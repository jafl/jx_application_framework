/******************************************************************************
 JLinkedList.h

	Interface for JLinkedList class

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLinkedList
#define _H_JLinkedList

#include "jx-af/jcore/JList.h"
#include "jx-af/jcore/JLinkedListIterator.h"

template <class T>
class JLinkedListElement
{
public:

	JLinkedListElement<T>*	prev;
	T						data;
	JLinkedListElement<T>*	next;

	JLinkedListElement(const T& d)
		:
		prev(nullptr),
		data(d),
		next(nullptr)
		{ };
};

template <class T>
class JLinkedList : public JList<T>
{
	friend class JLinkedListIterator<T>;

public:

	JLinkedList();
	JLinkedList(const JLinkedList<T>& source);

	virtual ~JLinkedList();

	JLinkedList<T>& operator=(const JLinkedList<T>& source);

	virtual T		GetFirstElement() const;
	virtual T		GetLastElement() const;

	virtual void	PrependElement(const T& data);
	virtual void	AppendElement(const T& data);

	virtual void	RemoveAll();

	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0);
	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) const;

private:

	JLinkedListElement<T>*	itsFirstElement;
	JLinkedListElement<T>*	itsLastElement;

private:

	void	CopyList(const JLinkedList<T>& source);

	void	DeleteAll();
	void	DeleteChain(JLinkedListElement<T>* firstElement);

	JLinkedListElement<T>*	IteratorFindElement(const JIndex index) const;

	void	IteratorInsertElementAfter(const JIndex index, const T& data,
									   JLinkedListElement<T>* listElement);
	void	IteratorSetElement(const JIndex index, const T& data,
							   JLinkedListElement<T>* listElement);
	void	IteratorRemovePrevElements(const JIndex lastIndex, const JSize count,
									   JLinkedListElement<T>* origElement);
	void	IteratorRemoveNextElements(const JIndex firstIndex, const JSize count,
									   JLinkedListElement<T>* origElement);

	JLinkedListElement<T>*	UnlinkNextElements(const JIndex firstIndex, const JSize count,
											   JLinkedListElement<T>* origElement = nullptr);
};

#include "JLinkedList.tmpl"
#include "JLinkedListIterator.tmpl"

#endif
