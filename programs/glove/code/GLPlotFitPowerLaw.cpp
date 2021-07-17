/*********************************************************************************
 GLPlotFitPowerLaw.cpp
 
	GLPlotFitPowerLaw class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotFitPowerLaw.h"
#include "J2DPlotWidget.h"
#include "JPlotDataBase.h"

#include <JString.h>
#include <JArray.h>
#include <JMatrix.h>
#include <JVector.h>

#include <jMath.h>
#include <JMinMax.h>
#include <jAssert.h>

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

GLPlotFitPowerLaw::GLPlotFitPowerLaw
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotFitBase(plot, fitData, xMin, xMax)
{
	JPlotFitPowerLawX(plot, fitData);
}

GLPlotFitPowerLaw::GLPlotFitPowerLaw
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	GLPlotFitBase(plot, fitData, xmin, xmax, ymin, ymax)
{
	JPlotFitPowerLawX(plot, fitData);
}

void
GLPlotFitPowerLaw::JPlotFitPowerLawX
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData
	)
{
	itsAParm	= 0;
	itsBParm	= 0;
	itsAErr	= 0;
	itsBErr	= 0;
	SetFunctionString(JString("a*x^b", JString::kNoCopy));
	SetParameterCount(2);
	SetHasGoodnessOfFit(true);
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
GLPlotFitPowerLaw::~GLPlotFitPowerLaw()
{
}

/******************************************************************************
 GenerateFit (public)

 ******************************************************************************/

void
GLPlotFitPowerLaw::GenerateFit()
{
	CalculateFirstPass();
	JVector parms(2);
	parms.SetElement(1, itsAParm);
	parms.SetElement(2, itsBParm);
	GLPlotFitBase::GenerateFit(parms, itsChi2Start);
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

bool
GLPlotFitPowerLaw::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	*y =  itsAParm * pow(x, itsBParm);
	return true;
}

/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

bool
GLPlotFitPowerLaw::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	if (index == 1)
		{
		*name = "a";
		}
	else if (index == 2)
		{
		*name = "b";
		}
	else
		{
		return false;
		}		
	return true;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

bool
GLPlotFitPowerLaw::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	if (index == 1)
		{
		*value = itsAParm;
		}
	else if (index == 2)
		{
		*value = itsBParm;
		}
	else
		{
		return false;
		}		
	return true;
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

bool
GLPlotFitPowerLaw::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	const JPlotDataBase* data = GetData();
	if (!data->HasXErrors() && !data->HasYErrors())
		{
		return false;
		}
	if (index == 1)
		{
		*value = itsAErr;
		}
	else if (index == 2)
		{
		*value = itsBErr;
		}
	else
		{
		return false;
		}		
	return true;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
GLPlotFitPowerLaw::SetCurrentParameters
	(
	const JVector& p
	)
{
	itsAParm	= p.GetElement(1);
	itsBParm	= p.GetElement(2);
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
GLPlotFitPowerLaw::SetErrors
	(
	const JVector& p
	)
{
	itsAErr	= p.GetElement(1);
	itsBErr	= p.GetElement(2);
}

/*********************************************************************************
 FunctionN
 
 ********************************************************************************/

JFloat 
GLPlotFitPowerLaw::FunctionN
	(
	const JFloat x
	)
{
	return itsAParm * pow(x, itsBParm);
}

/*********************************************************************************
 FunctionNPrimed
 
 ********************************************************************************/

JFloat 
GLPlotFitPowerLaw::FunctionNPrimed
	(
	const JFloat x
	)
{
	return itsAParm * itsBParm * pow(x, itsBParm - 1);
}

/*********************************************************************************
 CalculateFirstPass
 

 ********************************************************************************/

void
GLPlotFitPowerLaw::CalculateFirstPass()
{
	J2DDataPoint point;
	const JSize count	= GetRealElementCount();
	JSize rcount		= 0;
	for (JIndex i = 1; i <= count; i++)
		{
		point	= GetRealElement(i);
		if (point.y > 0)
			{
			rcount++;
			}
		}
	JMatrix odata(rcount, 2, 1.0);
	JVector yData(rcount);

	rcount	= 0;
	for (JIndex i = 1; i <= count; i++)
		{
		point	= GetRealElement(i);
		if (point.y > 0)
			{
			rcount++;
			JFloat yerr = point.yerr;
			if (yerr == 0)
				{
				yerr = 1;
				}
			else
				{
				yerr	= log((point.y - point.yerr)/point.y);
				}
			odata.SetElement(rcount, 1, 1/(yerr*yerr));
			odata.SetElement(rcount, 2, log(point.x)/(yerr*yerr));
			yData.SetElement(rcount, log(point.x)/(yerr*yerr));
			}
		}
/*
	for (JIndex i = 1; i <= rcount; i++)
		{
		do
			{
			point = GetRealElement(i);
			}
		while (point.y == 0);

		JFloat yerr = point.yerr;
		if (yerr == 0)
			{
			yerr = 1;
			}
		else
			{
			yerr	= log((point.y - point.yerr)/point.y);
			}
		odata.SetElement(i, 1, 1/(yerr*yerr));
		odata.SetElement(i, 2, log(point.x)/(yerr*yerr));
		yData.SetElement(i, log(point.x)/(yerr*yerr));
		}
*/
	JMatrix tData = odata.Transpose();
	JMatrix lData = tData * odata;
	JMatrix rData = tData * yData;
	JMatrix parms(2,1);
	JGaussianElimination(lData, rData, &parms);
	JVector eparms(2);
	eparms.SetElement(1, exp(parms.GetElement(1,1)));
	eparms.SetElement(2, exp(parms.GetElement(2,1)));
	SetCurrentParameters(eparms);

	itsChi2Start = 0;
	for (JIndex i = 1; i <= count; i++)
		{
//		do
//			{
			point = GetRealElement(i);
//			}
//		while (point.y == 0);
		JFloat yerr = point.yerr;
		if (yerr == 0)
			{
			yerr = 1;
			}
		itsChi2Start += pow(point.y - FunctionN(point.x),2)/(yerr*yerr);
		}

}
