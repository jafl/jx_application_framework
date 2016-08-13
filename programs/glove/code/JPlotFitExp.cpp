/*********************************************************************************
 JPlotFitExp.cpp
 
	JPlotFitExp class.
 
	Copyright @ 2000 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#include "JPlotFitExp.h"
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

JPlotFitExp::JPlotFitExp
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFitBase(plot, fitData, xMin, xMax)
{
	JPlotFitExpX(plot, fitData);
}

JPlotFitExp::JPlotFitExp
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	JPlotFitBase(plot, fitData, xmin, xmax, ymin, ymax)
{
	JPlotFitExpX(plot, fitData);
}

void
JPlotFitExp::JPlotFitExpX
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData
	)
{
	itsAParm	= 0;
	itsBParm	= 0;
	itsAErr	= 0;
	itsBErr	= 0;
	SetFunctionString("a*e^(b*x)");
	SetParameterCount(2);
	SetHasGoodnessOfFit(kJTrue);
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
JPlotFitExp::~JPlotFitExp()
{
}

/******************************************************************************
 GenerateFit (public)

 ******************************************************************************/

void
JPlotFitExp::GenerateFit()
{
	CalculateFirstPass();
	JVector parms(2);
	parms.SetElement(1, itsAParm);
	parms.SetElement(2, itsBParm);
	JPlotFitBase::GenerateFit(parms, itsChi2Start);
}


/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

JBoolean
JPlotFitExp::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	*y =  itsAParm * exp(itsBParm * x);
	return kJTrue;
}

/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

JBoolean
JPlotFitExp::GetParameterName
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
		return kJFalse;
		}		
	return kJTrue;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

JBoolean
JPlotFitExp::GetParameter
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
		return kJFalse;
		}		
	return kJTrue;
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

JBoolean
JPlotFitExp::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	const JPlotDataBase* data = GetData();
	if (!data->HasXErrors() && !data->HasYErrors())
		{
		return kJFalse;
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
		return kJFalse;
		}		
	return kJTrue;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
JPlotFitExp::SetCurrentParameters
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
JPlotFitExp::SetErrors
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
JPlotFitExp::FunctionN
	(
	const JFloat x
	)
{
	return itsAParm * exp(itsBParm * x);
}

/*********************************************************************************
 FunctionNPrimed
 
 ********************************************************************************/

JFloat 
JPlotFitExp::FunctionNPrimed
	(
	const JFloat x
	)
{
	return itsAParm * itsBParm * exp(itsBParm * x);
}

/*********************************************************************************
 CalculateFirstPass
 

 ********************************************************************************/

void
JPlotFitExp::CalculateFirstPass()
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

	JMatrix tData = odata.Transpose();
	JMatrix lData = tData * odata;
	JMatrix rData = tData * yData;
	JMatrix parms(2,1);
	JGaussianElimination(lData, rData, &parms);
	JVector eparms(2);
	eparms.SetElement(1, exp(parms.GetElement(1,1)));
	eparms.SetElement(2, parms.GetElement(2,1));
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
