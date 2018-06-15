/******************************************************************************
 JXStyleTable.cpp

	Convenience class that demonstrates how to use JStyleTableData.

	BASE CLASS = JXEditTable

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "JXStyleTable.h"
#include "JXInputField.h"
#include "JXColorManager.h"
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
	itsStyleData = jnew JStyleTableData(this, GetFontManager());
	assert( itsStyleData != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStyleTable::~JXStyleTable()
{
	jdelete itsStyleData;
}

/******************************************************************************
 GetFont

 ******************************************************************************/

const JFont&
JXStyleTable::GetFont()
	const
{
	return itsStyleData->GetFont();
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JXStyleTable::SetFont
	(
	const JString&	name,
	const JSize		size
	)
{
	itsStyleData->SetFont(name, size);

	JXInputField* input = nullptr;
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
	JXInputField* input = nullptr;
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

	JXInputField* input = nullptr;
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
		return kMin1DVisCharCount *	(GetFont()).GetCharWidth(GetFontManager(), 'W');
		}
}
