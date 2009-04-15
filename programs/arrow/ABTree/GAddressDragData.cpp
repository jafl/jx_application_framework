/******************************************************************************
 GAddressDragData.cc

	BASE CLASS = public JXSelectionData

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GAddressDragData.h>

#include <jAssert.h>

static const JCharacter* kDragAddressBooksXAtomName = "GAddressBooks";

/******************************************************************************
 Constructor

 *****************************************************************************/

GAddressDragData::GAddressDragData
	(
	JXWidget*			widget,
	const JCharacter*	id
	)
	:
	JXSelectionData(widget, id)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GAddressDragData::~GAddressDragData()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
GAddressDragData::AddTypes
	(
	const Atom selectionName
	)
{
	AddType(kDragAddressBooksXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
GAddressDragData::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char** data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	return kJFalse;
}
