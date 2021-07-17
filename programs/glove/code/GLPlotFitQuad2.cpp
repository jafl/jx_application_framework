/*********************************************************************************
 GLPlotFitQuad2.cpp
 
	GLPlotFitQuad2 class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotFitQuad2.h"
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

GLPlotFitQuad2::GLPlotFitQuad2
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotFitBase(plot, fitData, xMin, xMax)
{
	JPlotFitQuad2X(plot, fitData);
}

GLPlotFitQuad2::GLPlotFitQuad2
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
	JPlotFitQuad2X(plot, fitData);
}

void
GLPlotFitQuad2::JPlotFitQuad2X
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData
	)
{
	SetParameterCount(3);
	SetHasGoodnessOfFit(true);
	itsFunctionName = "y = a + bx + cx^2";
	CalculateFirstPass();
	JVector p(3);
	p.SetElement(1, itsAParameter);
	p.SetElement(2, itsBParameter);
	p.SetElement(3, itsCParameter);
	GenerateFit(p, itsChi2Start);
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
GLPlotFitQuad2::~GLPlotFitQuad2()
{
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

bool
GLPlotFitQuad2::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	*y = itsAParameter + itsBParameter*x + itsCParameter*x*x;
	return true;
}


/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

bool
GLPlotFitQuad2::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	if ((index > 3) || (index < 1))
		{
		return false;
		}
	if (index == 1)
		{
		*name = "a";
		}
	else if (index == 2)
		{
		*name = "b";
		}
	else if (index == 3)
		{
		*name = "c";
		}
	return true;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

bool
GLPlotFitQuad2::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	if ((index > 3) || (index < 1))
		{
		return false;
		}
	if (index == 1)
		{
		*value = itsAParameter;
		}
	else if (index == 2)
		{
		*value = itsBParameter;
		}
	else if (index == 3)
		{
		*value = itsCParameter;
		}
	return true;		
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

bool
GLPlotFitQuad2::GetParameterError
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
		*value = itsAErrParameter;
		}
	else if (index == 2)
		{
		*value = itsBErrParameter;
		}
	else if (index == 3)
		{
		*value = itsCErrParameter;
		}
	return true;
}

/*********************************************************************************
 GetFunctionString
 

 ********************************************************************************/

JString
GLPlotFitQuad2::GetFunctionString()
	const
{
	return itsFunctionName;
}

/*********************************************************************************
 GetFitFunctionString
 

 ********************************************************************************/

JString
GLPlotFitQuad2::GetFitFunctionString()
	const
{
	return itsFunctionName;
}


/*********************************************************************************
 CalculateFirstPass
 

 ********************************************************************************/

void
GLPlotFitQuad2::CalculateFirstPass()
{
	JFloat Y, X, X2, YX, X3, YX2, X4, Sig;
	JFloat tempa, tempb, tempc, det;
	JSize i,j, k;
	JArray<JFloat> yAdjError;
	
	J2DDataPoint point;
	JSize rcount = GetRealElementCount();
	for (i=1; i<= rcount; i++)
		{
		J2DDataPoint point = GetRealElement(i);
		JFloat newVal = 1;
		if (point.yerr != 0)
			{
			newVal = point.yerr;
			}
		yAdjError.AppendElement(newVal);
		}

	JMatrix odata(rcount, 3, 1.0);
	JVector yData(rcount);
	for (i=1; i<= rcount; i++)
		{
		point = GetRealElement(i);
		JFloat yerr = yAdjError.GetElement(i);
		odata.SetElement(i, 1, 1/(yerr*yerr));
		odata.SetElement(i, 2, point.x/(yerr*yerr));
		odata.SetElement(i, 3, point.x*point.x/(yerr*yerr));
		yData.SetElement(i, point.y/(yerr*yerr));
		}
	JMatrix tData = odata.Transpose();
	JMatrix lData = tData * odata;
	JMatrix rData = tData * yData;
	JMatrix parms(3,1);
	JGaussianElimination(lData, rData, &parms);

	for (k=1; k<= 4; k++)
		{
		Y = 0;
		X = 0;
		X2 = 0;
		YX = 0;
		X3 = 0;
		YX2 = 0;
		X4 = 0;
		Sig = 0;
		for (i=1; i<= rcount; i++)
			{
			point = GetRealElement(i);
			JFloat yerr = yAdjError.GetElement(i);
			Y += point.y/(yerr*yerr);
			X += point.x/(yerr*yerr);
			X2 += point.x*point.x/(yerr*yerr);
			YX += point.y*point.x/(yerr*yerr);
			X3 += point.x*point.x*point.x/(yerr*yerr);
			YX2 += point.x*point.x*point.y/(yerr*yerr);
			X4 += point.x*point.x*point.x*point.x/(yerr*yerr);
			Sig += 1/(yerr*yerr);
			}
		JFloat cv1 = 0, cv2 = 0, cv3 = 0;
		for (i=1; i<= rcount; i++)
			{
			point = GetRealElement(i);
			JFloat syi = yAdjError.GetElement(i);
			JFloat yi = point.y;
			JFloat xi = point.x;
			for (j = 1; j <= rcount; j++)
				{
				point = GetRealElement(j);
				JFloat syj = yAdjError.GetElement(j);
				JFloat yj = point.y;
				JFloat xj = point.x;
				cv1 += xi*xj*xj*(xi*yj-yi*xj)/(syi*syi*syj*syj);
				cv2 += (xi*xj*xj*(yi - yj))/(syi*syi*syj*syj);
				cv3 += (xi*xj*xj*(xj - xi))/(syi*syi*syj*syj);
				}
			}
		det = Sig*(X2*X4-X3*X3) + X*(X3*X2-X*X4) + X2*(X*X3-X2*X2);
		tempa = (Y*(X2*X4-X3*X3) + X*(X3*YX2-YX*X4) + X2*(YX*X3-X2*YX2))/det;
		tempb = (Sig*(YX*X4-YX2*X3) + Y*(X3*X2-X*X4) + X2*(X*YX2-YX*X2))/det;
		tempc = (Sig*cv1 + X*cv2 + Y*cv3)/det;

		for (i=1; i<=rcount; i++)
			{
			J2DDataPoint point = GetRealElement(i);
			JFloat newVal = 
				sqrt(point.yerr*point.yerr + (tempb+2*tempc*point.x)*(tempb+2*tempc*point.x)*point.xerr*point.xerr);
			if (newVal == 0)
				{
				newVal = 1;
				}
			yAdjError.SetElement(i, newVal);
			}
		}
//	itsAParameter	= tempa;
//	itsBParameter	= tempb;
//	itsCParameter	= tempc;
	itsAParameter	= parms.GetElement(1, 1);
	itsBParameter	= parms.GetElement(2, 1);
	itsCParameter	= parms.GetElement(3, 1);

	itsChi2Start = 0;
	for (i=1; i<= rcount; i++)
		{
		point = GetRealElement(i);
		JFloat yerr = yAdjError.GetElement(i);
		itsChi2Start += pow(point.y - tempa - tempb*point.x - tempc*point.x*point.x,2)/(yerr*yerr);
		}

	itsAErrParameter = 0;
	itsBErrParameter = 0;
	itsCErrParameter = 0;
}

/*********************************************************************************
 FunctionN
 
 ********************************************************************************/

JFloat 
GLPlotFitQuad2::FunctionN
	(
	const JFloat x
	)
{
	return itsAParameter + itsBParameter*x + itsCParameter*x*x;
}

/*********************************************************************************
 FunctionNPrimed
 
 ********************************************************************************/

JFloat 
GLPlotFitQuad2::FunctionNPrimed
	(
	const JFloat x
	)
{
	return 2*itsCParameter*x + itsBParameter;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
GLPlotFitQuad2::SetCurrentParameters
	(
	const JVector& p
	)
{
	itsAParameter	= p.GetElement(1);
	itsBParameter	= p.GetElement(2);
	itsCParameter	= p.GetElement(3);
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
GLPlotFitQuad2::SetErrors
	(
	const JVector& p
	)
{
	itsAErrParameter	= p.GetElement(1);
	itsBErrParameter	= p.GetElement(2);
	itsCErrParameter	= p.GetElement(3);
}
