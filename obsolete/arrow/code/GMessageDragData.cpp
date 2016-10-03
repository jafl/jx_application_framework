/******************************************************************************
 GMessageDragData.cc

	BASE CLASS = public JXSelectionData

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMessageDragData.h>
#include "GMessageTableDir.h"
#include "GMessageHeader.h"
#include "GMMailboxData.h"
#include <sstream>
#include <jAssert.h>

static const JCharacter* kDragMessagesXAtomName = "GMailMessages";

/******************************************************************************
 Constructor

 *****************************************************************************/

GMessageDragData::GMessageDragData
	(
	JXWidget*			widget,
	const JCharacter*	id
	)
	:
	JXSelectionData(widget, id)
{
	itsHeaders = new JPtrArray<GMessageHeader>(JPtrArrayT::kForgetAll);
	assert(itsHeaders != NULL);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMessageDragData::~GMessageDragData()
{
	delete itsHeaders;
}

/******************************************************************************
 SetDirector (public)

 ******************************************************************************/

void
GMessageDragData::SetDirector
	(
	GMessageTableDir* dir
	)
{
	itsDir = dir;
}

/******************************************************************************
 SetHeaders (public)

 ******************************************************************************/

void
GMessageDragData::SetHeaders
	(
	JPtrArray<GMessageHeader>* headers
	)
{
	const JSize count	= headers->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsHeaders->Append(headers->NthElement(i));
		}
}


/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
GMessageDragData::AddTypes
	(
	const Atom selectionName
	)
{
	itsMessageXAtom	= AddType(kDragMessagesXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
GMessageDragData::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char** data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	JXSelectionManager* selMgr = GetSelectionManager();
	*bitsPerBlock = 8;

	if (requestType == itsMessageXAtom)
		{
		std::ostringstream os;

		os << itsDir->GetData()->GetMailFile();

		const JSize count = itsHeaders->GetElementCount();
		os << count << ' ';
		for (JIndex i = 1; i <= count; i++)
			{
			GMessageHeader* header = itsHeaders->NthElement(i);
			os << *header;
			}

		const std::string s = os.str();

		*returnType = itsMessageXAtom;
		*dataLength = s.length();
		*data       = new unsigned char[ *dataLength ];
		assert (*data != NULL);
		memcpy(*data, s.data(), *dataLength);
		return kJTrue;
		}
	else if (requestType == selMgr->GetDeleteSelectionXAtom())
		{
		const JSize count = itsHeaders->GetElementCount();
		for (JIndex i = 1; i <= count; i++)
			{
			GMessageHeader* header = itsHeaders->NthElement(i);
			itsDir->GetData()->ChangeMessageStatus(header, GMessageHeader::kDelete);
			}

		*data       = new unsigned char[1];
		*dataLength = 0;
		*returnType = selMgr->GetNULLXAtom();
		return kJTrue;
		}
	return kJFalse;
}
