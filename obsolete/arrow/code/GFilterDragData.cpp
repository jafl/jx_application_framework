/******************************************************************************
 GFilterDragData.cc

	BASE CLASS = public JXSelectionData

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GFilterDragData.h>

#include <jAssert.h>

static const JCharacter* kDragFilterXAtomName	= "GMFilterName";

/******************************************************************************
 Constructor

 *****************************************************************************/

GFilterDragData::GFilterDragData
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

GFilterDragData::~GFilterDragData()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
GFilterDragData::AddTypes
	(
	const Atom selectionName
	)
{
	AddType(kDragFilterXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
GFilterDragData::ConvertData
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
