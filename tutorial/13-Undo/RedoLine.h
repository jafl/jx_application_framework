/******************************************************************************
 RedoLine.h

	Interface for the RedoLine class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_RedoLine
#define _H_RedoLine

#include <JUndo.h>
#include <JPoint.h>

class UndoWidget;

class RedoLine : public JUndo
{
public:

	RedoLine(UndoWidget* uw, const JPoint& start,
				const JPoint& end);

	virtual ~RedoLine();

	virtual void	Undo();

private:

	UndoWidget*	itsWidget;
	JPoint		itsStartPt;
	JPoint		itsEndPt;
};

#endif
