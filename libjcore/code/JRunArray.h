/******************************************************************************
 JRunArray.h

	Interface for JRunArray class

	Copyright (C) 1994-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRunArray
#define _H_JRunArray

#include "jx-af/jcore/JArray.h"
#include "jx-af/jcore/JRunArrayIterator.h"
#include "jx-af/jcore/JRange.h"

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

	JRunArray(const JSize blockSize = 10);
	JRunArray(const JRunArray<T>& source);
	JRunArray(const JRunArray<T>& source, const JIndexRange& range);

	virtual ~JRunArray();

	const JRunArray<T>& operator=(const JRunArray<T>& source);

	virtual T	GetFirstElement() const override;
	virtual T	GetLastElement() const override;

	virtual void	PrependElement(const T& data) override;
	virtual void	AppendElement(const T& data) override;

	void	PrependElements(const T& data, const JSize count);
	void	AppendElements(const T& data, const JSize count);

	void	PrependSlice(const JRunArray<T>& source, const JIndexRange& range);
	void	AppendSlice(const JRunArray<T>& source, const JIndexRange& range);

	virtual void	RemoveAll() override;

	bool		AllElementsEqual(T* data = nullptr) const;
	JInteger	SumElements(const JIndex startIndex, const JIndex endIndex,
							JInteger (*value)(const T& data)) const;
	bool		FindPositiveSum(const JInteger requestedSum, const JIndex startIndex,
								JIndex* endIndex, JInteger* trueSum,
								JInteger (*value)(const T& data)) const;

	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) override;
	virtual JListIterator<T>*
		NewIterator(const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0) const override;

	JSize	GetRunCount() const;

	JSize	GetBlockSize() const;
	void	SetBlockSize(const JSize newBlockSize);

private:

	JArray< JRunArrayElement<T> >*	itsRuns;

private:

	JSize		GetRunLength(const JIndex runIndex) const;
	const T&	GetRunData(const JIndex runIndex) const;
	void		SetRunData(const JIndex runIndex, const T& data);
	bool		FindRun(const JIndex elementIndex,
						JIndex* runIndex, JIndex* firstIndexInRun) const;
	bool		IncrementalFindRun(const JIndex origIndex, const JIndex newIndex,
								   JIndex* runIndex, JIndex* firstIndexInRun) const;

	void	InsertRun(const JIndex runIndex, const JSize runLength, const T& item);
	void	RemoveRun(const JIndex runIndex,
					  JIndex* newRunIndex = nullptr, JIndex* newFirstInRun = nullptr);

	void	SetRunLength(const JIndex runIndex, const JSize newLength);
	void	IncrementRunLength(const JIndex runIndex, const JSize delta);
	void	DecrementRunLength(const JIndex runIndex, const JSize delta);

	void	IteratorInsertElementsAtIndex(const JIndex elementIndex,
										  const T& data, const JSize newElementCount,
										  JIndex* runIndex, JIndex* firstIndexInRun);
	void	IteratorInsertSlice(const JIndex elementIndex,
								const JRunArray<T>& source, const JIndexRange& range,
								JIndex* runIndex, JIndex* firstIndexInRun);
	void	IteratorSetElements(const JIndex elementIndex, const T& data, const JSize count,
								JIndex* runIndex, JIndex* firstIndexInRun);
	void	IteratorRemoveElements(const JIndex firstIndex, const JSize count,
								   JIndex* runIndex, JIndex* firstIndexInRun);

	JIndex	PrivateInsertElementsAtIndex(const JIndex elementIndex,
										 const T& data, const JSize newElementCount,
										 JIndex* runIndex, JIndex* firstIndexInRun);
	bool	PrivateSetElement(const JIndex elementIndex, const T& data,
							  JIndex* runIndex, JIndex* firstIndexInRun);
	void	PrivateRemoveElements(const JIndex firstIndex, const JSize count,
								  JIndex* runIndex, JIndex* firstIndexInRun);
};

#include "JRunArray.tmpl"
#include "JRunArrayIterator.tmpl"

#endif
