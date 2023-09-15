/******************************************************************************
 JLinkedList.h

	Interface for JLinkedList class

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLinkedList
#define _H_JLinkedList

#include "JList.h"
#include "JLinkedListIterator.h"

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

	~JLinkedList() override;

	JLinkedList<T>& operator=(const JLinkedList<T>& source);

	T		GetFirstElement() const override;
	T		GetLastElement() const override;

	void	PrependElement(const T& data) override;
	void	AppendElement(const T& data) override;

	void	RemoveAll() override;

	JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) override;
	JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) const override;

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
