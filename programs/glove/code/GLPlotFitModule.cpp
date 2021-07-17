/******************************************************************************
 GLPlotFitModule.cpp

	BASE CLASS = public GLPlotFitBase

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GLPlotFitModule.h>
#include "GLDLFitModule.h"

#include <JVector.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GLPlotFitModule::GLPlotFitModule
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotFitBase(plot, fitData, xMin, xMax),
	itsModule(nullptr),
	itsErrors(nullptr)
{
}

GLPlotFitModule::GLPlotFitModule
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	GLPlotFitBase(plot, fitData, xmin, xmax, ymin, ymax),
	itsModule(nullptr),
	itsErrors(nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLPlotFitModule::~GLPlotFitModule()
{
//	jdelete itsModule; 		we don't own it
	jdelete itsErrors;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
GLPlotFitModule::SetCurrentParameters
	(
	const JVector& p
	)
{
	assert(itsModule != nullptr);

	itsModule->SetCurrentParameters(p);
	*itsParameters	= p;
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
GLPlotFitModule::SetErrors
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
GLPlotFitModule::FunctionN
	(
	const JFloat x
	)
{
	assert(itsModule != nullptr);
	return itsModule->Function(x);
}

/******************************************************************************
 FunctionN (virtual protected)

 ******************************************************************************/

JFloat
GLPlotFitModule::FunctionNPrimed
	(
	const JFloat x
	)
{
	assert(itsModule != nullptr);
	if (!itsModule->HasFPrimed())
		{
		return GLPlotFitBase::FunctionNPrimed(x);
		}
	return itsModule->FPrimed(x);
}

/******************************************************************************
 SetFitModule (public)

 ******************************************************************************/

void
GLPlotFitModule::SetFitModule
	(
	GLDLFitModule* fit
	)
{
	itsModule	= fit;
	const JSize count	= itsModule->GetParameterCount();
	itsParameters		= jnew JVector(count);
	assert(itsParameters != nullptr);
	SetParameterCount(count);
	itsErrors	= jnew JVector(count);
	assert(itsErrors != nullptr);
	SetFunctionString(itsModule->GetFunctionalForm());
}

/******************************************************************************
 SetInitialParameters (public)

 ******************************************************************************/

void
GLPlotFitModule::SetInitialParameters
	(
	const JVector& p
	)
{
	GenerateFit(p, 0);
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

bool
GLPlotFitModule::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	GLPlotFitModule* th	= const_cast< GLPlotFitModule* >(this);
	*y	= th->itsModule->Function(x);
	return true;
}


/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

bool
GLPlotFitModule::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	assert(itsModule != nullptr);
	if (index > itsModule->GetParameterCount() || index < 1)
		{
		return false;
		}
	*name	= itsModule->GetParameterName(index);
	return true;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

bool
GLPlotFitModule::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	if (index > itsParameters->GetDimensionCount() || index < 1)
		{
		return false;
		}
	*value	= itsParameters->GetElement(index);
	return true;
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

bool
GLPlotFitModule::GetParameterError
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
	if (index > itsErrors->GetDimensionCount())
		{
		return false;
		}
	*value	= itsErrors->GetElement(index);
	return true;		
}
