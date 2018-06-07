/*********************************************************************************
 JPlotFunctionBase.cpp

	JPlotFunctionBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include <JPlotFunctionBase.h>
#include <J2DPlotWidget.h>
#include <JMinMax.h>
#include <jAssert.h>

const JSize kDefSampleCount = 100;	// # of points for approximating y range

/*********************************************************************************
 Constructor

 ********************************************************************************/

JPlotFunctionBase::JPlotFunctionBase
	(
	const Type		type,
	J2DPlotWidget*	plot,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotDataBase(type)
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

JPlotFunctionBase::~JPlotFunctionBase()
{
	jdelete itsValues;
}

/*********************************************************************************
 IsFunction (virtual)

 ********************************************************************************/

JBoolean
JPlotFunctionBase::IsFunction()
	const
{
	return kJTrue;
}

/*********************************************************************************
 GetElement (virtual)

 ********************************************************************************/

void
JPlotFunctionBase::GetElement
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
JPlotFunctionBase::GetXRange
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

JBoolean
JPlotFunctionBase::GetYRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const JBoolean	xLinear,
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
JPlotFunctionBase::Receive
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
		JPlotDataBase::Receive(sender, message);
		}
}

/*********************************************************************************
 UpdateFunction (protected)

 ********************************************************************************/

void
JPlotFunctionBase::UpdateFunction()
{
	JFloat min, max, inc;
	itsPlot->GetXScale(&min, &max, &inc);
	UpdateFunction(min, max, itsPlot->GetSmoothSteps());
}

void
JPlotFunctionBase::UpdateFunction
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
JPlotFunctionBase::EvaluateFunction
	(
	const JFloat	min,
	const JFloat	max,
	const JBoolean	linear,
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
		if (GetYValue(pt.x, &(pt.y)))
			{
			list->AppendElement(pt);
			}
		}
}
