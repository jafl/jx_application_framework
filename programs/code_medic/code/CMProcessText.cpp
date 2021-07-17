/******************************************************************************
 CMProcessText.cpp

	BASE CLASS = public JXStaticText

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "CMProcessText.h"
#include "CMChooseProcessDialog.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMProcessText::CMProcessText
	(
	CMChooseProcessDialog*	dir,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXStaticText(JString::empty, true, true, true, scrollbarSet,
				 enclosure, hSizing, vSizing, x,y, w, h),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMProcessText::~CMProcessText()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMProcessText::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXStaticText::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
	JString text;
	if (clickCount == 2 && GetSelection(&text))
		{
		JInteger value;
		if (text.ConvertToInteger(&value))
			{
			itsDir->SetProcessID(value);
			itsDir->EndDialog(true);
			}
		}
}
