/*********************************************************************************
 JPlotFitLinearEq.cpp
 
	JPlotFitLinearEq class.
 
	Copyright @ 2000 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#include "JPlotFitLinearEq.h"
#include "J2DPlotWidget.h"
#include "JPlotDataBase.h"

#include <JString.h>
#include <JArray.h>
#include <JMatrix.h>
#include <JVector.h>

#include <jMath.h>
#include <JMinMax.h>
#include <jAssert.h>

const JIndex kPowOptimizationIndex	= 7;

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

JPlotFitLinearEq::JPlotFitLinearEq
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFitBase(plot, fitData, xMin, xMax)
{
	JPlotFitLinearEqX(plot, fitData);
}

JPlotFitLinearEq::JPlotFitLinearEq
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat 	xmin, 
	const JFloat 	xmax,
	const JFloat 	ymin, 
	const JFloat 	ymax
	)
	:
	JPlotFitBase(plot, fitData, xmin, xmax, ymin, ymax)
{
	JPlotFitLinearEqX(plot, fitData);
}

void
JPlotFitLinearEq::JPlotFitLinearEqX
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData
	)
{
	itsPowers		= NULL;
	itsParameters 	= NULL;
	itsErrors		= NULL;
}

/******************************************************************************
 InitializePolynomial (public)

 ******************************************************************************/

void
JPlotFitLinearEq::InitializePolynomial
	(
	const JArray<JIndex>& powers
	)
{
	itsPowers			= jnew JArray<JIndex>(powers);
	assert(itsPowers != NULL);
	const JSize count	= itsPowers->GetElementCount();
	SetParameterCount(count);
	SetHasGoodnessOfFit(kJTrue);
	JString name;
	for (JIndex i = 1; i <= count; i++)
		{
		JString parm	= "a" + JString(i - 1, JString::kBase10);
		JString xTerm;
		JIndex power	= itsPowers->GetElement(i);
		if (power > 0)
			{
			xTerm = " * x";
			}
		if (power > 1)
			{
			xTerm += "^" + JString(power);
			}
		name += parm + xTerm;
		if (i != count)
			{
			name += " + ";
			}
		}
	SetFunctionString(name);

	itsParameters	= jnew JVector(count);
	assert(itsParameters != NULL);
	itsErrors		= jnew JVector(count);
	assert(itsErrors != NULL);
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
JPlotFitLinearEq::~JPlotFitLinearEq()
{
	jdelete itsPowers;
	jdelete itsParameters;
	jdelete itsErrors;
}

/******************************************************************************
 GenerateFit (public)

 ******************************************************************************/

void
JPlotFitLinearEq::GenerateFit()
{
	CalculateFirstPass();
	JPlotFitBase::GenerateFit(*itsParameters, itsChi2Start);
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

JBoolean
JPlotFitLinearEq::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	const JSize count	= itsPowers->GetElementCount();
	*y	= 0;
	for (JIndex i = 1; i <= count; i++)
		{
		JIndex power = itsPowers->GetElement(i);
		JFloat term  = itsParameters->GetElement(i);
		if (power > kPowOptimizationIndex)
			{
			term  *= pow((double)x, (double)power);
			}
		else
			{
			for (JIndex j = 1; j <= power; j++)
				{
				term *= x;
				}
			}
		*y += term;
		}

	return kJTrue;
}


/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

JBoolean
JPlotFitLinearEq::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	if (index > itsPowers->GetElementCount())
		{
		return kJFalse;
		}
	*name = "a" + JString(index - 1, JString::kBase10);
	return kJTrue;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

JBoolean
JPlotFitLinearEq::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	if (index > itsPowers->GetElementCount())
		{
		return kJFalse;
		}
	*value	= itsParameters->GetElement(index);
	return kJTrue;		
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

JBoolean
JPlotFitLinearEq::GetParameterError
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
	if (index > itsPowers->GetElementCount())
		{
		return kJFalse;
		}
	*value	= itsErrors->GetElement(index);
	return kJTrue;		
}

/*********************************************************************************
 CalculateFirstPass
 

 ********************************************************************************/

void
JPlotFitLinearEq::CalculateFirstPass()
{
	J2DDataPoint point;
	const JSize rcount	= GetRealElementCount();
	const JSize pcount	= itsPowers->GetElementCount();
	JMatrix odata(rcount, pcount, 1.0);
	JVector yData(rcount);
	for (JIndex i = 1; i <= rcount; i++)
		{
		point = GetRealElement(i);
		JFloat yerr = point.yerr;
		if (yerr == 0)
			{
			yerr = 1;
			}
		for (JIndex j = 1; j <= pcount; j++)
			{
			JIndex power	= itsPowers->GetElement(j);
			JFloat term 	= 1;
			if (power > kPowOptimizationIndex)
				{
				term  *= pow((double)point.x, (double)power);
				}
			else
				{
				for (JIndex j = 1; j <= power; j++)
					{
					term *= point.x;
					}
				}			
			odata.SetElement(i, j, term/(yerr*yerr));
			}
		yData.SetElement(i, point.y/(yerr*yerr));
		}
	JMatrix tData = odata.Transpose();
	JMatrix lData = tData * odata;
	JMatrix rData = tData * yData;
	JMatrix parms(pcount,1);
	JGaussianElimination(lData, rData, &parms);
	SetCurrentParameters(parms.GetColVector(1));

	const JPlotDataBase* data = GetData();
	if (data->HasXErrors() && 0)
		{
		for (JIndex i = 1; i <= 3; i++)
			{
			JMatrix odata2(rcount, pcount, 1.0);
			JVector yData2(rcount);
			for (JIndex j = 1; j <= rcount; j++)
				{
				point = GetRealElement(i);
				JFloat B = FunctionNPrimed(point.x);
				JFloat errTerm = (point.yerr * point.yerr + B * B * point.xerr * point.xerr);
				if (errTerm == 0)
					{
					errTerm = 1;
					}
				for (JIndex k = 1; k <= pcount; k++)
					{
					JIndex power	= itsPowers->GetElement(k);
					JFloat term 	= 1;
					if (power > kPowOptimizationIndex)
						{
						term  *= pow((double)point.x, (double)power);
						}
					else
						{
						for (JIndex m = 1; m <= power; m++)
							{
							term *= point.x;
							}
						}			
					odata2.SetElement(j, k, term/errTerm);
					}
				yData2.SetElement(j, point.y/errTerm);
				}

			JMatrix tData2 = odata2.Transpose();
			JMatrix lData2 = tData2 * odata2;
			JMatrix rData2 = tData2 * yData2;
			JMatrix parms2(pcount,1);
			JGaussianElimination(lData2, rData2, &parms2);
			SetCurrentParameters(parms2.GetColVector(1));
			}
		}

	itsChi2Start = 0;
	for (JIndex i = 1; i <= rcount; i++)
		{
		point = GetRealElement(i);
		JFloat yerr = point.yerr;
		if (yerr == 0)
			{
			yerr = 1;
			}
		itsChi2Start += pow((double)(point.y - FunctionN(point.x)),(double)2)/(yerr*yerr);
		}

	itsErrors->SetAllElements(0);
}

/*********************************************************************************
 FunctionN
 
 ********************************************************************************/

JFloat 
JPlotFitLinearEq::FunctionN
	(
	const JFloat x
	)
{
	JFloat y;
	GetYValue(x, &y);
	return y;
}

/*********************************************************************************
 FunctionNPrimed
 
 ********************************************************************************/

JFloat 
JPlotFitLinearEq::FunctionNPrimed
	(
	const JFloat x
	)
{
	const JSize count	= itsPowers->GetElementCount();
	JFloat y	= 0;
	for (JIndex i = 1; i <= count; i++)
		{
		JIndex power = itsPowers->GetElement(i);
		if (power == 0)
			{
			continue;
			}
		JFloat term  = itsParameters->GetElement(i);
		if (power - 1 > kPowOptimizationIndex)
			{
			term  *= pow((double)x, (double)(power - 1));
			}
		else
			{
			for (JIndex j = 1; j < power; j++)
				{
				term *= x;
				}
			}
		term *= power;
		y += term;
		}

	return y;
	
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
JPlotFitLinearEq::SetCurrentParameters
	(
	const JVector& p
	)
{
	assert(p.GetDimensionCount() == itsParameters->GetDimensionCount());
	*itsParameters	= p;
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
JPlotFitLinearEq::SetErrors
	(
	const JVector& p
	)
{
	assert(p.GetDimensionCount() == itsErrors->GetDimensionCount());
	*itsErrors	= p;
}
