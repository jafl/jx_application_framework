/******************************************************************************
 SyGFolderDragSource.cpp

	Displays a folder icon that the user can drag and also JXCurrentPathMenu.

	BASE CLASS = JXImageWidget

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "SyGFolderDragSource.h"
#include "SyGFileTreeTable.h"
#include "SyGGlobals.h"
#include <JXFileSelection.h>
#include <JXDNDManager.h>
#include <JXSelectionManager.h>
#include <JXDisplay.h>
#include <JXPathInput.h>
#include <JXCurrentPathMenu.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <jMouseUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_folder_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFolderDragSource::SyGFolderDragSource
	(
	JXPathInput*		pathInput,
	JXCurrentPathMenu**	pathMenu,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXImageWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsPathInput(pathInput)
{
	SetImage(GetDisplay()->GetImageCache()->GetImage(jx_folder_small), false);

	SetHint(JGetString("Hint::SyGFolderDragSource"));

	itsPathMenu = jnew JXCurrentPathMenu(JGetRootDirectory(), this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsPathMenu != nullptr );
	itsPathMenu->Hide();
	itsPathMenu->SetToHiddenPopupMenu(true);
	*pathMenu = itsPathMenu;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFolderDragSource::~SyGFolderDragSource()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
SyGFolderDragSource::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsStartPt       = pt;
	itsMouseDownTime = JXGetApplication()->GetCurrentTime();
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
SyGFolderDragSource::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JString path;
	const bool pathValid = itsPathInput->GetPath(&path);

	if (pathValid && !JMouseMoved(itsStartPt, pt) &&
		JXGetApplication()->GetCurrentTime() >= itsMouseDownTime + kJXDoubleClickTime)
		{
		itsPathMenu->SetPath(path);
		itsPathMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else if (pathValid && JMouseMoved(itsStartPt, pt))
		{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);
		list.Append(&const_cast<JString&>(path));

		auto* data = jnew JXFileSelection(GetDisplay(), list);
		assert( data != nullptr );

		BeginDND(pt, buttonStates, modifiers, data);
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
SyGFolderDragSource::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JString path;
	const bool pathValid = itsPathInput->GetPath(&path);
	assert( pathValid );	// checked before BeginDND()

	return SyGFileTreeTable::GetDNDAction(this, path, target, modifiers);
}

/******************************************************************************
 GetDNDAskActions (virtual protected)

	This is called when the value returned by GetDropAction() changes to
	XdndActionAsk.  If GetDropAction() repeatedly returns XdndActionAsk,
	this function is not called again because it is assumed that the
	actions are the same within a single DND session.

	This function must place at least 2 elements in askActionList and
	askDescriptionList.

	The first element should be the default action.

 ******************************************************************************/

void
SyGFolderDragSource::GetDNDAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JArray<Atom>*			askActionList,
	JPtrArray<JString>*		askDescriptionList
	)
{
	SyGGetDNDAskActions(askActionList, askDescriptionList);
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
SyGFolderDragSource::HandleDNDResponse
	(
	const JXContainer*	target,
	const bool		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDDirectoryCursor(dropAccepted, action));
}
