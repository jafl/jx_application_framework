/******************************************************************************
 JPlotFitProxy.cpp

	BASE CLASS = class JPlotFitFunction

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <glStdInc.h>
#include <JPlotFitProxy.h>

#include "GVarList.h"

#include <J2DPlotData.h>
#include <JFunction.h>
#include <jParseFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JPlotFitProxy::JPlotFitProxy
	(
	JPlotFitFunction*	fit,
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData
	)
	:
	JPlotFitFunction(plot, fitData, 0, 0),
	itsErrors(NULL),
	itsHasGOF(fit->HasParameterErrors()),
	itsGOFName(fit->HasGoodnessOfFit()),
	itsFnString(fit->GetFitFunctionString())
{
	if (itsHasGOF)
		{
		fit->GetGoodnessOfFitName(&itsGOFName);
		fit->GetGoodnessOfFit(&itsGOF);
		}
	itsParms	= new GVarList();
	assert(itsParms != NULL);
	itsParms->AddVariable("x", 0);
	const JSize count	= fit->GetParameterCount();
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
		{
		JString name;
		JBoolean ok	= fit->GetParameterName(i, &name);
		assert(ok);
		JFloat value;
		ok	= fit->GetParameter(i, &value);
		assert(ok);
		itsParms->AddVariable(name, value);
		}

	if (fit->HasParameterErrors())
		{
		SetHasParameterErrors(kJTrue);
		itsErrors	= new JArray<JFloat>;
		assert(itsErrors != NULL);
		for (JIndex i = 1; i <= count; i++)
			{
			JFloat value;
			JBoolean ok	= fit->GetParameterError(i, &value);
			assert(ok);
			itsErrors->AppendElement(value);
			}
		}

	J2DPlotData* diff	= fit->GetDiffData();
	J2DPlotData* data;
	if (J2DPlotData::Create(&data, diff->GetXData(), diff->GetYData(), kJFalse))
		{
		const JArray<JFloat>* xerrors;
		if (diff->GetXPErrorData(&xerrors))
			{
			data->SetXErrors(*xerrors);
			}
		const JArray<JFloat>* yerrors;
		if (diff->GetYPErrorData(&yerrors))
			{
			data->SetYErrors(*yerrors);
			}
		SetDiffData(data);
		}
	JFloat xMin;
	JFloat xMax;
	fit->GetXRange(&xMin, &xMax);
	SetXRange(xMin, xMax);
	itsFn	= NULL;
	JParseFunction(itsFnString, itsParms, &itsFn);
	SetHasGoodnessOfFit(itsHasGOF);
}

JPlotFitProxy::JPlotFitProxy
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	istream& 			is
	)
	:
	JPlotFitFunction(plot, fitData, 0, 0),
	itsErrors(NULL)
{
	is >> itsHasGOF;
	if (itsHasGOF)
		{
		is >> itsGOFName;
		is >> itsGOF;
		}

	itsParms	= new GVarList();
	assert(itsParms != NULL);
	itsParms->AddVariable("x", 0);

	JSize count;
	is >> count;
	for (JIndex i = 1; i <= count; i++)
		{
		JString name;
		is >> name;
		JFloat value;
		is >> value;
		itsParms->AddVariable(name, value);
		}

	SetParameterCount(count);
		
	JBoolean hasParameterErrors;
	is >> hasParameterErrors;
	
	if (hasParameterErrors)
		{
		SetHasParameterErrors(kJTrue);
		itsErrors	= new JArray<JFloat>;
		assert(itsErrors != NULL);
		for (JIndex i = 1; i <= count; i++)
			{
			JFloat value;
			is >> value;
			itsErrors->AppendElement(value);
			}
		}
	
	JFloat xMin;
	JFloat xMax;

	is >> xMin;
	is >> xMax;

	SetXRange(xMin, xMax);

	is >> itsFnString;
	
	itsFn	= NULL;
	JParseFunction(itsFnString, itsParms, &itsFn);
	SetHasGoodnessOfFit(itsHasGOF);

	GenerateDiffData();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JPlotFitProxy::~JPlotFitProxy()
{
	delete itsParms;
	delete itsErrors;
}

/******************************************************************************
 WriteData

 *****************************************************************************/

void
JPlotFitProxy::WriteData
	(
	ostream& os
	)
{
	os << itsHasGOF << ' ';
	if (itsHasGOF)
		{
		os << itsGOFName << ' ';
		os << itsGOF << ' ';
		}

	JSize count	= itsParms->GetVariableCount();
	os << count - 1;

	for (JIndex i = 2; i <= count; i++) // don't count 'x'
		{
		JString name	= itsParms->GetVariableName(i);		
		os << name << ' ';
		JFloat value;
		itsParms->GetNumericValue(i, 1, &value);
		os << value << ' ';
		}
		
	os << HasParameterErrors();
	
	if (HasParameterErrors())
		{
		for (JIndex i = 1; i < count; i++)
			{
			os << itsErrors->GetElement(i) << ' ';
			}
		}
	
	JFloat xMin;
	JFloat xMax;

	GetXRange(&xMin, &xMax);

	os << xMin << ' ';
	os << xMax << ' ';

	os << itsFnString << ' ';	
}

/******************************************************************************
 GetParameterName (public)

 ******************************************************************************/

JBoolean
JPlotFitProxy::GetParameterName
	(
	const JIndex 	index, 
	JString* 		name
	)
	const
{
	if (index >= itsParms->GetVariableCount())
		{
		return kJFalse;
		}
	*name	= itsParms->GetVariableName(index + 1);
	return kJTrue;
}

/******************************************************************************
 GetParameter (public)

 ******************************************************************************/

JBoolean
JPlotFitProxy::GetParameter
	(
	const JIndex 	index, 
	JFloat* 		value
	)
	const
{
	return itsParms->GetNumericValue(index + 1, 1, value);
}

/******************************************************************************
 GetParameterError (public)

 ******************************************************************************/

JBoolean
JPlotFitProxy::GetParameterError
	(
	const JIndex 	index, 
	JFloat*			value
	)
	const
{
	if (!HasParameterErrors())
		{
		return kJFalse;
		}

	assert(itsErrors != NULL);
	if (index > itsErrors->GetElementCount())
		{
		return kJFalse;
		}

	*value	= itsErrors->GetElement(index);
	return kJTrue;
}

/******************************************************************************
 GetGoodnessOfFitName (public)

 ******************************************************************************/

JBoolean
JPlotFitProxy::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	if (!itsHasGOF)
		{
		return kJFalse;
		}
	*name	= itsGOFName;
	return kJTrue;
}

/******************************************************************************
 GetGoodnessOfFit (public)

 ******************************************************************************/

JBoolean
JPlotFitProxy::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	if (!itsHasGOF)
		{
		return kJFalse;
		}
	*value	= itsGOF;
	return kJTrue;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

JString
JPlotFitProxy::GetFitFunctionString()
	const
{
	return itsFnString;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

JString
JPlotFitProxy::GetFunctionString()
	const
{
	return itsFnString;
}

/******************************************************************************
 GetYValue (public)

 ******************************************************************************/

JBoolean
JPlotFitProxy::GetYValue
	(
	const JFloat 	x, 
	JFloat* 		y
	)
	const
{
	itsParms->SetValue(1, x);
	return itsFn->Evaluate(y);
}
