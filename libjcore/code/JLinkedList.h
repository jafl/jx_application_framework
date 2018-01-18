/******************************************************************************
 JLinkedList.h

	Interface for JLinkedList class

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JLinkedList
#define _H_JLinkedList

#include <JList.h>
#include <JLinkedListIterator.h>

template <class T>
class JLinkedListElement
{
public:

	JLinkedListElement<T>*	prevElement;
	T						data;
	JLinkedListElement<T>*	nextElement;

	JLinkedListElement()
		:
		prevElement( NULL ),
		nextElement( NULL )
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

	const JLinkedList<T>& operator=(const JLinkedList<T>& source);

	virtual void	InsertElementAtIndex(const JIndex index, const T& data) override;

	virtual void	RemoveNextElements(const JIndex firstIndex, const JSize count) override;
	virtual void	RemoveAll() override;

	virtual T		GetElement(const JIndex index) const override;
	virtual void	SetElement(const JIndex index, const T& data) override;

	virtual void	MoveElementToIndex(const JIndex currentIndex, const JIndex newIndex) override;
	virtual void	SwapElements(const JIndex index1, const JIndex index2) override;

	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) override;
	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) const override;

protected:

	const T&	ProtectedGetElement(const JIndex index) const;

private:

	JLinkedListElement<T>*	itsFirstElement;
	JLinkedListElement<T>*	itsLastElement;

private:

	void	CopyList(const JLinkedList<T>& source);

	JLinkedListElement<T>*	CreateElement(const JIndex index, JIndex* trueIndex);
	void					DeleteNextElements(const JIndex firstIndex, const JSize count,
											   JLinkedListElement<T>* origElement = NULL);
	void					DeleteAll();
	void					DeleteChain(JLinkedListElement<T>* firstElement);

	void					LinkInElement(const JIndex index,
										  JLinkedListElement<T>* theElement);
	JLinkedListElement<T>*	UnlinkNextElements(const JIndex firstIndex, const JSize count,
											   JLinkedListElement<T>* origElement = NULL);

	void	StoreData(JLinkedListElement<T>* listElement, const T& data);

	void	PrivateMoveElementToIndex(const JIndex currentIndex, const JIndex newIndex);

	JLinkedListElement<T>*	FindElement(const JIndex index) const;

	void	IteratorSetElement(const JIndex index, const T& data,
							   JLinkedListElement<T>* listElement);
	void	IteratorRemoveNextElements(const JIndex firstIndex, const JSize count,
									   JLinkedListElement<T>* origElement);
};

#include <JLinkedList.tmpl>
#include <JLinkedListIterator.tmpl>

#endif
