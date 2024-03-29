/******************************************************************************
 JXSelectionManager_Win32.cpp

	Global object to interface with X Selection mechanism.

	Nobody else seems to support MULTIPLE, so I see no reason to support
	it here.  If we ever need to support it, here is the basic idea:

		Pass in filled JArray<Atom>* and empty JArray<char*>*
		We remove the unconverted types from JArray<Atom> and
			fill in the JArray<char*> with the converted data

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWidget.h>
#include <jXGlobals.h>
#include <jTime.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <X11/Xlib.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSelectionManager::JXSelectionManager
	(
	JXDisplay* display
	)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSelectionManager::~JXSelectionManager()
{
}

/******************************************************************************
 GetAvailableTypes

	time can be CurrentTime.

 ******************************************************************************/

bool
JXSelectionManager::GetAvailableTypes
	(
	const Atom		selectionName,
	const Time		time,
	JArray<Atom>*	typeList
	)
{
	return false;
}

/******************************************************************************
 GetData

	time can be CurrentTime.

	*** Caller is responsible for calling DeleteData() on the data
		that is returned.

 ******************************************************************************/

bool
JXSelectionManager::GetData
	(
	const Atom		selectionName,
	const Time		time,
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	DeleteMethod*	delMethod
	)
{
	return false;
}

/******************************************************************************
 DeleteData

	This must be called with all data returned by GetData().
	The DeleteMethod must be the method returned by GetData().

 ******************************************************************************/

void
JXSelectionManager::DeleteData
	(
	unsigned char**		data,
	const DeleteMethod	delMethod
	)
{
}

/******************************************************************************
 SendDeleteRequest

	Implements the DELETE selection protocol.

	window should be any one that X can attach the data to.
	Widgets can simply pass in the result from GetWindow().

	time can be CurrentTime.

 ******************************************************************************/

void
JXSelectionManager::SendDeleteRequest
	(
	const Atom	selectionName,
	const Time	time
	)
{
}

/******************************************************************************
 HandleSelectionRequest

 ******************************************************************************/

void
JXSelectionManager::HandleSelectionRequest
	(
	const XSelectionRequestEvent& selReqEvent
	)
{
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

	This catches errors while sending and receiving data.

 ******************************************************************************/

void
JXSelectionManager::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	JBroadcaster::ReceiveWithFeedback(sender, message);
}

/******************************************************************************
 SetData

	Set the data for the given selection.

	*** 'data' must be allocated on the heap.
		We take ownership of 'data' even if the function returns false.

 ******************************************************************************/

bool
JXSelectionManager::SetData
	(
	const Atom			selectionName,
	JXSelectionData*	data
	)
{
	return false;
}

/******************************************************************************
 ClearData

	time can be CurrentTime.

	We have to pass endTime to GetData() because sometimes we have to
	rewrite the history book:  We called SetData() while we thought we owned
	the selection, but there was a SelectionClear event on its way, which
	means that, in reality, we lost ownership for a short while.

 ******************************************************************************/

void
JXSelectionManager::ClearData
	(
	const Atom	selectionName,
	const Time	endTime
	)
{
}

/******************************************************************************
 GetData

	time can be CurrentTime.

	In the private version, if index != nullptr, it contains the index into itsDataList.

 ******************************************************************************/

bool
JXSelectionManager::GetData
	(
	const Atom				selectionName,
	const Time				time,
	const JXSelectionData**	data
	)
{
	return false;
}
