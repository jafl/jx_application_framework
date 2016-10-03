/******************************************************************************
 GMMessageDragSource.cc

	Displays a mailbox icon that the user can drag to the Mailbox window
	and thereby transfer.

	BASE CLASS = JXImageWidget

	Copyright (C) 2000 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GMMessageDragSource.h>
#include "GMessageDragData.h"
#include "GMessageViewDir.h"
#include "GMessageTableDir.h"
#include "GMessageHeader.h"

#include "envelope-front.xpm"

#include <JXDisplay.h>
#include <JXDNDManager.h>
#include <JXDSSFinishSaveTask.h>
#include <JXImage.h>
#include <JXInputField.h>
#include <JXSelectionManager.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <jXUtil.h>

#include <jDirUtil.h>
#include <jAssert.h>

static const JCharacter* kHintText =
	"Drag this to the Mailbox window to transfer the message.";

static const JCharacter* kDragMessagesXAtomName = "GMailMessages";
static const JCharacter* kDNDActionCopyDescrip  = "copy the message";
static const JCharacter* kDNDActionMoveDescrip  = "move the message";

static const JCharacter* kDNDClassID			= "GMMessageDragSource";

/******************************************************************************
 Constructor

 ******************************************************************************/

GMMessageDragSource::GMMessageDragSource
	(
	GMessageViewDir*	dir,
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
	itsDir	= dir;

	JXImage* icon = new JXImage(GetDisplay(), envelope_front);
	assert( icon != NULL );
	icon->ConvertToRemoteStorage();
	SetImage(icon, kJTrue);

	SetHint(kHintText);

	SetBorderWidth(0);

	// targets for DND

	itsMessageXAtom	= GetDisplay()->RegisterXAtom(kDragMessagesXAtomName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMMessageDragSource::~GMMessageDragSource()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GMMessageDragSource::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	GMessageDragData* data =
		new GMessageDragData(this, kDNDClassID);
	assert(data != NULL);
	BeginDND(pt, buttonStates, modifiers, data);
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
GMMessageDragSource::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (modifiers.control())
		{
		return GetDNDManager()->GetDNDActionAskXAtom();
		}
	else if (modifiers.meta())
		{
		return GetDNDManager()->GetDNDActionCopyXAtom();
		}
	else
		{
		return GetDNDManager()->GetDNDActionMoveXAtom();
		}
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
GMMessageDragSource::GetDNDAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JArray<Atom>*			askActionList,
	JPtrArray<JString>*		askDescriptionList
	)
{
	JXDNDManager* dndMgr = GetDNDManager();
	askActionList->AppendElement(dndMgr->GetDNDActionCopyXAtom());
	askActionList->AppendElement(dndMgr->GetDNDActionMoveXAtom());

	JString* s = new JString(kDNDActionCopyDescrip);
	assert( s != NULL );
	askDescriptionList->Append(s);

	s = new JString(kDNDActionMoveDescrip);
	assert( s != NULL );
	askDescriptionList->Append(s);
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
GMMessageDragSource::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	if (strcmp(id, kDNDClassID) == 0)
		{
		GMessageDragData* messageData =
			dynamic_cast<GMessageDragData*>(data);
		assert(messageData != NULL);

		messageData->SetDirector(itsDir->GetDir());
		JPtrArray<GMessageHeader> headers(JPtrArrayT::kForgetAll);

		GMessageHeader* header = itsDir->GetMessageHeader();
		headers.Append(header);
		messageData->SetHeaders(&headers);
		}
	else
		{
		JXImageWidget::GetSelectionData(data, id);
		}
}
