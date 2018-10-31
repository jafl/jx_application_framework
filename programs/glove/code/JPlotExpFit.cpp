/*********************************************************************************
 JPlotExpFit.cpp
 
	JPlotExpFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "JPlotExpFit.h"
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

JPlotExpFit::JPlotExpFit
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData, 
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotLinearFit(plot, fitData, xMin, xMax, kJFalse)
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
 
JPlotExpFit::~JPlotExpFit()
{
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

JBoolean
JPlotExpFit::GetYValue
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
JPlotExpFit::GetYRange
	(
	JFloat* min, 
	JFloat* max,
	JFloat  xMin, 
	JFloat  xMax
	)
{
	JFloat a, b;
	GetParameter(1, &a);
	GetParameter(2, &b);
	
	JFloat tempMin = a * exp(b*xMin);
	JFloat tempMax = a * exp(b*xMax);
	
	if (tempMin <= tempMax)
		{
		*min = tempMin;
		*max = tempMax;
		}
	else
		{
		*min = tempMax;
		*max = tempMin;
		}
	return kJTrue;
}

/*********************************************************************************
 GetDataToFit
 

 ********************************************************************************/

const JPlotDataBase*
JPlotExpFit::GetDataToFit()
	const
{
	return itsAdjustedData;
}

/*********************************************************************************
 AdjustData
 

 ********************************************************************************/

void
JPlotExpFit::AdjustData()
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
JPlotExpFit::CreateData()
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
JPlotExpFit::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	JFloat linVal;
	JBoolean success = JPlotLinearFit::GetParameter(index, &linVal);
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
JPlotExpFit::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	JFloat linVal;
	JFloat aVal;
	JBoolean success = JPlotLinearFit::GetParameterError(index, &linVal);
	JPlotLinearFit::GetParameter(index, &aVal);
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
JPlotExpFit::AdjustDiffData()
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
