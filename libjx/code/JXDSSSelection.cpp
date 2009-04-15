/******************************************************************************
 JXDSSSelection.cpp

	XdndDirectSave0
	http://www.newplanetsoftware.com/xds/

	BASE CLASS = JXSelectionData

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDSSSelection.h>
#include <JXDirectSaveAction.h>
#include <JXDNDManager.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <jXUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

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
	AddType((GetDNDManager())->GetDNDDirectSave0XAtom());
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
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
		unsigned char* rawData = NULL;
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

			if (itsAction != NULL && !fileNameList.IsEmpty())
				{
				itsAction->Save(*(fileNameList.FirstElement()));
				}
			else if (itsAction != NULL)
				{
				delete itsAction;
				(JGetUserNotification())->ReportError(
					"You cannot save the file on a different computer.");
				}
			const_cast<JXDSSSelection*>(this)->itsAction = NULL;
			}

		XFree(rawData);

		// respond with error -- terminate session now that we have the file name

		*returnType = XA_STRING;
		*dataLength = 1;
		*data = new unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			**data = 0x45;	// E
			return kJTrue;
			}
		}

	*data       = NULL;
	*dataLength = 0;
	*returnType = None;
	return kJFalse;
}
