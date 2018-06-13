/******************************************************************************
 JXFileSelection.cpp

	text/url

	BASE CLASS = JXSelectionData

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

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
	const JUtf8Byte*	id
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
	itsList   = nullptr;
	itsBuffer = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileSelection::~JXFileSelection()
{
	jdelete itsList;
	jdelete itsBuffer;
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
	AddType(GetSelectionManager()->GetURLXAtom());
	AddType(GetSelectionManager()->GetURLNoCharsetXAtom());
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
	if (itsList == nullptr)
		{
		itsList = jnew JDCCPtrArray<JString>(list, JPtrArrayT::kDeleteAll);
		assert( itsList != nullptr );
		}
	else
		{
		itsList->CopyObjects(list, JPtrArrayT::kDeleteAll, kJFalse);
		}

	jdelete itsBuffer;
	itsBuffer = nullptr;
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
	jdelete itsList;

	itsList = list;
	itsList->SetCleanUpAction(JPtrArrayT::kDeleteAll);

	jdelete itsBuffer;
	itsBuffer = nullptr;
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

	if ((requestType == GetSelectionManager()->GetURLXAtom() ||
		 requestType == GetSelectionManager()->GetURLNoCharsetXAtom()) &&
		itsList != nullptr && !itsList->IsEmpty())
		{
		CreateBuffer();

		*returnType = requestType;
		*dataLength = itsBuffer->GetByteCount();
		*data       = (unsigned char*) itsBuffer->AllocateBytes();
		return kJTrue;
		}

	*data       = nullptr;
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
	if (itsBuffer == nullptr)
		{
		(const_cast<JXFileSelection*>(this))->itsBuffer =
			jnew JString(JXPackFileNames(*itsList));
		assert( itsBuffer != nullptr );
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
	const Atom urlXAtom1 = selMgr->GetURLXAtom(),
			   urlXAtom2 = selMgr->GetURLNoCharsetXAtom();

	JBoolean ok = kJFalse;

	JArray<Atom> typeList;
	if (selMgr->GetAvailableTypes(selectionName, CurrentTime, &typeList))
		{
		for (const Atom type : typeList)
			{
			if (type == urlXAtom1 || type == urlXAtom2)
				{
				Atom returnType;
				unsigned char* data;
				JSize dataLength;
				JXSelectionManager::DeleteMethod delMethod;
				if (selMgr->GetData(selectionName, time, type,
									&returnType, &data, &dataLength, &delMethod))
					{
					if (returnType == type)
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
