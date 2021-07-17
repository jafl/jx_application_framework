/******************************************************************************
 CBExecHistoryMenu.cpp

	Maintains a fixed-length list of executable commands.

	BASE CLASS = JXStringHistoryMenu

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBExecHistoryMenu.h"
#include <JXDisplay.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <jAssert.h>

#include <jx_executable_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBExecHistoryMenu::CBExecHistoryMenu
	(
	const JSize			historyLength,
	const JString&		title,
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
	SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jx_executable_small), false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBExecHistoryMenu::~CBExecHistoryMenu()
{
}
