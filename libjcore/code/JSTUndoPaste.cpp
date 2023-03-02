/******************************************************************************
 JSTUndoPaste.cpp

	Class to undo paste into a JStyledText object.

	BASE CLASS = JSTUndoTextBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JSTUndoPaste.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

	Saves a snapshot of the specified range, to allow undo.

 ******************************************************************************/

JSTUndoPaste::JSTUndoPaste
	(
	JStyledText*					text,
	const JStyledText::TextRange&	range
	)
	:
	JSTUndoTextBase(text, range),
	itsRange(range)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTUndoPaste::~JSTUndoPaste()
{
}

/******************************************************************************
 SetCount (virtual)

	Saves the number of characters that need to be replaced when we undo.

 ******************************************************************************/

void
JSTUndoPaste::SetCount
	(
	const JStyledText::TextCount& count
	)
{
	itsRange.SetCount(count);
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JSTUndoPaste::Undo()
{
	UndoText(itsRange);
}

/******************************************************************************
 SameStartIndex

 ******************************************************************************/

bool
JSTUndoPaste::SameStartIndex
	(
	const JStyledText::TextRange& range
	)
	const
{
	return range.charRange.first == itsRange.charRange.first &&
				 range.byteRange.first == itsRange.byteRange.first;
}
