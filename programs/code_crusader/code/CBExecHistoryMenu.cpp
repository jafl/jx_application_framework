/******************************************************************************
 CBExecHistoryMenu.cpp

	Maintains a fixed-length list of executable commands.

	BASE CLASS = JXStringHistoryMenu

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBExecHistoryMenu.h"
#include <JXImage.h>
#include <jAssert.h>

#include <jx_executable_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBExecHistoryMenu::CBExecHistoryMenu
	(
	const JSize			historyLength,
	const JCharacter*	title,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStringHistoryMenu(historyLength, title, enclosure, hSizing, vSizing, x,y, w,h)
{
	CBExecHistoryMenuX();
}

CBExecHistoryMenu::CBExecHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXStringHistoryMenu(historyLength, owner, itemIndex, enclosure)
{
	CBExecHistoryMenuX();
}

// private

void
CBExecHistoryMenu::CBExecHistoryMenuX()
{
	JXImage* icon = jnew JXImage(GetDisplay(), jx_executable_small);
	assert( icon != NULL );
	SetDefaultIcon(icon, kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBExecHistoryMenu::~CBExecHistoryMenu()
{
}
