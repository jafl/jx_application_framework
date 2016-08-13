/******************************************************************************
 JVMGetLocalVars.cpp

	BASE CLASS = CMGetLocalVars

	Copyright © 2009 by John Lindal. All rights reserved.

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
	CMGetLocalVars("NOP"),
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
