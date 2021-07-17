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
	GLPlotLinearFit(plot, fitData, xMin, xMax, false)
{
	SetFunctionName(JString("y = a Exp(bx)", JString::kNoCopy));
	itsXData = jnew JArray<JFloat>;
	itsYData = jnew JArray<JFloat>;
	itsHasXErrors = false;
	itsHasYErrors = false;
	if (fitData->HasXErrors())
		{
		itsXErrData = jnew JArray<JFloat>;
		itsHasXErrors = true;
		}
	if (fitData->HasYErrors())
		{
		itsYErrData = jnew JArray<JFloat>;
		itsHasYErrors = true;
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

bool
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
	return true;
}


/*********************************************************************************
 GetYRange
 

 ********************************************************************************/

bool
GLPlotExpFit::GetYRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const bool	xLinear,
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
	return true;
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
	J2DPlotData::Create(&itsAdjustedData, *itsXData, *itsYData, true);
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

bool
GLPlotExpFit::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	JFloat linVal;
	bool success = GLPlotLinearFit::GetParameter(index, &linVal);
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

bool
GLPlotExpFit::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	JFloat linVal;
	JFloat aVal;
	bool success = GLPlotLinearFit::GetParameterError(index, &linVal);
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
