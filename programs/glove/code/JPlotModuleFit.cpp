/*********************************************************************************
 JPlotModuleFit.cpp
 
	JPlotModuleFit class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#include "JPlotModuleFit.h"
#include "J2DPlotWidget.h"
#include "JPlotDataBase.h"
#include "GVarList.h"
#include <JFunction.h>
#include <jParseFunction.h>

#include <JString.h>
#include <JArray.h>
#include <jMath.h>
#include <jAssert.h>

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

JPlotModuleFit::JPlotModuleFit
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	const JFloat		xMin,
	const JFloat		xMax,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	GVarList* 			list, 
	const JSize 		parmscount,	
	const JBoolean 		errors, 
	const JBoolean 		gof
	)
	:
	JPlotFitFunction(plot, fitData, xMin, xMax)
{
	itsUsingRange = kJFalse;
	JPlotModuleFitX(plot, fitData, names, values, function, list, parmscount, errors, gof);
}

JPlotModuleFit::JPlotModuleFit
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	GVarList* 			list,
	const JFloat 		xmin, 
	const JFloat 		xmax,
	const JFloat 		ymin, 
	const JFloat 		ymax,
	const JSize 		parmscount,	
	const JBoolean 		errors, 
	const JBoolean 		gof
	)
	:
	JPlotFitFunction(plot, fitData, xmin, xmax)
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

JPlotModuleFit::JPlotModuleFit
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	istream& 			is
	)
	:
	JPlotFitFunction(plot, fitData, 0, 10)
{
	JPtrArray<JString>* names = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	JArray<JFloat>* values = new JArray<JFloat>;
	int count;
	is >> count;
	for (int i = 1; i <= count; i++)
		{
		JString* name = new JString();
		JFloat value;
		is >> *name;
		names->Append(name);
		is >> value;
		values->AppendElement(value);
		}
	int parmscount;
	is >> parmscount;
	JBoolean errors;
	is >> errors;
	JBoolean gof;
	is >> gof;
	JString fstring;
	is >> fstring;
	GVarList* list = new GVarList;
	list->AddVariable("x", 0);
	for (int i = 1; i <= parmscount; i++)
		{
		JSize index = i;
		if (errors)
			{
			index = i * 2 - 1;
			}
		JString parm(*(names->NthElement(index)));
		JFloat value = values->GetElement(index);
		JBoolean added = list->AddVariable(parm, value);
		}
	JFunction* f;
	JParseFunction(fstring, list, &f);
	JPlotModuleFitX(plot, fitData, names, values, f, list, parmscount, errors, gof);	
}

void
JPlotModuleFit::JPlotModuleFitX
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	GVarList* 			list,
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
	itsFunctionName = new JString(function->Print());
	GenerateDiffData();
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
JPlotModuleFit::~JPlotModuleFit()
{
	itsNames->DeleteAll();
	delete itsNames;
	delete itsValues;
	delete itsFunction;
	delete itsFunctionName;
	delete itsList;
}

/*********************************************************************************
 GetElement
 

 ********************************************************************************/

void
JPlotModuleFit::GetElement
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
JPlotModuleFit::GetYValue
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
 GetYRange
 

 ********************************************************************************/

JBoolean
JPlotModuleFit::GetYRange
	(
	JFloat* min, 
	JFloat* max,
	JFloat  xMin, 
	JFloat  xMax
	)
{
	JFloat tempMin;
	GetYValue(xMin, &tempMin);
	JFloat tempMax;
	GetYValue(xMax, &tempMax);
	
	if (tempMin <= tempMax)
		{
		*min = tempMin;
		*max = tempMax;
		}
	else
		{
		*min = tempMax;
		*max = tempMin;
		}
	return kJTrue;
}

/*********************************************************************************
 UpdateFunction
 

 ********************************************************************************/

void
JPlotModuleFit::UpdateFunction
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
JPlotModuleFit::GetParameterName
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
JPlotModuleFit::GetParameter
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
JPlotModuleFit::GetParameterError
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
JPlotModuleFit::GetGoodnessOfFitName
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
	*name = *(itsNames->NthElement(arrayIndex));
	return kJTrue;
}

/*********************************************************************************
 GetGoodnessOfFit
 

 ********************************************************************************/

JBoolean
JPlotModuleFit::GetGoodnessOfFit
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
JPlotModuleFit::GetFunctionString()
	const
{
	return *itsFunctionName;
}

/*********************************************************************************
 GetFitFunctionString
 

 ********************************************************************************/

JString
JPlotModuleFit::GetFitFunctionString()
	const
{
	return *itsFunctionName;
}

/*********************************************************************************
 DataElementValid
 

 ********************************************************************************/

JBoolean
JPlotModuleFit::DataElementValid
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
JPlotModuleFit::GetDataElement
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
JPlotModuleFit::WriteData
	(
	ostream& os
	)
{
	JSize count = itsNames->GetElementCount();
	os << count << " ";
	for (JSize i = 1; i <= count; i++)
		{
		os << *(itsNames->NthElement(i)) << " ";
		os << itsValues->GetElement(i) << " ";
		}
	os << GetParameterCount() << " ";
	os << HasParameterErrors() << " ";
	os << HasGoodnessOfFit() << " ";
	os << GetFunctionString() << " ";
}
