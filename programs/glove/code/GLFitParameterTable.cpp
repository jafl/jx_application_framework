/******************************************************************************
 GLFitParameterTable.cpp

	BASE CLASS = public JXEditTable

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <GLFitParameterTable.h>
#include "GLFitDescription.h"
#include <GLParmColHeaderWidget.h>

#include <J2DPlotWidget.h>

#include <JXColorManager.h>
#include <JXFloatInput.h>

#include <JFontManager.h>
#include <JPagePrinter.h>
#include <JPainter.h>
#include <JTableSelection.h>

#include <jGlobals.h>
#include <jASCIIConstants.h>

#include <JMinMax.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;
const JCoordinate kDefColWidth  = 100;

const JIndex kNameColIndex		= 1;
const JIndex kStartColIndex		= 2;
const JIndex kFitColIndex		= 3;
const JIndex kErrorColIndex		= 4;

const JUtf8Byte* GLFitParameterTable::kValueChanged = "GLFitParameterTable::kValueChanged";

/******************************************************************************
 Constructor

 *****************************************************************************/

GLFitParameterTable::GLFitParameterTable
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
	JXEditTable(1, kDefColWidth, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsInput(nullptr),
	itsHasStartValues(true),
	itsColHeaderWidget(nullptr)
{
	itsMinColWidth = 1;

	JFontManager* fontMgr = GetFontManager();
	const JSize rowHeight = 2*kVMarginWidth + JFontManager::GetDefaultFont().GetLineHeight(fontMgr);
	SetDefaultRowHeight(rowHeight);

	itsNameList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsNameList != nullptr);

	itsStartValues	= jnew JArray<JFloat>;
	assert(itsStartValues != nullptr);

	itsFitValues	= jnew JArray<JFloat>;
	assert(itsFitValues != nullptr);

	itsErrorValues	= jnew JArray<JFloat>;
	assert(itsErrorValues != nullptr);

	AppendCols(4, kDefColWidth);
	AdjustColWidth();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLFitParameterTable::~GLFitParameterTable()
{
	itsNameList->DeleteAll();
	jdelete itsNameList;		
	jdelete itsStartValues;
	jdelete itsFitValues;
	jdelete itsErrorValues;
}

/******************************************************************************
 HandleMouseDown (protected)

 ******************************************************************************/

void
GLFitParameterTable::HandleMouseDown
	(
	const JPoint& 			pt,
	const JXMouseButton 	button,
	const JSize 			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && clickCount == 1 &&
		GetCell(pt, &cell))
		{
		if (itsHasStartValues && cell.x == (JCoordinate)kStartColIndex)
			{
			BeginEditing(cell);
			}
		}
	else
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 BeginEditingStartValues (public)

 ******************************************************************************/

bool
GLFitParameterTable::BeginEditingStartValues()
{
	if (itsHasStartValues && itsStartValues->GetElement(1) == 0)
		{
		JPoint cell((JCoordinate)kStartColIndex, 1);
		BeginEditing(cell);
		return true;
		}
	return false;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GLFitParameterTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	JIndex realIndex = cell.x;

	if (realIndex == kStartColIndex && !itsHasStartValues)
		{
		realIndex = kFitColIndex;
		}
	else if (realIndex == kFitColIndex && !itsHasStartValues)
		{
		realIndex = kErrorColIndex;
		}

	JString str;
	if (realIndex == kNameColIndex)
		{
		str	= *(itsNameList->GetElement(cell.y));
		}
	else if (realIndex == kStartColIndex)
		{
		str = JString(itsStartValues->GetElement(cell.y), JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
		}
	else if (realIndex == kFitColIndex)
		{
		str = JString(itsFitValues->GetElement(cell.y), JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
		}
	else if (realIndex == kErrorColIndex)
		{
		str = JString(itsErrorValues->GetElement(cell.y), JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
		}

	JRect r = rect;
	r.left += kHMarginWidth;
	p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GLFitParameterTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidth();
}

/******************************************************************************
 AdjustColWidth (private)

 ******************************************************************************/

void
GLFitParameterTable::AdjustColWidth()
{
	AdjustColWidth(GetApertureWidth());
}

void
GLFitParameterTable::AdjustColWidth
	(
	const JSize availabeWidth
	)
{
	JCoordinate lineWidth;
	JColorID color;
	GetColBorderInfo(&lineWidth, &color);
	JSize usedWidth = 
		2 * GetColWidth(kFitColIndex) + 2 * lineWidth;

	if (itsHasStartValues)
		{
		usedWidth += GetColWidth(kStartColIndex) + lineWidth;
		}

	if (availabeWidth > usedWidth)
		{
		SetColWidth(kNameColIndex, availabeWidth - usedWidth);
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GLFitParameterTable::CreateXInputField
	(
	const JPoint& 		cell,
	const JCoordinate 	x,
	const JCoordinate 	y,
	const JCoordinate 	w,
	const JCoordinate 	h
	)
{
	assert(itsInput == nullptr);
	itsInput = jnew JXFloatInput(this, kFixedLeft, kFixedTop, x, y, w, h);
	assert(itsInput != nullptr);

	itsInput->SetValue(itsStartValues->GetElement(cell.y));
	itsInput->SetIsRequired();
	return itsInput;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GLFitParameterTable::PrepareDeleteXInputField()
{
	itsInput = nullptr;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
GLFitParameterTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	if (!itsInput->InputValid())
		{
		return false;
		}
	JFloat value;
	bool ok	= itsInput->GetValue(&value);
	assert(ok);
	itsStartValues->SetElement(cell.y, value);
	Broadcast(ValueChanged(cell.y, value));
	return true;
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
GLFitParameterTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int 				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (c == kJReturnKey && GetEditedCell(&cell))
		{
		if (EndEditing())
			{
			if (!modifiers.shift() && cell.y < (JCoordinate)GetRowCount())
				{
				BeginEditing(JPoint(cell.x, cell.y + 1));
				}
			else if (modifiers.shift() && cell.y > 1)
				{
				BeginEditing(JPoint(cell.x, cell.y - 1));
				}
			}
		}
	else if (c == kJTabKey)
		{
		// do nothing
		}

	else
		{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 SetColHeaderWidget (public)

 ******************************************************************************/

void
GLFitParameterTable::SetColHeaderWidget
	(
	GLParmColHeaderWidget* widget
	)
{
	itsColHeaderWidget	= widget;
	
	itsColHeaderWidget->SetColTitle(1, JGetString("ParmNameTitle::GLFitParameterTable"));
	itsColHeaderWidget->SetColTitle(2, JGetString("ParmStartTitle::GLFitParameterTable"));
	itsColHeaderWidget->SetColTitle(3, JGetString("ParmFitTitle::GLFitParameterTable"));
	itsColHeaderWidget->SetColTitle(4, JGetString("ParmErrorTitle::GLFitParameterTable"));
}

/******************************************************************************
 SetFitDescription (public)

 ******************************************************************************/

void
GLFitParameterTable::SetFitDescription
	(
	const GLFitDescription& fit
	)
{
	RemoveAllRows();
	itsNameList->DeleteAll();
	itsStartValues->RemoveAll();
	itsFitValues->RemoveAll();
	itsErrorValues->RemoveAll();

	if (fit.RequiresStartValues())
		{
		if (!itsHasStartValues)
			{
			InsertCols(kStartColIndex, 1, kDefColWidth);
			itsColHeaderWidget->SetColTitle(2, JGetString("ParmStartTitle::GLFitParameterTable"));
			itsHasStartValues	= true;
			AdjustColWidth();
			}
		}
	else
		{
		if (itsHasStartValues)
			{
			RemoveCol(kStartColIndex);
			itsHasStartValues	= false;
			AdjustColWidth();
			}
		}

	const JSize count	= fit.GetParameterCount();
	AppendRows(count);
	for (JIndex i = 1; i <= count; i++)
		{
		JString* str = jnew JString;
		fit.GetParameterName(i, str);
		itsNameList->Append(str);
		itsStartValues->AppendElement(0);
		itsFitValues->AppendElement(0);
		itsErrorValues->AppendElement(0);
		}			
}

/******************************************************************************
 ShowStartCol (public)

 ******************************************************************************/

void
GLFitParameterTable::ShowStartCol
	(
	const bool show
	)
{
	if (itsHasStartValues == show)
		{
		return;
		}
	if (itsHasStartValues)
		{
		RemoveCol(kStartColIndex);
		itsHasStartValues	= false;
		AdjustColWidth();
		}
	else
		{
		InsertCols(kStartColIndex, 1, kDefColWidth);
		itsColHeaderWidget->SetColTitle(2, JGetString("ParmStartTitle::GLFitParameterTable"));
		itsHasStartValues	= true;
		AdjustColWidth();
		}
}

/******************************************************************************
 SetValue (public)

 ******************************************************************************/

void
GLFitParameterTable::SetValue
	(
	const JIndex index, 
	const JFloat value, 
	const JFloat error
	)
{
	assert(itsFitValues->IndexValid(index));
	itsFitValues->SetElement(index, value);
	itsErrorValues->SetElement(index, error);
	TableRefresh();
}

/******************************************************************************
 ClearValues (public)

 ******************************************************************************/

void
GLFitParameterTable::ClearValues()
{
	JSize count	= itsFitValues->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsFitValues->SetElement(i, 0);
		}
	count	= itsStartValues->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsStartValues->SetElement(i, 0);
		}
	count	= itsErrorValues->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsErrorValues->SetElement(i, 0);
		}
}

/******************************************************************************
 GetStartValues (public)

 ******************************************************************************/

const JArray<JFloat>&
GLFitParameterTable::GetStartValues()
	const
{
	return *itsStartValues;
}

/******************************************************************************
 CopyParmValuesToStart (public)

 ******************************************************************************/

void
GLFitParameterTable::CopyParmValuesToStart()
{
	const JSize count	= itsFitValues->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsStartValues->SetElement(i, itsFitValues->GetElement(i));
		}
	ShowStartCol(true);
}

/******************************************************************************
 GetValueString (public)

 ******************************************************************************/

void
GLFitParameterTable::GetValueString
	(
	JString* text
	)
{
	const JSize count	= itsNameList->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		JString* str	= itsNameList->GetElement(i);
		JFloat start	= itsStartValues->GetElement(i);
		JFloat fit		= itsFitValues->GetElement(i);
		JFloat error	= itsErrorValues->GetElement(i);
		*text += *str + "\n\t";
		if (itsHasStartValues)
			{
			*text += "Start value: " + JString(start, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 6);
			text->Append("\n\t");
			}
		*text += "Fit value:   " + JString(fit, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 6);
		text->Append("\n\t");
		*text += "Error value: " + JString(error, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 6);
		text->Append("\n");
		}
}

/******************************************************************************
 PrintOnPage (public)

 ******************************************************************************/

void
GLFitParameterTable::PrintOnPage
	(
	JPagePrinter& 		p, 
	JCoordinate* 		height
	)
{
	const JSize origWidth	= GetBoundsWidth();
	JSize printWidth		= JSize(p.GetPageWidth() * 0.9);

	AdjustSize(printWidth - origWidth, 0);
//	SetSize(printWidth, GetBoundsHeight());
	AdjustColWidth(printWidth);
	
	itsColHeaderWidget->PrintOnPage(p);
	p.ShiftOrigin(0, itsColHeaderWidget->GetBoundsHeight());

	TableDraw(p, GetBounds());

	AdjustSize(origWidth - printWidth, 0);
//	SetSize(origWidth, GetBoundsHeight());

	*height	= GetBoundsHeight() + itsColHeaderWidget->GetBoundsHeight();
}
