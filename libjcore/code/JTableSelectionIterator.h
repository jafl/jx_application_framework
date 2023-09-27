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

	enum Position
	{
		kStartAtBeginning,	// absolute
		kStartAtEnd,		// absolute
		kStartBefore,		// relative to given index
		kStartAfter,		// relative to given index
	};

public:

	JTableSelectionIterator(const JTableSelection* tableSelection,
							const Direction d = kIterateByCol,
							const Position start = kStartAtBeginning,
							const JIndex row = 0, const JIndex col = 0);
	JTableSelectionIterator(const JTableSelectionIterator& source);

	~JTableSelectionIterator() override;

	Direction	GetDirection() const;
	void		SetDirection(const Direction d);

	bool	Prev(JPoint* cell);
	bool	Prev(JIndex* row, JIndex* col);

	bool	Next(JPoint* cell);
	bool	Next(JIndex* row, JIndex* col);

	void	MoveTo(const Position newPosition, const JPoint& cell);
	void	MoveTo(const Position newPosition,
				   const JIndex row, const JIndex col);

	bool	AtBeginning() const;
	bool	AtEnd() const;

protected:

	const JTableSelection*	GetTableSelection() const;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	const JTableSelection*	itsTableSelection;	// JTableSelection that is being iterated over
	Direction				itsDirection;		// By row or by column
	JPoint					itsCursor;			// Current iterator position
	bool					itsAtEndFlag;

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
	const Position	newPosition,
	const JIndex	row,
	const JIndex	col
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
