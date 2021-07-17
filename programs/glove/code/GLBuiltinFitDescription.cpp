/******************************************************************************
 GLBuiltinFitDescription.cpp

	<Description>

	BASE CLASS = public GLFitDescription

	Copyright (C) 1999 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GLBuiltinFitDescription.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GLBuiltinFitDescription::GLBuiltinFitDescription
	(
	const FitType type
	)
	:
	GLFitDescription(type)
{
	if (type == GLFitDescription::kBLinear)
		{
		SetParameterCount(2);
		GetVarList()->AddVariable(JString("a0", JString::kNoCopy), 0);
		GetVarList()->AddVariable(JString("a1", JString::kNoCopy), 0);
		SetFnName(JGetString("LinearName::GLBuiltinFitDescription"));
		SetFitFunctionString(JString("a0 + a1 * x", JString::kNoCopy));
		}
	else if (type == GLFitDescription::kBExp)
		{
		SetParameterCount(2);
		GetVarList()->AddVariable(JString("a", JString::kNoCopy), 0);
		GetVarList()->AddVariable(JString("b", JString::kNoCopy), 0);
		SetFnName(JGetString("ExponentialName::GLBuiltinFitDescription"));
		SetFitFunctionString(JString("a*e^(b*x)", JString::kNoCopy));
		}
	else if (type == GLFitDescription::kBPower)
		{
		SetParameterCount(2);
		GetVarList()->AddVariable(JString("a", JString::kNoCopy), 0);
		GetVarList()->AddVariable(JString("b", JString::kNoCopy), 0);
		SetFnName(JGetString("PowerLawName::GLBuiltinFitDescription"));
		SetFitFunctionString(JString("a*x^b", JString::kNoCopy));
		}

}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLBuiltinFitDescription::~GLBuiltinFitDescription()
{
}

