/******************************************************************************
 CMCommandOutputDisplay.cpp

	BASE CLASS = CMTextDisplayBase

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMCommandOutputDisplay.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMCommandOutputDisplay::CMCommandOutputDisplay
	(
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CMTextDisplayBase(kFullEditor, kJFalse, menuBar, scrollbarSet,
					  enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMCommandOutputDisplay::~CMCommandOutputDisplay()
{
}

/******************************************************************************
 PlaceCursorAtEnd

 ******************************************************************************/

void
CMCommandOutputDisplay::PlaceCursorAtEnd()
{
	if (!IsEmpty())
		{
		SetCaretLocation(GetTextLength()+1);
		if (GetText().GetLastCharacter() != '\n')
			{
			Paste("\n");
			}
		}
}
