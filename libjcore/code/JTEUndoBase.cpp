/******************************************************************************
 JTEUndoBase.cpp

	Base class to support undoing any operation on a JStyledText object.

	BASE CLASS = JUndo

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoBase.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoBase::JTEUndoBase
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

JTEUndoBase::~JTEUndoBase()
{
}

/******************************************************************************
 SetCount (virtual)

	Supports JStyledText::MoveText()

 ******************************************************************************/

void
JTEUndoBase::SetCount
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
JTEUndoBase::SetFont
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
JTEUndoBase::SetFont
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
