/*********************************************************************************
 GLFitBase.cpp
  
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "GLFitBase.h"
#include "jAssert.h"

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

GLFitBase::GLFitBase()
{
	itsParameterCount = 0;
	itsHasParameterErrors = kJFalse;
	itsHasGOF = kJFalse;
}


GLFitBase::GLFitBase
	(
	const JSize paramCount, 
	const JBoolean errors, 
	const JBoolean gof
	)
	:
	itsParameterCount(paramCount),
	itsHasParameterErrors(errors),
	itsHasGOF(gof)
{
}

/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
GLFitBase::~GLFitBase()
{

}

/*********************************************************************************
 SetHasParameterErrors
 

 ********************************************************************************/
 
void
GLFitBase::SetHasParameterErrors
	(
	const JBoolean errors
	)
{
	itsHasParameterErrors = errors;
}

/*********************************************************************************
 SetParameterCount
 

 ********************************************************************************/
 
void
GLFitBase::SetParameterCount
	(
	const JSize count
	)
{
	itsParameterCount = count;
}

/*********************************************************************************
 SetHasGoodnessOfFit
 

 ********************************************************************************/
 
void
GLFitBase::SetHasGoodnessOfFit
	(
	const JBoolean gof
	)
{
	itsHasGOF = gof;

}
