/******************************************************************************
 JVMGetLocalVars.cpp

	BASE CLASS = CMGetLocalVars

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetLocalVars.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetLocalVars::JVMGetLocalVars
	(
	CMVarNode* rootNode
	)
	:
	CMGetLocalVars(JString("NOP", JString::kNoCopy)),
	itsRootNode(rootNode)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetLocalVars::~JVMGetLocalVars()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetLocalVars::HandleSuccess
	(
	const JString& data
	)
{
}
