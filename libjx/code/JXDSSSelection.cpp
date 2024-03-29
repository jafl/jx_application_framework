/******************************************************************************
 JXDSSSelection.cpp

	XdndDirectSave0
	http://johnlindal.wixsite.com/xdsave

	BASE CLASS = JXSelectionData

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXDSSSelection.h"
#include "JXDirectSaveAction.h"
#include "JXDNDManager.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "jXUtil.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDSSSelection::JXDSSSelection
	(
	JXWindow*			window,
	JXDirectSaveAction*	action
	)
	:
	JXSelectionData(window->GetDisplay()),
	itsWindow(window),
	itsAction(action)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDSSSelection::~JXDSSSelection()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
JXDSSSelection::AddTypes
	(
	const Atom selectionName
	)
{
	AddType(GetDNDManager()->GetDNDDirectSave0XAtom());
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
JXDSSSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	*bitsPerBlock = 8;

	JXSelectionManager* selMgr = GetSelectionManager();
	JXDNDManager* dndMgr       = GetDNDManager();

	if (requestType == dndMgr->GetDNDDirectSave0XAtom())
	{
		// get URL

		Atom actualType;
		int actualFormat;
		unsigned long itemCount, remainingBytes;
		unsigned char* rawData = nullptr;
		XGetWindowProperty(*(GetDisplay()), itsWindow->GetXWindow(),
						   dndMgr->GetDNDDirectSave0XAtom(),
						   0, LONG_MAX, True, selMgr->GetMimePlainTextXAtom(),
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &rawData);

		if (actualType == selMgr->GetMimePlainTextXAtom() &&
			actualFormat == 8 && itemCount > 0)
		{
			JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
							   urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) rawData, itemCount, &fileNameList, &urlList);

			if (itsAction != nullptr && !fileNameList.IsEmpty())
			{
				itsAction->Save(*(fileNameList.GetFirstItem()));
			}
			else if (itsAction != nullptr)
			{
				jdelete itsAction;
				JGetUserNotification()->ReportError(JGetString("MustSaveSameOnComputer::JXDSSSelection"));
			}
			const_cast<JXDSSSelection*>(this)->itsAction = nullptr;
		}

		XFree(rawData);

		// respond with error -- terminate session now that we have the file name

		*returnType = XA_STRING;
		*dataLength = 1;
		*data = jnew_allow_null unsigned char[ *dataLength ];
		if (*data != nullptr)
		{
			**data = 0x45;	// E
			return true;
		}
	}

	*data       = nullptr;
	*dataLength = 0;
	*returnType = None;
	return false;
}
