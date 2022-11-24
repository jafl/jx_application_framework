/******************************************************************************
 RedoLine.h

	Interface for the RedoLine class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_RedoLine
#define _H_RedoLine

#include <jx-af/jcore/JUndo.h>
#include <jx-af/jcore/JPoint.h>

class UndoWidget;

class RedoLine : public JUndo
{
public:

	RedoLine(UndoWidget* uw, const JPoint& start,
				const JPoint& end);

	virtual void	Undo();

private:

	UndoWidget*	itsWidget;
	JPoint		itsStartPt;
	JPoint		itsEndPt;
};

#endif
