/******************************************************************************
 UndoLine.h

	Interface for the UndoLine class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoLine
#define _H_UndoLine

#include <JUndo.h>

class UndoWidget;

class UndoLine : public JUndo
{
public:

	UndoLine(UndoWidget* uw);

	void	Undo() override;

private:

	UndoWidget*	itsWidget;
};

#endif
