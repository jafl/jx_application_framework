/*********************************************************************************
 GLPlotFitQuad.cpp
 
	GLPlotFitQuad class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotFitQuad.h"
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

GLPlotFitQuad::GLPlotFitQuad
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotFitLinearEq(plot, fitData, xMin, xMax)
{
	JPlotFitQuadX(plot, fitData);
}

GLPlotFitQuad::GLPlotFitQuad
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
	JPlotFitQuadX(plot, fitData);
}

void
GLPlotFitQuad::JPlotFitQuadX
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData
	)
{
	JArray<JIndex> powers;
	powers.AppendElement(0);
	powers.AppendElement(1);
	powers.AppendElement(2);
	InitializePolynomial(powers);
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
GLPlotFitQuad::~GLPlotFitQuad()
{
}

