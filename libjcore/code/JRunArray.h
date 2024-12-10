/******************************************************************************
 JRunArray.h

	Interface for JRunArray class

	Copyright (C) 1994-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRunArray
#define _H_JRunArray

#include "JArray.h"
#include "JRunArrayIterator.h"
#include "JRange.h"

template <class T>
class JRunArrayItem
{
public:

	JSize	length;
	T		data;

	JRunArrayItem()
		:
		length( 0 )
		{ };

	JRunArrayItem
		(
		const JSize	runLength,
		const T&	runData
		)
		:
		length( runLength ),
		data( runData )
		{ };
};

template <class T>
class JRunArray : public JList<T>
{
	friend class JRunArrayIterator<T>;

public:

	JRunArray(const JSize minSize = 0);
	JRunArray(const JRunArray<T>& source);
	JRunArray(const JRunArray<T>& source, const JIndexRange& range);

	~JRunArray() override;

	JRunArray<T>& operator=(const JRunArray<T>& source);

	T	GetFirstItem() const override;
	T	GetLastItem() const override;

	void	PrependItem(const T& data) override;
	void	AppendItem(const T& data) override;

	void	PrependItems(const T& data, const JSize count);
	void	AppendItems(const T& data, const JSize count);

	void	PrependSlice(const JRunArray<T>& source, const JIndexRange& range);
	void	AppendSlice(const JRunArray<T>& source, const JIndexRange& range);

	void	RemoveAll() override;

	bool		AllItemsEqual(T* data = nullptr) const;
	JInteger	SumItems(const JIndex startIndex, const JIndex endIndex,
							JInteger (*value)(const T& data)) const;
	bool		FindPositiveSum(const JInteger requestedSum, const JIndex startIndex,
								JIndex* endIndex, JInteger* trueSum,
								JInteger (*value)(const T& data)) const;

	JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) override;
	JListIterator<T>*
		NewIterator(const JListT::Position start = JListT::kStartAtBeginning,
					const JIndex index = 0) const override;

	JSize	GetRunCount() const;

	JSize	GetMinLgSize() const;
	void	SetMinLgSize(const JSize lgSize);
	void	SetMinSize(const JSize count);

private:

	JArray< JRunArrayItem<T> >*	itsRuns;

private:

	JSize		GetRunLength(const JIndex runIndex) const;
	const T&	GetRunData(const JIndex runIndex) const;
	void		SetRunData(const JIndex runIndex, const T& data);
	bool		FindRun(const JIndex itemIndex,
						JIndex* runIndex, JIndex* firstIndexInRun) const;
	bool		IncrementalFindRun(const JIndex origIndex, const JIndex newIndex,
								   JIndex* runIndex, JIndex* firstIndexInRun) const;

	void	InsertRun(const JIndex runIndex, const JSize runLength, const T& item);
	void	RemoveRun(const JIndex runIndex,
					  JIndex* newRunIndex = nullptr, JIndex* newFirstInRun = nullptr);

	void	SetRunLength(const JIndex runIndex, const JSize newLength);
	void	IncrementRunLength(const JIndex runIndex, const JSize delta);
	void	DecrementRunLength(const JIndex runIndex, const JSize delta);

	void	IteratorInsertItemsAtIndex(const JIndex itemIndex,
									   const T& data, const JSize newElementCount,
									   JIndex* runIndex, JIndex* firstIndexInRun);
	void	IteratorInsertSlice(const JIndex itemIndex,
								const JRunArray<T>& source, const JIndexRange& range,
								JIndex* runIndex, JIndex* firstIndexInRun);
	void	IteratorSetItems(const JIndex itemIndex, const T& data, const JSize count,
							 JIndex* runIndex, JIndex* firstIndexInRun);
	void	IteratorRemoveItems(const JIndex firstIndex, const JSize count,
								JIndex* runIndex, JIndex* firstIndexInRun);

	JIndex	PrivateInsertItemsAtIndex(const JIndex itemIndex,
									  const T& data, const JSize newElementCount,
									  JIndex* runIndex, JIndex* firstIndexInRun);
	bool	PrivateSetItem(const JIndex itemIndex, const T& data,
						   JIndex* runIndex, JIndex* firstIndexInRun);
	void	PrivateRemoveItems(const JIndex firstIndex, const JSize count,
							   JIndex* runIndex, JIndex* firstIndexInRun);
};

#include "JRunArray.tmpl"
#include "JRunArrayIterator.tmpl"

#endif
