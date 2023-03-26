/*********************************************************************************
 J2DPlotFunctionBase.cpp

	J2DPlotFunctionBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "J2DPlotFunctionBase.h"
#include "J2DPlotWidget.h"
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

const JSize kDefSampleCount = 100;	// # of points for approximating y range

/*********************************************************************************
 Constructor

 ********************************************************************************/

J2DPlotFunctionBase::J2DPlotFunctionBase
	(
	const Type		type,
	J2DPlotWidget*	plot,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	J2DPlotDataBase(type)
{
	itsPlot = plot;
	itsXMin = JMin(xMin, xMax);
	itsXMax = JMax(xMin, xMax);

	itsValues = jnew JArray<Point>(kDefSampleCount);
	assert( itsValues != nullptr );

	ListenTo(itsPlot);
}

/*********************************************************************************
 Destructor

 ********************************************************************************/

J2DPlotFunctionBase::~J2DPlotFunctionBase()
{
	jdelete itsValues;
}

/*********************************************************************************
 IsFunction (virtual)

 ********************************************************************************/

bool
J2DPlotFunctionBase::IsFunction()
	const
{
	return true;
}

/*********************************************************************************
 GetElement (virtual)

 ********************************************************************************/

void
J2DPlotFunctionBase::GetElement
	(
	const JIndex	index,
	J2DDataPoint*	data
	)
	const
{
	const Point pt = itsValues->GetElement(index);
	data->x        = pt.x;
	data->y        = pt.y;
}

/*********************************************************************************
 GetXRange (virtual)

 ********************************************************************************/

void
J2DPlotFunctionBase::GetXRange
	(
	JFloat* min,
	JFloat* max
	)
	const
{
	*min = itsXMin;
	*max = itsXMax;
}

/*********************************************************************************
 GetYRange (virtual)

 ********************************************************************************/

bool
J2DPlotFunctionBase::GetYRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const bool		xLinear,
	JFloat*			yMin,
	JFloat*			yMax
	)
	const
{
	const JSize pointCount = itsPlot->GetSmoothSteps();
	JArray<Point> list(pointCount);
	EvaluateFunction(xMin, xMax, xLinear, pointCount, &list);

	*yMin = *yMax = 0.0;

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const Point pt = list.GetElement(i);
		if (pt.y < *yMin || i == 1)
		{
			*yMin = pt.y;
		}
		if (pt.y > *yMax || i == 1)
		{
			*yMax = pt.y;
		}
	}

	return !list.IsEmpty();
}

/*********************************************************************************
 Receive (virtual protected)

 ********************************************************************************/

void
J2DPlotFunctionBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlot && message.Is(J2DPlotWidget::kScaleChanged))
	{
		UpdateFunction();
	}
	else
	{
		J2DPlotDataBase::Receive(sender, message);
	}
}

/*********************************************************************************
 UpdateFunction (protected)

 ********************************************************************************/

void
J2DPlotFunctionBase::UpdateFunction()
{
	JFloat min, max, inc;
	itsPlot->GetXScale(&min, &max, &inc);
	UpdateFunction(min, max, itsPlot->GetSmoothSteps());
}

void
J2DPlotFunctionBase::UpdateFunction
	(
	const JFloat	min,
	const JFloat	max,
	const JSize		stepCount
	)
{
	EvaluateFunction(min, max, itsPlot->XAxisIsLinear(), stepCount, itsValues);
	SetElementCount(itsValues->GetElementCount());
}

/*********************************************************************************
 EvaluateFunction (private)

 ********************************************************************************/

void
J2DPlotFunctionBase::EvaluateFunction
	(
	const JFloat	min,
	const JFloat	max,
	const bool		linear,
	const JSize		stepCount,
	JArray<Point>*	list
	)
	const
{
	list->RemoveAll();

	JFloat stepSize = 0.0, logMin = 0.0;
	if (linear)
	{
		stepSize = (max - min)/((JFloat) stepCount);
	}
	else
	{
		assert( min > 0.0 && max > 0.0 );
		stepSize = (log10(max) - log10(min))/((JFloat) stepCount);
		logMin   = log10(min);
	}

	Point pt;
	for (JIndex i = 0; i <= stepCount; i++)
	{
		if (linear)
		{
			pt.x = min + i*stepSize;
		}
		else
		{
			pt.x = pow(10, logMin + i*stepSize);
		}
		if (GetYValue(pt.x, &pt.y))
		{
			list->AppendElement(pt);
		}
	}
}
