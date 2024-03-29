/******************************************************************************
 TestMDIServer.cpp

	BASE CLASS = JMDIServer

	Written by John Lindal.

 ******************************************************************************/

#include "TestMDIServer.h"
#include "JString.h"
#include <ace/Reactor.h>
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

TestMDIServer::TestMDIServer
	(
	const JUtf8Byte* signature
	)
	:
	JMDIServer(signature),
	itsQuitFlag(false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestMDIServer::~TestMDIServer()
{
}

/******************************************************************************
 CanAcceptMDIRequest (virtual protected)

 ******************************************************************************/

bool
TestMDIServer::CanAcceptMDIRequest()
{
	return true;
}

/******************************************************************************
 HandleMDIRequest (virtual protected)

 ******************************************************************************/

void
TestMDIServer::HandleMDIRequest
	(
	const JString&				dir,
	const JPtrArray<JString>&	argList
	)
{
	const JSize argCount = argList.GetItemCount();
	if (argCount == 2 && *argList.GetItem(2) == "--quit")
	{
		itsQuitFlag = true;
	}
}
