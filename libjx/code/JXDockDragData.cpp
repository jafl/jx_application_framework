/******************************************************************************
 JXDockDragData.cpp

	For dragging a window to a dock.

	BASE CLASS = JXSelectionData

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include "JXDockDragData.h"
#include "JXDockManager.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockDragData::JXDockDragData
	(
	JXWindow* window
	)
	:
	JXSelectionData(window->GetDisplay()),
	itsWindow(window)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockDragData::~JXDockDragData()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
JXDockDragData::AddTypes
	(
	const Atom selectionName
	)
{
	AddType((JXGetDockManager())->GetDNDMinSizeAtom());
	AddType((JXGetDockManager())->GetDNDWindowAtom());
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
JXDockDragData::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	if (requestType == (JXGetDockManager())->GetDNDMinSizeAtom())
		{
		*returnType   = XA_POINT;
		*bitsPerBlock = 16;
		*dataLength   = sizeof(XPoint);
		*data         = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
			{
			const JPoint minSize = itsWindow->GetMinSize();
			XPoint* xpt          = (XPoint*) *data;
			xpt->x               = JMin(minSize.x, (JCoordinate) SHRT_MAX);
			xpt->y               = JMin(minSize.y, (JCoordinate) SHRT_MAX);
			return kJTrue;
			}
		}

	else if (requestType == (JXGetDockManager())->GetDNDWindowAtom())
		{
		*returnType   = XA_WINDOW;
		*bitsPerBlock = 32;
		*dataLength   = sizeof(Window);
		*data         = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
			{
			*((Window*) *data) = itsWindow->GetXWindow();
			return kJTrue;
			}
		}

	*bitsPerBlock = 8;
	*data         = nullptr;
	*dataLength   = 0;
	*returnType   = None;
	return kJFalse;
}
