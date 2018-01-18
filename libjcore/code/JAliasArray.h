/******************************************************************************
 JAliasArray.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JAliasArray
#define _H_JAliasArray

#include <JArray.h>

template <class T>
class JAliasArray : public JCollection
{
public:

	JAliasArray(JList<T>* data,
				JListT::CompareResult (*compareFn)(const T&, const T&),
				const JListT::SortOrder order);
	JAliasArray(JList<T>* data,
				const JElementComparison<T>& compareObj,
				const JListT::SortOrder order);

	virtual ~JAliasArray();

	JList<T>*			GetData();
	const JList<T>&	GetData() const;

	JIndex	GetElementIndex(const JIndex index) const;
	JIndex	GetElementIndexFromEnd(const JIndex index) const;

	const T	GetElement(const JIndex index) const;
	void	SetElement(const JIndex index, const T& data);

	const T	GetFirstElement() const;
	const T	GetLastElement() const;

	const T	GetElementFromEnd(const JIndex index) const;
	void	SetElementFromEnd(const JIndex index, const T& data);

	// sorting functions -- the reason this class exists

	JBoolean	GetCompareFunction(JListT::CompareResult (**compareFn)(const T&, const T&)) const;
	void		SetCompareFunction(JListT::CompareResult (*compareFn)(const T&, const T&));

	const JElementComparison<T>&	GetCompareObject() const;
	void	SetCompareObject(const JElementComparison<T>& compareObj);

	JListT::SortOrder	GetSortOrder() const;
	void					SetSortOrder(const JListT::SortOrder order);

	JBoolean	SearchSorted(const T& target, const JListT::SearchReturn which,
							 JIndex* index) const;

	JIndex		SearchSorted1(const T& target,
							  const JListT::SearchReturn which,
							  JBoolean* found) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JList<T>*	itsData;		// not owned; die if NULL
	JArray<JIndex>*	itsIndexArray;

	JListT::CompareResult	(*itsCompareFn)(const T&, const T&);	// can be NULL
	JElementComparison<T>*		itsCompareObj;							// can be NULL

private:

	void	JAliasArrayX1(const JListT::SortOrder order);
	void	JAliasArrayX2();

	// not allowed

	JAliasArray(const JAliasArray<T>& source);
	const JAliasArray<T>& operator=(const JAliasArray<T>& source);
};

#include <JAliasArray.tmpl>

#endif
