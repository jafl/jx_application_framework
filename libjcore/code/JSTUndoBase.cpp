/******************************************************************************
 JSTUndoBase.cpp

	Base class to support undoing any operation on a JStyledText object.

	BASE CLASS = JUndo

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JSTUndoBase.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JSTUndoBase::JSTUndoBase
	(
	JStyledText* text
	)
	:
	JUndo(),
	itsText(text)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTUndoBase::~JSTUndoBase()
{
}

/******************************************************************************
 SetCount (virtual)

	Supports JStyledText::MoveText()

 ******************************************************************************/

void
JSTUndoBase::SetCount
	(
	const JStyledText::TextCount& count
	)
{
}

/******************************************************************************
 SetFont (virtual)

	Called by JStyledText::SetAllFontNameAndSize().

 ******************************************************************************/

void
JSTUndoBase::SetFont
	(
	const JString&	name,
	const JSize		size
	)
{
}

/******************************************************************************
 SetFont (protected)

	Convenience function for derived classes that need to implement SetFont().

 ******************************************************************************/

void
JSTUndoBase::SetFont
	(
	JRunArray<JFont>*	styles,
	const JString&		name,
	const JSize			size
	)
{
	const JSize runCount = styles->GetRunCount();
	for (JIndex i=1; i<=runCount; i++)
		{
		JFont f = styles->GetRunData(i);
		f.Set(name, size, f.GetStyle());
		styles->SetRunData(i, f);
		}
}
