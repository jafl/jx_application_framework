/******************************************************************************
 JTEUndoBase.cpp

	Base class to support undoing any operation on a JTextEditor object.

	BASE CLASS = JUndo

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoBase.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoBase::JTEUndoBase
	(
	JTextEditor* te
	)
	:
	JUndo()
{
	itsTE = te;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoBase::~JTEUndoBase()
{
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoBase::SetFont
	(
	const JCharacter*	name,
	const JSize			size
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
	JRunArray<JTextEditor::Font>*	styles,
	const JCharacter*				name,
	const JSize						size
	)
{
	const JFontManager* fontMgr = itsTE->TEGetFontManager();

	const JSize runCount = styles->GetRunCount();
	for (JIndex i=1; i<=runCount; i++)
		{
		JTextEditor::Font f = styles->GetRunData(i);
		f.size = size;
		f.id   = fontMgr->GetFontID(name, f.size, f.style);
		styles->SetRunData(i, f);
		}
}

/******************************************************************************
 SetPasteLength (virtual)

	Required by some derived classes.

 ******************************************************************************/

void
JTEUndoBase::SetPasteLength
	(
	const JSize length
	)
{
	assert( 0 /* programmer forgot to override JTEUndoBase::SetPasteLength */ );
}
