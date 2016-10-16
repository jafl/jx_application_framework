/******************************************************************************
 JOrderedSet.h

	Interface for JOrderedSet class

	Copyright (C) 1994-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JOrderedSet
#define _H_JOrderedSet

#include <JCollection.h>
#include <JOrderedSetIterator.h>

// namespace to own JBroadcaster messages, etc (template is unnecessary)

class JOrderedSetT
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
	static const JUtf8Byte* kElementChanged;
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

			JBoolean
			Contains(const JIndex index) const
			{
				return JConvertToBoolean(
						GetFirstIndex() <= index && index <= GetLastIndex() );
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

			JBoolean	AdjustIndex(JIndex* index) const;
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

	class ElementChanged : public ElementMessage
		{
		public:

			ElementChanged(const JIndex index)
				:
				ElementMessage(kElementChanged, index, 1)
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


// pure virtual template classes

template <class T>
class JElementComparison
{
public:

	virtual ~JElementComparison();

	virtual JOrderedSetT::CompareResult	Compare(const T&, const T&) const = 0;
	virtual JElementComparison<T>*		Copy() const = 0;
};

template <class T>
class JCompareFnWrapper : public JElementComparison<T>
{
public:

	JCompareFnWrapper(JOrderedSetT::CompareResult (*compareFn)(const T&, const T&));
	virtual ~JCompareFnWrapper();

	virtual JOrderedSetT::CompareResult	Compare(const T&, const T&) const;
	virtual JElementComparison<T>*		Copy() const;

private:

	JOrderedSetT::CompareResult	(*itsCompareFn)(const T&, const T&);
};

template <class T>
class JOrderedSet : public JCollection
{
	friend class JOrderedSetIterator<T>;

public:

	JOrderedSet();
	JOrderedSet(const JOrderedSet<T>& source);

	virtual ~JOrderedSet();

	virtual void	InsertElementAtIndex(const JIndex index, const T& data) = 0;

	void	PrependElement(const T& data);
	void	AppendElement(const T& data);

	void			RemoveElement(const JIndex index);
	virtual void	RemoveNextElements(const JIndex firstIndex, const JSize count) = 0;
	void			RemovePrevElements(const JIndex lastIndex, const JSize count);
	void			RemoveElements(const JOrderedSetT::ElementsRemoved& info);
	virtual void	RemoveAll() = 0;	// separate so derived classes can optimize

	virtual T		GetElement(const JIndex index) const = 0;
	virtual void	SetElement(const JIndex index, const T& data) = 0;

	T		GetElementFromEnd(const JIndex index) const;
	void	SetElementFromEnd(const JIndex index, const T& data);

	T	GetFirstElement() const;
	T	GetLastElement() const;

	// the second version has a different name to avoid shadowing

	virtual void	MoveElementToIndex(const JIndex currentIndex,
									   const JIndex newIndex) = 0;
	void			MoveElementToIndexWithMsg(const JOrderedSetT::ElementMoved& info);

	virtual void	SwapElements(const JIndex index1, const JIndex index2) = 0;
	void			SwapElementsWithMsg(const JOrderedSetT::ElementsSwapped& info);

	virtual JOrderedSetIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0);
	virtual JOrderedSetIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) const;

	// sorting functions -- virtual so they can be optimized for particular storage methods

	JBoolean	GetCompareFunction(JOrderedSetT::CompareResult (**compareFn)(const T&, const T&)) const;
	void		SetCompareFunction(JOrderedSetT::CompareResult (*compareFn)(const T&, const T&));

	JBoolean	GetCompareObject(const JElementComparison<T>** compareObj) const;
	void		SetCompareObject(const JElementComparison<T>& compareObj);
	void		ClearCompareObject();

	JOrderedSetT::SortOrder	GetSortOrder() const;
	void					SetSortOrder(const JOrderedSetT::SortOrder order);

	JBoolean		IsSorted() const;
	virtual void	Sort();

	JBoolean	InsertSorted(const T& data, const JBoolean insertIfDuplicate = kJTrue,
							 JIndex* index = NULL);
	JIndex		GetInsertionSortIndex(const T& data, JBoolean* isDuplicate = NULL) const;

	JBoolean	SearchSorted(const T& target, const JOrderedSetT::SearchReturn which,
							 JIndex* index) const;

	virtual JIndex	SearchSorted1(const T& target,
								  const JOrderedSetT::SearchReturn which,
								  JBoolean* found) const;

protected:

	void	OrderedSetAssigned(const JOrderedSet<T>& source);
	void	NotifyIterators(const JBroadcaster::Message& message);

private:

	JOrderedSetT::CompareResult	(*itsCompareFn)(const T&, const T&);	// can be NULL
	JElementComparison<T>*		itsCompareObj;							// can be NULL

	JOrderedSetT::SortOrder		itsSortOrder;
	JOrderedSetIterator<T>*		itsFirstIterator;	// linked list of active iterators

private:

	// must be overridden

	const JOrderedSet<T>& operator=(const JOrderedSet<T>& source);
};

#include <JOrderedSet.tmpl>
#include <JOrderedSetIterator.tmpl>

#endif
