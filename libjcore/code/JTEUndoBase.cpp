/******************************************************************************
 JTEUndoBase.cpp

	Base class to support undoing any operation on a JStyledTextBuffer object.

	BASE CLASS = JUndo

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JTEUndoBase.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoBase::JTEUndoBase
	(
	JStyledTextBuffer* buffer
	)
	:
	JUndo(),
	itsBuffer(buffer)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoBase::~JTEUndoBase()
{
}

/******************************************************************************
 SetFont (virtual)

	Called by JStyledTextBuffer::SetAllFontNameAndSize().

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
	const JFontManager* fontMgr = itsBuffer->GetFontManager();

	const JSize runCount = styles->GetRunCount();
	for (JIndex i=1; i<=runCount; i++)
		{
		JFont f = styles->GetRunData(i);
		f.Set(name, size, f.GetStyle());
		styles->SetRunData(i, f);
		}
}
