/******************************************************************************
 GAddressDragData.cc

	BASE CLASS = public JXSelectionData

	Copyright (C) 2000 by Glenn W. Bach.

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
