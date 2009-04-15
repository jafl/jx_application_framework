/******************************************************************************
 RedoLine.h

	Interface for the RedoLine class.

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_RedoLine
#define _H_RedoLine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

private:

	// not allowed

	RedoLine(const RedoLine& source);
	const RedoLine& operator=(const RedoLine& source);
};

#endif
