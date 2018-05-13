/******************************************************************************
 JTEUndoPaste.cpp

	Class to undo paste into a JStyledText object.

	BASE CLASS = JTEUndoTextBase

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JTEUndoPaste.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Saves a snapshot of the specified range, to allow undo.

 ******************************************************************************/

JTEUndoPaste::JTEUndoPaste
	(
	JStyledText*					text,
	const JStyledText::TextRange&	range
	)
	:
	JTEUndoTextBase(text, range),
	itsRange(range)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoPaste::~JTEUndoPaste()
{
}

/******************************************************************************
 SetCount (virtual)

	Saves the number of characters that need to be replaced when we undo.

 ******************************************************************************/

void
JTEUndoPaste::SetCount
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
JTEUndoPaste::Undo()
{
	UndoText(itsRange);
}

/******************************************************************************
 SameStartIndex

 ******************************************************************************/

JBoolean
JTEUndoPaste::SameStartIndex
	(
	const JStyledText::TextRange& range
	)
	const
{
	return JI2B( range.charRange.first == itsRange.charRange.first &&
				 range.byteRange.first == itsRange.byteRange.first );
}
