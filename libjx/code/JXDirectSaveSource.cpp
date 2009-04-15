/******************************************************************************
 JXDirectSaveSource.cpp

	Displays a file icon that the user can drag to a file manager
	and thereby save.

	BASE CLASS = JXImageWidget

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDirectSaveSource.h>
#include <JXDSSSelection.h>
#include <JXDSSFinishSaveTask.h>
#include <JXDNDManager.h>
#include <JXSelectionManager.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXInputField.h>
#include <JXImage.h>
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>

// string ID's

static const JCharacter* kHintID = "Hint::JXDirectSaveSource";

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

	JXImage* icon = new JXImage(GetDisplay(), GetColormap(), jx_plain_file_small);
	assert( icon != NULL );
	icon->ConvertToRemoteStorage();
	SetImage(icon, kJTrue);

	SetHint(JGetString(kHintID));
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
	JXDSSFinishSaveTask* task = new JXDSSFinishSaveTask(itsDialog);
	assert( task != NULL );

	JXDSSSelection* data = new JXDSSSelection(GetWindow(), task);
	assert( data != NULL );

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
	Init(GetWindow(), itsNameInput->GetText());
}

/******************************************************************************
 Init (static)

 ******************************************************************************/

void
JXDirectSaveSource::Init
	(
	JXWindow*			window,
	const JCharacter*	fileName
	)
{
	JXDisplay* d = window->GetDisplay();
	XChangeProperty(*d, window->GetXWindow(),
					(d->GetDNDManager())->GetDNDDirectSave0XAtom(),
					(d->GetSelectionManager())->GetMimePlainTextXAtom(), 8,
					PropModeReplace,
					(unsigned char*) fileName, strlen(fileName));
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
	return (GetDNDManager())->GetDNDActionDirectSaveXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
JXDirectSaveSource::HandleDNDResponse
	(
	const JXContainer*	target,
	const JBoolean		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor((GetDNDManager())->GetDNDFileCursor(dropAccepted, action));
}
