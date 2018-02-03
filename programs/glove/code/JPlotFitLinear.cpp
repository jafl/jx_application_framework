/*********************************************************************************
 JPlotFitLinear.cpp
 
	JPlotFitLinear class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "JPlotFitLinear.h"
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

JPlotFitLinear::JPlotFitLinear
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFitLinearEq(plot, fitData, xMin, xMax)
{
	JPlotFitLinearX(plot, fitData);
}

JPlotFitLinear::JPlotFitLinear
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	JPlotFitLinearEq(plot, fitData, xmin, xmax, ymin, ymax)
{
	JPlotFitLinearX(plot, fitData);
}

void
JPlotFitLinear::JPlotFitLinearX
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData
	)
{
	JArray<JIndex> powers;
	powers.AppendElement(0);
	powers.AppendElement(1);
	InitializePolynomial(powers);
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
JPlotFitLinear::~JPlotFitLinear()
{
}

