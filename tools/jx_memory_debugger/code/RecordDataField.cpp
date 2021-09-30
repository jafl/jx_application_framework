/******************************************************************************
 RecordDataField.cpp

	Maintains a line of selectable text.

	BASE CLASS = JXInputField

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "RecordDataField.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

RecordDataField::RecordDataField
	(
	const JString&		text,
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
	GetText()->SetText(text);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RecordDataField::~RecordDataField()
{
}
