/*********************************************************************************
 JPlotFitQuad.cpp
 
	JPlotFitQuad class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "JPlotFitQuad.h"
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

JPlotFitQuad::JPlotFitQuad
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFitLinearEq(plot, fitData, xMin, xMax)
{
	JPlotFitQuadX(plot, fitData);
}

JPlotFitQuad::JPlotFitQuad
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
	JPlotFitQuadX(plot, fitData);
}

void
JPlotFitQuad::JPlotFitQuadX
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
 
JPlotFitQuad::~JPlotFitQuad()
{
}

