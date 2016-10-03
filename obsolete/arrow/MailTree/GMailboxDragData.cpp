/******************************************************************************
 GMailboxDragData.cc

	BASE CLASS = public JXSelectionData

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMailboxDragData.h>

#include <jAssert.h>

static const JCharacter* kDragMailboxesXAtomName = "GMailBoxes";

/******************************************************************************
 Constructor

 *****************************************************************************/

GMailboxDragData::GMailboxDragData
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

GMailboxDragData::~GMailboxDragData()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
GMailboxDragData::AddTypes
	(
	const Atom selectionName
	)
{
	AddType(kDragMailboxesXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
GMailboxDragData::ConvertData
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
