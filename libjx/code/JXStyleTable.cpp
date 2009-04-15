/******************************************************************************
 JXStyleTable.cpp

	Convenience class that demonstrates how to use JStyleTableData.

	BASE CLASS = JXEditTable

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStyleTable.h>
#include <JXInputField.h>
#include <JXColormap.h>
#include <JStyleTableData.h>
#include <JFontManager.h>
#include <jAssert.h>

const JCoordinate kDefColWidth = 100;
const JSize kMin1DVisCharCount = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStyleTable::JXStyleTable
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1,kDefColWidth, scrollbarSet, enclosure,
				hSizing,vSizing, x,y, w,h)
{
	itsStyleData = new JStyleTableData(this, GetFontManager(), GetColormap());
	assert( itsStyleData != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStyleTable::~JXStyleTable()
{
	delete itsStyleData;
}

/******************************************************************************
 GetFont

 ******************************************************************************/

const JString&
JXStyleTable::GetFont
	(
	JSize* size
	)
	const
{
	return itsStyleData->GetFont(size);
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JXStyleTable::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	itsStyleData->SetFont(name, size);

	JXInputField* input = NULL;
	if (GetXInputField(&input))
		{
		input->SetFontName(name);
		input->SetFontSize(size);
		}
}

/******************************************************************************
 GetCellStyle

 ******************************************************************************/

JFontStyle
JXStyleTable::GetCellStyle
	(
	const JPoint& cell
	)
	const
{
	return itsStyleData->GetCellStyle(cell);
}

/******************************************************************************
 SetCellStyle

 ******************************************************************************/

void
JXStyleTable::SetCellStyle
	(
	const JPoint&		cell,
	const JFontStyle&	style
	)
{
	itsStyleData->SetCellStyle(cell, style);

	JPoint editCell;
	JXInputField* input = NULL;
	if (GetEditedCell(&editCell) && editCell == cell &&
		GetXInputField(&input))
		{
		input->SetFontStyle(style);
		}
}

/******************************************************************************
 SetAllCellStyles

 ******************************************************************************/

void
JXStyleTable::SetAllCellStyles
	(
	const JFontStyle& style
	)
{
	itsStyleData->SetAllCellStyles(style);

	JXInputField* input = NULL;
	if (IsEditing() && GetXInputField(&input))
		{
		input->SetFontStyle(style);
		}
}

/******************************************************************************
 GetMin1DVisibleWidth (virtual protected)

	We want at least a few characters to be visible.

 ******************************************************************************/

JCoordinate
JXStyleTable::GetMin1DVisibleWidth
	(
	const JPoint& cell
	)
	const
{
	if (IsEditing())
		{
		return JXEditTable::GetMin1DVisibleWidth(cell);
		}
	else
		{
		JSize size;
		const JString& name = GetFont(&size);
		return kMin1DVisCharCount *
			(GetFontManager())->GetCharWidth(name, size, JFontStyle(), 'W');
		}
}
