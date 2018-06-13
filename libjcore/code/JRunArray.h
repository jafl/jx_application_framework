/******************************************************************************
 JRunArray.h

	Interface for JRunArray class

	Copyright (C) 1994-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRunArray
#define _H_JRunArray

#include <JArray.h>
#include <JRunArrayIterator.h>
#include <JIndexRange.h>

template <class T>
class JRunArrayElement
{
public:

	JSize	length;
	T		data;

	JRunArrayElement()
		:
		length( 0 )
		{ };

	JRunArrayElement
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

	JRunArray();
	JRunArray(const JRunArray<T>& source);
	JRunArray(const JRunArray<T>& source, const JIndexRange& range);
	JRunArray(const JRunArray<T>& source,
			  const JIndex startIndex, const JIndex endIndex);

	virtual ~JRunArray();

	const JRunArray<T>& operator=(const JRunArray<T>& source);

	virtual void	InsertElementAtIndex(const JIndex index, const T& data) override;
	void			InsertElementsAtIndex(const JIndex elementIndex,
										  const T& data, const JSize newElementCount);
	void			PrependElements(const T& data, const JSize newElementCount);
	void			AppendElements(const T& data, const JSize newElementCount);

	void			InsertElementsAtIndex(const JIndex insertionIndex,
										  const JRunArray<T>& source,
										  const JIndex firstIndex, const JSize count);

	virtual void	RemoveNextElements(const JIndex firstIndex, const JSize count) override;
	virtual void	RemoveAll() override;

	virtual T		GetElement(const JIndex index) const override;
	virtual void	SetElement(const JIndex index, const T& data) override;
	void			SetNextElements(const JIndex firstIndex, const JSize count, const T& data);

	virtual void	MoveElementToIndex(const JIndex currentIndex, const JIndex newIndex) override;
	virtual void	SwapElements(const JIndex index1, const JIndex index2) override;

	virtual void	Sort() override;
	virtual JIndex	SearchSorted1(const T& target,
								  const JListT::SearchReturn which,
								  JBoolean* found) const override;

	void	QuickSort(int (*compare)(const void*, const void*));

	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) override;
	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) const override;

	void	SetBlockSize(const JSize newBlockSize);

	// specific to JRunArray

	JSize		GetRunCount() const;
	JSize		GetRunLength(const JIndex runIndex) const;
	T			GetRunData(const JIndex runIndex) const;
	const T&	GetRunDataRef(const JIndex runIndex) const;
	void		SetRunData(const JIndex runIndex, const T& data);
	JBoolean	FindRun(const JIndex elementIndex,
						JIndex* runIndex, JIndex* firstIndexInRun) const;

	JInteger	SumElements(const JIndex startIndex, const JIndex endIndex,
							JInteger (*value)(const T& data)) const;
	JBoolean	FindPositiveSum(const JInteger requestedSum, const JIndex startIndex,
								JIndex* endIndex, JInteger* trueSum,
								JInteger (*value)(const T& data)) const;

	void	InsertSlice(const JIndex index, const JRunArray<T>& source,
						const JIndexRange& range);
	void	InsertSlice(const JIndex index, const JRunArray<T>& source,
						const JIndex startIndex, const JIndex endIndex);
	void	PrependSlice(const JRunArray<T>& source,
						 const JIndexRange& range);
	void	PrependSlice(const JRunArray<T>& source,
						 const JIndex startIndex, const JIndex endIndex);
	void	AppendSlice(const JRunArray<T>& source,
						const JIndexRange& range);
	void	AppendSlice(const JRunArray<T>& source,
						const JIndex startIndex, const JIndex endIndex);

	// use with extreme caution

	void		InsertElementsAtIndex(const JIndex elementIndex,
									  const T& data, const JSize newElementCount,
									  JIndex* runIndex, JIndex* firstIndexInRun);
	void		RemoveNextElements(const JIndex firstIndex, const JSize count,
								   JIndex* runIndex, JIndex* firstIndexInRun);
	void		SetNextElements(const JIndex firstIndex, const JSize count, const T& data,
								JIndex* runIndex, JIndex* firstIndexInRun);
	JBoolean	FindRun(const JIndex origIndex, const JIndex newIndex,
						JIndex* runIndex, JIndex* firstIndexInRun) const;

private:

	JArray< JRunArrayElement<T> >*	itsRuns;	// JArray object that stores the data

	static const JElementComparison<T>*	itsCurrentCompareObj;	// nullptr unless sorting

private:

	JIndex	PrivateInsertElementsAtIndex(const JIndex elementIndex,
										 const T& data, const JSize newElementCount);
	JIndex	PrivateInsertElementsAtIndex(const JIndex elementIndex,
										 const T& data, const JSize newElementCount,
										 JIndex* runIndex, JIndex* firstIndexInRun);

	void	PrivateRemoveNextElements(const JIndex firstIndex, const JSize count);
	void	PrivateRemoveNextElements(const JIndex firstIndex, const JSize count,
									  JIndex* runIndex, JIndex* firstIndexInRun);

	JBoolean	PrivateSetElement(const JIndex elementIndex, const T& data);
	JBoolean	PrivateSetElement(const JIndex elementIndex, const T& data,
								  JIndex* runIndex, JIndex* firstIndexInRun);

	void	IteratorSetElement(const JIndex elementIndex, const T& data,
							   const JIndex runIndex, const JIndex firstIndexInRun);
	void	IteratorRemoveNextElements(const JIndex firstIndex, const JSize count,
									   const JIndex runIndex, const JIndex firstIndexInRun);

	void	InsertRun(const JIndex runIndex, const JSize runLength, const T& item);
	void	RemoveRun(const JIndex runIndex,
					  JIndex* newRunIndex = nullptr, JIndex* newFirstInRun = nullptr);

	void	SetRunLength(const JIndex runIndex, const JSize newLength);
	void	IncrementRunLength(const JIndex runIndex, const JSize delta);
	void	DecrementRunLength(const JIndex runIndex, const JSize delta);

	void	SetRunInfo(const JIndex runIndex, const JSize newLength,
					   const T& data);

	void	MergeAdjacentRuns();

	static JListT::CompareResult
		CompareRuns(const JRunArrayElement<T>& r1, const JRunArrayElement<T>& r2);

	static int	QuickSortCompareRuns(const void* p1, const void* p2);
};

#include <JRunArray.tmpl>
#include <JRunArrayIterator.tmpl>

#endif
