/******************************************************************************
 CMCommandOutputDisplay.cpp

	BASE CLASS = CMTextDisplayBase

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

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
	CMTextDisplayBase(kFullEditor, false, menuBar, scrollbarSet,
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
	if (!GetText()->IsEmpty())
		{
		SetCaretLocation(GetText()->GetText().GetCharacterCount()+1);
		if (GetText()->GetText().GetLastCharacter() != '\n')
			{
			Paste(JString::newline);
			}
		}
}
