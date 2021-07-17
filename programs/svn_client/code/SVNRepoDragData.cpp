/******************************************************************************
 SVNRepoDragData.cpp

	For dragging a window to a dock.

	BASE CLASS = JXSelectionData

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include "SVNRepoDragData.h"
#include <JXDisplay.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNRepoDragData::SVNRepoDragData
	(
	JXDisplay*		display,
	const Atom		type,
	const JString&	uri
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

bool
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
		*dataLength   = itsURI.GetByteCount();
		*data         = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
			{
			memcpy(*data, itsURI.GetRawBytes(), *dataLength);
			return true;
			}
		}

	*bitsPerBlock = 8;
	*data         = nullptr;
	*dataLength   = 0;
	*returnType   = None;
	return false;
}
