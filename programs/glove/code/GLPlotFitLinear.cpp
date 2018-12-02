/*********************************************************************************
 GLPlotFitLinear.cpp
 
	GLPlotFitLinear class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotFitLinear.h"
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

GLPlotFitLinear::GLPlotFitLinear
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotFitLinearEq(plot, fitData, xMin, xMax)
{
	JPlotFitLinearX(plot, fitData);
}

GLPlotFitLinear::GLPlotFitLinear
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	GLPlotFitLinearEq(plot, fitData, xmin, xmax, ymin, ymax)
{
	JPlotFitLinearX(plot, fitData);
}

void
GLPlotFitLinear::JPlotFitLinearX
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
 
GLPlotFitLinear::~GLPlotFitLinear()
{
}

