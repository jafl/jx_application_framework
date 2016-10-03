/******************************************************************************
 GNBTreeWidgetDragData.cc

	BASE CLASS = public JXSelectionManager::Data

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <GNBTreeWidgetDragData.h>

#include <jAssert.h>

static const JCharacter* kDragNBsXAtomName 	= "GNBBooks";

/******************************************************************************
 Constructor

 *****************************************************************************/

GNBTreeWidgetDragData::GNBTreeWidgetDragData
	(
	JXWidget* 			widget, 
	const JCharacter*	id
	)
	:
	JXSelectionData(widget, id)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GNBTreeWidgetDragData::~GNBTreeWidgetDragData()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
GNBTreeWidgetDragData::AddTypes
	(
	const Atom selectionName
	)
{
	AddType(kDragNBsXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
GNBTreeWidgetDragData::ConvertData
	(
	const Atom 		requestType, 
	Atom* 			returnType,
	unsigned char** data, 
	JSize* 			dataLength,
	JSize* 			bitsPerBlock
	)
	const
{
	return kJFalse;
}

