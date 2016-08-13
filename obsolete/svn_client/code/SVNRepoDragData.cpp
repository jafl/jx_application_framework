/******************************************************************************
 SVNRepoDragData.cpp

	For dragging a window to a dock.

	BASE CLASS = JXSelectionData

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SVNRepoDragData.h"
#include <JXDisplay.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNRepoDragData::SVNRepoDragData
	(
	JXDisplay*			display,
	const Atom			type,
	const JCharacter*	uri
	)
	:
	JXSelectionData(display),
	itsType(type),
	itsURI(uri)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNRepoDragData::~SVNRepoDragData()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
SVNRepoDragData::AddTypes
	(
	const Atom selectionName
	)
{
	AddType(itsType);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
SVNRepoDragData::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	if (requestType == itsType)
		{
		*returnType   = XA_STRING;
		*bitsPerBlock = 8;
		*dataLength   = itsURI.GetLength();
		*data         = new unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			memcpy(*data, itsURI.GetCString(), *dataLength);
			return kJTrue;
			}
		}

	*bitsPerBlock = 8;
	*data         = NULL;
	*dataLength   = 0;
	*returnType   = None;
	return kJFalse;
}
