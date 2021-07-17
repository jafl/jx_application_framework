/*********************************************************************************
 GLPlotFitFunction.cpp
 
	GLPlotFitFunction class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotFitFunction.h"
#include "J2DPlotWidget.h"
#include "JPlotDataBase.h"
#include "J2DPlotData.h"
#include "J2DDataPoint.h"
#include "jAssert.h"

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

GLPlotFitFunction::GLPlotFitFunction
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFunctionBase(JPlotDataBase::kScatterPlot, plot, xMin, xMax),
	GLFitBase(),
	itsDiffData(nullptr)
{
	itsData = fitData;
	itsHasXErrors = false;
	itsHasYErrors = false;
	if (itsData->HasXErrors())
		{
		itsHasXErrors = true;
		}
	if (itsData->HasYErrors())
		{
		itsHasYErrors = true;
		}
}

/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
GLPlotFitFunction::~GLPlotFitFunction()
{
}

/*********************************************************************************
 GenerateDiffData (protected)
 
 ********************************************************************************/
 
void
GLPlotFitFunction::GenerateDiffData()
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
				// Save the following for when it gets merged into GLPlotFitBase
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
	J2DPlotData::Create(&itsDiffData, xdata, ydata, false);
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
GLPlotFitFunction::GetDiffData()
	const
{
	assert(itsDiffData != nullptr);
	return itsDiffData;
}

/******************************************************************************
 SetDiffData (protected)

 ******************************************************************************/

void
GLPlotFitFunction::SetDiffData
	(
	J2DPlotData* data
	)
{
	if (itsDiffData != nullptr)
		{
		jdelete itsDiffData;
		}
	itsDiffData	= data;
}


/*********************************************************************************
 CalculateStdDev (protected)
 

 ********************************************************************************/
 
void
GLPlotFitFunction::CalculateStdDev()
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
GLPlotFitFunction::AdjustDiffDataValue
	(
	const JIndex index, 
	const JFloat value
	)
{
}

/*********************************************************************************
 DataElementValid
 

 ********************************************************************************/

bool
GLPlotFitFunction::DataElementValid
	(
	const JIndex index
	)
{
	return true;
}

/******************************************************************************
 GetDataElement
 

 *****************************************************************************/

bool
GLPlotFitFunction::GetDataElement
	(
	const JIndex index,
	J2DDataPoint* point
	)
{
	bool valid = DataElementValid(index);
	if (!valid)
		{
		return false;
		}
	const JPlotDataBase* data = GetData();
	data->GetElement(index, point);
	return true;
}
