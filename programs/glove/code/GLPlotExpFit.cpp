/*********************************************************************************
 GLPlotExpFit.cpp
 
	GLPlotExpFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotExpFit.h"
#include "J2DPlotWidget.h"
#include "JPlotDataBase.h"
#include "J2DPlotData.h"
#include <JString.h>
#include <JArray.h>
#include <jMath.h>
#include <jAssert.h>

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

GLPlotExpFit::GLPlotExpFit
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData, 
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotLinearFit(plot, fitData, xMin, xMax, kJFalse)
{
	SetFunctionName(JString("y = a Exp(bx)", kJFalse));
	itsXData = jnew JArray<JFloat>;
	itsYData = jnew JArray<JFloat>;
	itsHasXErrors = kJFalse;
	itsHasYErrors = kJFalse;
	if (fitData->HasXErrors())
		{
		itsXErrData = jnew JArray<JFloat>;
		itsHasXErrors = kJTrue;
		}
	if (fitData->HasYErrors())
		{
		itsYErrData = jnew JArray<JFloat>;
		itsHasYErrors = kJTrue;
		}

	AdjustData();
	CreateData();
	GenerateFit();
	GenerateDiffData();
	AdjustDiffData();
}

/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
GLPlotExpFit::~GLPlotExpFit()
{
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

JBoolean
GLPlotExpFit::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	JFloat a, b;
	GetParameter(1, &a);
	GetParameter(2, &b);
	*y = a * exp(b*x);
	return kJTrue;
}


/*********************************************************************************
 GetYRange
 

 ********************************************************************************/

JBoolean
GLPlotExpFit::GetYRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const JBoolean	xLinear,
	JFloat*			yMin,
	JFloat*			yMax
	)
	const
{
	JFloat y1, y2;
	GetYValue(xMin, &y1);
	GetYValue(xMax, &y2);

	*yMin = JMin(y1, y2);
	*yMax = JMax(y1, y2);
	return kJTrue;
}

/*********************************************************************************
 GetDataToFit
 

 ********************************************************************************/

const JPlotDataBase*
GLPlotExpFit::GetDataToFit()
	const
{
	return itsAdjustedData;
}

/*********************************************************************************
 AdjustData
 

 ********************************************************************************/

void
GLPlotExpFit::AdjustData()
{
	const JPlotDataBase* data = GetData();
	itsXData->RemoveAll();
	itsYData->RemoveAll();
	if (data->HasYErrors())
		{
		itsYErrData->RemoveAll();
		}
	if (data->HasXErrors())
		{
		itsXErrData->RemoveAll();
		}
	J2DDataPoint point;
	JSize count = data->GetElementCount();
	JSize i;
	for (i = 1; i <= count; i++)
		{
		data->GetElement(i, &point);
		if (point.y > 0)
			{
			itsXData->AppendElement(point.x);
			itsYData->AppendElement(log(point.y));
			if (itsHasYErrors)
				{
				itsYErrData->AppendElement(log((point.y - point.yerr)/point.y));
				}
			if (itsHasXErrors)
				{
				itsXErrData->AppendElement(point.xerr);
				}
			}
		}
}

/*********************************************************************************
 CreateData
 

 ********************************************************************************/

void
GLPlotExpFit::CreateData()
{
	J2DPlotData::Create(&itsAdjustedData, *itsXData, *itsYData, kJTrue);
	if (itsHasYErrors)
		{
		itsAdjustedData->SetYErrors(*itsYErrData);
		}
	if (itsHasXErrors)
		{
		itsAdjustedData->SetXErrors(*itsXErrData);
		}
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

JBoolean
GLPlotExpFit::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	JFloat linVal;
	JBoolean success = GLPlotLinearFit::GetParameter(index, &linVal);
	if (index == 1)
		{
		*value = exp(linVal);
		}
	else
		{
		*value = linVal;
		}
	return success;
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

JBoolean
GLPlotExpFit::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	JFloat linVal;
	JFloat aVal;
	JBoolean success = GLPlotLinearFit::GetParameterError(index, &linVal);
	GLPlotLinearFit::GetParameter(index, &aVal);
		if (index == 1)
		{
		*value = exp(aVal) - exp(aVal - linVal);
		}
	else
		{
		*value = linVal;
		}
	return success;
}

/*********************************************************************************
 AdjustDiffData
 

 ********************************************************************************/

void
GLPlotExpFit::AdjustDiffData()
{
	JFloat B;
	GetParameter(2, &B);
	JPlotDataBase* pwd = GetDiffData();
	J2DDataPoint dataD;
	J2DDataPoint data;
	const JSize count = pwd->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		pwd->GetElement(i, &dataD);
		GetData()->GetElement(i, &data);
		JFloat yerr;
		if (GetData()->HasYErrors() || itsHasXErrors)
			{
			yerr = sqrt(data.yerr*data.yerr + B*B*data.y*data.y*data.xerr*data.xerr);
			AdjustDiffDataValue(i, yerr);
			}
		}
}
