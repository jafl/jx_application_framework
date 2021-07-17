/******************************************************************************
 GLModuleFitDescription.cpp

	<Description>

	BASE CLASS = public GLFitDescription

	Copyright (C) 1999 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GLModuleFitDescription.h>
#include "GLDLFitModule.h"
#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GLModuleFitDescription::GLModuleFitDescription
	(
	GLDLFitModule* fit
	)
	:
	GLFitDescription(GLFitDescription::kModule, fit->GetFunctionalForm(), fit->GetFitName()),
	itsModule(fit)
{
	const JSize count	= fit->GetParameterCount();
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
		{
		GetVarList()->AddVariable(itsModule->GetParameterName(i), 0);
		}
	DoesRequireStartValues(true);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLModuleFitDescription::~GLModuleFitDescription()
{
}

