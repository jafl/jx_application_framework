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
	itsHasParameterErrors = false;
	itsHasGOF = false;
}


GLFitBase::GLFitBase
	(
	const JSize paramCount, 
	const bool errors, 
	const bool gof
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
	const bool errors
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
	const bool gof
	)
{
	itsHasGOF = gof;

}
