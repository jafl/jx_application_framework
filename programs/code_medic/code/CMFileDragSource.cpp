/******************************************************************************
 CMFileDragSource.cpp

	Displays a file icon that the user can drag.

	BASE CLASS = JXImageWidget

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMFileDragSource.h"
#include "CMSourceDirector.h"
#include <JXFileSelection.h>
#include <JXDNDManager.h>
#include <JXSelectionManager.h>
#include <JXWebBrowser.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <jXGlobals.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMFileDragSource::CMFileDragSource
	(
	CMSourceDirector*	doc,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXImageWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsDoc(doc)
{
	SetImage(GetDisplay()->GetImageCache()->GetImage(jx_plain_file_small), false);

	SetHint(JGetString("Hint::CMFileDragSource"));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMFileDragSource::~CMFileDragSource()
{
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
CMFileDragSource::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	const JString* fileName;
	if (!itsDoc->GetFileName(&fileName))
		{
		DisplayCursor(kJXInactiveCursor);
		}
	else
		{
		JXImageWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMFileDragSource::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const JString* fileName;
	const bool hasFile = itsDoc->GetFileName(&fileName);
	if (hasFile && clickCount == 2)
		{
		(JXGetWebBrowser())->ShowFileLocation(*fileName);
		}
	else if (hasFile)
		{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);
		list.Append(const_cast<JString*>(fileName));

		auto* data = jnew JXFileSelection(GetDisplay(), list);
		assert( data != nullptr );

		BeginDND(pt, buttonStates, modifiers, data);
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
CMFileDragSource::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
CMFileDragSource::HandleDNDResponse
	(
	const JXContainer*	target,
	const bool		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}
