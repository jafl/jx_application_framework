/*********************************************************************************
 GLPlotModuleFit.cpp
 
	GLPlotModuleFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotModuleFit.h"
#include "J2DPlotWidget.h"
#include "JPlotDataBase.h"
#include "GLVarList.h"
#include <JExprParser.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

GLPlotModuleFit::GLPlotModuleFit
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	const JFloat		xMin,
	const JFloat		xMax,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	GLVarList* 			list, 
	const JSize 		parmscount,	
	const JBoolean 		errors, 
	const JBoolean 		gof
	)
	:
	GLPlotFitFunction(plot, fitData, xMin, xMax)
{
	itsUsingRange = kJFalse;
	JPlotModuleFitX(plot, fitData, names, values, function, list, parmscount, errors, gof);
}

GLPlotModuleFit::GLPlotModuleFit
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	GLVarList* 			list,
	const JFloat 		xmin, 
	const JFloat 		xmax,
	const JFloat 		ymin, 
	const JFloat 		ymax,
	const JSize 		parmscount,	
	const JBoolean 		errors, 
	const JBoolean 		gof
	)
	:
	GLPlotFitFunction(plot, fitData, xmin, xmax)
{
	if (xmax > xmin)
		{
		itsRangeXMax = xmax;
		itsRangeXMin = xmin;
		}
	else
		{
		itsRangeXMax = xmin;
		itsRangeXMin = xmax;
		}
	if (ymax > ymin)
		{
		itsRangeYMax = ymax;
		itsRangeYMin = ymin;
		}
	else
		{
		itsRangeYMax = ymin;
		itsRangeYMin = ymax;
		}
	itsUsingRange = kJTrue;
	JPlotModuleFitX(plot, fitData, names, values, function, list, parmscount, errors, gof);
}

GLPlotModuleFit::GLPlotModuleFit
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	std::istream& 	is
	)
	:
	GLPlotFitFunction(plot, fitData, 0, 10)
{
	JPtrArray<JString>* names = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	JArray<JFloat>* values = jnew JArray<JFloat>;
	int count;
	is >> count;
	for (int i = 1; i <= count; i++)
		{
		JString* name = jnew JString();
		JFloat value;
		is >> *name;
		names->Append(name);
		is >> value;
		values->AppendElement(value);
		}
	int parmscount;
	is >> parmscount;
	JBoolean errors;
	is >> JBoolFromString(errors);
	JBoolean gof;
	is >> JBoolFromString(gof);
	JString fstring;
	is >> fstring;
	GLVarList* list = jnew GLVarList;
	list->AddVariable(JGetString("DefaultVarName::GLGlobal"), 0);
	for (int i = 1; i <= parmscount; i++)
		{
		JSize index = i;
		if (errors)
			{
			index = i * 2 - 1;
			}
		JString parm(*(names->GetElement(index)));
		JFloat value = values->GetElement(index);
		list->AddVariable(parm, value);
		}

	JExprParser p(list);

	JFunction* f;
	const JBoolean ok = p.Parse(fstring, &f);
	assert( ok );

	JPlotModuleFitX(plot, fitData, names, values, f, list, parmscount, errors, gof);	
}

void
GLPlotModuleFit::JPlotModuleFitX
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	GLVarList* 			list,
	const JSize 		parmscount,	
	const JBoolean 		errors, 
	const JBoolean 		gof
	)
{
	SetHasParameterErrors(errors);
	SetParameterCount(parmscount);
	SetHasGoodnessOfFit(gof);
	itsNames = names;
	itsValues = values;
	itsFunction = function;
	itsList = list;
	itsFunctionName = function->Print();
	GenerateDiffData();
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
GLPlotModuleFit::~GLPlotModuleFit()
{
	itsNames->DeleteAll();
	jdelete itsNames;
	jdelete itsValues;
	jdelete itsFunction;
	jdelete itsList;
}

/*********************************************************************************
 GetElement
 

 ********************************************************************************/

void
GLPlotModuleFit::GetElement
	(
	const JIndex index, 
	J2DDataPoint* data
	)
	const
{
	JFloat range = itsCurrentXMax - itsCurrentXMin;
	JFloat x = itsCurrentXMin + (index-1)*range/itsCurrentStepCount;
	JFloat y;
	itsList->SetValue(1, x);
	itsFunction->Evaluate(&y);
	data->x = x;
	data->y = y;
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

JBoolean
GLPlotModuleFit::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	itsList->SetValue(1, x);
	itsFunction->Evaluate(y);
	return kJTrue;
}

/*********************************************************************************
 UpdateFunction
 

 ********************************************************************************/

void
GLPlotModuleFit::UpdateFunction
	(
	const JFloat xmin, 
	const JFloat xmax, 
	const JSize steps
	)
{
	itsCurrentXMin = xmin;
	itsCurrentXMax = xmax;
	itsCurrentStepCount = steps;
	SetElementCount(steps+1);
}

/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

JBoolean
GLPlotModuleFit::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	if ((index > GetParameterCount()) || (index < 1))
		{
		return kJFalse;
		}
	
	*name = itsList->GetVariableName(index + 1);
	return kJTrue;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

JBoolean
GLPlotModuleFit::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	if ((index > GetParameterCount()) || (index < 1))
		{
		return kJFalse;
		}
	return itsList->GetNumericValue(index + 1, 1, value);
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

JBoolean
GLPlotModuleFit::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	const JPlotDataBase* data = GetDataToFit();
	if (!data->HasXErrors() && !data->HasYErrors())
		{
		return kJFalse;
		}
	if (!HasParameterErrors())
		{
		return kJFalse;
		}
	JIndex arrayIndex = index * 2;
	*value = itsValues->GetElement(arrayIndex);
	return kJTrue;
}

/*********************************************************************************
 GetGoodnessOfFitName
 

 ********************************************************************************/

JBoolean
GLPlotModuleFit::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	if (!HasGoodnessOfFit())
		{
		return kJFalse;
		}
	JIndex arrayIndex = itsNames->GetElementCount();
	*name = *(itsNames->GetElement(arrayIndex));
	return kJTrue;
}

/*********************************************************************************
 GetGoodnessOfFit
 

 ********************************************************************************/

JBoolean
GLPlotModuleFit::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	if (!HasGoodnessOfFit())
		{
		return kJFalse;
		}
	JIndex arrayIndex = itsValues->GetElementCount();
	*value = itsValues->GetElement(arrayIndex);
	
	return kJTrue;
}

/*********************************************************************************
 GetFunctionString
 

 ********************************************************************************/

JString
GLPlotModuleFit::GetFunctionString()
	const
{
	return itsFunctionName;
}

/*********************************************************************************
 GetFitFunctionString
 

 ********************************************************************************/

JString
GLPlotModuleFit::GetFitFunctionString()
	const
{
	return itsFunctionName;
}

/*********************************************************************************
 DataElementValid
 

 ********************************************************************************/

JBoolean
GLPlotModuleFit::DataElementValid
	(
	const JIndex index
	)
{
	const JPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	data->GetElement(index, &point);
	
	if (itsUsingRange)
		{		
		if ((point.x >= itsRangeXMin) &&
			(point.x <= itsRangeXMax) &&
			(point.y >= itsRangeYMin) &&
			(point.y <= itsRangeYMax))
			{
			return kJTrue;
			}
		else
			{
			return kJFalse;
			}
		}
	return kJTrue;
}

/******************************************************************************
 GetDataElement
 

 *****************************************************************************/

JBoolean
GLPlotModuleFit::GetDataElement
	(
	const JIndex index,
	J2DDataPoint* point
	)
{
	JBoolean valid = DataElementValid(index);
	if (!valid)
		{
		return kJFalse;
		}
	const JPlotDataBase* data = GetDataToFit();
	data->GetElement(index, point);
	return kJTrue;
}

/*********************************************************************************
 WriteData
 

 ********************************************************************************/

void
GLPlotModuleFit::WriteData
	(
	std::ostream& os
	)
{
	JSize count = itsNames->GetElementCount();
	os << count << " ";
	for (JSize i = 1; i <= count; i++)
		{
		os << *(itsNames->GetElement(i)) << " ";
		os << itsValues->GetElement(i) << " ";
		}
	os << GetParameterCount() << " ";
	os << JBoolToString(HasParameterErrors())
	   << JBoolToString(HasGoodnessOfFit()) << " ";
	os << GetFunctionString() << " ";
}