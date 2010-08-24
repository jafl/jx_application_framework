/******************************************************************************
 MDMDIServer.cc

	BASE CLASS = public JXMDIServer

	Copyright © 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <mdStdInc.h>
#include "MDMDIServer.h"
#include "mdGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

MDMDIServer::MDMDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDMDIServer::~MDMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
MDMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
}
