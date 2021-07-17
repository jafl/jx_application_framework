/******************************************************************************
 GLPlotFitNonLinear.cpp

	<Description>

	BASE CLASS = public GLPlotFitBase

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include "GLPlotFitNonLinear.h"
#include "GLVarList.h"
#include <JExprParser.h>
#include <JVector.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GLPlotFitNonLinear::GLPlotFitNonLinear
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotFitBase(plot, fitData, xMin, xMax),
	itsVarList(nullptr),
	itsFunction(nullptr),
	itsFPrimed(nullptr),
	itsErrors(nullptr)
{
}

GLPlotFitNonLinear::GLPlotFitNonLinear
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
	itsVarList(nullptr),
	itsFunction(nullptr),
	itsFPrimed(nullptr),
	itsErrors(nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLPlotFitNonLinear::~GLPlotFitNonLinear()
{
//	jdelete itsVarList; 		we don't own it
	jdelete itsFunction;
	jdelete itsFPrimed;
	jdelete itsErrors;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
GLPlotFitNonLinear::SetCurrentParameters
	(
	const JVector& p
	)
{
	assert(itsVarList != nullptr);

	const JSize count	= p.GetDimensionCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsVarList->SetValue(i + 1, p.GetElement(i));
		}
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
GLPlotFitNonLinear::SetErrors
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
GLPlotFitNonLinear::FunctionN
	(
	const JFloat x
	)
{
	assert(itsVarList != nullptr);
	assert(itsFunction != nullptr);
	itsVarList->SetValue(1, x);
	JFloat y = 0;;
	itsFunction->Evaluate(&y);
	return y;
}

/******************************************************************************
 FunctionN (virtual protected)

 ******************************************************************************/

JFloat
GLPlotFitNonLinear::FunctionNPrimed
	(
	const JFloat x
	)
{
	if (itsFPrimed == nullptr)
		{
		return GLPlotFitBase::FunctionNPrimed(x);
		}
	assert(itsVarList != nullptr);
	assert(itsFunction != nullptr);
	itsVarList->SetValue(1, x);
	JFloat y = 0;
	itsFPrimed->Evaluate(&y);
	return y;
}

/******************************************************************************
 SetVarList (public)

 ******************************************************************************/

void
GLPlotFitNonLinear::SetVarList
	(
	GLVarList* list
	)
{
	itsVarList	= list;
	const JSize count	= list->GetVariableCount() - 1;
	SetParameterCount(count);
	itsErrors	= jnew JVector(count);
	assert(itsErrors != nullptr);
}

/******************************************************************************
 SetFunction (public)

 ******************************************************************************/

void
GLPlotFitNonLinear::SetFunction
	(
	const JString& function
	)
{
	assert(itsVarList != nullptr);
	SetFunctionString(function);

	JExprParser p(itsVarList);

	const bool ok = p.Parse(function, &itsFunction);
	assert(ok);
}

/******************************************************************************
 SetFPrimed (public)

 ******************************************************************************/

void
GLPlotFitNonLinear::SetFPrimed
	(
	const JString& fPrimed
	)
{
	assert(itsVarList != nullptr);

	JExprParser p(itsVarList);

	const bool ok = p.Parse(fPrimed, &itsFPrimed);
	assert(ok);
}

/******************************************************************************
 SetInitialParameters (public)

 ******************************************************************************/

void
GLPlotFitNonLinear::SetInitialParameters
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
GLPlotFitNonLinear::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	GLPlotFitNonLinear* th	= const_cast< GLPlotFitNonLinear* >(this);
	*y	= th->FunctionN(x);
	return true;
}


/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

bool
GLPlotFitNonLinear::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	*name	= itsVarList->GetVariableName(index + 1);
	return true;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

bool
GLPlotFitNonLinear::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	bool ok	= itsVarList->GetNumericValue(index + 1, 1, value);
	return ok;
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

bool
GLPlotFitNonLinear::GetParameterError
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
