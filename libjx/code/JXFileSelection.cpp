/******************************************************************************
 JXFileSelection.cpp

	text/url

	BASE CLASS = JXSelectionData

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFileSelection.h>
#include <JXDisplay.h>
#include <jXUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFileSelection::JXFileSelection
	(
	JXDisplay*					display,
	const JPtrArray<JString>&	list
	)
	:
	JXSelectionData(display)
{
	JXFileSelectionX();
	SetData(list);
}

JXFileSelection::JXFileSelection
	(
	JXDisplay*			display,
	JPtrArray<JString>*	list
	)
	:
	JXSelectionData(display)
{
	JXFileSelectionX();
	SetData(list);
}

JXFileSelection::JXFileSelection
	(
	JXWidget*			widget,
	const JCharacter*	id
	)
	:
	JXSelectionData(widget, id)
{
	JXFileSelectionX();
}

// private

void
JXFileSelection::JXFileSelectionX()
{
	itsList   = NULL;
	itsBuffer = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileSelection::~JXFileSelection()
{
	delete itsList;
	delete itsBuffer;
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
JXFileSelection::AddTypes
	(
	const Atom selectionName
	)
{
	AddType((GetSelectionManager())->GetURLXAtom());
}

/******************************************************************************
 SetData

	Makes a copy of the list.

 ******************************************************************************/

void
JXFileSelection::SetData
	(
	const JPtrArray<JString>& list
	)
{
	const JSize count = list.GetElementCount();
	if (itsList == NULL)
		{
		itsList = new JPtrArray<JString>(list, JPtrArrayT::kDeleteAll, kJTrue);
		assert( itsList != NULL );
		}
	else
		{
		itsList->CopyObjects(list, JPtrArrayT::kDeleteAll, kJFalse);
		}

	delete itsBuffer;
	itsBuffer = NULL;
}

/******************************************************************************
 SetData

	Takes ownership of the list and its contents.

 ******************************************************************************/

void
JXFileSelection::SetData
	(
	JPtrArray<JString>* list
	)
{
	delete itsList;

	itsList = list;
	itsList->SetCleanUpAction(JPtrArrayT::kDeleteAll);

	delete itsBuffer;
	itsBuffer = NULL;
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
JXFileSelection::ConvertData
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

	const Atom urlAtom = (GetSelectionManager())->GetURLXAtom();

	if (requestType == urlAtom && itsList != NULL && !itsList->IsEmpty())
		{
		CreateBuffer();

		*returnType = urlAtom;
		*dataLength = itsBuffer->GetLength();
		*data = new unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			memcpy(*data, *itsBuffer, *dataLength);
			return kJTrue;
			}
		}

	*data       = NULL;
	*dataLength = 0;
	*returnType = None;
	return kJFalse;
}

/******************************************************************************
 CreateBuffer (private)

 ******************************************************************************/

void
JXFileSelection::CreateBuffer()
	const
{
	if (itsBuffer == NULL)
		{
		(const_cast<JXFileSelection*>(this))->itsBuffer =
			new JString(JXPackFileNames(*itsList));
		assert( itsBuffer != NULL );
		}
}

/******************************************************************************
 GetFileList (static)

 ******************************************************************************/

JBoolean
JXFileSelection::GetFileList
	(
	const Atom			selectionName,
	const Time			time,
	JXDisplay*			display,
	JPtrArray<JString>*	fileNameList,
	JPtrArray<JString>*	urlList
	)
{
	return GetFileList(display->GetSelectionManager(), selectionName, time,
					   fileNameList, urlList);
}

JBoolean
JXFileSelection::GetFileList
	(
	JXSelectionManager* selMgr,
	const Atom			selectionName,
	const Time			time,
	JPtrArray<JString>*	fileNameList,
	JPtrArray<JString>*	urlList
	)
{
	const Atom urlXAtom = selMgr->GetURLXAtom();

	JBoolean ok = kJFalse;

	JArray<Atom> typeList;
	if (selMgr->GetAvailableTypes(selectionName, CurrentTime, &typeList))
		{
		const JSize count = typeList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (typeList.GetElement(i) == urlXAtom)
				{
				Atom returnType;
				unsigned char* data;
				JSize dataLength;
				JXSelectionManager::DeleteMethod delMethod;
				if (selMgr->GetData(selectionName, time, urlXAtom,
									&returnType, &data, &dataLength, &delMethod))
					{
					if (returnType == urlXAtom)
						{
						JXUnpackFileNames((char*) data, dataLength, fileNameList, urlList);
						ok = kJTrue;
						}

					selMgr->DeleteData(&data, delMethod);
					}

				break;		// found type we were looking for
				}
			}
		}

	return ok;
}
