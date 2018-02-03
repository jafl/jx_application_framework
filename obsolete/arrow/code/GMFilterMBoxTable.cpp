/******************************************************************************
 GMFilterMBoxTable.cc

	BASE CLASS = public JXEditTable

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <GMFilterMBoxTable.h>
#include <GMFilter.h>

#include <JXInputField.h>
#include <JXTextButton.h>

#include <jAssert.h>

const JCoordinate kDefRowHeight		= 20;
const JCoordinate kDefColWidth		= 1000;

const JCoordinate kCopyColWidth		= 70;
const JCoordinate kMBoxColWidth		= 200;

const JIndex kCopyColumn			= 1;
const JIndex kMBoxColumn			= 2;

/******************************************************************************
 Create (static)

 *****************************************************************************/

GMFilterMBoxTable*
GMFilterMBoxTable::Create
	(
	GMFilterDialog*	dialog,
	JXTextButton*		chooseButton,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	GMFilterMBoxTable* widget	=
		new GMFilterMBoxTable(dialog, chooseButton,
			enclosure, hSizing, vSizing, x,y, w,h);
	assert(widget != NULL);
	widget->GMFilterMBoxTableX();
	return widget;
}

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilterMBoxTable::GMFilterMBoxTable
	(
	GMFilterDialog*	dialog,
	JXTextButton*		chooseButton,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(kDefRowHeight, kDefColWidth, NULL,
			enclosure, hSizing, vSizing, x,y, w,h),
	itsDialog(dialog),
	itsChooseButton(chooseButton),
	itsInput(NULL)
{
}

/******************************************************************************
 GMFilterMBoxTableX (private)

 ******************************************************************************/

void
GMFilterMBoxTable::GMFilterMBoxTableX()
{
	AppendCol(kCopyColWidth);
	AppendCol(kMBoxColWidth);
	AdjustColWidths();
	AdjustButtons();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilterMBoxTable::~GMFilterMBoxTable()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GMFilterMBoxTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GMFilterMBoxTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsInput	=
		new JXInputField(this, kFixedLeft, kFixedTop, x, y, w, h);
	assert(itsInput != NULL);
	return itsInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GMFilterMBoxTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	return kJFalse;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GMFilterMBoxTable::PrepareDeleteXInputField()
{
	itsInput = NULL;
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GMFilterMBoxTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
GMFilterMBoxTable::AdjustColWidths()
{
	const JSize apWidth = GetApertureWidth();

	JCoordinate lineWidth;
	JColorIndex color;
	GetColBorderInfo(&lineWidth, &color);
	const JSize usedWidth =
		GetColWidth(kCopyColumn) + lineWidth;

	if (apWidth > usedWidth)
		{
		SetColWidth(kMBoxColumn, apWidth - usedWidth);
		}
	else
		{
		// I'll really need to adjust this relative to the size of the patterns.
		}
}

/******************************************************************************
 SetFilter (public)

 ******************************************************************************/

void
GMFilterMBoxTable::SetFilter
	(
	GMFilter* filter
	)
{
	itsFilter	= filter;
	if (filter == NULL)
		{
		RemoveAllRows();
		}
	else if (GetRowCount() == 0)
		{
		AppendRow(GetApertureHeight());
		}
	AdjustButtons();
	TableRefresh();
}

/******************************************************************************
 AdjustButtons (private)

 ******************************************************************************/

void
GMFilterMBoxTable::AdjustButtons()
{
	if (GetRowCount() == 0)
		{
		itsChooseButton->Deactivate();
		}
	else
		{
		itsChooseButton->Activate();
		}
}

/******************************************************************************
 OKToSwitch (public)

 ******************************************************************************/

JBoolean
GMFilterMBoxTable::OKToSwitch()
{
	return kJTrue;
}
