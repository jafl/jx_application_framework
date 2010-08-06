/******************************************************************************
 JStyleTableData.cpp

	Stores font and style information for a table.  The font and size
	are restricted to be table-wide, while the style can be changed for
	each cell.

	BASE CLASS = JAuxTableData<JFontStyle>

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JStyleTableData.h>
#include <JTable.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kVMarginWidth = 1;

// JBroadcaster messages

const JCharacter* JStyleTableData::kFontChanged = "FontChanged::JStyleTableData";

/******************************************************************************
 Constructor

 ******************************************************************************/

JStyleTableData::JStyleTableData
	(
	JTable*				table,
	const JFontManager*	fontManager,
	JColormap*			colormap
	)
	:
	JAuxTableData<JFontStyle>(table, JFontStyle()),
	itsFontManager(fontManager),
	itsColormap(colormap)
{
	SetFont(JGetDefaultFontName(), kJDefaultFontSize);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStyleTableData::~JStyleTableData()
{
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JStyleTableData::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	itsFontName = name;
	itsFontSize = size;

	const JSize rowHeight = 2*kVMarginWidth +
		itsFontManager->GetLineHeight(itsFontName, itsFontSize, JFontStyle());
	(GetTable())->SetDefaultRowHeight(rowHeight);
	(GetTable())->SetAllRowHeights(rowHeight);

	Broadcast(FontChanged());
}

/******************************************************************************
 SetCellStyle

 ******************************************************************************/

void
JStyleTableData::SetCellStyle
	(
	const JPoint&		cell,
	const JFontStyle&	style
	)
{
	const JFontStyle origStyle = GetCellStyle(cell);
	if (style != origStyle)
		{
		SetElement(cell, style);
		}
}

/******************************************************************************
 SetAllCellStyles

 ******************************************************************************/

void
JStyleTableData::SetAllCellStyles
	(
	const JFontStyle& style
	)
{
	SetAllElements(style);
}
