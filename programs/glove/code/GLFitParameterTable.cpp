/******************************************************************************
 GLFitParameterTable.cpp

	BASE CLASS = public JXEditTable

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GLFitParameterTable.h>
#include "GLFitDescription.h"
#include <GLParmColHeaderWidget.h>

#include <J2DPlotWidget.h>

#include <JXColormap.h>
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
const JCoordinate kSigFigCount	= 5;

const JIndex kNameColIndex		= 1;
const JIndex kStartColIndex		= 2;
const JIndex kFitColIndex		= 3;
const JIndex kErrorColIndex		= 4;

const JCharacter* GLFitParameterTable::kValueChanged = "GLFitParameterTable::kValueChanged";

const JCharacter* kParmNameTitle	= "Parameter name";
const JCharacter* kParmStartTitle	= "Start values";
const JCharacter* kParmFitTitle		= "Fit values";
const JCharacter* kParmErrorTitle	= "Error values";

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
	itsInput(NULL),
	itsHasStartValues(kJTrue),
	itsColHeaderWidget(NULL)
{
	itsMinColWidth = 1;

	const JFontManager* fontMgr = GetFontManager();
	const JSize rowHeight = 2*kVMarginWidth + fontMgr->GetDefaultFont().GetLineHeight();
	SetDefaultRowHeight(rowHeight);

	itsNameList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsNameList != NULL);

	itsStartValues	= jnew JArray<JFloat>;
	assert(itsStartValues != NULL);

	itsFitValues	= jnew JArray<JFloat>;
	assert(itsFitValues != NULL);

	itsErrorValues	= jnew JArray<JFloat>;
	assert(itsErrorValues != NULL);

	AppendCols(4, kDefColWidth);
	AdjustColWidth();

	JXColormap* colormap = GetColormap();
//	SetRowBorderInfo(0, colormap->GetBlackColor());
//	SetColBorderInfo(0, colormap->GetBlackColor());

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

JBoolean
GLFitParameterTable::BeginEditingStartValues()
{
	if (itsHasStartValues && itsStartValues->GetElement(1) == 0)
		{
		JPoint cell((JCoordinate)kStartColIndex, 1);
		BeginEditing(cell);
		return kJTrue;
		}
	return kJFalse;
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
		str	= *(itsNameList->NthElement(cell.y));
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
	JColorIndex color;
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
	assert(itsInput == NULL);
	itsInput = jnew JXFloatInput(this, kFixedLeft, kFixedTop, x, y, w, h);
	assert(itsInput != NULL);

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
	itsInput = NULL;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GLFitParameterTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	if (!itsInput->InputValid())
		{
		return kJFalse;
		}
	JFloat value;
	JBoolean ok	= itsInput->GetValue(&value);
	assert(ok);
	itsStartValues->SetElement(cell.y, value);
	Broadcast(ValueChanged(cell.y, value));
	return kJTrue;
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
GLFitParameterTable::HandleKeyPress
	(
	const int 				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (key == kJReturnKey && GetEditedCell(&cell))
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
	else if (key == kJTabKey)
		{
		// do nothing
		}

	else
		{
		JXEditTable::HandleKeyPress(key, modifiers);
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
	
	itsColHeaderWidget->SetColTitle(1, kParmNameTitle);
	itsColHeaderWidget->SetColTitle(2, kParmStartTitle);
	itsColHeaderWidget->SetColTitle(3, kParmFitTitle);
	itsColHeaderWidget->SetColTitle(4, kParmErrorTitle);
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

	GLFitDescription::FitType type = fit.GetType();
	if (fit.RequiresStartValues())
		{
		if (!itsHasStartValues)
			{
			InsertCols(kStartColIndex, 1, kDefColWidth);
			itsColHeaderWidget->SetColTitle(2, kParmStartTitle);
			itsHasStartValues	= kJTrue;
			AdjustColWidth();
			}
		}
	else
		{
		if (itsHasStartValues)
			{
			RemoveCol(kStartColIndex);
			itsHasStartValues	= kJFalse;
			AdjustColWidth();
			}
		}

	const JSize count	= fit.GetParameterCount();
	AppendRows(count);
	for (JIndex i = 1; i <= count; i++)
		{
		JString* str = jnew JString("");
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
	const JBoolean show
	)
{
	if (itsHasStartValues == show)
		{
		return;
		}
	if (itsHasStartValues)
		{
		RemoveCol(kStartColIndex);
		itsHasStartValues	= kJFalse;
		AdjustColWidth();
		}
	else
		{
		InsertCols(kStartColIndex, 1, kDefColWidth);
		itsColHeaderWidget->SetColTitle(2, kParmStartTitle);
		itsHasStartValues	= kJTrue;
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
	ShowStartCol(kJTrue);
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
		JString* str	= itsNameList->NthElement(i);
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
		text->AppendCharacter('\n');
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
