/******************************************************************************
 UndoLine.h

	Interface for the UndoLine class.

	Copyright © 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoLine
#define _H_UndoLine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUndo.h>

class UndoWidget;

class UndoLine : public JUndo
{
public:

	UndoLine(UndoWidget* uw);

	virtual ~UndoLine();

	virtual void	Undo();

private:

	UndoWidget*	itsWidget;

private:

	// not allowed

	UndoLine(const UndoLine& source);
	const UndoLine& operator=(const UndoLine& source);
};

#endif
