/******************************************************************************
 JPlotFitModule.cpp

	<Description>

	BASE CLASS = public JPlotFitBase

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <JPlotFitModule.h>
#include "GLDLFitModule.h"

#include <JVector.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

JPlotFitModule::JPlotFitModule
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFitBase(plot, fitData, xMin, xMax),
	itsModule(NULL),
	itsErrors(NULL)
{
}

JPlotFitModule::JPlotFitModule
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	JPlotFitBase(plot, fitData, xmin, xmax, ymin, ymax),
	itsModule(NULL),
	itsErrors(NULL)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JPlotFitModule::~JPlotFitModule()
{
//	delete itsModule; 		we don't own it
	delete itsErrors;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
JPlotFitModule::SetCurrentParameters
	(
	const JVector& p
	)
{
	assert(itsModule != NULL);

	itsModule->SetCurrentParameters(p);
	*itsParameters	= p;
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
JPlotFitModule::SetErrors
	(
	const JVector& p
	)
{
	*itsErrors	= p;
}

/******************************************************************************
 FunctionN (virtual protected)

 ******************************************************************************/

JFloat
JPlotFitModule::FunctionN
	(
	const JFloat x
	)
{
	assert(itsModule != NULL);
	return itsModule->Function(x);
}

/******************************************************************************
 FunctionN (virtual protected)

 ******************************************************************************/

JFloat
JPlotFitModule::FunctionNPrimed
	(
	const JFloat x
	)
{
	assert(itsModule != NULL);
	if (!itsModule->HasFPrimed())
		{
		return JPlotFitBase::FunctionNPrimed(x);
		}
	return itsModule->FPrimed(x);
}

/******************************************************************************
 SetFitModule (public)

 ******************************************************************************/

void
JPlotFitModule::SetFitModule
	(
	GLDLFitModule* fit
	)
{
	itsModule	= fit;
	const JSize count	= itsModule->GetParameterCount();
	itsParameters		= new JVector(count);
	assert(itsParameters != NULL);
	SetParameterCount(count);
	itsErrors	= new JVector(count);
	assert(itsErrors != NULL);
	SetFunctionString(itsModule->GetFunctionalForm());
}

/******************************************************************************
 SetInitialParameters (public)

 ******************************************************************************/

void
JPlotFitModule::SetInitialParameters
	(
	const JVector& p
	)
{
	GenerateFit(p, 0);
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

JBoolean
JPlotFitModule::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	JPlotFitModule* th	= const_cast< JPlotFitModule* >(this);
	*y	= th->itsModule->Function(x);
	return kJTrue;
}


/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

JBoolean
JPlotFitModule::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	assert(itsModule != NULL);
	if (index > itsModule->GetParameterCount() || index < 1)
		{
		return kJFalse;
		}
	*name	= itsModule->GetParameterName(index);
	return kJTrue;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

JBoolean
JPlotFitModule::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	if (index > itsParameters->GetDimensionCount() || index < 1)
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
JPlotFitModule::GetParameterError
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
	if (index > itsErrors->GetDimensionCount())
		{
		return kJFalse;
		}
	*value	= itsErrors->GetElement(index);
	return kJTrue;		
}
