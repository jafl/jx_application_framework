/******************************************************************************
 JList.h

	Copyright (C) 1994-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JList
#define _H_JList

#include "JCollection.h"
#include "JListIterator.h"
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

	enum CompareResult
	{
		kFirstLessSecond,
		kFirstEqualSecond,
		kFirstGreaterSecond
	};

	enum SearchReturn
	{
		kFirstMatch,
		kLastMatch,
		kAnyMatch
	};

public:

	// for all objects

	static const JUtf8Byte* kElementsInserted;
	static const JUtf8Byte* kElementsRemoved;

	// for JBroadcasters

	static const JUtf8Byte* kElementMoved;
	static const JUtf8Byte* kElementsSwapped;
	static const JUtf8Byte* kElementsChanged;
	static const JUtf8Byte* kSorted;

	// for iterators

	static const JUtf8Byte* kGoingAway;
	static const JUtf8Byte* kCopied;

private:

	// base class for JBroadcaster messages

	class ElementMessage : public JBroadcaster::Message
		{
		public:

			ElementMessage(const JUtf8Byte* type,
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

	class ElementsInserted : public ElementMessage
		{
		public:

			ElementsInserted(const JIndex firstIndex, const JSize count)
				:
				ElementMessage(kElementsInserted, firstIndex, count)
				{ };

			void	AdjustIndex(JIndex* index) const;
		};

	class ElementsRemoved : public ElementMessage
		{
		public:

			ElementsRemoved(const JIndex firstIndex, const JSize count)
				:
				ElementMessage(kElementsRemoved, firstIndex, count)
				{ };

			bool	AdjustIndex(JIndex* index) const;
		};

	// for JBroadcasters

	class ElementMoved : public JBroadcaster::Message
		{
		public:

			ElementMoved(const JIndex origIndex, const JIndex newIndex)
				:
				JBroadcaster::Message(kElementMoved),
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

	class ElementsSwapped : public JBroadcaster::Message
		{
		public:

			ElementsSwapped(const JIndex index1, const JIndex index2)
				:
				JBroadcaster::Message(kElementsSwapped),
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

	class ElementsChanged : public ElementMessage
		{
		public:

			ElementsChanged(const JIndex firstIndex, const JSize count)
				:
				ElementMessage(kElementsChanged, firstIndex, count)
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


template <class T>
class JList : public JCollection
{
	friend class JListIterator<T>;

public:

	JList();
	JList(const JList<T>& source);

	~JList() override;

	virtual T	GetFirstElement() const = 0;
	virtual T	GetLastElement() const = 0;

	virtual void	PrependElement(const T& data) = 0;
	virtual void	AppendElement(const T& data) = 0;

	virtual void	RemoveAll() = 0;

	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) = 0;
	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) const = 0;

	// sorting functions -- virtual so they can be optimized for particular storage methods

	void	SetCompareFunction(const std::function<JListT::CompareResult(const T&, const T&)> compareFn);
	void	CopyCompareFunction(const JList<T>& source);
	void	ClearCompareFunction();

	JListT::SortOrder	GetSortOrder() const;
	void				SetSortOrder(const JListT::SortOrder order);

	bool	IsSorted() const;

protected:

	// asserts that the result is not nullptr
	std::function<JListT::CompareResult(const T&, const T&)>*	GetCompareFunction() const;

	void	ListAssigned(const JList<T>& source);
	void	NotifyIterators(const JBroadcaster::Message& message);

private:

	JListT::SortOrder											itsSortOrder;
	std::function<JListT::CompareResult(const T&, const T&)>*	itsCompareFn;	// can be nullptr

	JListIterator<T>*		itsFirstIterator;	// linked list of active iterators

private:

	// must be overridden

	JList<T>& operator=(const JList<T>& source);
};

#include "JList.tmpl"
#include "JListIterator.tmpl"

#endif
