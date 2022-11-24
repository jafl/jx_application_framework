/******************************************************************************
 GFilterDragData.cpp

	BASE CLASS = public JXSelectionData

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include "GFilterDragData.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kDragFilterXAtomName	= "GMFilterName";

/******************************************************************************
 Constructor

 *****************************************************************************/

GFilterDragData::GFilterDragData
	(
	JXWidget* 			widget, 
	const JUtf8Byte*	id
	)
	:
	JXSelectionData(widget, id)
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

bool
GFilterDragData::ConvertData
	(
	const Atom 		requestType, 
	Atom* 			returnType,
	unsigned char** data, 
	JSize* 			dataLength,
	JSize* 			bitsPerBlock
	)
	const
{
	return false;
}

