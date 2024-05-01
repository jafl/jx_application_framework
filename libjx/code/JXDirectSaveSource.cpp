/******************************************************************************
 JXDirectSaveSource.cpp

	Displays a file icon that the user can drag to a file manager
	and thereby save.

	BASE CLASS = JXImageWidget

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXDirectSaveSource.h"
#include "JXDSSSelection.h"
#include "JXDSSFinishSaveTask.h"
#include "JXDNDManager.h"
#include "JXSelectionManager.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXInputField.h"
#include "JXImageCache.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

#include "jx_plain_file_small.xpm"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDirectSaveSource::JXDirectSaveSource
	(
	JXSaveFileDialog*	dialog,
	JXInputField*		nameInput,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXImageWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsDialog    = dialog;
	itsNameInput = nameInput;

	SetImage(GetDisplay()->GetImageCache()->GetImage(jx_plain_file_small), false);
	SetHint(JGetString("Hint::JXDirectSaveSource"));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDirectSaveSource::~JXDirectSaveSource()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXDirectSaveSource::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	auto* task = jnew JXDSSFinishSaveTask(itsDialog);
	auto* data = jnew JXDSSSelection(GetWindow(), task);

	BeginDND(pt, buttonStates, modifiers, data);
}

/******************************************************************************
 DNDInit (virtual protected)

 ******************************************************************************/

void
JXDirectSaveSource::DNDInit
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	Init(GetWindow(), itsNameInput->GetText()->GetText());
}

/******************************************************************************
 Init (static)

 ******************************************************************************/

void
JXDirectSaveSource::Init
	(
	JXWindow*		window,
	const JString&	fileName
	)
{
	JXDisplay* d = window->GetDisplay();
	XChangeProperty(*d, window->GetXWindow(),
					(d->GetDNDManager())->GetDNDDirectSave0XAtom(),
					(d->GetSelectionManager())->GetMimePlainTextXAtom(), 8,
					PropModeReplace,
					(unsigned char*) fileName.GetRawBytes(),
					fileName.GetByteCount());
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
JXDirectSaveSource::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionDirectSaveXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
JXDirectSaveSource::HandleDNDResponse
	(
	const JXContainer*	target,
	const bool		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}
