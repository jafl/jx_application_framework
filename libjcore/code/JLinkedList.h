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
class JLinkedListItem
{
public:

	JLinkedListItem<T>*	prev;
	T						data;
	JLinkedListItem<T>*	next;

	JLinkedListItem(const T& d)
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

	T		GetFirstItem() const override;
	T		GetLastItem() const override;

	void	PrependItem(const T& data) override;
	void	AppendItem(const T& data) override;

	void	RemoveAll() override;

	JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) override;
	JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) const override;

private:

	JLinkedListItem<T>*	itsFirstItem;
	JLinkedListItem<T>*	itsLastItem;

private:

	void	CopyList(const JLinkedList<T>& source);

	void	DeleteAll();
	void	DeleteChain(JLinkedListItem<T>* firstItem);

	JLinkedListItem<T>*	IteratorFindItem(const JIndex index) const;

	void	IteratorInsertItemAfter(const JIndex index, const T& data,
									JLinkedListItem<T>* listItem);
	void	IteratorSetItem(const JIndex index, const T& data,
							JLinkedListItem<T>* listItem);
	void	IteratorRemovePrevItems(const JIndex lastIndex, const JSize count,
									JLinkedListItem<T>* origItem);
	void	IteratorRemoveNextItems(const JIndex firstIndex, const JSize count,
									JLinkedListItem<T>* origItem);

	JLinkedListItem<T>*	UnlinkNextItems(const JIndex firstIndex, const JSize count,
										JLinkedListItem<T>* origItem = nullptr);
};

#include "JLinkedList.tmpl"
#include "JLinkedListIterator.tmpl"

#endif
