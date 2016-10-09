/******************************************************************************
 CBFileDragSource.cpp

	Displays a file icon that the user can drag.

	BASE CLASS = JXImageWidget

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBFileDragSource.h"
#include "CBTextDocument.h"
#include "CBDSSFinishSaveTask.h"
#include <JXFileSelection.h>
#include <JXDSSSelection.h>
#include <JXDirectSaveSource.h>
#include <JXDNDManager.h>
#include <JXSelectionManager.h>
#include <JXInputField.h>
#include <JXWebBrowser.h>
#include <JXImage.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>

static const JCharacter* kXDNDHintID      = "XDNDHint::CBFileDragSource";
static const JCharacter* kXDSHintID       = "XDSHint::CBFileDragSource";
static const JCharacter* kHowToXDS1HintID = "HowToXDS1HintID::CBFileDragSource";
static const JCharacter* kHowToXDS2HintID = "HowToXDS2HintID::CBFileDragSource";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileDragSource::CBFileDragSource
	(
	CBTextDocument*		doc,
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
	itsDoc(doc)
{
	JXImage* icon = jnew JXImage(GetDisplay(), jx_plain_file_small);
	assert( icon != NULL );
	SetImage(icon, kJTrue);

	ProvideDirectSave(NULL);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFileDragSource::~CBFileDragSource()
{
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
CBFileDragSource::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if ((itsNameInput == NULL && !itsDoc->ExistsOnDisk()) ||
		(itsNameInput != NULL &&
		 (itsNameInput->GetText()).EndsWith(ACE_DIRECTORY_SEPARATOR_STR)))
		{
		DisplayCursor(kJXInactiveCursor);
		SetHint(JGetString(kHowToXDS1HintID));
		}
	else
		{
		if (itsNameInput != NULL && itsNameInput->IsEmpty())
			{
			SetHint(JGetString(kHowToXDS2HintID));
			}
		else if (itsNameInput != NULL)
			{
			SetHint(JGetString(kXDSHintID));
			}
		else
			{
			SetHint(JGetString(kXDNDHintID));
			}

		JXImageWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBFileDragSource::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JBoolean onDisk;
	JString fileName = itsDoc->GetFullName(&onDisk);
	if (clickCount == 2 && onDisk)
		{
		(JXGetWebBrowser())->ShowFileLocation(fileName);
		}
	else if (itsNameInput == NULL && onDisk)
		{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);
		list.Append(&fileName);

		JXFileSelection* data = jnew JXFileSelection(GetDisplay(), list);
		assert( data != NULL );

		BeginDND(pt, buttonStates, modifiers, data);
		}
	else if (itsNameInput != NULL &&
			 !(itsNameInput->GetText()).EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
		{
		CBDSSFinishSaveTask* task = jnew CBDSSFinishSaveTask(itsDoc);
		assert( task != NULL );

		JXDSSSelection* data = jnew JXDSSSelection(GetWindow(), task);
		assert( data != NULL );

		BeginDND(pt, buttonStates, modifiers, data);
		}
}

/******************************************************************************
 DNDInit (virtual protected)

 ******************************************************************************/

void
CBFileDragSource::DNDInit
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsNameInput != NULL && !itsNameInput->IsEmpty())
		{
		JString fullName = itsNameInput->GetText();
		JStripTrailingDirSeparator(&fullName);		// paranoia -- checked earlier

		JString path, name;
		JSplitPathAndName(fullName, &path, &name);

		JXDirectSaveSource::Init(GetWindow(), name);
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
CBFileDragSource::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsNameInput != NULL)
		{
		return GetDNDManager()->GetDNDActionDirectSaveXAtom();
		}
	else
		{
		return GetDNDManager()->GetDNDActionCopyXAtom();
		}
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
CBFileDragSource::HandleDNDResponse
	(
	const JXContainer*	target,
	const JBoolean		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}
