/******************************************************************************
 JTableSelectionIterator.h

	Interface for JTableSelectionIterator class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTableSelectionIterator
#define _H_JTableSelectionIterator

#include "JBroadcaster.h"
#include "JRect.h"

class JTableSelection;

class JTableSelectionIterator : virtual public JBroadcaster
{
public:

	enum Direction
	{
		kIterateByRow,
		kIterateByCol
	};

public:

	JTableSelectionIterator(const JTableSelection* tableSelection,
							const Direction d = kIterateByCol,
							const JIteratorPosition start = kJIteratorStartAtBeginning,
							const JIndex row = 0, const JIndex col = 0);
	JTableSelectionIterator(const JTableSelectionIterator& source);

	virtual ~JTableSelectionIterator();

	Direction	GetDirection() const;
	void		SetDirection(const Direction d);

	bool	Prev(JPoint* cell);
	bool	Prev(JIndex* row, JIndex* col);

	bool	Next(JPoint* cell);
	bool	Next(JIndex* row, JIndex* col);

	void	MoveTo(const JIteratorPosition newPosition, const JPoint& cell);
	void	MoveTo(const JIteratorPosition newPosition,
				   const JIndex row, const JIndex col);

	bool	AtBeginning();
	bool	AtEnd();

protected:

	const JTableSelection*	GetTableSelection() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	const JTableSelection*	itsTableSelection;	// JTableSelection that is being iterated over
	Direction				itsDirection;		// By row or by column
	JPoint					itsCursor;			// Current iterator position
	bool				itsAtEndFlag;

private:

	bool	NextCell();
	bool	PrevCell();

	// not allowed

	JTableSelectionIterator& operator=(const JTableSelectionIterator&) = delete;
};


/******************************************************************************
 Iteration direction

 ******************************************************************************/

inline JTableSelectionIterator::Direction
JTableSelectionIterator::GetDirection()
	const
{
	return itsDirection;
}

inline void
JTableSelectionIterator::SetDirection
	(
	const Direction d
	)
{
	itsDirection = d;
}

/******************************************************************************
 MoveTo

 ******************************************************************************/

inline void
JTableSelectionIterator::MoveTo
	(
	const JIteratorPosition	newPosition,
	const JIndex			row,
	const JIndex			col
	)
{
	MoveTo(newPosition, JPoint(col,row));
}

/******************************************************************************
 GetTableSelection (protected)

 ******************************************************************************/

inline const JTableSelection*
JTableSelectionIterator::GetTableSelection()
	const
{
	return itsTableSelection;
}

#endif
