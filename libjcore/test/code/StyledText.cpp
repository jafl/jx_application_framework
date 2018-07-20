/******************************************************************************
 StyledText.cpp

	Class to test JStyledText.

	BASE CLASS = JStyledText

	Written by John Lindal.

 ******************************************************************************/

#include "StyledText.h"
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

StyledText::StyledText
	(
	const JBoolean useMultipleUndo
	)
	:
	JStyledText(useMultipleUndo, kJTrue),
	itsStyler(nullptr),
	itsTokenStartList(nullptr)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StyledText::~StyledText()
{
	jdelete itsTokenStartList;
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

 ******************************************************************************/

void
StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&		text,
	JRunArray<JFont>*	styles,
	TextRange*			recalcRange,
	TextRange*			redrawRange,
	const JBoolean		deletion
	)
{
	if (itsStyler != nullptr)
		{
		itsStyler->UpdateStyles(this, text, styles, recalcRange, redrawRange, deletion, itsTokenStartList);
		}
}
