/*********************************************************************************
 JFitBase.cpp
 
	JFitBase class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/

#include <glStdInc.h>
#include "JFitBase.h"
#include "jAssert.h"

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

JFitBase::JFitBase()
{
	itsParameterCount = 0;
	itsHasParameterErrors = kJFalse;
	itsHasGOF = kJFalse;
}


JFitBase::JFitBase
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
 
JFitBase::~JFitBase()
{

}

/*********************************************************************************
 SetHasParameterErrors
 

 ********************************************************************************/
 
void
JFitBase::SetHasParameterErrors
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
JFitBase::SetParameterCount
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
JFitBase::SetHasGoodnessOfFit
	(
	const JBoolean gof
	)
{
	itsHasGOF = gof;

}
