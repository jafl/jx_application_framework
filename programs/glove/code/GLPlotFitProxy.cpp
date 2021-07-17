/******************************************************************************
 GLPlotFitProxy.cpp

	BASE CLASS = class GLPlotFitFunction

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include "GLPlotFitProxy.h"
#include "GLVarList.h"
#include <J2DPlotData.h>
#include <JExprParser.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GLPlotFitProxy::GLPlotFitProxy
	(
	GLPlotFitFunction*	fit,
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData
	)
	:
	GLPlotFitFunction(plot, fitData, 0, 0),
	itsErrors(nullptr),
	itsHasGOF(fit->HasParameterErrors()),
	itsGOFName(fit->HasGoodnessOfFit()),
	itsFnString(fit->GetFitFunctionString())
{
	if (itsHasGOF)
		{
		fit->GetGoodnessOfFitName(&itsGOFName);
		fit->GetGoodnessOfFit(&itsGOF);
		}
	itsParms = jnew GLVarList();
	assert(itsParms != nullptr);
	itsParms->AddVariable(JGetString("DefaultVarName::GLGlobal"), 0);
	const JSize count	= fit->GetParameterCount();
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
		{
		JString name;
		bool ok	= fit->GetParameterName(i, &name);
		assert(ok);
		JFloat value;
		ok	= fit->GetParameter(i, &value);
		assert(ok);
		itsParms->AddVariable(name, value);
		}

	if (fit->HasParameterErrors())
		{
		SetHasParameterErrors(true);
		itsErrors	= jnew JArray<JFloat>;
		assert(itsErrors != nullptr);
		for (JIndex i = 1; i <= count; i++)
			{
			JFloat value;
			bool ok	= fit->GetParameterError(i, &value);
			assert(ok);
			itsErrors->AppendElement(value);
			}
		}

	J2DPlotData* diff	= fit->GetDiffData();
	J2DPlotData* data;
	if (J2DPlotData::Create(&data, diff->GetXData(), diff->GetYData(), false))
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
	itsFn	= nullptr;

	JExprParser p(itsParms);

	const bool ok = p.Parse(itsFnString, &itsFn);
	assert( ok );

	SetHasGoodnessOfFit(itsHasGOF);
}

GLPlotFitProxy::GLPlotFitProxy
	(
	J2DPlotWidget* 		plot, 
	JPlotDataBase* 		fitData,
	std::istream& 			is
	)
	:
	GLPlotFitFunction(plot, fitData, 0, 0),
	itsErrors(nullptr)
{
	is >> JBoolFromString(itsHasGOF);
	if (itsHasGOF)
		{
		is >> itsGOFName;
		is >> itsGOF;
		}

	itsParms = jnew GLVarList();
	assert(itsParms != nullptr);
	itsParms->AddVariable(JGetString("DefaultVarName::GLGlobal"), 0);

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
		
	bool hasParameterErrors;
	is >> JBoolFromString(hasParameterErrors);
	
	if (hasParameterErrors)
		{
		SetHasParameterErrors(true);
		itsErrors = jnew JArray<JFloat>;
		assert(itsErrors != nullptr);
		for (JIndex i=1; i<=count; i++)
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
	
	itsFn = nullptr;

	JExprParser p(itsParms);

	const bool ok = p.Parse(itsFnString, &itsFn);
	assert( ok );

	SetHasGoodnessOfFit(itsHasGOF);
	GenerateDiffData();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLPlotFitProxy::~GLPlotFitProxy()
{
	jdelete itsParms;
	jdelete itsErrors;
}

/******************************************************************************
 WriteData

 *****************************************************************************/

void
GLPlotFitProxy::WriteData
	(
	std::ostream& os
	)
{
	os << JBoolToString(itsHasGOF) << ' ';
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
		
	os << JBoolToString(HasParameterErrors());
	
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

bool
GLPlotFitProxy::GetParameterName
	(
	const JIndex 	index, 
	JString* 		name
	)
	const
{
	if (index >= itsParms->GetVariableCount())
		{
		return false;
		}
	*name	= itsParms->GetVariableName(index + 1);
	return true;
}

/******************************************************************************
 GetParameter (public)

 ******************************************************************************/

bool
GLPlotFitProxy::GetParameter
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

bool
GLPlotFitProxy::GetParameterError
	(
	const JIndex 	index, 
	JFloat*			value
	)
	const
{
	if (!HasParameterErrors())
		{
		return false;
		}

	assert(itsErrors != nullptr);
	if (index > itsErrors->GetElementCount())
		{
		return false;
		}

	*value	= itsErrors->GetElement(index);
	return true;
}

/******************************************************************************
 GetGoodnessOfFitName (public)

 ******************************************************************************/

bool
GLPlotFitProxy::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	if (!itsHasGOF)
		{
		return false;
		}
	*name	= itsGOFName;
	return true;
}

/******************************************************************************
 GetGoodnessOfFit (public)

 ******************************************************************************/

bool
GLPlotFitProxy::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	if (!itsHasGOF)
		{
		return false;
		}
	*value	= itsGOF;
	return true;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

JString
GLPlotFitProxy::GetFitFunctionString()
	const
{
	return itsFnString;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

JString
GLPlotFitProxy::GetFunctionString()
	const
{
	return itsFnString;
}

/******************************************************************************
 GetYValue (public)

 ******************************************************************************/

bool
GLPlotFitProxy::GetYValue
	(
	const JFloat 	x, 
	JFloat* 		y
	)
	const
{
	itsParms->SetValue(1, x);
	return itsFn->Evaluate(y);
}
