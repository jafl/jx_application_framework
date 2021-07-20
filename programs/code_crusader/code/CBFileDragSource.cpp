/******************************************************************************
 CBFileDragSource.cpp

	Displays a file icon that the user can drag.

	BASE CLASS = JXImageWidget

	Copyright © 2001 by John Lindal.

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
#include <JXImageCache.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>

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
	SetImage(GetDisplay()->GetImageCache()->GetImage(jx_plain_file_small), false);

	ProvideDirectSave(nullptr);
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
	if ((itsNameInput == nullptr && !itsDoc->ExistsOnDisk()) ||
		(itsNameInput != nullptr &&
		 itsNameInput->GetText()->GetText().EndsWith(ACE_DIRECTORY_SEPARATOR_STR)))
		{
		DisplayCursor(kJXInactiveCursor);
		SetHint(JGetString("kFileSelected::JXFSDirMenu"));
		}
	else
		{
		if (itsNameInput != nullptr && itsNameInput->GetText()->IsEmpty())
			{
			SetHint(JGetString("HowToXDS2HintID::CBFileDragSource"));
			}
		else if (itsNameInput != nullptr)
			{
			SetHint(JGetString("XDSHint::CBFileDragSource"));
			}
		else
			{
			SetHint(JGetString("XDNDHint::CBFileDragSource"));
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
	bool onDisk;
	JString fileName = itsDoc->GetFullName(&onDisk);
	if (clickCount == 2 && onDisk)
		{
		(JXGetWebBrowser())->ShowFileLocation(fileName);
		}
	else if (itsNameInput == nullptr && onDisk)
		{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);
		list.Append(&fileName);

		auto* data = jnew JXFileSelection(GetDisplay(), list);
		assert( data != nullptr );

		BeginDND(pt, buttonStates, modifiers, data);
		}
	else if (itsNameInput != nullptr &&
			 !itsNameInput->GetText()->GetText().EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
		{
		auto* task = jnew CBDSSFinishSaveTask(itsDoc);
		assert( task != nullptr );

		auto* data = jnew JXDSSSelection(GetWindow(), task);
		assert( data != nullptr );

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
	if (itsNameInput != nullptr && !itsNameInput->GetText()->IsEmpty())
		{
		JString fullName = itsNameInput->GetText()->GetText();
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
	if (itsNameInput != nullptr)
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
	const bool		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}
