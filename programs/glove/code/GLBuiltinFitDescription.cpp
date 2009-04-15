/******************************************************************************
 GLBuiltinFitDescription.cpp

	<Description>

	BASE CLASS = public GLFitDescription

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <glStdInc.h>
#include <GLBuiltinFitDescription.h>

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
		GetVarList()->AddVariable("a0", 0);
		GetVarList()->AddVariable("a1", 0);
		SetFnName("Linear");
		SetFitFunctionString("a0 + a1 * x");
		}
	else if (type == GLFitDescription::kBExp)
		{
		SetParameterCount(2);
		GetVarList()->AddVariable("a", 0);
		GetVarList()->AddVariable("b", 0);
		SetFnName("Exponential");
		SetFitFunctionString("a*e^(b*x)");
		}
	else if (type == GLFitDescription::kBPower)
		{
		SetParameterCount(2);
		GetVarList()->AddVariable("a", 0);
		GetVarList()->AddVariable("b", 0);
		SetFnName("Power Law");
		SetFitFunctionString("a*x^b");
		}

}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLBuiltinFitDescription::~GLBuiltinFitDescription()
{
}

