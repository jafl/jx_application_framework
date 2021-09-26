/******************************************************************************
 JXFontNameMenuTable.cpp

	BASE CLASS = JXTextMenuTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXFontNameMenuTable.h"
#include "jx-af/jx/JXFontNameMenu.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFontNameMenuTable::JXFontNameMenuTable
	(
	JXFontNameMenu*		menu,
	JXTextMenuData*		data,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenuTable(menu, data, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsMenu = menu;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFontNameMenuTable::~JXFontNameMenuTable()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXFontNameMenuTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (!c.IsAlnum())
	{
		JXTextMenuTable::HandleKeyPress(c, keySym, modifiers);
		return;
	}

	const JString s1(c);
	JString s2;

	const JSize count = itsMenu->GetItemCount();
	for (JIndex i=itsMenu->GetHistoryCount()+1; i<=count; i++)
	{
		s2.Set(itsMenu->GetItemText(i).GetFirstCharacter());
		if (JString::Compare(s1, s2, JString::kIgnoreCase) == 0)
		{
			ScrollTo(GetCellRect(JPoint(1,i-1)).topLeft());
			break;
		}
	}
}
