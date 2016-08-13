/*********************************************************************************
 JPlotFitFunction.cpp
 
	JPlotFitFunction class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#include "JPlotFitFunction.h"
#include "J2DPlotWidget.h"
#include "JPlotDataBase.h"
#include "J2DPlotData.h"
#include "J2DDataPoint.h"
#include "jAssert.h"

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

JPlotFitFunction::JPlotFitFunction
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFunctionBase(JPlotDataBase::kScatterPlot, plot, xMin, xMax),
	JFitBase(),
	itsDiffData(NULL)
{
	itsData = fitData;
	itsHasXErrors = kJFalse;
	itsHasYErrors = kJFalse;
	if (itsData->HasXErrors())
		{
		itsHasXErrors = kJTrue;
		}
	if (itsData->HasYErrors())
		{
		itsHasYErrors = kJTrue;
		}
}

/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
JPlotFitFunction::~JPlotFitFunction()
{
}

/*********************************************************************************
 GenerateDiffData (protected)
 
 ********************************************************************************/
 
void
JPlotFitFunction::GenerateDiffData()
{
	JArray<JFloat> xdata;
	JArray<JFloat> ydata;
	JArray<JFloat> xerrdata;
	JArray<JFloat> yerrdata;
	if (!itsData->HasYErrors())
		{
		CalculateStdDev();
		}
	const JSize count = itsData->GetElementCount();
	J2DDataPoint data;
	for (JSize i = 1; i <= count; i++)
		{
		if (GetDataElement(i, &data))
			{
			JFloat fitY;
			GetYValue(data.x, &fitY);
			xdata.AppendElement(data.x);
			ydata.AppendElement(data.y - fitY);
			if (itsHasYErrors)
				{
				// Save the following for when it gets merged into JPlotFitBase
//				if (itsHasXErrors)
//					{
//					JFloat b	= FunctionNPrimed(data.x);
//					JFloat err	= sqrt(data.yerr * data.yerr + b * b * data.xerr * data.xerr);
//					yerrdata->AppendElement(err);
//					}
//				else
//					{
					yerrdata.AppendElement(data.yerr);
//					}				
				}
			else
				{
				yerrdata.AppendElement(itsStdDev);
				}
			if (itsHasXErrors)
				{
				xerrdata.AppendElement(data.xerr);
				}
			}
		}
	J2DPlotData::Create(&itsDiffData, xdata, ydata, kJFalse);
	itsDiffData->SetYErrors(yerrdata);
	if (itsHasXErrors)
		{
		itsDiffData->SetXErrors(xerrdata);
		}
}

/*********************************************************************************
 GetDiffData
 

 ********************************************************************************/
 
J2DPlotData*
JPlotFitFunction::GetDiffData()
	const
{
	assert(itsDiffData != NULL);
	return itsDiffData;
}

/******************************************************************************
 SetDiffData (protected)

 ******************************************************************************/

void
JPlotFitFunction::SetDiffData
	(
	J2DPlotData* data
	)
{
	if (itsDiffData != NULL)
		{
		delete itsDiffData;
		}
	itsDiffData	= data;
}


/*********************************************************************************
 CalculateStdDev (protected)
 

 ********************************************************************************/
 
void
JPlotFitFunction::CalculateStdDev()
{
	JSize count = itsData->GetElementCount();
	J2DDataPoint data;
	JFloat current = 0;
	for (JSize i = 1; i <= count; i++)
		{
		itsData->GetElement(i, &data);
		JFloat fitY;
		GetYValue(data.x, &fitY);
		current += (fitY - data.y)*(fitY - data.y);
		}
	itsStdDev = sqrt(current/(count - 2));
}

/*********************************************************************************
 AdjustDiffDataValue (protected)
 

 ********************************************************************************/
 
void
JPlotFitFunction::AdjustDiffDataValue
	(
	const JIndex index, 
	const JFloat value
	)
{
}

/*********************************************************************************
 DataElementValid
 

 ********************************************************************************/

JBoolean
JPlotFitFunction::DataElementValid
	(
	const JIndex index
	)
{
	return kJTrue;
}

/******************************************************************************
 GetDataElement
 

 *****************************************************************************/

JBoolean
JPlotFitFunction::GetDataElement
	(
	const JIndex index,
	J2DDataPoint* point
	)
{
	JBoolean valid = DataElementValid(index);
	if (!valid)
		{
		return kJFalse;
		}
	const JPlotDataBase* data = GetData();
	data->GetElement(index, point);
	return kJTrue;
}
