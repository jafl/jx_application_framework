/******************************************************************************
 JList.h

	Copyright (C) 1994-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JList
#define _H_JList

#include "JCollection.h"
#include "JRange.h"

// namespace to own JBroadcaster messages, etc (template is unnecessary)

class JListT
{
public:

	enum SortOrder
	{
		kSortAscending,
		kSortDescending
	};

	enum SearchReturn
	{
		kFirstMatch,
		kLastMatch,
		kAnyMatch
	};

	enum Position
	{
		kStartAtBeginning,	// absolute
		kStartAtEnd,		// absolute
		kStartBefore,		// relative to given index
		kStartAfter,		// relative to given index
	};

	enum Action
	{
		kStay = 0,
		kMove = 1
	};

public:

	// for all objects

	static const JUtf8Byte* kItemsInserted;
	static const JUtf8Byte* kItemsRemoved;

	// for JBroadcasters

	static const JUtf8Byte* kItemMoved;
	static const JUtf8Byte* kItemsSwapped;
	static const JUtf8Byte* kItemsChanged;
	static const JUtf8Byte* kSorted;

	// for iterators

	static const JUtf8Byte* kGoingAway;
	static const JUtf8Byte* kCopied;

public:

	// base class for JBroadcaster messages

	class ItemMessage : public JBroadcaster::Message
	{
	public:

		ItemMessage(const JUtf8Byte* type,
					const JIndex firstIndex, const JSize count)
			:
			JBroadcaster::Message(type),
			itsFirstIndex(firstIndex),
			itsCount(count)
			{ };

		JIndex
		GetFirstIndex() const
		{
			return itsFirstIndex;
		};

		JIndex
		GetLastIndex() const
		{
			return itsFirstIndex + itsCount-1;
		};

		JSize
		GetCount() const
		{
			return itsCount;
		};

		bool
		Contains(const JIndex index) const
		{
			return itsFirstIndex <= index && index <= GetLastIndex();
		};

	private:

		JIndex	itsFirstIndex;
		JSize	itsCount;
	};

public:

	// for all objects

	class ItemsInserted : public ItemMessage
	{
	public:

		ItemsInserted(const JIndex firstIndex, const JSize count)
			:
			ItemMessage(kItemsInserted, firstIndex, count)
			{ };

		void	AdjustIndex(JIndex* index) const;
	};

	class ItemsRemoved : public ItemMessage
	{
	public:

		ItemsRemoved(const JIndex firstIndex, const JSize count)
			:
			ItemMessage(kItemsRemoved, firstIndex, count)
			{ };

		bool	AdjustIndex(JIndex* index) const;
	};

	// for JBroadcasters

	class ItemMoved : public JBroadcaster::Message
	{
	public:

		ItemMoved(const JIndex origIndex, const JIndex newIndex)
			:
			JBroadcaster::Message(kItemMoved),
			itsOrigIndex(origIndex),
			itsNewIndex(newIndex)
			{ };

		void	AdjustIndex(JIndex* index) const;

		JIndex
		GetOrigIndex() const
		{
			return itsOrigIndex;
		};

		JIndex
		GetNewIndex() const
		{
			return itsNewIndex;
		};

	private:

		JIndex	itsOrigIndex;
		JIndex	itsNewIndex;
	};

	class ItemsSwapped : public JBroadcaster::Message
	{
	public:

		ItemsSwapped(const JIndex index1, const JIndex index2)
			:
			JBroadcaster::Message(kItemsSwapped),
			itsIndex1(index1),
			itsIndex2(index2)
			{ };

		void	AdjustIndex(JIndex* index) const;

		JIndex
		GetIndex1() const
		{
			return itsIndex1;
		};

		JIndex
		GetIndex2() const
		{
			return itsIndex2;
		};

	private:

		JIndex	itsIndex1;
		JIndex	itsIndex2;
	};

	class ItemsChanged : public ItemMessage
	{
	public:

		ItemsChanged(const JIndex firstIndex, const JSize count)
			:
			ItemMessage(kItemsChanged, firstIndex, count)
			{ };
	};

	class Sorted : public JBroadcaster::Message
	{
	public:

		Sorted()
			:
			JBroadcaster::Message(kSorted)
			{ };
	};

	// for iterators

	class GoingAway : public JBroadcaster::Message
	{
	public:

		GoingAway()
			:
			JBroadcaster::Message(kGoingAway)
			{ };
	};

	class Copied : public JBroadcaster::Message
	{
	public:

		Copied()
			:
			JBroadcaster::Message(kCopied)
			{ };
	};
};

#include "JListIterator.h"

template <class T>
class JList : public JCollection
{
	friend class JListIterator<T>;

public:

	JList();
	JList(const JList<T>& source);

	~JList() override;

	virtual T	GetFirstItem() const = 0;
	virtual T	GetLastItem() const = 0;

	virtual void	PrependItem(const T& data) = 0;
	virtual void	AppendItem(const T& data) = 0;

	virtual void	RemoveAll() = 0;

	virtual JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) = 0;
	virtual JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) const = 0;

	// sorting functions -- virtual so they can be optimized for particular storage methods

	void	SetCompareFunction(const std::function<std::weak_ordering(const T&, const T&)> compareFn);
	void	CopyCompareFunction(const JList<T>& source);
	void	ClearCompareFunction();

	JListT::SortOrder	GetSortOrder() const;
	void				SetSortOrder(const JListT::SortOrder order);

	bool	IsSorted() const;

protected:

	// asserts that the result is not nullptr
	std::function<std::weak_ordering(const T&, const T&)>*	GetCompareFunction() const;

	void	ListAssigned(const JList<T>& source);
	void	NotifyIterators(const JBroadcaster::Message& message);

private:

	JListT::SortOrder										itsSortOrder;
	std::function<std::weak_ordering(const T&, const T&)>*	itsCompareFn;	// can be nullptr

	JListIterator<T>*	itsFirstIterator;	// linked list of active iterators

private:

	// must be overridden

	JList<T>& operator=(const JList<T>& source);
};

#endif
