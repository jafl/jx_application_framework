/******************************************************************************
 FitParmsTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <FitParmsTable.h>
#include <JXColormap.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <JFontStyle.h>
#include <JString.h>
#include <jGlobals.h>
#include <jAssert.h>

const JSize kDefaultSize = kJDefaultFontSize;

const JCoordinate kDefColWidth  = 50;
const JCoordinate kHMarginWidth = 5;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

FitParmsTable::FitParmsTable
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
	JXTable(1,kDefColWidth, scrollbarSet, enclosure,
				hSizing,vSizing, x,y, w,h)
{
	AppendCols(2, kDefColWidth);
	itsCol1 = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsCol1 != NULL);
	itsCol2 = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsCol2 != NULL);
	SetColBorderInfo(0, GetColormap()->GetBlackColor());
	SetRowBorderInfo(0, GetColormap()->GetBlackColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FitParmsTable::~FitParmsTable()
{
	itsCol1->DeleteAll();
	jdelete itsCol1;
	itsCol2->DeleteAll();
	jdelete itsCol2;
}

/******************************************************************************
 TableDrawCell (virtual protected)


 ******************************************************************************/

void
FitParmsTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JString* str;
	p.SetFont(GetFontManager()->GetDefaultFont());
	JRect r = rect;
	if (cell.x == 1)
		{
		r.right -= kHMarginWidth;
		str = itsCol1->GetElement(cell.y);
		p.String(r, *str, JPainter::kHAlignRight, JPainter::kVAlignCenter);
		}
	else
		{
		r.left += kHMarginWidth;
		str = itsCol2->GetElement(cell.y);
		p.String(r, *str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 Clear


 ******************************************************************************/

void
FitParmsTable::Clear()
{
	itsCol1->DeleteAll();
	itsCol2->DeleteAll();
	RemoveAllRows();
	SetColWidth(1, kDefColWidth);
	SetColWidth(2, kDefColWidth);
	TableRefresh();
}


/******************************************************************************
 Append
 

 ******************************************************************************/

void
FitParmsTable::Append
	(
	const JCharacter* col1, 
	const JCharacter* col2
	)
{
	
	const JFontManager* fm = GetFontManager();
	JSize lineHeight = fm->GetDefaultFont().GetLineHeight();
	AppendRows(1, lineHeight + 2);
	JString* str = jnew JString(col1);
	itsCol1->Append(str);
	JSize col1Width = GetColWidth(1);
	JSize strWidth = fm->GetDefaultFont().GetStringWidth(*str);
	if (strWidth + 10 > col1Width)
		{
		SetColWidth(1, strWidth + 10);
		}
	str = jnew JString(col2);
	itsCol2->Append(str);
	JSize col2Width = GetColWidth(2);
	strWidth = fm->GetDefaultFont().GetStringWidth(*str);
	if (strWidth + 10 > col2Width)
		{
		SetColWidth(2, strWidth + 10);
		}
	TableRefresh();
}
