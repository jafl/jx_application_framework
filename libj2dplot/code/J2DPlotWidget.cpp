/*******************************************************************************
 J2DPlotWidget.cpp

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "jx-af/j2dplot/J2DPlotWidget.h"
#include "jx-af/j2dplot/J2DPlotDataBase.h"
#include "jx-af/j2dplot/J2DPlotData.h"
#include "jx-af/j2dplot/J2DDataRect.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/JEPSPrinter.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JFontStyle.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

const JSize kLabelBuffer			= 10;
const JSize kRightBuffer			= 30;
const JSize kLegendBuffer			= 15;
const JSize kLegendItemBuffer		= 15;
const JSize kMaxLegendStringWidth	= 200;
const JSize kTitleBuffer			= 15;
const JSize kMajorTicLength			= 4;
const JSize kCursorValueBuffer		= 5;
const JSize kErrorBarEndHalfWidth   = 2;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores all 8 cursor values, regardless of visibility

const JFileVersion kCurrentCurveSetupVersion = 0;

// JBroadcaster messages

const JUtf8Byte* J2DPlotWidget::kTitleChanged    = "TitleChanged::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kScaleChanged    = "ScaleChanged::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kGeometryChanged = "GeometryChanged::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kPlotChanged     = "PlotChanged::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kIsEmpty         = "IsEmpty::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kCurveAdded      = "CurveAdded::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kCurveRemoved    = "CurveRemoved::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kCursorsChanged  = "CursorsChanged::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kCursorMarked    = "CursorMarked::J2DPlotWidget";
const JUtf8Byte* J2DPlotWidget::kMarkRemoved     = "MarkRemoved::J2DPlotWidget";

/*******************************************************************************
 Constructor

 ******************************************************************************/

J2DPlotWidget::J2DPlotWidget
	(
	const JColorID black,
	const JColorID white,
	const JColorID gray,
	const JColorID selection
	)
	:
	itsTitle(JGetString("DefaultTitle::J2DPlotWidget")),
	itsXLabel(JGetString("DefaultXLabel::J2DPlotWidget")),
	itsYLabel(JGetString("DefaultYLabel::J2DPlotWidget")),
	itsBlackColor(black),
	itsWhiteColor(white),
	itsGrayColor(gray),
	itsSelectionColor(selection)
{
	itsShowLegendFlag	= false;
	itsShowGridFlag		= false;
	itsShowFrameFlag	= true;

	itsCurveInfo = jnew JArray<J2DCurveInfo>;
	assert(itsCurveInfo != nullptr);

	itsColors = jnew JArray<JColorID>;
	assert(itsColors != nullptr);

	itsColorUsage = jnew JArray<JSize>;
	assert(itsColorUsage != nullptr);

	AddColor(itsBlackColor);

	itsCurves = jnew JPtrArray<J2DPlotDataBase>(JPtrArrayT::kForgetAll);
	assert(itsCurves != nullptr);

	for (auto& v : itsSymbolUsage)
	{
		v = 0;
	}

	itsFontName					= JFontManager::GetDefaultFontName();
	itsFontSize					= JFontManager::GetDefaultFontSize();

	itsLegendWidth				= 0;

	itsGeometryNeedsAdjustmentFlag	= true;
	itsAutomaticRefreshFlag			= true;
	itsIgnoreCurveChangedFlag		= false;

	itsXDecimalPoints			= 0;
	itsYDecimalPoints			= 0;

	itsXAxisIsLinear			= true;
	itsYAxisIsLinear			= true;

	itsIsZoomedFlag				= false;
	itsShowXMinorTics			= true;
	itsShowYMinorTics			= true;
	itsUseRealXStart			= false;
	itsUseRealYStart			= false;

	itsUsingRange				= false;
	itsForceXExp				= false;
	itsForceYExp				= false;

	itsIsPrintingFlag			= false;

	// Cursors

	itsXMarks = jnew JArray<JFloat>;
	assert(itsXMarks != nullptr);

	itsYMarks = jnew JArray<JFloat>;
	assert(itsYMarks != nullptr);

	itsXCursorVisible	= false;
	itsYCursorVisible	= false;
	itsDualCursors		= false;
	itsXCursorPos1		= 0;
	itsXCursorPos2		= 0;
	itsYCursorPos1		= 0;
	itsYCursorPos2		= 0;
	itsSelectedCursor	= kNoCursor;
	itsXCursor1InitFlag	= false;
	itsXCursor2InitFlag	= false;
	itsYCursor1InitFlag	= false;
	itsYCursor2InitFlag	= false;

	ResetScale(true);
}

/*******************************************************************************
 Destructor

 ******************************************************************************/

J2DPlotWidget::~J2DPlotWidget()
{
	jdelete itsColors;
	jdelete itsColorUsage;

	const JSize count = itsCurves->GetElementCount();
	for (JIndex i=count; i>=1; i--)
	{
		J2DCurveInfo info = itsCurveInfo->GetElement(i);
		if (info.own)
		{
			StopListening(itsCurves->GetElement(i));
			itsCurves->DeleteElement(i);
		}
		jdelete info.name;
	}
	jdelete itsCurves;
	jdelete itsCurveInfo;

	// Cursors

	jdelete itsXMarks;
	jdelete itsYMarks;
}

/*******************************************************************************
 ShowLegend

 ******************************************************************************/

void
J2DPlotWidget::ShowLegend
	(
	const bool show
	)
{
	if (itsShowLegendFlag != show)
	{
		itsShowLegendFlag = show;
		UpdatePlot(true);
	}
}

/*******************************************************************************
 ShowFrame

 ******************************************************************************/

void
J2DPlotWidget::ShowFrame
	(
	const bool show
	)
{
	if (itsShowFrameFlag != show)
	{
		itsShowFrameFlag = show;
		if (show == false)
		{
			itsShowGridFlag = false;
		}
		UpdatePlot(true);
	}
}

/*******************************************************************************
 ShowGrid

 ******************************************************************************/

void
J2DPlotWidget::ShowGrid
	(
	const bool show
	)
{
	if (itsShowGridFlag != show)
	{
		itsShowGridFlag = show;
		if (show)
		{
			itsShowFrameFlag = true;
		}
		UpdatePlot(true);
	}
}

/*******************************************************************************
 ShowCurve

 ******************************************************************************/

void
J2DPlotWidget::ShowCurve
	(
	const JIndex	curveIndex,
	const bool	show
	)
{
	J2DCurveInfo info = itsCurveInfo->GetElement(curveIndex);
	info.show = show;
	itsCurveInfo->SetElement(curveIndex, info);
	UpdatePlot();
}

/*******************************************************************************
 ShowAllCurves

 ******************************************************************************/

void
J2DPlotWidget::ShowAllCurves()
{
	const JSize count = itsCurveInfo->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		J2DCurveInfo info = itsCurveInfo->GetElement(i);
		info.show = true;
		itsCurveInfo->SetElement(i, info);
	}

	UpdatePlot();
}

/*******************************************************************************
 HideAllOtherCurves

 ******************************************************************************/

void
J2DPlotWidget::HideAllOtherCurves
	(
	const JIndex curveIndex
	)
{
	assert( itsCurveInfo->IndexValid(curveIndex) );

	const JSize count = itsCurveInfo->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		J2DCurveInfo info = itsCurveInfo->GetElement(i);
		info.show = i == curveIndex;
		itsCurveInfo->SetElement(i, info);
	}

	UpdatePlot();
}

/*******************************************************************************
 ShowLines

 ******************************************************************************/

void
J2DPlotWidget::ShowLines
	(
	const JIndex curveIndex,
	const bool show
	)
{
	J2DCurveInfo info = itsCurveInfo->GetElement(curveIndex);
	info.lines = show;
	itsCurveInfo->SetElement(curveIndex, info);
	UpdatePlot();
}

/*******************************************************************************
 ShowSymbols

 ******************************************************************************/

void
J2DPlotWidget::ShowSymbols
	(
	const JIndex curveIndex,
	const bool show
	)
{
	J2DCurveInfo info = itsCurveInfo->GetElement(curveIndex);
	info.symbols = show;
	itsCurveInfo->SetElement(curveIndex, info);
	UpdatePlot();
}

/*******************************************************************************
 ShowXErrors

 ******************************************************************************/

void
J2DPlotWidget::ShowXErrors
	(
	const JIndex	index,
	const bool	show
	)
{
	J2DCurveInfo info = itsCurveInfo->GetElement(index);
	if (info.xerrors != show)
	{
		info.xerrors = show;
		itsCurveInfo->SetElement(index, info);
		UpdatePlot();
	}
}

/*******************************************************************************
 ShowYErrors

 ******************************************************************************/

void
J2DPlotWidget::ShowYErrors
	(
	const JIndex	index,
	const bool	show
	)
{
	J2DCurveInfo info = itsCurveInfo->GetElement(index);
	if (info.yerrors != show)
	{
		info.yerrors = show;
		itsCurveInfo->SetElement(index, info);
		UpdatePlot();
	}
}

/*******************************************************************************
 SetCurveStyle

 ******************************************************************************/

void
J2DPlotWidget::SetCurveStyle
	(
	const JIndex	curveIndex,
	const bool	lines,
	const bool	symbols
	)
{
	J2DCurveInfo info = itsCurveInfo->GetElement(curveIndex);
	info.lines   = lines;
	info.symbols = symbols;
	itsCurveInfo->SetElement(curveIndex, info);
	UpdatePlot();
}

/*******************************************************************************
 ProtectCurve

 ******************************************************************************/

void
J2DPlotWidget::ProtectCurve
	(
	const JIndex curveIndex,
	const bool protect
	)
{
	J2DCurveInfo info = itsCurveInfo->GetElement(curveIndex);
	info.protect = protect;
	itsCurveInfo->SetElement(curveIndex, info);
	ProtectionChanged();
}

/*******************************************************************************
 SetCurveName

 ******************************************************************************/

void
J2DPlotWidget::SetCurveName
	(
	const JIndex	index,
	const JString&	name
	)
{
	J2DCurveInfo info = itsCurveInfo->GetElement(index);
	(info.name)->Set(name);
	UpdatePlot(true);
}

/******************************************************************************
 SetCurveInfoArray (protected)

 ******************************************************************************/

void
J2DPlotWidget::SetCurveInfoArray
	(
	const JArray<J2DCurveInfo>& infoArray
	)
{
	assert(infoArray.GetElementCount() == itsCurveInfo->GetElementCount());
	const JSize count = itsCurveInfo->GetElementCount();
	for (JSize i = 1; i <= count; i++)
	{
		J2DCurveInfo info = infoArray.GetElement(i);
		info.name = jnew JString(*info.name);
		assert(info.name != nullptr);
		itsCurveInfo->SetElement(i, info);
	}
	UpdatePlot(true);
}

/*******************************************************************************
 AddCurve

 ******************************************************************************/

JIndex
J2DPlotWidget::AddCurve
	(
	J2DPlotDataBase*	data,
	const bool	ownsData,
	const JString&	name
	)
{
	const bool f = data->IsFunction();
	return AddCurve(data, ownsData, name, f, !f);
}

JIndex
J2DPlotWidget::AddCurve
	(
	J2DPlotDataBase*	data,
	const bool	ownsData,
	const JString&	name,
	const bool	line,
	const bool	symbol
	)
{
	itsCurves->Append(data);

	auto* str = jnew JString(name);
	assert(str != nullptr);

	J2DCurveInfo info(true, line, symbol,
						ownsData, true, true, false,
						GetFirstAvailableColor(),
						GetFirstAvailableSymbol(), str);

	itsCurveInfo->AppendElement(info);

	ListenTo(data);
	Broadcast(CurveAdded(itsCurves->GetElementCount()));
	Broadcast(ScaleChanged());	// Do I still need this?
	UpdatePlot(true, !itsIsZoomedFlag);
	Broadcast(ScaleChanged());
	return itsCurves->GetElementCount();
}

/*******************************************************************************
 AddCurve

 ******************************************************************************/

bool
J2DPlotWidget::AddCurve
	(
	const JArray<JFloat>&	x,
	const JArray<JFloat>&	y,
	const bool				listen,
	const JString&			label,
	JIndex*					index,
	const bool				line,
	const bool				symbol
	)
{
	J2DPlotData* newData;
	if (J2DPlotData::Create(&newData, x, y, listen))
	{
		*index = AddCurve(newData, true, label, line, symbol);
		return true;
	}
	else
	{
		*index = 0;
		return false;
	}
}

/*******************************************************************************
 RemoveCurve

 ******************************************************************************/

void
J2DPlotWidget::RemoveCurve
	(
	const JIndex index
	)
{
	assert (itsCurveInfo->IndexValid(index));
	J2DCurveInfo info = itsCurveInfo->GetElement(index);

	if (info.protect)
	{
		JGetUserNotification()->ReportError(JGetString("CannotRemoveCurve::J2DPlotWidget"));
		return;
	}

	if (info.own)
	{
		itsCurves->DeleteElement(index);
	}
	else
	{
		itsCurves->RemoveElement(index);
	}

	const JSize colorUsage = itsColorUsage->GetElement(info.color);
	assert( colorUsage > 0 );
	itsColorUsage->SetElement(info.color, colorUsage - 1);

	const JIndex symbolIndex = info.symbol;
	assert( symbolIndex < kSymbolCount &&
			itsSymbolUsage [ symbolIndex ] > 0 );
	itsSymbolUsage [ symbolIndex ]--;

	jdelete info.name;
	itsCurveInfo->RemoveElement(index);

	if (!itsCurves->IsEmpty())
	{
		Broadcast(CurveRemoved(index));
		UpdatePlot(true, !itsIsZoomedFlag);
	}
	else
	{
		PWRefresh();
		Broadcast(CurveRemoved(index));
		Broadcast(IsEmpty());		// might delete us
	}
}

/*******************************************************************************
 ArrayIsInCurve

 ******************************************************************************/

bool
J2DPlotWidget::ArrayIsInCurve
	(
	JArray<JFloat>* testArray
	)
	const
{
	assert_msg( 0, "unimplemented" );
/*
	for (JSize i = 1; i <= itsCurves->GetElementCount(); i++)
	{
		if (itsCurves->GetElement(i)->ArrayInData(testArray))
		{
			return true;
		}
	}
*/
	return false;
}

/*******************************************************************************
 RemoveCurveContainingArray

 ******************************************************************************/

void
J2DPlotWidget::RemoveCurvesContainingArray
	(
	JArray<JFloat>* testArray
	)
{
	assert_msg( 0, "unimplemented" );
/*
	for (JSize i = 1; i<= itsCurves->GetElementCount(); i++)
	{
		if (itsCurves->GetElement(i)->ArrayInData(testArray))
		{
			RemoveCurve(i);
			i--;
		}
	}
	UpdatePlot(true, true);
*/
}

/*******************************************************************************
 Receive

 ******************************************************************************/

void
J2DPlotWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(J2DPlotDataBase::kCurveChanged))
	{
		HandleCurveChanged();
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/*******************************************************************************
 HandleCurveChanged (private)

 ******************************************************************************/

void
J2DPlotWidget::HandleCurveChanged()
{
	if (!itsIgnoreCurveChangedFlag)
	{
		if (!itsIsZoomedFlag)
		{
			UpdateScale();
		}

		PWRefresh();
	}
}

/*******************************************************************************
 UpdatePlot (private)

 ******************************************************************************/

void
J2DPlotWidget::UpdatePlot
	(
	const bool geometry,
	const bool scale
	)
{
	if (geometry)
	{
		itsGeometryNeedsAdjustmentFlag = true;
	}

	if (scale)
	{
		UpdateScale();
	}

	Broadcast(PlotChanged());
	PWRefresh();
}

/*******************************************************************************
 UpdateScale (private)

 ******************************************************************************/

void
J2DPlotWidget::UpdateScale()
{
	ResetScale(false);
	itsGeometryNeedsAdjustmentFlag = true;
}

/*******************************************************************************
 Zoom

 ******************************************************************************/

void
J2DPlotWidget::Zoom
	(
	const JRect&	zRect,
	const bool	clean
	)
{
	JRect rect;
	if (JIntersection(zRect, GetFrameGeometry(), &rect))
	{
		UpdateScale(false, &itsXAxisIsLinear,
					GetRealX(rect.left), GetRealX(rect.right), clean, itsXScale);
		UpdateScale(false, &itsYAxisIsLinear,
					GetRealY(rect.bottom), GetRealY(rect.top), clean, itsYScale);

		itsIsZoomedFlag = true;
		SetPlotDecPlaces();
		Broadcast(ScaleChanged());
		UpdatePlot(true);
	}
}

/*******************************************************************************
 GetScale (private)

 ******************************************************************************/

void
J2DPlotWidget::GetScale
	(
	const JFloat*	scale,
	const bool	linear,
	JFloat*			min,
	JFloat*			max,
	JFloat*			inc
	)
	const
{
	if (linear)
	{
		*min = scale[kMin];
		*max = scale[kMax];
		*inc = scale[kInc];
	}
	else
	{
		*min = pow(10, scale[kMin]);
		*max = pow(10, scale[kMax]);
		*inc = pow(10, scale[kInc]);
	}
}

/*******************************************************************************
 SetScale (private)

 ******************************************************************************/

void
J2DPlotWidget::SetScale
	(
	const JFloat	min,
	const JFloat	max,
	const JFloat	inc,
	const bool	linear,
	JFloat*			scale
	)
{
	assert( min < max );
	assert( linear || min > 0.0 );

	if (linear)
	{
		scale[kMin] = min;
		scale[kMax] = max;
		scale[kInc] = inc;

		if (inc > max - min ||
			inc < (max-min)/100.0 ||
			inc <= 0.0)
		{
			JFloat start, end;
			FindRange(min, max, &start, &end, &(scale[kInc]));
			scale[kStart] = start;
			if (scale == itsXScale)
			{
				itsUseRealXStart = true;
			}
			else
			{
				itsUseRealYStart = true;
			}
		}
	}
	else
	{
		scale[kMin] = log10(min);
		scale[kMax] = log10(max);

		const JFloat loginc = (inc <= 0.0 ? 0.0 : log10(inc));
		if (loginc > scale[kMax] - scale[kMin] ||
			loginc < (scale[kMax]-scale[kMin])/100.0 ||
			loginc <= 0.0)
		{
			JFloat start, end;
			FindLogRange(min, max, &start, &end, &(scale[kInc]));
		}
		else
		{
			scale[kInc] = loginc;
		}
	}

	itsIsZoomedFlag = true;
	SetPlotDecPlaces();
	Broadcast(ScaleChanged());
	UpdatePlot(true);
}

/*******************************************************************************
 ResetScale

 ******************************************************************************/

void
J2DPlotWidget::ResetScale()
{
	ResetScale(true);
	UpdatePlot(true);
}

/*******************************************************************************
 ResetScale (private)

 ******************************************************************************/

void
J2DPlotWidget::ResetScale
	(
	const bool allowResetToLinear
	)
{
	JFloat min, max;
	GetXDataRange(&min, &max);
	UpdateScale(allowResetToLinear, &itsXAxisIsLinear, min, max, true, itsXScale);

	GetYDataRange(&min, &max);
	UpdateScale(allowResetToLinear, &itsYAxisIsLinear, min, max, true, itsYScale);

	itsIsZoomedFlag = false;
	SetPlotDecPlaces();
	Broadcast(ScaleChanged());
}

/*******************************************************************************
 UpdateScale (private)

 ******************************************************************************/

void
J2DPlotWidget::UpdateScale
	(
	const bool	allowResetToLinear,
	bool*		linear,
	const JFloat	min,
	const JFloat	max,
	const bool	clean,
	JFloat*			scale
	)
{
	if (min == max && !clean)
	{
		return;
	}

	if (max <= 0.0 && allowResetToLinear)
	{
		*linear = true;
	}

	if (*linear)
	{
		FindRange(min, max, &scale[kMin], &scale[kMax], &scale[kInc]);
		if (!clean)
		{
			scale[kStart] = scale[kMin];
			scale[kMin]   = min;
			scale[kMax]   = max;
		}
	}
	else if (max > 0.0)
	{
		JFloat pMin = min;
		if (min <= 0.0 &&
			!GetMinimumPositiveValue(scale == itsXScale, &pMin))
		{
			return;
		}
		FindLogRange(pMin, max, &scale[kMin], &scale[kMax], &scale[kInc]);
		if (!clean)
		{
			scale[kStart] = scale[kMin];
			scale[kMin]   = log10(pMin);
			scale[kMax]   = log10(max);
		}
	}
	else
	{
		return;
	}

	if (scale == itsXScale)
	{
		itsUseRealXStart = !clean;
	}
	else
	{
		itsUseRealYStart = !clean;
	}
}

/*******************************************************************************
 GetMinimumPositiveValue (private)

 ******************************************************************************/

bool
J2DPlotWidget::GetMinimumPositiveValue
	(
	const bool	isX,
	JFloat*		min
	)
	const
{
	*min = 1.0;
	bool foundPMin = false;

	const JSize ccount = itsCurves->GetElementCount();
	for (JIndex i=1; i<=ccount; i++)
	{
		const J2DPlotDataBase* data = itsCurves->GetElement(i);
		const JSize dcount = data->GetElementCount();
		for (JIndex j=1; j<=dcount; j++)
		{
			J2DDataPoint point;
			data->GetElement(j, &point);
			JFloat value;
			JFloat errValue;
			JFloat other;
			bool linear;
			bool linearOther;
			if (isX)
			{
				value		= point.x;
				errValue	= point.x - point.xmerr;
				other		= point.y;
				linear		= itsXAxisIsLinear;
				linearOther	= itsYAxisIsLinear;
			}
			else
			{
				value		= point.y;
				errValue	= point.y - point.ymerr;
				other		= point.x;
				linear		= itsYAxisIsLinear;
				linearOther	= itsXAxisIsLinear;
			}

			if (linear || errValue > 0.0)
			{
				value = errValue;
			}

			if (value > 0.0 && (linearOther || other > 0.0) &&
				((foundPMin && value < *min) || !foundPMin))
			{
				*min      = value;
				foundPMin = true;
			}
		}
	}

	return foundPMin;
}

/*******************************************************************************
 GetXDataRange (private)

 ******************************************************************************/

void
J2DPlotWidget::GetXDataRange
	(
	JFloat* min,
	JFloat* max
	)
	const
{
	*min = *max = 0.0;

	const JSize count = itsCurves->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const J2DPlotDataBase* data = itsCurves->GetElement(i);

		JFloat testMin, testMax;
		data->GetXRange(&testMin, &testMax);

		if (testMax > *max || i == 1)
		{
			*max = testMax;
		}
		if (testMin < *min || i == 1)
		{
			*min = testMin;
		}
	}
}

/*******************************************************************************
 GetYDataRange (private)

	*** Can be called only after X scale is set.

 ******************************************************************************/

void
J2DPlotWidget::GetYDataRange
	(
	JFloat* min,
	JFloat* max
	)
	const
{
	*min = *max = 0.0;

	JFloat xmin, xmax, xinc;
	GetXScale(&xmin, &xmax, &xinc);

	bool first = true;
	const JSize count = itsCurves->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const J2DPlotDataBase* data = itsCurves->GetElement(i);

		JFloat testMin, testMax;
		if (data->GetYRange(xmin, xmax, itsXAxisIsLinear, &testMin, &testMax))
		{
			if (testMax > *max || first)
			{
				*max = testMax;
			}
			if (testMin < *min || first)
			{
				*min = testMin;
			}
			first = false;
		}
	}
}

/*******************************************************************************
 SetPlotDecPlaces (private)

 ******************************************************************************/

void
J2DPlotWidget::SetPlotDecPlaces()
{
	if (itsCurves->GetElementCount() == 0)
	{
		return;
	}

	SetExp(itsXScale, &itsXExp, &itsForceXExp);
	SetPlotDecPlaces1(itsXScale, !itsUseRealXStart, itsForceXExp, itsXExp,
					  &itsXDecimalPoints);

	SetExp(itsYScale, &itsYExp, &itsForceYExp);
	SetPlotDecPlaces1(itsYScale, !itsUseRealYStart, itsForceYExp, itsYExp,
					  &itsYDecimalPoints);
}

/*******************************************************************************
 SetPlotDecPlaces1 (static private)

 ******************************************************************************/

void
J2DPlotWidget::SetPlotDecPlaces1
	(
	const JFloat*		scale,
	const bool		useExactRange,
	const bool		forceExp,
	const JCoordinate	exp,
	JSize*				dpCount
	)
{
	JSize minDP, maxDP;
	if (useExactRange)
	{
		minDP = GetDecPlaces(scale[kMin]);
		maxDP = GetDecPlaces(scale[kMax]);
	}
	else
	{
		const JFloat max = scale[kStart] +
			JLCeil((scale[kMax] - scale[kMin]) / scale[kInc]) * scale[kInc];

		minDP = GetDecPlaces(scale[kStart]);
		maxDP = GetDecPlaces(max);
	}

	const JSize incDP = GetDecPlaces(scale[kInc]);

	*dpCount = JMax(JMax(minDP, maxDP), incDP);

	const long pl = JLFloor(log10(scale[kInc]));
	if (forceExp && exp > pl)
	{
		*dpCount = JMax(*dpCount, (JSize) (exp - pl));
	}
}

/*******************************************************************************
 GetDecPlaces (static private)

 ******************************************************************************/

static const JRegex exponentPattern = "[eE]";

JSize
J2DPlotWidget::GetDecPlaces
	(
	const JFloat origValue
	)
{
	if (origValue == 0.0)
	{
		return 0;
	}

	const JFloat value = fabs(origValue);

	JSize places = 0;
	JString valStr(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 7);
	if (value < 1e-3 || 1e5 <= value)
	{
		valStr = JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0);
	}

	bool hadE = false;

	JStringIterator iter(valStr);

	JIndex dotIndex = 0;
	if (iter.Next("."))
	{
		dotIndex = iter.GetPrevCharacterIndex();
	}

	iter.MoveTo(kJIteratorStartAtBeginning, 0);

	if (iter.Next(exponentPattern))
	{
		if (dotIndex == 0)
		{
			places = 0;
		}
		else
		{
			places = iter.GetPrevCharacterIndex() - dotIndex - 1;
		}
		hadE = true;
	}
	iter.Invalidate();

	if (!hadE)
	{
		JCoordinate signedPlaces = JLFloor(log10(value));
		if (signedPlaces >= 0)
		{
			places = 0;
		}
		else
		{
			places = -signedPlaces;
		}
		JFloat vp10 = value * pow(10.0, (int) places);
		while (vp10 - floor(vp10) > 1e-6 && places <= 8)
		{
			places++;
			vp10 = value * pow(10.0, (int) places);
		}
	}

	return places;
}

/*******************************************************************************
 SetExp (static private)

 ******************************************************************************/

void
J2DPlotWidget::SetExp
	(
	const JFloat*	scale,
	JInteger*		exp,
	bool*		force
	)
{
	JInteger min, max;
	GetExp(scale[kMin], &min);
	GetExp(scale[kMax], &max);

	if (min == 0 && max == 0)
	{
		*exp   = 0;
		*force = false;
	}
	else if (max > 0)
	{
		*exp   = JMax(min, max);
		*force = true;
	}
	else
	{
		*exp   = JMin(min, max);
		*force = true;
	}
}

/*******************************************************************************
 GetExp (static private)

 ******************************************************************************/

void
J2DPlotWidget::GetExp
	(
	const JFloat	value,
	JInteger*		exp
	)
{
	*exp = 0;

	const JFloat absValue = fabs(value);
	if (absValue >= 1e5 ||
		(0 < absValue && absValue < 1e-3))
	{
		*exp = JLFloor(log10(absValue));
	}
}

/*********************************************************************************
 FindRange (static)

 ********************************************************************************/

const JSize kMinDivisionCount = 4;

static const JFloat kIncrementDivisor[] = { 1.0, 2.0, 5.0 };
const JSize kIncrementDivisorCount      = sizeof(kIncrementDivisor) / sizeof(JFloat);

void
J2DPlotWidget::FindRange
	(
	const JFloat	origMin,
	const JFloat	origMax,
	JFloat*			start,
	JFloat*			end,
	JFloat*			inc
	)
{
	assert( origMin <= origMax );

	JFloat min = origMin;
	JFloat max = origMax;
	if (max == min)
	{
		const JFloat delta = (max == 0.0 ? 0.1 : max/10.0);
		max += delta;
		min -= delta;
	}

	const JFloat range = max-min;

	bool changeStart = true;
	bool changeEnd   = true;
	if (range > fabs(min) && range > fabs(max))
	{
		// range contains origin
	}
	else if (range > fabs(min))
	{
		// range large relative to distance above origin

		*start      = 0.0;
		changeStart = false;
	}
	else if (range > fabs(max))
	{
		// range large relative to distance below origin

		*end      = 0.0;
		changeEnd = false;
	}
	else
	{
		// range small relative to distance from origin
	}

	const JFloat baseInc = pow(10, floor(log10(range)));

	JIndex i = 0;
	while (i < kIncrementDivisorCount)
	{
		*inc = baseInc / kIncrementDivisor[i];

		if (changeStart)
		{
			*start = *inc * floor(min/(*inc));
		}
		if (changeEnd)
		{
			*end = *inc * ceil (max/(*inc));
		}

		const JSize incCount = JRound((*end - *start)/(*inc));
		if (incCount >= kMinDivisionCount)
		{
			break;
		}
		i++;
	}
}

/*******************************************************************************
 FindLogRange (static)

 ******************************************************************************/

void
J2DPlotWidget::FindLogRange
	(
	const JFloat	origMin,
	const JFloat	origMax,
	JFloat*			start,
	JFloat*			end,
	JFloat*			inc
	)
{
	assert( origMin > 0.0 && origMax > 0.0 );
	assert( origMin <= origMax );

	JFloat min = origMin;
	JFloat max = origMax;
	if (max == min)
	{
		min /= 10.0;
		max *= 10.0;
	}

	*start = floor(log10(min));
	*end   = ceil (log10(max));
	*inc   = GetLogIncrement(*start, *end);
	*start = *inc * floor(*start/(*inc));
	*end   = *inc * ceil (*end  /(*inc));
}

/*******************************************************************************
 GetLogIncrement (static)

 ******************************************************************************/

JFloat
J2DPlotWidget::GetLogIncrement
	(
	const JFloat min,
	const JFloat max
	)
{
	assert( min < max );

	const JFloat delta = ceil(max) - floor(min);

	JFloat inc = 1.0;
	while (true)
	{
		const JFloat divCount = delta/inc;
		if (divCount <= 10.0)
		{
			return inc;
		}
		if (divCount <= 20.0)
		{
			return 2.0*inc;
		}
		if (divCount <= 50.0)
		{
			return 5.0*inc;
		}

		inc *= 10.0;
	}
}

/*******************************************************************************
 GetRealX

 ******************************************************************************/

JFloat
J2DPlotWidget::GetRealX
	(
	const JCoordinate frameX
	)
	const
{
	JFloat value = (frameX - itsXAxisStart)/itsXTrans + itsXScale[kMin];
	if (!itsXAxisIsLinear)
	{
		value = pow(10, value);
	}
	return value;
}

/*******************************************************************************
 GetRealY

 ******************************************************************************/

JFloat
J2DPlotWidget::GetRealY
	(
	const JCoordinate frameY
	)
	const
{
	JFloat value = (itsYAxisStart - frameY)/itsYTrans + itsYScale[kMin];
	if (!itsYAxisIsLinear)
	{
		value = pow(10, value);
	}
	return value;
}

/*******************************************************************************
 GetFrameX

 ******************************************************************************/

bool
J2DPlotWidget::GetFrameX
	(
	const JFloat	x,
	JFloat*			frameX
	)
	const
{
	if (!itsXAxisIsLinear && x <= 0.0)
	{
		*frameX = 0.0;
		return false;
	}
	else
	{
		*frameX = itsXAxisStart +
			itsXTrans * ((itsXAxisIsLinear ? x : log10(x)) - itsXScale[kMin]);
		return true;
	}
}

/*******************************************************************************
 GetFrameY

 ******************************************************************************/

bool
J2DPlotWidget::GetFrameY
	(
	const JFloat	y,
	JFloat*			frameY
	)
	const
{
	if (!itsYAxisIsLinear && y <= 0.0)
	{
		*frameY = 0.0;
		return false;
	}
	else
	{
		*frameY = itsYAxisStart -
			itsYTrans * ((itsYAxisIsLinear ? y : log10(y)) - itsYScale[kMin]);
		return true;
	}
}

/*******************************************************************************
 Print

	PostScript

 ******************************************************************************/

void
J2DPlotWidget::Print
	(
	JPagePrinter& p
	)
{
	if (p.OpenDocument())
	{
		bool cancelled  = false;
		bool redoBounds = false;

		if (p.NewPage())
		{
			itsIsPrintingFlag  = true;
			JCoordinate xShift = 0;
			JCoordinate yShift = 0;
			if (p.GetOrientation() == JPagePrinter::kPortrait)
			{
				GetPSPortraitPrintSize(p.GetPageWidth(), p.GetPageHeight(),
									   &itsPrintWidth, &itsPrintHeight);
				xShift = (p.GetPageWidth() - itsPrintWidth)/2;
				yShift = JMax(0L, (p.GetPageHeight() - itsPrintHeight - GetMarksHeight())/2);
			}
			else
			{
				GetPSLandscapePrintSize(p.GetPageWidth(), p.GetPageHeight(),
										&itsPrintWidth, &itsPrintHeight);
			}
			p.ShiftOrigin(xShift, yShift);
			PWBoundsChanged();
			redoBounds = true;
			PWDraw(p, p.GetPageRect());
			p.ShiftOrigin(-xShift, -yShift);

			bool putOnSamePage = false;
			JRect partialPageRect;
			if (p.GetOrientation() == JPagePrinter::kPortrait)
			{
				putOnSamePage = true;
				partialPageRect.Set(yShift + itsPrintHeight + p.GetLineHeight(), xShift,
									p.GetPageHeight(), xShift + itsPrintWidth);
			}
			if (!PrintMarks(p, putOnSamePage, partialPageRect))
			{
				cancelled = true;
			}
		}

		if (!cancelled)
		{
			p.CloseDocument();
		}
		itsIsPrintingFlag = false;
		if (redoBounds)
		{
			PWBoundsChanged();
		}
	}
}

/*******************************************************************************
 GetPSPortraitPrintSize (static)

 ******************************************************************************/

void
J2DPlotWidget::GetPSPortraitPrintSize
	(
	const JCoordinate	pageWidth,
	const JCoordinate	pageHeight,
	JCoordinate*		w,
	JCoordinate*		h
	)
{
	*w = JRound(pageWidth * 0.9);
	*h = JRound(*w * 0.8);
}

/*******************************************************************************
 GetPSLandscapePrintSize (static)

 ******************************************************************************/

void
J2DPlotWidget::GetPSLandscapePrintSize
	(
	const JCoordinate	pageWidth,
	const JCoordinate	pageHeight,
	JCoordinate*		w,
	JCoordinate*		h
	)
{
	*w = JRound(pageWidth * 0.9);
	*h = pageHeight;
}

/*******************************************************************************
 DrawForPrint

	Prints to a specified rectange on the page.

 ******************************************************************************/

void
J2DPlotWidget::DrawForPrint
	(
	JPagePrinter&	p,
	const JRect&	rect
	)
{
	itsIsPrintingFlag  = true;
	itsPrintWidth      = rect.width();
	itsPrintHeight     = rect.height();
	PWBoundsChanged();

	p.ShiftOrigin(rect.topLeft());

	PWDraw(p, JRect(0, 0, itsPrintHeight, itsPrintWidth));

	p.ShiftOrigin(-(rect.topLeft()));

	itsIsPrintingFlag = false;
	PWBoundsChanged();
}

/*******************************************************************************
 Print

	EPS

 ******************************************************************************/

void
J2DPlotWidget::Print
	(
	JEPSPrinter&	p,
	const JRect&	rect
	)
{
	itsIsPrintingFlag  = true;
	itsPrintWidth      = rect.width();
	itsPrintHeight     = rect.height();
	PWBoundsChanged();

	if (p.WantsPreview())
	{
		JPainter& p1 = p.GetPreviewPainter(rect);
		PWDraw(p1, rect);
	}

	if (p.OpenDocument(rect))
	{
		PWDraw(p, rect);
		p.CloseDocument();
	}

	itsIsPrintingFlag = false;
	PWBoundsChanged();
}

/*******************************************************************************
 PWDraw (virtual protected)

	This is virtual so derived classes can override it to draw extra
	things on the plot.

 ******************************************************************************/

void
J2DPlotWidget::PWDraw
	(
	JPainter&		p,
	const JRect&	rect
	)
{
	p.SetFontName(itsFontName);
	p.SetFontSize(itsFontSize);

	if (itsGeometryNeedsAdjustmentFlag)
	{
		AdjustGeometry(p);
	}

	if (rect.top < (JCoordinate) (PWGetHeight() - GetCursorLabelHeight(p)))
	{
		if (itsShowLegendFlag)
		{
			DrawLegend(p);
		}

		DrawAxes(p);
		if (!itsCurves->IsEmpty())
		{
			DrawData(p);
			DrawMarks(p);
			DrawCursors(p);
		}
	}

	DrawCursorLabels(p);
}

/*******************************************************************************
 AdjustGeometry

 ******************************************************************************/

void
J2DPlotWidget::AdjustGeometry
	(
	JPainter& p
	)
{
	itsLineHeight			= p.GetLineHeight();
	const JCoordinate kYLabelBuffer	= itsLineHeight + 2 * kLabelBuffer;
	const JCoordinate kXLabelBuffer = itsLineHeight + 2 * kLabelBuffer;
	const JCoordinate kTitleHeight	= itsLineHeight + 2 * kTitleBuffer;

	JSize maxLabelWidth = 0;

	const JSize count = itsCurves->GetElementCount();
	for (JSize i = 1; i <= count; i++)
	{
		J2DCurveInfo info = itsCurveInfo->GetElement(i);
		JSize width = p.GetStringWidth(*(info.name));
		if (width > kMaxLegendStringWidth)
		{
			TruncateCurveName(p,i);
		}
		if (width > maxLabelWidth && (!itsIsPrintingFlag || info.show))
		{
			maxLabelWidth = width;
		}
	}

	itsMaxCurveNameWidth = JMin(kMaxLegendStringWidth, maxLabelWidth);
	itsLegendWidth       = itsMaxCurveNameWidth + 3 * kLegendItemBuffer;

	JSize strWidth, strWidthMin;
	if (itsYAxisIsLinear == true)
	{
		JString str, strMin;
		if (itsForceYExp)
		{
			str    = JString(itsYScale[kMax], itsYDecimalPoints, JString::kUseGivenExponent, itsYExp);
			strMin = JString(itsYScale[kMin], itsYDecimalPoints, JString::kUseGivenExponent, itsYExp);
		}
		else
		{
			str    = JString(itsYScale[kMax], itsYDecimalPoints);
			strMin = JString(itsYScale[kMin], itsYDecimalPoints);
		}

		strWidthMin = p.GetStringWidth(strMin);
		strWidth    = p.GetStringWidth(str);

		if (strWidthMin > strWidth)
		{
			strWidth = strWidthMin;
		}
	}
	else
	{
		JArray<JFloat> major;
		JArray<JFloat> minor;
		GetLogTicks(itsYScale[kMin], itsYScale[kMax], itsYScale[kInc], &major, &minor);

		strWidth = 0;
		JString str;
		JSize count = major.GetElementCount();
		for (JSize i = 1; i <= count; i++)
		{
			str = GetLogAxisString(major.GetElement(i));
			JSize tempWidth = p.GetStringWidth(str);
			if (tempWidth > strWidth)
			{
				strWidth = tempWidth;
			}
		}
		if (count == 0)
		{
			count = minor.GetElementCount();
			for (JSize i = 1; i <= count; i++)
			{
				str = GetLogAxisString(minor.GetElement(i));
				JSize tempWidth = p.GetStringWidth(str);
				if (tempWidth > strWidth)
				{
					strWidth = tempWidth;
				}
			}
			if (count == 0)
			{
				str = GetLogAxisString(pow(10, itsYScale[kMin]));
				JSize tempWidth = p.GetStringWidth(str);
				if (tempWidth > strWidth)
				{
					strWidth = tempWidth;
				}
				str = GetLogAxisString(pow(10, itsYScale[kMax]));
				tempWidth = p.GetStringWidth(str);
				if (tempWidth > strWidth)
				{
					strWidth = tempWidth;
				}
			}
		}
	}

	JString strXMin;
	JString strXMax;
	if (itsXAxisIsLinear)
	{
		if (itsForceXExp)
		{
			strXMax = JString(itsXScale[kMax], itsXDecimalPoints, JString::kUseGivenExponent, itsXExp);
			strXMin = JString(itsXScale[kMin], itsXDecimalPoints, JString::kUseGivenExponent, itsXExp);
		}
		else
		{
			strXMax = JString(itsXScale[kMax], itsXDecimalPoints);
			strXMin = JString(itsXScale[kMin], itsXDecimalPoints);
		}
	}
	else
	{
		if ( (itsXScale[kMin] > 3) || (itsXScale[kMin] < -3) )
		{
			strXMin = JString(pow(10,itsXScale[kMin]), 0, JString::kForceExponent, 0);
		}
		else if (itsXScale[kMin] < 0)
		{
			int prec = (int)fabs(itsXScale[kMin]);
			strXMin = JString(pow(10,itsXScale[kMin]), prec);
		}
		else
		{
			strXMin = JString(pow(10,itsXScale[kMin]), 0);
		}

		if ( (itsXScale[kMax] > 3) || (itsXScale[kMax] < -3) )
		{
			strXMin = JString(pow(10,itsXScale[kMax]), 0, JString::kForceExponent, 0);
		}
		else if (itsXScale[kMax] < 0)
		{
			int prec = (int)fabs(itsXScale[kMax]);
			strXMax = JString(pow(10,itsXScale[kMax]), prec);
		}
		else
		{
			strXMax = JString(pow(10,itsXScale[kMax]), 0);
		}
	}
	const JSize strXWidthMin = p.GetStringWidth(strXMin);
	const JSize strXWidthMax = p.GetStringWidth(strXMax);


	if (itsShowFrameFlag)
	{
		if (strWidth < strXWidthMin/2)
		{
			strWidth = strXWidthMin/2;
		}

		itsXAxisStart = strWidth + kYLabelBuffer + kMajorTicLength; // tics inside

		if (itsShowLegendFlag)
		{
			itsXAxisEnd = PWGetWidth() - 2 * kLegendBuffer - itsLegendWidth - strXWidthMax/2;
		}

		else
		{
			itsXAxisEnd = PWGetWidth() - kRightBuffer - strXWidthMax/2;
		}
	}

	else
	{
		if ( ((itsXScale[kMin] >= 0) && (itsXScale[kMax] >= 0)) ||
				!itsXAxisIsLinear )
		{
			if (strWidth < strXWidthMin/2)
			{
				strWidth = strXWidthMin/2;
			}

			itsXAxisStart = strWidth + kYLabelBuffer + 2 * kMajorTicLength;

			if (itsShowLegendFlag)
			{
				itsXAxisEnd = PWGetWidth() - 2 * kLegendBuffer - itsLegendWidth - strXWidthMax/2;
			}

			else
			{
				itsXAxisEnd = PWGetWidth() - kRightBuffer - strXWidthMax/2;
			}
		}
		else if ( (itsXScale[kMin] <= 0) && (itsXScale[kMax] <= 0) )
		{
			if (strWidth < strXWidthMax/2)
			{
				strWidth = strXWidthMax/2;
			}

			itsXAxisStart = kYLabelBuffer + strXWidthMin/2;

			if (itsShowLegendFlag)
			{
				itsXAxisEnd = PWGetWidth() - 2 * kLegendBuffer - itsLegendWidth
							- strWidth -2 *  kMajorTicLength;
			}

			else
			{
				itsXAxisEnd = PWGetWidth() - kRightBuffer
							- strWidth -2 *  kMajorTicLength;
			}
		}

		else
		{
			itsXAxisStart = kYLabelBuffer + strXWidthMin/2;

			if (itsShowLegendFlag)
			{
				itsXAxisEnd = PWGetWidth() - 2 * kLegendBuffer - itsLegendWidth - strXWidthMax/2;
			}

			else
			{
				itsXAxisEnd = PWGetWidth() - kRightBuffer - strXWidthMax/2;
			}
		}
	}

	itsMaxXLabelWidth = strWidth;
	JSize cursorLabelHeight	= GetCursorLabelHeight(p);

	if (itsShowFrameFlag)
	{
		itsYAxisStart = PWGetHeight() -
			(kXLabelBuffer +  kMajorTicLength + itsLineHeight + cursorLabelHeight) ;

		itsYAxisEnd = kTitleHeight;
	}

	else
	{
		if ( ((itsYScale[kMin] >= 0) && (itsYScale[kMax] >= 0)) ||
			 !itsYAxisIsLinear)
		{
			itsYAxisStart = PWGetHeight() -
			(kXLabelBuffer +  2 * kMajorTicLength + itsLineHeight + cursorLabelHeight) ;

			itsYAxisEnd = kTitleHeight;
		}

		else if ( (itsYScale[kMin] <= 0) && (itsYScale[kMax] <= 0) )
		{
			itsYAxisStart = PWGetHeight() - (kXLabelBuffer + cursorLabelHeight);

			itsYAxisEnd = kTitleHeight + 2 * kMajorTicLength + itsLineHeight;
		}

		else
		{
			itsYAxisStart = PWGetHeight() - (kXLabelBuffer + itsLineHeight/2 + cursorLabelHeight);

			itsYAxisEnd = kTitleHeight;
		}
	}

	itsXTrans = (itsXAxisEnd - itsXAxisStart)/(itsXScale[kMax] - itsXScale[kMin]);
	itsShowXMinorTics = itsXTrans >= 35;

	itsYTrans = (itsYAxisStart - itsYAxisEnd)/(itsYScale[kMax] - itsYScale[kMin]);
	itsShowYMinorTics = itsYTrans >= 35;

	itsGeometryNeedsAdjustmentFlag = false;
	AdjustCursors();
	Broadcast(GeometryChanged());
}

/*******************************************************************************
 TruncateCurveName

 ******************************************************************************/

void
J2DPlotWidget::TruncateCurveName
	(
	JPainter&		p,
	const JIndex	index
	)
{
	const J2DCurveInfo info = itsCurveInfo->GetElement(index);

	JStringIterator iter(info.name);
	while (p.GetStringWidth(*(info.name)) > kMaxLegendStringWidth)
	{
		iter.RemovePrev();
	}
}

/*******************************************************************************
 DrawLegend

 ******************************************************************************/

void
J2DPlotWidget::DrawLegend
	(
	JPainter& p
	)
{
JIndex i;

	const JSize items  = itsCurves->GetElementCount();
	JSize visibleItems = items;
	if (itsIsPrintingFlag)
	{
		visibleItems = 0;
		for (i = 1; i <= items; i++)
		{
			J2DCurveInfo info = itsCurveInfo->GetElement(i);
			if (info.show)
			{
				visibleItems++;
			}
		}
	}
	JSize strHeight         = p.GetLineHeight();
	JSize cursorLabelHeight = GetCursorLabelHeight(p);

	JSize height = (visibleItems + 2) * strHeight;
	JSize starty = (PWGetHeight() - height - cursorLabelHeight)/2;
	JSize startx = PWGetWidth() - kLegendBuffer - itsLegendWidth;

	itsLegendRect.Set(starty, startx, starty + height, startx + itsLegendWidth);
	p.Rect(startx, starty, itsLegendWidth, height);

	JSize vis = 1;
	for (i = 1; i <= items; i++)
	{
		J2DCurveInfo info = itsCurveInfo->GetElement(i);
		p.SetPenColor(itsBlackColor);

		if (!itsIsPrintingFlag || info.show)
		{
			const JSize yPos = starty + vis*strHeight;
			p.SetFontStyle(info.show ? itsBlackColor : itsGrayColor);
			p.String(startx + 2 * kLegendItemBuffer,
					yPos, *(info.name),
					itsMaxCurveNameWidth, JPainter::kHAlignLeft, strHeight);

			p.SetPenColor(itsColors->GetElement(info.color));

			if (info.lines)
			{
				p.Line( startx + kLegendItemBuffer/2,
						yPos + strHeight/2,
						JRound(startx + 1.5*kLegendItemBuffer + 1),
						yPos + strHeight/2 );
			}

			if (info.symbols)
			{
				DrawSymbol(p, JPoint(startx + kLegendItemBuffer,
							yPos + strHeight/2),
							info.symbol);
			}

			if (info.points())
			{
				p.Rect(JPoint(startx + kLegendItemBuffer, yPos + strHeight/2), 2,2);
			}

			const J2DPlotDataBase* curve     = itsCurves->GetElement(i);
			const J2DPlotDataBase::Type type = curve->GetType();
			if (type == J2DPlotDataBase::kVectorPlot)
			{
				JPoint vstart(startx + kLegendItemBuffer, yPos + strHeight/2);
				JPoint vend = vstart + JPoint(4,-4);
				p.Line(vstart, vend);
			}
			vis++;
		}
	}

	p.SetFontStyle(itsBlackColor);
	p.SetPenColor(itsBlackColor);
}

/*******************************************************************************
 DrawAxes

 ******************************************************************************/

void
J2DPlotWidget::DrawAxes
	(
	JPainter& p
	)
{
	if (!itsCurves->IsEmpty())
	{
		DrawTicks(p);
	}

	if (itsShowFrameFlag || itsCurves->IsEmpty())
	{
		p.Rect(itsXAxisStart, itsYAxisEnd,
				itsXAxisEnd - itsXAxisStart + 1, itsYAxisStart - itsYAxisEnd + 1);

		itsYAxisRect.Set(itsYAxisEnd,
						 itsXAxisStart - itsMaxXLabelWidth - kMajorTicLength,
						 itsYAxisStart, itsXAxisStart);
		itsXAxisRect.Set(itsYAxisStart, itsXAxisStart,
						 itsYAxisStart + kMajorTicLength + itsLineHeight,
						 itsXAxisEnd);
	}

	else
	{
		// Draw vertical axis

		if ( ((itsXScale[kMin] >= 0) && (itsXScale[kMax] >= 0)) ||
				!itsXAxisIsLinear)
		{
			p.Line(itsXAxisStart, itsYAxisEnd,
					itsXAxisStart, itsYAxisStart);

			itsYAxisRect.Set(itsYAxisEnd,
							 itsXAxisStart - itsMaxXLabelWidth - 2 * kMajorTicLength,
							 itsYAxisStart, itsXAxisStart);
		}

		else if ( (itsXScale[kMin] <= 0) && (itsXScale[kMax] <= 0) )
		{
			p.Line(itsXAxisEnd, itsYAxisEnd,
					itsXAxisEnd, itsYAxisStart);

			itsYAxisRect.Set(itsYAxisEnd, itsXAxisEnd,
							 itsYAxisStart, itsXAxisEnd + itsMaxXLabelWidth + 2 * kMajorTicLength);
		}

		else
		{
			const JCoordinate x = JRound(itsXAxisStart - itsXScale[kMin]*itsXTrans);
			p.Line(x, itsYAxisEnd, x, itsYAxisStart);

			itsYAxisRect.Set(itsYAxisEnd, x - itsMaxXLabelWidth - 2 * kMajorTicLength,
							 itsYAxisStart, x);
		}

		// Draw horizontal axis

		if ( ((itsYScale[kMin] >= 0) && (itsYScale[kMax] >= 0)) ||
				!itsYAxisIsLinear)
		{
			p.Line(itsXAxisStart, itsYAxisStart,
					itsXAxisEnd, itsYAxisStart);

			itsXAxisRect.Set(itsYAxisStart, itsXAxisStart,
							 itsYAxisStart + 2 * kMajorTicLength + itsLineHeight,
							 itsXAxisEnd);
		}

		else if ( (itsYScale[kMin] <= 0) && (itsYScale[kMax] <= 0) )
		{
			p.Line(itsXAxisStart, itsYAxisEnd,
					itsXAxisEnd, itsYAxisEnd);

			itsXAxisRect.Set(itsYAxisEnd - 2 * kMajorTicLength - itsLineHeight, itsXAxisStart,
							 itsYAxisEnd,
							 itsXAxisEnd);
		}

		else
		{
			const JCoordinate y = JRound(itsYAxisStart + itsYScale[kMin]*itsYTrans);
			p.Line(itsXAxisStart, y, itsXAxisEnd, y);

			itsXAxisRect.Set(y, itsXAxisStart,
							 y + 2 * kMajorTicLength + itsLineHeight,
							 itsXAxisEnd);
		}
	}

	DrawLabels(p);
}

/*******************************************************************************
 DrawTicks

 ******************************************************************************/

void
J2DPlotWidget::DrawTicks
	(
	JPainter& p
	)
{
	const JSize labelHeight = p.GetLineHeight();

	// X ticks

	JSize yTick = 0, yLabel;
	if (itsShowFrameFlag)
	{
		yLabel = itsYAxisStart + kMajorTicLength;
	}
	else if ( ((itsYScale[kMin] >= 0) && (itsYScale[kMax] >= 0)) ||
				!itsYAxisIsLinear )
	{
		yTick  = itsYAxisStart;
		yLabel = itsYAxisStart + 2 * kMajorTicLength;
	}
	else if ( (itsYScale[kMin] <= 0) && (itsYScale[kMax] <= 0) )
	{
		yTick  = itsYAxisEnd;
		yLabel = itsYAxisEnd - 2 * kMajorTicLength - labelHeight;
	}
	else
	{
		yTick  = JRound(itsYAxisStart + itsYScale[kMin]*itsYTrans);
		yLabel = yTick + 2 * kMajorTicLength;
	}

	JFloat min = itsXScale[kMin];
	if (itsUseRealXStart)
	{
		min = itsXScale[kStart];
	}
	const JSize xTickCount =
		JRound( (itsXScale[kMax] - min)/itsXScale[kInc]) + 1;

	if (itsXAxisIsLinear)
	{
		DrawXTicks(p, yTick, yLabel, xTickCount);
	}
	else
	{
		DrawXLogMinorTicks(p, yTick, xTickCount, yLabel);
	}

	// Y ticks

	JSize xTick = 0, xLabel;
	bool xLabelEndKnown = true;

	if (itsShowFrameFlag)
	{
		xLabel = itsXAxisStart - kMajorTicLength;
	}
	else if ( ((itsXScale[kMin] >= 0) && (itsXScale[kMax] >= 0)) ||
				!itsXAxisIsLinear )
	{
		xTick  = itsXAxisStart - kMajorTicLength;
		xLabel = itsXAxisStart - 2 * kMajorTicLength;
	}
	else if ( (itsXScale[kMin] <= 0) && (itsXScale[kMax] <= 0) )
	{
		xTick  = itsXAxisEnd - kMajorTicLength;
		xLabel = itsXAxisEnd + 2 * kMajorTicLength;
		xLabelEndKnown = false;
	}
	else
	{
		xTick  = JRound(itsXAxisStart - itsXScale[kMin]*itsXTrans - kMajorTicLength);
		xLabel = xTick - 2 * kMajorTicLength;
	}

	min = itsYScale[kMin];
	if (itsUseRealYStart)
	{
		min = itsYScale[kStart];
	}
	const JSize yTickCount =
		JRound( (itsYScale[kMax] - min)/itsYScale[kInc]) + 1;

	if (itsYAxisIsLinear)
	{
		DrawYTicks(p, xTick, xLabel, xLabelEndKnown, yTickCount);
	}
	else
	{
		DrawYLogMinorTicks(p, xTick, yTickCount, xLabel, xLabelEndKnown);
	}
}

/*******************************************************************************
 DrawXTicks

 ******************************************************************************/

void
J2DPlotWidget::DrawXTicks
	(
	JPainter&	p,
	JCoordinate yTick,
	JCoordinate yLabel,
	JSize		tickCount
	)
{
	JFloat axisTemp = itsXScale[kMin];
	JFloat axisMin  = itsXScale[kMin];

	if (itsUseRealXStart)
	{
		axisTemp = itsXScale[kStart];
		axisMin	 = axisTemp;
	}

	for (JIndex i = 1; i <= tickCount; i++)
	{
		bool lightGrid = true;
		if (fabs(axisTemp) < (itsXScale[kInc]/100000))
		{
			axisTemp  = 0;
			lightGrid = false;
		}
		DrawXTick(p, axisTemp, yTick, kMajorTicLength,
				  yLabel, true, lightGrid);
		axisTemp = axisMin + i * itsXScale[kInc];
	}
}

/*******************************************************************************
 DrawXLogMinorTicks

 ******************************************************************************/

void
J2DPlotWidget::DrawXLogMinorTicks
	(
	JPainter&			p,
	const JCoordinate	yTick,
	const JSize			tickCount,
	const JCoordinate	yLabel
	)
{
	JIndex i;
	JArray<JFloat> major;
	JArray<JFloat> minor;

	GetLogTicks(itsXScale[kMin], itsXScale[kMax], itsXScale[kInc], &major, &minor);

	for (i = 1; i <= major.GetElementCount(); i++)
	{
		DrawXTick(p, major.GetElement(i), yTick,
				  kMajorTicLength, yLabel,
				  true, false);
	}

	if (itsXScale[kInc] > 1)
	{
		return;
	}

	bool drawStr = major.GetElementCount() == 0;
	for (i = 1; i <= minor.GetElementCount(); i++)
	{
		DrawXTick(p, minor.GetElement(i), yTick,
				  kMajorTicLength - 1, yLabel,
				  drawStr, true);
	}

	if (major.GetElementCount() == 0 && minor.GetElementCount() == 0)
	{
		DrawXTick(p, pow(10, itsXScale[kMin]), yTick,
				  kMajorTicLength, yLabel,
				  true, false);
		DrawXTick(p, pow(10, itsXScale[kMax]), yTick,
				  kMajorTicLength, yLabel,
				  true, false);
	}
}

/******************************************************************************
 DrawXTick (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawXTick
	(
	JPainter&			p,
	const JFloat		value,
	const JCoordinate	yVal,
	const JCoordinate	tickLength,
	const JCoordinate	yLabel,
	const bool		drawStr,
	const bool		lightGrid
	)
{
	const JSize labelHeight = p.GetLineHeight();
	JCoordinate x;
	if (GetFrameX(value, &x) && itsXAxisStart <= x && x <= itsXAxisEnd)
	{
		if (drawStr)
		{
			const JString str =
				BuildTickLabel(true, value, itsXAxisIsLinear, itsXDecimalPoints,
							   itsForceXExp, itsXExp);

			const JCoordinate sWidth = p.GetStringWidth(str);
			JSize strStart = x - sWidth/2;
			p.String(strStart, yLabel, str, sWidth, JPainter::kHAlignCenter, labelHeight);
		}
		if (itsShowFrameFlag && (x > itsXAxisStart + 1) && (x < itsXAxisEnd - 1))
		{
			if (itsShowGridFlag)
			{
				if (lightGrid)
				{
					p.SetPenColor(itsGrayColor);
				}
				p.Line(x, itsYAxisStart, x, itsYAxisEnd);
				p.SetPenColor(itsBlackColor);
			}
			else
			{
				p.Line(x, itsYAxisStart, x, itsYAxisStart - tickLength);
				p.Line(x, itsYAxisEnd, x, itsYAxisEnd + tickLength);
			}
		}
		else if (!itsShowFrameFlag)
		{
			p.Line(x, yVal - tickLength, x, yVal + tickLength);
		}
	}
}

/*******************************************************************************
 DrawYTicks

 ******************************************************************************/

void
J2DPlotWidget::DrawYTicks
	(
	JPainter&			p,
	const JCoordinate	xTick,
	const JCoordinate	xLabel,
	const bool		xLabelEndKnown,
	const JSize			tickCount
	)
{
	JFloat axisTemp = itsYScale[kMin];
	JFloat axisMin  = itsYScale[kMin];
	if (itsUseRealYStart)
	{
		axisTemp = itsYScale[kStart];
		axisMin	 = axisTemp;
	}

	for (JIndex i = 1; i <= tickCount; i++)
	{
		bool lightGrid = true;
		if (fabs(axisTemp) < (itsYScale[kInc]/100000))
		{
			axisTemp  = 0;
			lightGrid = false;
		}
		DrawYTick(p, axisTemp, xTick, kMajorTicLength,
				  xLabel, xLabelEndKnown,
				  true, lightGrid);
		axisTemp = axisMin + i * itsYScale[kInc];
	}
}

/*******************************************************************************
 DrawYLogMinorTicks

 ******************************************************************************/

void
J2DPlotWidget::DrawYLogMinorTicks
	(
	JPainter&			p,
	const JCoordinate	xTick,
	const JSize			tickCount,
	const JCoordinate	xLabel,
	const bool		xLabelEndKnown
	)
{
	JSize i;
	JArray<JFloat> major;
	JArray<JFloat> minor;

	GetLogTicks(itsYScale[kMin], itsYScale[kMax], itsYScale[kInc], &major, &minor);
	JSize count = major.GetElementCount();
	for (i = 1; i <= count; i++)
	{
		DrawYTick(p, major.GetElement(i), xTick,
				  kMajorTicLength, xLabel, xLabelEndKnown,
				  true, false);
	}

	if (itsYScale[kInc] > 1)
	{
		return;
	}

	bool drawStr = major.GetElementCount() == 0;
	for (i = 1; i <= minor.GetElementCount(); i++)
	{
		DrawYTick(p, minor.GetElement(i), xTick,
				  kMajorTicLength - 1, xLabel, xLabelEndKnown,
				  drawStr, true);
	}

	if ((major.GetElementCount() == 0) && (minor.GetElementCount() == 0))
	{
		DrawYTick(p, pow(10, itsYScale[kMin]), xTick,
				  kMajorTicLength, xLabel, xLabelEndKnown,
				  true, false);
		DrawYTick(p, pow(10, itsYScale[kMax]), xTick,
				  kMajorTicLength, xLabel, xLabelEndKnown,
				  true, false);
	}

}

/******************************************************************************
 DrawYTick (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawYTick
	(
	JPainter&			p,
	const JFloat		value,
	const JCoordinate	xVal,
	const JCoordinate	tickLength,
	const JCoordinate	xLabel,
	const JCoordinate	xLabelEndKnown,
	const bool		drawStr,
	const bool		lightGrid
	)
{
	const JSize labelHeight = p.GetLineHeight();
	JCoordinate y;
	if (GetFrameY(value, &y) && itsYAxisEnd <= y && y <= itsYAxisStart)
	{
		if (drawStr)
		{
			const JString str =
				BuildTickLabel(false, value, itsYAxisIsLinear, itsYDecimalPoints,
							   itsForceYExp, itsYExp);

			JSize strStart;
			const JCoordinate sWidth = p.GetStringWidth(str);
			if (xLabelEndKnown)
			{
				strStart = xLabel - sWidth;
				p.String(strStart, y - labelHeight/2,
					str, sWidth, JPainter::kHAlignRight, labelHeight);
			}
			else
			{
				strStart = xLabel;
				p.String(strStart, y - labelHeight/2,
					str, sWidth, JPainter::kHAlignLeft, labelHeight);
			}
		}
		if (itsShowFrameFlag && (y > itsYAxisEnd + 1) && (y < itsYAxisStart - 1))
		{
			if (itsShowGridFlag)
			{
				if (lightGrid)
				{
					p.SetPenColor(itsGrayColor);
				}
				p.Line(itsXAxisStart, y, itsXAxisEnd, y);
				p.SetPenColor(itsBlackColor);
			}
			else
			{
				p.Line(itsXAxisStart, y, itsXAxisStart + tickLength, y);
				p.Line(itsXAxisEnd, y, itsXAxisEnd - tickLength, y);
			}
		}
		else if (!itsShowFrameFlag)
		{
			JCoordinate tStart = xVal + 1;
			if (!lightGrid)
			{
				tStart = xVal;
			}
			p.Line(tStart, y, tStart + 2 * tickLength, y);
		}
	}
}

/******************************************************************************
 BuildTickLabel (virtual protected)

 ******************************************************************************/

JString
J2DPlotWidget::BuildTickLabel
	(
	const bool	isXAxis,
	const JFloat	value,
	const bool	axisIsLinear,
	const JInteger	precision,
	const bool	forceExponent,
	const JInteger	exponent
	)
{
	if (axisIsLinear && forceExponent)
	{
		return JString(value, precision, JString::kUseGivenExponent, exponent);
	}
	else if (axisIsLinear)
	{
		return JString(value, precision);
	}
	else
	{
		return GetLogAxisString(value);
	}
}

/******************************************************************************
 GetLogAxisString (protected)

 ******************************************************************************/

JString
J2DPlotWidget::GetLogAxisString
	(
	const JFloat value
	)
	const
{
	JFloat exponent = log10(value);
	bool nonint = false;
	if (JRound(exponent)/exponent - 1 > 0.0001)
	{
		nonint = true;
	}
	if (3 < exponent || exponent < -3)
	{
		return JString(value, 0, JString::kForceExponent, 0);
	}
	else
	{
		int prec;
		if (exponent < 0)
		{
			prec = (int) fabs(exponent);
		}
		else
		{
			prec = 0;
		}
		if (nonint)
		{
			prec++;
		}
		return JString(value, prec);
	}
}

/******************************************************************************
 GetLogTicks (private)

 ******************************************************************************/

void
J2DPlotWidget::GetLogTicks
	(
	JFloat			min,
	JFloat			max,
	JFloat			inc,
	JArray<JFloat>* major,
	JArray<JFloat>* minor
	)
	const
{
	JFloat rStart	= pow(10, min);
	JFloat rEnd		= pow(10, max);
	JFloat tStart	= floor(min);
	JFloat tEnd		= ceil(max);

	for (JFloat i = tStart; i <= tEnd; i += inc)
	{
		JFloat pIndex = pow(10, i);
		for (JIndex index = 1; index <= 9; index++)
		{
			JFloat value = pIndex * index;
			if ((value >= rStart) &&
				(value <= rEnd))
			{
				if (index == 1)
				{
					major->AppendElement(value);
				}
				else if (inc == 1)
				{
					minor->AppendElement(value);
				}
			}
		}
	}
}

/*******************************************************************************
 DrawLabels

 ******************************************************************************/

void
J2DPlotWidget::DrawLabels
	(
	JPainter& p
	)
{
	const JSize cursorLabelHeight	= GetCursorLabelHeight(p);
	const JCoordinate yWidth		= p.GetStringWidth(itsYLabel);
	const JCoordinate yStart		= (itsYAxisStart + itsYAxisEnd + yWidth)/2;

	const JCoordinate xWidth		= p.GetStringWidth(itsXLabel);
	const JCoordinate xStart		= (itsXAxisStart + itsXAxisEnd - xWidth)/2;
	const JCoordinate xTop			= PWGetHeight() - kLabelBuffer - itsLineHeight - cursorLabelHeight;

	const JCoordinate titleWidth	= p.GetStringWidth(itsTitle);
	const JCoordinate titleStart	= itsXAxisStart + (itsXAxisEnd - itsXAxisStart - titleWidth)/2;

	itsXLabelRect.Set(	xTop,
						xStart, xTop + itsLineHeight,
						xStart + xWidth);

	itsYLabelRect.Set(	yStart - yWidth, kLabelBuffer,
						yStart, kLabelBuffer + itsLineHeight);

	itsTitleRect.Set(	kTitleBuffer, titleStart,
						2 * kTitleBuffer + itsLineHeight,
						titleStart + titleWidth);

// Draw X Axis Label

	p.String(itsXLabelRect, itsXLabel, JPainter::kHAlignCenter, JPainter::kVAlignCenter);

// Draw Y Axis Label

	p.String(90.0, itsYLabelRect, itsYLabel, JPainter::kHAlignCenter, JPainter::kVAlignCenter);

// Draw Title

	p.String(itsTitleRect, itsTitle, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/*******************************************************************************
 Drawing utilities (local)

 ******************************************************************************/

inline JFloat
J2DPlotWidget::GetGraphValue
	(
	const JFloat	value,
	const bool	linear
	)
	const
{
	return (linear ? value : (value <= 0.0 ? -HUGE_VAL : log10(value)));
}

inline bool
J2DPlotWidget::ConvertLog10
	(
	J2DDataPoint* data
	)
	const
{
	if ((!itsXAxisIsLinear && data->x <= 0.0) ||
		(!itsYAxisIsLinear && data->y <= 0.0))
	{
		return false;
	}
	else
	{
		data->x = GetGraphValue(data->x, itsXAxisIsLinear);
		data->y = GetGraphValue(data->y, itsYAxisIsLinear);
		return true;
	}
}

/*******************************************************************************
 DrawData (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawData
	(
	JPainter& p
	)
	const
{
	J2DDataRect visRect;
	CalcVisibleRange(itsXScale[kMin], itsXScale[kMax],
					 GetGraphValue(itsRangeXMin, itsXAxisIsLinear),
					 GetGraphValue(itsRangeXMax, itsXAxisIsLinear),
					 &(visRect.xmin), &(visRect.xmax));
	CalcVisibleRange(itsYScale[kMin], itsYScale[kMax],
					 GetGraphValue(itsRangeYMin, itsYAxisIsLinear),
					 GetGraphValue(itsRangeYMax, itsYAxisIsLinear),
					 &(visRect.ymin), &(visRect.ymax));

	const JSize count = itsCurves->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const J2DCurveInfo info        = itsCurveInfo->GetElement(i);
		const J2DPlotDataBase* curve     = itsCurves->GetElement(i);
		const J2DPlotDataBase::Type type = curve->GetType();

		p.SetPenColor(itsColors->GetElement(info.color));

		if (info.show && type == J2DPlotDataBase::kScatterPlot)
		{
			DrawCurve(p, *curve, info, visRect);
		}
		else if (info.show && type == J2DPlotDataBase::kVectorPlot)
		{
			DrawVector(p, *curve, info, visRect);
		}
	}
}

/*******************************************************************************
 CalcVisibleRange (private)

 ******************************************************************************/

void
J2DPlotWidget::CalcVisibleRange
	(
	const JFloat	scaleMin,
	const JFloat	scaleMax,
	const JFloat	rangeMin,
	const JFloat	rangeMax,
	JFloat*			min,
	JFloat*			max
	)
	const
{
	*min = scaleMin;
	if (itsUsingRange && rangeMin > *min)
	{
		*min = rangeMin;
	}

	*max = scaleMax;
	if (itsUsingRange && rangeMax < *max)
	{
		*max = rangeMax;
	}
}

/*******************************************************************************
 DrawCurve (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawCurve
	(
	JPainter&				p,
	const J2DPlotDataBase&	curve,
	const J2DCurveInfo&		info,
	const J2DDataRect&		visRect
	)
	const
{
	auto* me         = const_cast<J2DPlotWidget*>(this);
	const bool drawErrors = (info.xerrors && curve.HasXErrors()) ||
									 (info.yerrors && curve.HasYErrors());

	J2DDataPoint data1, data2;
	JPoint pt;

	const JSize dataCount = curve.GetElementCount();
	for (JIndex i=1; i<=dataCount; i++)
	{
		if (info.lines || info.symbols)
		{
			bool move, draw, mark;
			Interpolate(i, curve, visRect, &data1, &data2, &move, &draw, &mark);

			const bool xlin = itsXAxisIsLinear;
			const bool ylin = itsYAxisIsLinear;
			me->itsXAxisIsLinear = me->itsYAxisIsLinear = true;	// trick GetFramePoint()

			if (info.lines && move)
			{
				const bool ok = GetFramePoint(data1.x, data1.y, &pt);
				assert( ok );
				p.SetPenLocation(pt);
			}

			if ((info.lines && draw) ||
				((info.symbols || drawErrors) && mark) ||
				(!move && !draw && mark))
			{
				const bool ok = GetFramePoint(data2.x, data2.y, &pt);
				assert( ok );
			}

			if (info.lines && draw)
			{
				p.LineTo(pt);
			}
			if (info.symbols && mark)
			{
				DrawSymbol(p, pt, info.symbol);
			}
			if (drawErrors && mark)
			{
				DrawError(p, pt, curve, info, i, xlin, ylin, visRect);
			}
			if (!move && !draw && mark)		// in case data1 illegal (log scale)
			{
				p.SetPenLocation(pt);
			}

			me->itsXAxisIsLinear = xlin;
			me->itsYAxisIsLinear = ylin;
		}

		else
		{
			curve.GetElement(i, &data1);
			if (ConvertLog10(&data1) &&
				visRect.Contains(data1.x, data1.y))
			{
				const bool xlin = itsXAxisIsLinear;
				const bool ylin = itsYAxisIsLinear;
				me->itsXAxisIsLinear = me->itsYAxisIsLinear = true;	// trick GetFramePoint()

				const bool ok = GetFramePoint(data1.x, data1.y, &pt);
				assert( ok );

				if (drawErrors)
				{
					DrawError(p, pt, curve, info, i, xlin, ylin, visRect);
				}
				else
				{
					p.Point(pt);
				}

				me->itsXAxisIsLinear = xlin;
				me->itsYAxisIsLinear = ylin;
			}
		}
	}
}

/*******************************************************************************
 DrawError (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawError
	(
	JPainter&				p,
	const JPoint&			pt,
	const J2DPlotDataBase&	curve,
	const J2DCurveInfo&		info,
	const JIndex			index,
	const bool			xLinear,
	const bool			yLinear,
	const J2DDataRect&		visRect
	)
	const
{
	J2DDataPoint data;
	curve.GetElement(index, &data);

	const JPoint origPenLoc = p.GetPenLocation();

	if (info.xerrors && curve.HasXErrors())
	{
		JPoint pt1 = pt, pt2 = pt;
		bool moved1 = false, moved2 = false;

		JFloat xp = GetGraphValue(data.x + data.xerr, xLinear);
		if (xp > visRect.xmax)
		{
			xp     = visRect.xmax;
			moved1 = true;
		}
		bool ok = GetFrameX(xp, &pt1.x);
		assert( ok );

		JFloat xm = GetGraphValue(data.x - data.xmerr, xLinear);
		if (xm < visRect.xmin)
		{
			xm     = visRect.xmin;
			moved2 = true;
		}
		ok = GetFrameX(xm, &pt2.x);
		assert( ok );

		p.Line(pt1, pt2);
		if (!moved1)
		{
			p.Line(pt1.x, pt1.y-kErrorBarEndHalfWidth,
				   pt1.x, pt1.y+kErrorBarEndHalfWidth);
		}
		if (!moved2)
		{
			p.Line(pt2.x, pt2.y-kErrorBarEndHalfWidth,
				   pt2.x, pt2.y+kErrorBarEndHalfWidth);
		}
	}

	if (info.yerrors && curve.HasYErrors())
	{
		JPoint pt1 = pt, pt2 = pt;
		bool moved1 = false, moved2 = false;

		JFloat yp = GetGraphValue(data.y + data.yerr, yLinear);
		if (yp > visRect.ymax)
		{
			yp     = visRect.ymax;
			moved1 = true;
		}
		bool ok = GetFrameY(yp, &pt1.y);
		assert( ok );

		JFloat ym = GetGraphValue(data.y - data.ymerr, yLinear);
		if (ym < visRect.ymin)
		{
			ym     = visRect.ymin;
			moved2 = true;
		}
		ok = GetFrameY(ym, &pt2.y);
		assert( ok );

		p.Line(pt1, pt2);
		if (!moved1)
		{
			p.Line(pt1.x-kErrorBarEndHalfWidth, pt1.y,
				   pt1.x+kErrorBarEndHalfWidth, pt1.y);
		}
		if (!moved2)
		{
			p.Line(pt2.x-kErrorBarEndHalfWidth, pt2.y,
				   pt2.x+kErrorBarEndHalfWidth, pt2.y);
		}
	}

	p.SetPenLocation(origPenLoc);
}

/*******************************************************************************
 DrawVector (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawVector
	(
	JPainter&				p,
	const J2DPlotDataBase&	curve,
	const J2DCurveInfo&		info,
	const J2DDataRect&		visRect
	)
	const
{
	auto* me = const_cast<J2DPlotWidget*>(this);

	J2DVectorPoint vector;
	J2DDataPoint head, tail;
	JPoint headPt, tailPt;

	const JSize dataCount = curve.GetElementCount();
	for (JIndex i=1; i<=dataCount; i++)
	{
		curve.GetElement(i, &vector);
		vector.GetHead(&head.x, &head.y);
		vector.GetTail(&tail.x, &tail.y);

		if (ConvertLog10(&head) && ConvertLog10(&tail))
		{
			bool move, draw, mark;
			Interpolate(visRect, &head, &tail, &move, &draw, &mark);
			// move => head is not visible

			const bool xlin = itsXAxisIsLinear;
			const bool ylin = itsYAxisIsLinear;
			me->itsXAxisIsLinear = me->itsYAxisIsLinear = true;	// trick GetFramePoint()

			bool ok = GetFramePoint(head.x, head.y, &headPt);
			assert( ok );
			ok = GetFramePoint(tail.x, tail.y, &tailPt);
			assert( ok );

			me->itsXAxisIsLinear = xlin;
			me->itsYAxisIsLinear = ylin;

			if (draw)
			{
				p.Line(headPt, tailPt);
				if (!move)
				{
					DrawVectorHead(p, headPt, tailPt, vector);
				}
			}
			if (info.symbols && mark)
			{
				DrawSymbol(p, tailPt, info.symbol);
			}
		}
	}
}

/*******************************************************************************
 DrawVectorHead (private)

 ******************************************************************************/

const JCoordinate kVectorHeadLength = 6;
const JFloat deltaTheta             = kJPi/6.0;		// 30 degrees

void
J2DPlotWidget::DrawVectorHead
	(
	JPainter&				p,
	const JPoint&			headPt,
	const JPoint&			tailPt,
	const J2DVectorPoint&	vector
	)
	const
{
	const JPoint v    = headPt - tailPt;
	const JFloat norm = sqrt((JFloat) (v.x*v.x + v.y*v.y));
	const JFloat dx   = -kVectorHeadLength*v.x/norm;
	const JFloat dy   = -kVectorHeadLength*v.y/norm;

	const JFloat c = cos(deltaTheta);
	const JFloat s = sin(deltaTheta);
	JPoint v1(JRound(dx*c-dy*s), JRound( dx*s+dy*c));
	JPoint v2(JRound(dx*c+dy*s), JRound(-dx*s+dy*c));

	p.Line(headPt+v1, headPt);
	p.LineTo(headPt+v2);
}

/*******************************************************************************
 Interpolate (private)

 ******************************************************************************/

void
J2DPlotWidget::Interpolate
	(
	const JIndex			index,
	const J2DPlotDataBase&	curve,
	const J2DDataRect&		visRect,
	J2DDataPoint*			data1,
	J2DDataPoint*			data2,
	bool*				move,
	bool*				draw,
	bool*				mark
	)
	const
{
	curve.GetElement(index, data2);
	if (!ConvertLog10(data2))
	{
		*move = false;
		*draw = false;
		*mark = false;
		return;
	}

	const bool data2Visible = visRect.Contains(data2->x, data2->y);
	if (index == 1 && data2Visible)
	{
		*data1 = *data2;
		*move  = true;
		*draw  = false;
		*mark  = true;
		return;
	}
	else if (index == 1)
	{
		*move = false;
		*draw = false;
		*mark = false;
		return;
	}

	curve.GetElement(index-1, data1);
	if (!ConvertLog10(data1))
	{
		*move = false;
		*draw = false;
		*mark = data2Visible;
		return;
	}

	Interpolate(visRect, data1, data2, move, draw, mark);
}

void
J2DPlotWidget::Interpolate
	(
	const J2DDataRect&		visRect,
	J2DDataPoint*			data1,
	J2DDataPoint*			data2,
	bool*				move,
	bool*				draw,
	bool*				mark
	)
	const
{
	const bool horiz = data1->y == data2->y;
	const bool vert  = data1->x == data2->x;

	if ((data1->x < visRect.xmin && data2->x < visRect.xmin) ||
		(data1->x > visRect.xmax && data2->x > visRect.xmax) ||
		(data1->y < visRect.ymin && data2->y < visRect.ymin) ||
		(data1->y > visRect.ymax && data2->y > visRect.ymax))
	{
		*move = false;
		*draw = false;
		*mark = false;
		return;
	}

	*move = false;
	*draw = true;
	*mark = true;

	const JFloat slope = vert ? 0.0 : (data2->y - data1->y)/(data2->x - data1->x);

	if (!Interpolate1(data1, visRect, slope, horiz, vert))
	{
		*move = true;
	}
	if (!Interpolate1(data2, visRect, slope, horiz, vert))
	{
		*mark = false;
	}

	if (!visRect.Contains(data1->x, data1->y) ||
		!visRect.Contains(data2->x, data2->y))
	{
		*move = false;
		*draw = false;
		*mark = false;
	}
}

/*******************************************************************************
 Interpolate1 (private)

	Returns false if the point was moved.

 ******************************************************************************/

bool
J2DPlotWidget::Interpolate1
	(
	J2DDataPoint*		data,
	const J2DDataRect&	visRect,
	const JFloat		slope,
	const bool		horiz,
	const bool		vert
	)
	const
{
	if (visRect.Contains(data->x, data->y))
	{
		return true;
	}

	if (horiz)
	{
		data->x = JMin(data->x, visRect.xmax);
		data->x = JMax(data->x, visRect.xmin);
	}
	else if (vert)
	{
		data->y = JMin(data->y, visRect.ymax);
		data->y = JMax(data->y, visRect.ymin);
	}
	else
	{
		if (data->x < visRect.xmin)
		{
			data->y += slope*(visRect.xmin - data->x);
			data->x  = visRect.xmin;
		}
		else if (data->x > visRect.xmax)
		{
			data->y -= slope*(data->x - visRect.xmax);
			data->x  = visRect.xmax;
		}

		if (data->y < visRect.ymin)
		{
			data->x += (visRect.ymin - data->y)/slope;
			data->y  = visRect.ymin;
		}
		else if (data->y > visRect.ymax)
		{
			data->x -= (data->y - visRect.ymax)/slope;
			data->y  = visRect.ymax;
		}
	}

	return false;
}

/*******************************************************************************
 DrawSymbol

 ******************************************************************************/

static JPolygon diamondSymbol(4), triangleSymbol(3);

void
J2DPlotWidget::DrawSymbol
	(
	JPainter&			p,
	const JPoint&		pt,
	const J2DSymbolType	type
	)
	const
{
	const JPoint origPenLoc = p.GetPenLocation();

	if (type == kCircle)
	{
		p.Ellipse(pt.x - 2, pt.y - 2, 5, 5);
	}

	else if (type == kDiamond)
	{
		if (diamondSymbol.IsEmpty())
		{
			diamondSymbol.AppendElement(JPoint(-3,  0));
			diamondSymbol.AppendElement(JPoint( 0, -3));
			diamondSymbol.AppendElement(JPoint( 3,  0));
			diamondSymbol.AppendElement(JPoint( 0,  3));
		}

		p.Polygon(pt, diamondSymbol);
	}

	else if (type == kSquare)
	{
		p.Rect(pt.x - 2, pt.y - 2, 5, 5);
	}

	else if (type == kTriangle)
	{
		if (triangleSymbol.IsEmpty())
		{
			triangleSymbol.AppendElement(JPoint(-2,  2));
			triangleSymbol.AppendElement(JPoint( 0, -2));
			triangleSymbol.AppendElement(JPoint( 2,  2));
		}

		p.Polygon(pt, triangleSymbol);
	}

	else if (type == kPlus)
	{
		p.Line(pt.x, pt.y - 3, pt.x, pt.y + 3);
		p.Line(pt.x - 3, pt.y, pt.x + 3, pt.y);
	}

	else if (type == kCross)
	{
		p.Line(pt.x - 3, pt.y - 3, pt.x + 3, pt.y + 3);
		p.Line(pt.x - 3, pt.y + 3, pt.x + 3, pt.y - 3);
	}

	p.SetPenLocation(origPenLoc);
}

/*******************************************************************************
 GetFirstAvailableSymbol

 ******************************************************************************/

J2DSymbolType
J2DPlotWidget::GetFirstAvailableSymbol()
{
	assert( kCircle == 0 );
	JSize min             = itsSymbolUsage [ kCircle ];
	J2DSymbolType minType = kCircle;

	for (JIndex type=1; type<kSymbolCount; type++)
	{
		if (itsSymbolUsage[type] < min)
		{
			min     = itsSymbolUsage[type];
			minType = (J2DSymbolType) type;
		}
	}

	itsSymbolUsage[minType]++;
	return minType;
}

/*******************************************************************************
 GetFirstAvailableColor

 ******************************************************************************/

JIndex
J2DPlotWidget::GetFirstAvailableColor()
{
	JSize min       = itsColorUsage->GetElement(1);
	JIndex minIndex = 1;

	const JSize count = itsColorUsage->GetElementCount();
	for (JIndex color=2; color<=count; color++)
	{
		const JSize usage = itsColorUsage->GetElement(color);
		if (usage < min)
		{
			min      = usage;
			minIndex = color;
		}
	}

	itsColorUsage->SetElement(minIndex, min + 1);
	return minIndex;
}

/******************************************************************************
 PWWriteSetup

 ******************************************************************************/

void
J2DPlotWidget::PWWriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << GetFontName();
	output << ' ' << GetFontSize();
	output << ' ' << GetXLabel();
	output << ' ' << GetYLabel();
	output << ' ' << GetTitle();
	output << ' ' << JBoolToString(itsShowFrameFlag)
				  << JBoolToString(itsShowGridFlag)
				  << JBoolToString(itsShowLegendFlag)
				  << JBoolToString(itsXAxisIsLinear)
				  << JBoolToString(itsYAxisIsLinear)
				  << JBoolToString(itsIsZoomedFlag);
	output << ' ' << itsXScale[kMin];
	output << ' ' << itsXScale[kMax];
	output << ' ' << itsXScale[kInc];
	output << ' ' << itsYScale[kMin];
	output << ' ' << itsYScale[kMax];
	output << ' ' << itsYScale[kInc];

	output << ' ' << JBoolToString(itsUsingRange);
	if (itsUsingRange)
	{
		output << ' ' << itsRangeXMin;
		output << ' ' << itsRangeXMax;
		output << ' ' << itsRangeYMin;
		output << ' ' << itsRangeYMax;
	}

	// Cursors
	output << ' ' << JBoolToString(itsXCursorVisible)
				  << JBoolToString(itsYCursorVisible)
				  << JBoolToString(itsDualCursors);
	output << ' ' << itsXCursorPos1;
	output << ' ' << itsXCursorPos2;
	output << ' ' << itsYCursorPos1;
	output << ' ' << itsYCursorPos2;
	output << ' ' << itsXCursorVal1;
	output << ' ' << itsXCursorVal2;
	output << ' ' << itsYCursorVal1;
	output << ' ' << itsYCursorVal2;

	// Marks
	JSize i;
	output << ' ' << GetXMarkCount();
	for (i = 1; i <= GetXMarkCount(); i++)
	{
		output << ' ' << itsXMarks->GetElement(i);
	}
	output << ' ' << GetYMarkCount();
	for (i = 1; i <= GetYMarkCount(); i++)
	{
		output << ' ' << itsYMarks->GetElement(i);
	}

	output << ' ';
}

/******************************************************************************
 PWReadSetup

	This uses assert() to check the version number because we cannot use
	a marker to skip the data (since it contains strings), so it is the
	client's responsibility to check whether or not the file can be
	read (via a higher level version number).

 ******************************************************************************/

void
J2DPlotWidget::PWReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers = 0;
	input >> std::ws;
	if (isdigit(input.peek()))		// original code started with a string
	{
		input >> vers;
		assert( vers <= kCurrentSetupVersion );
	}

	JString temp;
	input >> temp;
	SetFontName(temp);
	JSize size;
	input >> size;
	SetFontSize(size);
	input >> temp;
	SetXLabel(temp);
	input >> temp;
	SetYLabel(temp);
	input >> temp;
	SetTitle(temp);
	bool bTest;
	input >> JBoolFromString(bTest);
	ShowFrame(bTest);
	input >> JBoolFromString(bTest);
	ShowGrid(bTest);
	input >> JBoolFromString(bTest);
	ShowLegend(bTest);
	input >> JBoolFromString(itsXAxisIsLinear);
	input >> JBoolFromString(itsYAxisIsLinear);
	input >> JBoolFromString(itsIsZoomedFlag);
	input >> itsXScale[kMin];
	input >> itsXScale[kMax];
	input >> itsXScale[kInc];
	input >> itsYScale[kMin];
	input >> itsYScale[kMax];
	input >> itsYScale[kInc];
	input >> JBoolFromString(itsUsingRange);
	if (itsUsingRange)
	{
		input >> itsRangeXMin >> itsRangeXMax;
		input >> itsRangeYMin >> itsRangeYMax;
	}

	SetPlotDecPlaces();
	Broadcast(ScaleChanged());
	UpdatePlot(true);

	// Cursors
	input >> JBoolFromString(itsXCursorVisible);
	input >> JBoolFromString(itsYCursorVisible);
	input >> JBoolFromString(itsDualCursors);
	if (vers == 0)
	{
		if (itsXCursorVisible)
		{
			input >> itsXCursorPos1;
			if (itsDualCursors)
			{
				input >> itsXCursorPos2;
			}
		}
		if (itsYCursorVisible)
		{
			input >> itsYCursorPos1;
			if (itsDualCursors)
			{
				input >> itsYCursorPos2;
			}
		}
	}
	else
	{
		input >> itsXCursorPos1 >> itsXCursorPos2;
		input >> itsYCursorPos1 >> itsYCursorPos2;
		input >> itsXCursorVal1 >> itsXCursorVal2;
		input >> itsYCursorVal1 >> itsYCursorVal2;
	}

	// Marks
	JSize i, count;
	input >> count;
	for (i = 1; i <= count; i++)
	{
		JFloat value;
		input >> value;
		itsXMarks->AppendElement(value);
		Broadcast(CursorMarked(true, value));
	}
	input >> count;
	for (i = 1; i <= count; i++)
	{
		JFloat value;
		input >> value;
		itsYMarks->AppendElement(value);
		Broadcast(CursorMarked(false, value));
	}

	Broadcast(CursorsChanged(	itsXCursorVisible,
								itsYCursorVisible,
								itsDualCursors,
								itsXCursorVal1,
								itsXCursorVal2,
								itsYCursorVal1,
								itsYCursorVal2));
}

/******************************************************************************
 PWWriteCurveSetup

 ******************************************************************************/

void
J2DPlotWidget::PWWriteCurveSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentCurveSetupVersion;

	// write sizes first so they can be checked immediately when reading

	const JSize curveCount = itsCurves->GetElementCount();
	output << ' ' << curveCount;

	const JSize colorCount = itsColors->GetElementCount();
	output << ' ' << colorCount;

	output << ' ' << (JSize) kSymbolCount;

	// write curve info

	for (JIndex i=1; i<=curveCount; i++)
	{
		J2DCurveInfo info = itsCurveInfo->GetElement(i);
		output << ' ' << info;
	}

	// must be able to restore color and symbol usage

	for (JIndex i=1; i<=colorCount; i++)
	{
		output << ' ' << itsColorUsage->GetElement(i);
	}

	for (auto v : itsSymbolUsage)
	{
		output << ' ' << v;
	}

	output << ' ';
}

/******************************************************************************
 PWReadCurveSetup

	This uses assert() to check the version number because we cannot use
	a marker to skip the data (since it contains strings), so it is the
	client's responsibility to check whether or not the file can be
	read (via a higher level version number).

	This function does nothing (i.e. skips the data) if the number of
	curves in the plot does not match the number of curves in the data
	stream, or if there are fewer colors in the plot than in the data
	stream.

 ******************************************************************************/

void
J2DPlotWidget::PWReadCurveSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentCurveSetupVersion );

	// check sizes first so we know whether the data can be used

	const JSize origCurveCount = itsCurves->GetElementCount();
	const JSize origColorCount = itsColors->GetElementCount();

	JSize curveCount, colorCount, symbolCount;
	input >> curveCount >> colorCount >> symbolCount;

	const bool setData = curveCount  == origCurveCount &&
								   colorCount  <= origColorCount &&
								   symbolCount <= kSymbolCount;

	// curve info

	JString name;
	for (JIndex i=1; i<=curveCount; i++)
	{
		J2DCurveInfo info;
		info.name = jnew JString;
		assert(info.name != nullptr);
		input >> info;

		if (setData)
		{
			itsCurveInfo->SetElement(i,info);
		}
	}

	// restore color and symbol usage

	JSize usage;
	for (JIndex i=1; i<=colorCount; i++)
	{
		input >> usage;
		if (setData)
		{
			itsColorUsage->SetElement(i, usage);
		}
	}
	if (setData)
	{
		for (JIndex i=colorCount+1; i<=origColorCount; i++)
		{
			itsColorUsage->SetElement(i, 0);
		}
	}

	for (JUnsignedOffset i=0; i<symbolCount; i++)
	{
		input >> usage;
		if (setData)
		{
			itsSymbolUsage[i] = usage;
		}
	}
	if (setData)
	{
		for (JUnsignedOffset i=symbolCount; i<kSymbolCount; i++)
		{
			itsSymbolUsage[i] = 0;
		}
	}
}

/******************************************************************************
 GetMouseCursor (protected)

 ******************************************************************************/

J2DPlotWidget::MouseCursor
J2DPlotWidget::GetMouseCursor
	(
	const JPoint& pt
	)
	const
{
	if ((itsShowLegendFlag && itsLegendRect.Contains(pt)) ||
		itsTitleRect.Contains(pt) ||
		itsXLabelRect.Contains(pt) ||
		itsYLabelRect.Contains(pt) ||
		itsXAxisRect.Contains(pt) ||
		itsYAxisRect.Contains(pt))
	{
		return kDblClickChangeCursor;
	}
	else
	{
		const CursorIndex cursor = GetCursorIndex(pt);
		if (cursor == kX1 || cursor == kX2)
		{
			return kDragXCursor;
		}
		else if (cursor == kY1 || cursor == kY2)
		{
			return kDragYCursor;
		}
		else
		{
			return kZoomCursor;
		}
	}
}

/*******************************************************************************
 GetCursorIndex (protected)

 ******************************************************************************/

J2DPlotWidget::CursorIndex
J2DPlotWidget::GetCursorIndex
	(
	const JPoint& pt
	)
	const
{
	if (!JRect(itsYAxisEnd,   itsXAxisStart,
			   itsYAxisStart, itsXAxisEnd).Contains(pt))
	{
		return kNoCursor;
	}
	if (itsXCursorVisible &&
		(pt.x >= itsXCursorPos1 - 3) &&
		(pt.x <= itsXCursorPos1 + 3))
	{
		return kX1;
	}
	else if (itsXCursorVisible && itsDualCursors &&
			 (pt.x >= itsXCursorPos2 - 3) &&
			 (pt.x <= itsXCursorPos2 + 3))
	{
		return kX2;
	}
	else if (itsYCursorVisible &&
			 (pt.y >= itsYCursorPos1 - 3) &&
			 (pt.y <= itsYCursorPos1 + 3))
	{
		return kY1;
	}
	else if (itsYCursorVisible && itsDualCursors &&
			 (pt.y >= itsYCursorPos2 - 3) &&
			 (pt.y <= itsYCursorPos2 + 3))
	{
		return kY2;
	}
	else
	{
		return kNoCursor;
	}
}

/******************************************************************************
 PWHandleMouseDown (protected)

 ******************************************************************************/

void
J2DPlotWidget::PWHandleMouseDown
	(
	const JPoint&	pt,
	const JSize	clickCount
	)
{
	itsIsCursorDragging = false;

	if (clickCount == 1)
	{
		itsSelectedCursor   = GetCursorIndex(pt);
		itsIsCursorDragging = itsSelectedCursor != kNoCursor;

		JPainter* p = PWCreateDragInsidePainter();
		if (itsIsCursorDragging)
		{
			if ((itsSelectedCursor == kX1) || (itsSelectedCursor == kX2))
			{
				p->Line(pt.x, itsYAxisStart, pt.x, itsYAxisEnd);
			}
			else
			{
				p->Line(itsXAxisStart, pt.y, itsXAxisEnd, pt.y);
			}
		}
		else
		{
			p->Rect(JRect(pt, pt));
		}
		itsStartPt = itsPrevPt = pt;
	}
	else if (clickCount == 2)
	{
		JIndex curveIndex;
		JPainter* p = nullptr;
		if (PWGetDragPainter(&p))
		{
			PWDeleteDragPainter();
		}
		if (itsYLabelRect.Contains(pt))
		{
			ChangeLabels(kYLabel);
		}
		else if (itsXLabelRect.Contains(pt))
		{
			ChangeLabels(kXLabel);
		}
		else if (itsTitleRect.Contains(pt))
		{
			ChangeLabels(kTitle);
		}
		else if (itsXAxisRect.Contains(pt))
		{
			ChangeScale(true);
		}
		else if (itsYAxisRect.Contains(pt))
		{
			ChangeScale(false);
		}
		else if (GetLegendIndex(pt, &curveIndex))
		{
			ChangeCurveOptions(curveIndex);
		}
	}
}

/******************************************************************************
 GetLegendIndex (protected)

	Returns the curve index clicked on in the legend.

 ******************************************************************************/

bool
J2DPlotWidget::GetLegendIndex
	(
	const JPoint&	pt,
	JIndex*			curveIndex
	)
	const
{
	if (itsShowLegendFlag && itsLegendRect.Contains(pt))
	{
		*curveIndex = 1 +
			(pt.y - (itsLegendRect.top + itsLineHeight)) / itsLineHeight;
		return itsCurves->IndexValid(*curveIndex);
	}
	else
	{
		*curveIndex = false;
		return false;
	}
}

/******************************************************************************
 PWHandleMouseDrag (protected)

 ******************************************************************************/

void
J2DPlotWidget::PWHandleMouseDrag
	(
	const JPoint& pt
	)
{
	JPainter* p = nullptr;
	if (pt != itsPrevPt && PWGetDragPainter(&p)) // no painter for multiple click
	{
		const JPoint newPt =
			JPinInRect(pt, JRect(itsYAxisEnd,   itsXAxisStart,
								 itsYAxisStart, itsXAxisEnd));

		const bool draggingX =
			(itsSelectedCursor == kX1) || (itsSelectedCursor == kX2);
		if (itsIsCursorDragging &&
			(( draggingX && newPt.x != itsPrevPt.x) ||
			 (!draggingX && newPt.y != itsPrevPt.y)))
		{
			if (draggingX)
			{
				p->Line(itsPrevPt.x, itsYAxisStart, itsPrevPt.x, itsYAxisEnd);
				p->Line(newPt.x, itsYAxisStart, newPt.x, itsYAxisEnd);
			}
			else
			{
				p->Line(itsXAxisStart, itsPrevPt.y, itsXAxisEnd, itsPrevPt.y);
				p->Line(itsXAxisStart, newPt.y, itsXAxisEnd, newPt.y);
			}

			if (itsSelectedCursor == kX1)
			{
				itsXCursorPos1 = newPt.x;
			}
			else if (itsSelectedCursor == kX2)
			{
				itsXCursorPos2 = newPt.x;
			}
			else if (itsSelectedCursor == kY1)
			{
				itsYCursorPos1 = newPt.y;
			}
			else if (itsSelectedCursor == kY2)
			{
				itsYCursorPos2 = newPt.y;
			}

			itsPrevPt = newPt;
			UpdateCursorInternals();
			PWRedrawRect(JRect(PWGetHeight() - GetCursorLabelHeight(*p), 0,
								PWGetHeight(), PWGetWidth()));
		}
		else if (!itsIsCursorDragging)
		{
			p->Rect(JRect(itsStartPt, itsPrevPt));
			p->Rect(JRect(itsStartPt, pt));
			itsPrevPt = pt;
			PWDisplayCursor(kZoomCursor);
		}
	}
}

/******************************************************************************
 PWHandleMouseUp (protected)

 ******************************************************************************/

void
J2DPlotWidget::PWHandleMouseUp
	(
	const JPoint&	pt,
	const bool	cleanZoom
	)
{
	JPainter* p = nullptr;
	if (PWGetDragPainter(&p))		// no painter for multiple click
	{
		if (itsIsCursorDragging)
		{
			if ((itsSelectedCursor == kX1) || (itsSelectedCursor == kX2))
			{
				p->Line(itsPrevPt.x, itsYAxisStart, itsPrevPt.x, itsYAxisEnd);
			}
			else
			{
				p->Line(itsXAxisStart, itsPrevPt.y, itsXAxisEnd, itsPrevPt.y);
			}
			PWDeleteDragPainter();
			PWRefresh();
		}
		else
		{
			JRect rect(itsStartPt, itsPrevPt);
			p->Rect(rect);
			PWDeleteDragPainter();

			if (itsPrevPt != itsStartPt)
			{
				Zoom(rect, cleanZoom);
			}
		}
	}
	itsIsCursorDragging = false;
}

/******************************************************************************
 GetXMarkValue

 ******************************************************************************/

JFloat
J2DPlotWidget::GetXMarkValue
	(
	const JIndex index
	)
	const
{
	assert(itsXMarks->IndexValid(index));
	return itsXMarks->GetElement(index);
}

/******************************************************************************
 GetYMarkValue

 ******************************************************************************/

JFloat
J2DPlotWidget::GetYMarkValue
	(
	const JIndex index
	)
	const
{
	assert(itsYMarks->IndexValid(index));
	return itsYMarks->GetElement(index);
}

/******************************************************************************
 MarkCurrentCursor

 ******************************************************************************/

void
J2DPlotWidget::MarkCurrentCursor()
{
	if (itsSelectedCursor == kX1)
	{
		itsXMarks->AppendElement(itsXCursorVal1);
		Broadcast(CursorMarked(true, itsXCursorVal1));
		Broadcast(PlotChanged());
	}
	else if (itsSelectedCursor == kX2)
	{
		itsXMarks->AppendElement(itsXCursorVal2);
		Broadcast(CursorMarked(true, itsXCursorVal2));
		Broadcast(PlotChanged());
	}
	else if (itsSelectedCursor == kY1)
	{
		itsYMarks->AppendElement(itsYCursorVal1);
		Broadcast(CursorMarked(false, itsYCursorVal1));
		Broadcast(PlotChanged());
	}
	else if (itsSelectedCursor == kY2)
	{
		itsYMarks->AppendElement(itsYCursorVal2);
		Broadcast(CursorMarked(false, itsYCursorVal2));
		Broadcast(PlotChanged());
	}
}

/******************************************************************************
 MarkAllVisibleCursors

 ******************************************************************************/

void
J2DPlotWidget::MarkAllVisibleCursors()
{
	if (itsXCursorVisible)
	{
		itsXMarks->AppendElement(itsXCursorVal1);
		Broadcast(CursorMarked(true, itsXCursorVal1));

		if (itsDualCursors)
		{
			itsXMarks->AppendElement(itsXCursorVal2);
			Broadcast(CursorMarked(true, itsXCursorVal2));
		}
	}

	if (itsYCursorVisible)
	{
		itsYMarks->AppendElement(itsYCursorVal1);
		Broadcast(CursorMarked(false, itsYCursorVal1));

		if (itsDualCursors)
		{
			itsYMarks->AppendElement(itsYCursorVal2);
			Broadcast(CursorMarked(false, itsYCursorVal2));
		}
	}

	if (itsXCursorVisible || itsYCursorVisible)
	{
		Broadcast(PlotChanged());
	}
}

/******************************************************************************
 RemoveAllMarks

 ******************************************************************************/

void
J2DPlotWidget::RemoveAllMarks()
{
	itsYMarks->RemoveAll();
	itsXMarks->RemoveAll();
	Broadcast(MarkRemoved(false, 1));
	Broadcast(MarkRemoved(true, 1));
	Broadcast(PlotChanged());
	PWRefresh();
}

/******************************************************************************
 RemoveXMark

 ******************************************************************************/

void
J2DPlotWidget::RemoveXMark
	(
	const JIndex index
	)
{
	itsXMarks->RemoveElement(index);
	Broadcast(MarkRemoved(true, index));
	Broadcast(PlotChanged());
	PWRefresh();
}

/******************************************************************************
 RemoveYMark

 ******************************************************************************/

void
J2DPlotWidget::RemoveYMark
	(
	const JIndex index
	)
{
	itsYMarks->RemoveElement(index);
	Broadcast(MarkRemoved(false, index));
	Broadcast(PlotChanged());
	PWRefresh();
}

/******************************************************************************
 ToggleXCursor

 ******************************************************************************/

void
J2DPlotWidget::ToggleXCursor()
{
	if (itsXCursorVisible && (itsSelectedCursor == kX1 || itsSelectedCursor == kX2))
	{
		itsSelectedCursor = kNoCursor;
	}
	itsXCursorVisible = !itsXCursorVisible;
	UpdatePlot(true);
	PWRefresh();
}

/******************************************************************************
 ToggleYCursor

 ******************************************************************************/

void
J2DPlotWidget::ToggleYCursor()
{
	if (itsYCursorVisible && (itsSelectedCursor == kY1 || itsSelectedCursor == kY2))
	{
		itsSelectedCursor = kNoCursor;
	}
	itsYCursorVisible = !itsYCursorVisible;
	UpdatePlot(true);
	PWRefresh();
}

/******************************************************************************
 ToggleDualCursors

 ******************************************************************************/

void
J2DPlotWidget::ToggleDualCursors()
{
	if (itsSelectedCursor == kX2 || itsSelectedCursor == kY2)
	{
		itsSelectedCursor = kNoCursor;
	}
	itsDualCursors = !itsDualCursors;
	UpdatePlot(true);
	PWRefresh();
}

/*******************************************************************************
 DrawCursors (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawCursors
	(
	JPainter& p
	)
{
	JCoordinate xmin, xmax, ymin, ymax;
	GetFrameGeometry(&xmin, &xmax, &ymin, &ymax);

	if (itsXCursorVisible)
	{
		if (itsSelectedCursor == kX1)
		{
			p.SetPenColor(itsSelectionColor);
		}
		else
		{
			p.SetPenColor(itsBlackColor);
		}
		if (xmin <= itsXCursorPos1 && itsXCursorPos1 <= xmax)
		{
			p.Line(itsXCursorPos1, ymin, itsXCursorPos1, ymax);
		}

		if (itsDualCursors)
		{
			if (itsSelectedCursor == kX2)
			{
				p.SetPenColor(itsSelectionColor);
			}
			else
			{
				p.SetPenColor(itsBlackColor);
			}
			if (xmin <= itsXCursorPos2 && itsXCursorPos2 <= xmax)
			{
				p.Line(itsXCursorPos2, ymin, itsXCursorPos2, ymax);
			}
		}
	}

	if (itsYCursorVisible)
	{
		if (itsSelectedCursor == kY1)
		{
			p.SetPenColor(itsSelectionColor);
		}
		else
		{
			p.SetPenColor(itsBlackColor);
		}
		if (ymax <= itsYCursorPos1 && itsYCursorPos1 <= ymin)
		{
			p.Line(xmin, itsYCursorPos1, xmax, itsYCursorPos1);
		}

		if (itsDualCursors)
		{
			if (itsSelectedCursor == kY2)
			{
				p.SetPenColor(itsSelectionColor);
			}
			else
			{
				p.SetPenColor(itsBlackColor);
			}
			if (ymax <= itsYCursorPos2 && itsYCursorPos2 <= ymin)
			{
				p.Line(xmin, itsYCursorPos2, xmax, itsYCursorPos2);
			}
		}
	}
}

/*******************************************************************************
 DrawCursorLabels (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawCursorLabels
	(
	JPainter& p
	)
{
	if (!itsXCursorVisible && !itsYCursorVisible)
	{
		return;
	}
	JSize labelHeight	= p.GetLineHeight();
	JSize cursorHeight	= GetCursorLabelHeight(p);
	JSize strwidth		= p.GetStringWidth(JString("x1 = 0000000000", JString::kNoCopy));
	JSize totalWidth	= 3 * strwidth + 2 * kCursorValueBuffer;
	JSize axisWidth		= itsXAxisEnd - itsXAxisStart;
	JCoordinate startx	= itsXAxisStart;

	if (((itsXCursorVisible && !itsYCursorVisible) ||
		 (!itsXCursorVisible && itsYCursorVisible)) && !itsDualCursors)
	{
		totalWidth = strwidth;
	}
	else if (itsXCursorVisible && itsYCursorVisible && !itsDualCursors)
	{
		totalWidth = 2 * strwidth + kCursorValueBuffer;
	}

	if (axisWidth > totalWidth)
	{
		startx += (axisWidth - totalWidth)/2;
	}

	JCoordinate starty = PWGetHeight() - cursorHeight + kCursorValueBuffer;

	if (itsXCursorVisible)
	{
		JString x1("x1 = ");
		x1 += FloatToString(itsXCursorVal1);

		p.String(startx,
				starty, x1, strwidth, JPainter::kHAlignLeft, labelHeight,
				JPainter::kVAlignCenter);

		if (itsDualCursors)
		{
			JString x2("x2 = ");
			x2 += FloatToString(itsXCursorVal2);

			JString dx("dx = ");
			dx += FloatToString(itsXCursorVal2 - itsXCursorVal1);

			p.String(startx + kCursorValueBuffer + strwidth,
					starty, x2, strwidth, JPainter::kHAlignLeft, labelHeight,
					JPainter::kVAlignCenter);
			p.String(startx + 2 * kCursorValueBuffer + 2 * strwidth,
					starty, dx, strwidth, JPainter::kHAlignLeft, labelHeight,
					JPainter::kVAlignCenter);
		}
	}

	if (itsYCursorVisible)
	{
		JString y1("y1 = ");
		y1 += FloatToString(itsYCursorVal1);

		if (itsDualCursors)
		{
			if (itsXCursorVisible)
			{
				starty += labelHeight;
			}

			JString y2("y2 = ");
			y2 += FloatToString(itsYCursorVal2);

			JString dy("dy = ");
			dy += FloatToString(itsYCursorVal2 - itsYCursorVal1);

			p.String(startx,
					starty, y1, strwidth, JPainter::kHAlignLeft, labelHeight,
					JPainter::kVAlignCenter);
			p.String(startx + kCursorValueBuffer + strwidth,
					starty, y2, strwidth, JPainter::kHAlignLeft, labelHeight,
					JPainter::kVAlignCenter);
			p.String(startx + 2 * kCursorValueBuffer + 2 * strwidth,
					starty, dy, strwidth, JPainter::kHAlignLeft, labelHeight,
					JPainter::kVAlignCenter);
		}
		else if (itsXCursorVisible)
		{
			p.String(startx + kCursorValueBuffer + strwidth,
					starty, y1, strwidth, JPainter::kHAlignLeft, labelHeight,
					JPainter::kVAlignCenter);
		}
		else
		{
			p.String(startx,
					starty, y1, strwidth, JPainter::kHAlignLeft, labelHeight,
					JPainter::kVAlignCenter);
		}
	}

}

/******************************************************************************
 GetCursorLabelHeight (private)

 ******************************************************************************/

JSize
J2DPlotWidget::GetCursorLabelHeight
	(
	JPainter& p
	)
	const
{
	const JSize labelHeight = p.GetLineHeight();

	JSize h = 0;
	if (itsXCursorVisible || itsYCursorVisible)
	{
		if (itsDualCursors && itsXCursorVisible && itsYCursorVisible)
		{
			h = 2 * (labelHeight + kCursorValueBuffer);
		}
		else
		{
			h = labelHeight + 2 * kCursorValueBuffer;
		}
	}
	return h;
}

/*******************************************************************************
 DrawMarks (private)

 ******************************************************************************/

void
J2DPlotWidget::DrawMarks
	(
	JPainter& p
	)
{
	p.SetPenColor(itsGrayColor);
	JCoordinate xmin, xmax, ymin, ymax;
	GetFrameGeometry(&xmin, &xmax, &ymin, &ymax);
	JSize i;
	for (i = 1; i <= itsXMarks->GetElementCount(); i++)
	{
		JFloat value = itsXMarks->GetElement(i);
		JCoordinate x;
		if (GetFrameX(value, &x))
		{
			p.Line(x, ymin, x, ymax);
		}
	}
	for (i = 1; i <= itsYMarks->GetElementCount(); i++)
	{
		JFloat value = itsYMarks->GetElement(i);
		JCoordinate y;
		if (GetFrameY(value, &y))
		{
			p.Line(xmin, y, xmax, y);
		}
	}
	p.SetPenColor(itsBlackColor);
}

/******************************************************************************
 PWHandleKeyPress (protected)

 ******************************************************************************/

// x visible, y visible, dual visible, reverse, current cursor

static const J2DPlotWidget::CursorIndex kNextCursor[2][2][2][2][5] =
{
{
	{
		{	// x=0, y=0, dual=0
			{ J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor },
			{ J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor }
		},
		{	// x=0, y=0, dual=1
			{ J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor },
			{ J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor, J2DPlotWidget::kNoCursor }
		}
	},
	{
		{	// x=0, y=1, dual=0
			{ J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY1 },
			{ J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY1 }
		},
		{	// x=0, y=1, dual=1
			{ J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY2, J2DPlotWidget::kY1 },
			{ J2DPlotWidget::kY2, J2DPlotWidget::kY2, J2DPlotWidget::kY2, J2DPlotWidget::kY2, J2DPlotWidget::kY1 }
		}
	}
},
{
	{
		{	// x=1, y=0, dual=0
			{ J2DPlotWidget::kX1, J2DPlotWidget::kX1, J2DPlotWidget::kX1, J2DPlotWidget::kX1, J2DPlotWidget::kX1 },
			{ J2DPlotWidget::kX1, J2DPlotWidget::kX1, J2DPlotWidget::kX1, J2DPlotWidget::kX1, J2DPlotWidget::kX1 }
		},
		{	// x=1, y=0, dual=1
			{ J2DPlotWidget::kX1, J2DPlotWidget::kX2, J2DPlotWidget::kX1, J2DPlotWidget::kX1, J2DPlotWidget::kX1 },
			{ J2DPlotWidget::kX2, J2DPlotWidget::kX2, J2DPlotWidget::kX1, J2DPlotWidget::kX2, J2DPlotWidget::kX2 }
		}
	},
	{
		{	// x=1, y=1, dual=0
			{ J2DPlotWidget::kX1, J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kX1, J2DPlotWidget::kX1 },
			{ J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kY1, J2DPlotWidget::kX1, J2DPlotWidget::kX1 }
		},
		{	// x=1, y=1, dual=1
			{ J2DPlotWidget::kX1, J2DPlotWidget::kX2, J2DPlotWidget::kY1, J2DPlotWidget::kY2, J2DPlotWidget::kX1 },
			{ J2DPlotWidget::kY2, J2DPlotWidget::kY2, J2DPlotWidget::kX1, J2DPlotWidget::kX2, J2DPlotWidget::kY1 }
		}
	}
}
};

void
J2DPlotWidget::PWHandleKeyPress
	(
	const JUtf8Character&	key,
	const bool			reverse,
	const bool			interval,
	const bool			skip
	)
{
	if (key == kJTabKey)
	{
		const CursorIndex newCursor = kNextCursor
			[ itsXCursorVisible ? 1 : 0 ]
			[ itsYCursorVisible ? 1 : 0 ]
			[ itsDualCursors    ? 1 : 0 ]
			[ reverse           ? 1 : 0 ]
			[ itsSelectedCursor         ];
		if (newCursor != itsSelectedCursor)
		{
			itsSelectedCursor = newCursor;
			PWRefresh();
		}
	}

	else if (key == kJLeftArrow &&
			 (itsSelectedCursor == kX1 || itsSelectedCursor == kX2))
	{
		if (MoveCursor(itsXAxisStart, itsXAxisEnd,
					   -(interval ? 10 : 1),
					   (itsSelectedCursor == kX1 ?
							(&itsXCursorPos1) : (&itsXCursorPos2))))
		{
			UpdateCursorInternals();
			PWRefresh();
		}
	}
	else if (key == kJRightArrow &&
			 (itsSelectedCursor == kX1 || itsSelectedCursor == kX2))
	{
		if (MoveCursor(itsXAxisStart, itsXAxisEnd,
					   (interval ? 10 : 1),
					   (itsSelectedCursor == kX1 ?
							(&itsXCursorPos1) : (&itsXCursorPos2))))
		{
			UpdateCursorInternals();
			PWRefresh();
		}
	}

	else if (key == kJUpArrow &&
			 (itsSelectedCursor == kY1 || itsSelectedCursor == kY2))
	{
		if (MoveCursor(itsYAxisEnd, itsYAxisStart,
					   -(interval ? 10 : 1),
					   (itsSelectedCursor == kY1 ?
							(&itsYCursorPos1) : (&itsYCursorPos2))))
		{
			UpdateCursorInternals();
			PWRefresh();
		}
	}
	else if (key == kJDownArrow &&
			 (itsSelectedCursor == kY1 || itsSelectedCursor == kY2))
	{
		if (MoveCursor(itsYAxisEnd, itsYAxisStart,
					   (interval ? 10 : 1),
					   (itsSelectedCursor == kY1 ?
							(&itsYCursorPos1) : (&itsYCursorPos2))))
		{
			UpdateCursorInternals();
			PWRefresh();
		}
	}

	else if (key == kJEscapeKey)
	{
		if (itsSelectedCursor != kNoCursor)
		{
			itsSelectedCursor = kNoCursor;
			PWRefresh();
		}
	}
}

/*******************************************************************************
 MoveCursor (private)

	Returns true if *cursorPos changed.

 ******************************************************************************/

bool
J2DPlotWidget::MoveCursor
	(
	const JCoordinate	min,
	const JCoordinate	max,
	const JCoordinate	delta,
	JCoordinate*		cursorPos
	)
	const
{
	JCoordinate newPos = *cursorPos + delta;
	if (newPos < min)
	{
		newPos = min;
	}
	else if (newPos > max)
	{
		newPos = max;
	}

	if (*cursorPos != newPos)
	{
		*cursorPos = newPos;
		return true;
	}
	else
	{
		return false;
	}
}

/*******************************************************************************
 AdjustCursors

 ******************************************************************************/

void
J2DPlotWidget::AdjustCursors()
{
	JCoordinate xmin, xmax, ymin, ymax;
	GetFrameGeometry(&xmin, &xmax, &ymin, &ymax);
	if (xmin == xmax || ymin == ymax)
	{
		return;
	}

	JCoordinate pos;
	if (GetFrameX(itsXCursorVal1, &pos) && itsXCursor1InitFlag)
	{
		itsXCursorPos1 = pos;
	}
	else if (itsXCursorPos1 < xmin || xmax < itsXCursorPos1)
	{
		itsXCursorPos1 = xmin + 20;
	}
	itsXCursor1InitFlag = itsXCursor1InitFlag || itsXCursorVisible;

	if (GetFrameX(itsXCursorVal2, &pos) && itsXCursor2InitFlag)
	{
		itsXCursorPos2 = pos;
	}
	else if (itsXCursorPos2 < xmin || xmax < itsXCursorPos2)
	{
		itsXCursorPos2 = xmin + 40;
	}
	itsXCursor2InitFlag = itsXCursor2InitFlag || (itsXCursorVisible && itsDualCursors);

	if (GetFrameY(itsYCursorVal1, &pos) && itsYCursor1InitFlag)
	{
		itsYCursorPos1 = pos;
	}
	else if (itsYCursorPos1 < ymax || ymin < itsYCursorPos1)
	{
		itsYCursorPos1 = ymin - 20;
	}
	itsYCursor1InitFlag = itsYCursor1InitFlag || itsYCursorVisible;

	if (GetFrameY(itsYCursorVal2, &pos) && itsYCursor2InitFlag)
	{
		itsYCursorPos2 = pos;
	}
	else if (itsYCursorPos2 < ymax || ymin < itsYCursorPos2)
	{
		itsYCursorPos2 = ymin - 40;
	}
	itsYCursor2InitFlag = itsYCursor2InitFlag || (itsYCursorVisible && itsDualCursors);

	itsXCursorVal1 = GetRealX(itsXCursorPos1);
	itsXCursorVal2 = GetRealX(itsXCursorPos2);
	itsYCursorVal1 = GetRealY(itsYCursorPos1);
	itsYCursorVal2 = GetRealY(itsYCursorPos2);
}

/*******************************************************************************
 UpdateCursorInternals (private)

 ******************************************************************************/

void
J2DPlotWidget::UpdateCursorInternals()
{
	itsXCursorVal1 = GetRealX(itsXCursorPos1);
	itsXCursorVal2 = GetRealX(itsXCursorPos2);
	itsYCursorVal1 = GetRealY(itsYCursorPos1);
	itsYCursorVal2 = GetRealY(itsYCursorPos2);
	Broadcast(CursorsChanged(	itsXCursorVisible,
								itsYCursorVisible,
								itsDualCursors,
								itsXCursorVal1,
								itsXCursorVal2,
								itsYCursorVal1,
								itsYCursorVal2));
	Broadcast(PlotChanged());
}
