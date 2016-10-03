/******************************************************************************
 MDRecordDataField.cpp

	Maintains a line of selectable text.

	BASE CLASS = JXInputField

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "MDRecordDataField.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MDRecordDataField::MDRecordDataField
	(
	const JCharacter*	text,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetType(kSelectableText);
	SetText(text);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MDRecordDataField::~MDRecordDataField()
{
}
