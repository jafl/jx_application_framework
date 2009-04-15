/******************************************************************************
 JAliasArray.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JAliasArray
#define _H_JAliasArray

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JArray.h>

template <class T>
class JAliasArray : public JCollection
{
public:

	JAliasArray(JOrderedSet<T>* data,
				JOrderedSetT::CompareResult (*compareFn)(const T&, const T&),
				const JOrderedSetT::SortOrder order);
	JAliasArray(JOrderedSet<T>* data,
				const JElementComparison<T>& compareObj,
				const JOrderedSetT::SortOrder order);

	virtual ~JAliasArray();

	JOrderedSet<T>*			GetData();
	const JOrderedSet<T>&	GetData() const;

	JIndex	GetElementIndex(const JIndex index) const;
	JIndex	GetElementIndexFromEnd(const JIndex index) const;

	const T	GetElement(const JIndex index) const;
	void	SetElement(const JIndex index, const T& data);

	const T	GetFirstElement() const;
	const T	GetLastElement() const;

	const T	GetElementFromEnd(const JIndex index) const;
	void	SetElementFromEnd(const JIndex index, const T& data);

	// sorting functions -- the reason this class exists

	JBoolean	GetCompareFunction(JOrderedSetT::CompareResult (**compareFn)(const T&, const T&)) const;
	void		SetCompareFunction(JOrderedSetT::CompareResult (*compareFn)(const T&, const T&));

	const JElementComparison<T>&	GetCompareObject() const;
	void	SetCompareObject(const JElementComparison<T>& compareObj);

	JOrderedSetT::SortOrder	GetSortOrder() const;
	void					SetSortOrder(const JOrderedSetT::SortOrder order);

	JBoolean	SearchSorted(const T& target, const JOrderedSetT::SearchReturn which,
							 JIndex* index) const;

	JIndex		SearchSorted1(const T& target,
							  const JOrderedSetT::SearchReturn which,
							  JBoolean* found) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JOrderedSet<T>*	itsData;		// not owned; die if NULL
	JArray<JIndex>*	itsIndexArray;

	JOrderedSetT::CompareResult	(*itsCompareFn)(const T&, const T&);	// can be NULL
	JElementComparison<T>*		itsCompareObj;							// can be NULL

private:

	void	JAliasArrayX1(const JOrderedSetT::SortOrder order);
	void	JAliasArrayX2();

	// not allowed

	JAliasArray(const JAliasArray<T>& source);
	const JAliasArray<T>& operator=(const JAliasArray<T>& source);
};

#endif
