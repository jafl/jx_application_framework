/******************************************************************************
 CBSVNFileInput.cpp

	Input field for entering a file or Subversion URL.

	BASE CLASS = JXFileInput

	Copyright (C) 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSVNFileInput.h"
#include <JXColormap.h>
#include <jWebUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSVNFileInput::CBSVNFileInput
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFileInput(enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSVNFileInput::~CBSVNFileInput()
{
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

	Draw the file in red if it is not a project file.

 ******************************************************************************/

void
CBSVNFileInput::AdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<JFont>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
	if (JIsURL(buffer))
		{
		JFont f = styles->GetFirstElement();
		styles->RemoveAll();
		f.SetColor(GetColormap()->GetBlackColor());
		styles->AppendElements(f, buffer.GetLength());
		}
	else
		{
		JXFileInput::AdjustStylesBeforeRecalc(buffer, styles, recalcRange,
											  redrawRange, deletion);
		}
}
