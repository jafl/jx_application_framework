/******************************************************************************
 CBProjectFileInput.cpp

	Input field for entering a path + project file.

	BASE CLASS = JXFileInput

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBProjectFileInput.h"
#include "CBProjectDocument.h"
#include <JXColorManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBProjectFileInput::CBProjectFileInput
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

CBProjectFileInput::~CBProjectFileInput()
{
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

	Draw the file in red if it is not a project file.

 ******************************************************************************/

void
CBProjectFileInput::xAdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<JFont>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
	JString fullName;
	if (GetFile(&fullName) &&
		CBProjectDocument::CanReadFile(fullName) != JXFileDocument::kFileReadable)
		{
		JIndex okLength;
		if (!buffer.LocateLastSubstring(ACE_DIRECTORY_SEPARATOR_STR, &okLength))
			{
			okLength = 0;
			}

		const JXColorManager* colormap = GetColormap();
		const JSize totalLength   = buffer.GetLength();
		JFont f                   = styles->GetFirstElement();

		styles->RemoveAll();
		if (okLength > 0)
			{
			f.SetColor(colormap->GetBlackColor());
			styles->AppendElements(f, okLength);
			}
		if (okLength < totalLength)
			{
			f.SetColor(colormap->GetRedColor());
			styles->AppendElements(f, totalLength - okLength);
			}

		*redrawRange += JIndexRange(1, totalLength);
		}
	else
		{
		JXFileInput::AdjustStylesBeforeRecalc(buffer, styles, recalcRange,
											  redrawRange, deletion);
		}
}
