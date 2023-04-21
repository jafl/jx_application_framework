/******************************************************************************
 JAliasArray.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAliasArray
#define _H_JAliasArray

#include "JArray.h"

template <class T>
class JAliasArray : public JCollection
{
public:

	JAliasArray(JArray<T>* data,
				const std::function<std::weak_ordering(const T&, const T&)> compareFn,
				const JListT::SortOrder order);

	~JAliasArray() override;

	JArray<T>*			GetData();
	const JArray<T>&	GetData() const;

	JIndex	GetElementIndex(const JIndex index) const;
	JIndex	GetElementIndexFromEnd(const JIndex index) const;

	const T	GetElement(const JIndex index) const;
	void	SetElement(const JIndex index, const T& data);

	const T	GetFirstElement() const;
	const T	GetLastElement() const;

	const T	GetElementFromEnd(const JIndex index) const;
	void	SetElementFromEnd(const JIndex index, const T& data);

	// sorting functions -- the reason this class exists

	void	SetCompareFunction(const std::function<std::weak_ordering(const T&, const T&)> compareFn);

	JListT::SortOrder	GetSortOrder() const;
	void				SetSortOrder(const JListT::SortOrder order);

	bool	SearchSorted(const T& target, const JListT::SearchReturn which,
						 JIndex* index) const;

	JIndex	SearchSortedOTI(const T& target,
						  const JListT::SearchReturn which,
						  bool* found) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JArray<T>*		itsData;		// not owned; die if nullptr
	JArray<JIndex>*	itsIndexArray;

	const std::function<std::weak_ordering(const T&, const T&)>*	itsCompareFn;

private:

	void	InstallCompareWrapper(const T* search);

	// not allowed

	JAliasArray(const JAliasArray<T>&) = delete;
	JAliasArray<T>& operator=(const JAliasArray<T>&) = delete;
};

#include "JAliasArray.tmpl"

#endif
