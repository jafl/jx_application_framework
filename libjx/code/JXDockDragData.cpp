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
#include <jx-af/jcore/jAssert.h>

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
	AddType(JXGetDockManager()->GetDNDMinSizeAtom());
	AddType(JXGetDockManager()->GetDNDWindowAtom());
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
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
	if (requestType == JXGetDockManager()->GetDNDMinSizeAtom())
	{
		*returnType   = XA_POINT;
		*bitsPerBlock = 16;
		*dataLength   = sizeof(XPoint);
		*data         = jnew_allow_null unsigned char[ *dataLength ];
		if (*data != nullptr)
		{
			const JPoint minSize = itsWindow->GetMinSize();
			auto* xpt            = (XPoint*) *data;
			xpt->x               = JMin(minSize.x, SHRT_MAX);
			xpt->y               = JMin(minSize.y, SHRT_MAX);
			return true;
		}
	}

	else if (requestType == JXGetDockManager()->GetDNDWindowAtom())
	{
		*returnType   = XA_WINDOW;
		*bitsPerBlock = 32;
		*dataLength   = sizeof(Window);
		*data         = jnew_allow_null unsigned char[ *dataLength ];
		if (*data != nullptr)
		{
			*((Window*) *data) = itsWindow->GetXWindow();
			return true;
		}
	}

	*bitsPerBlock = 8;
	*data         = nullptr;
	*dataLength   = 0;
	*returnType   = None;
	return false;
}
