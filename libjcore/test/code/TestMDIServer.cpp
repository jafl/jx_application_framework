/******************************************************************************
 TestMDIServer.cpp

	BASE CLASS = JMDIServer

	Written by John Lindal.

 ******************************************************************************/

#include "TestMDIServer.h"
#include <JString.h>
#include <ace/Reactor.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestMDIServer::TestMDIServer
	(
	const JUtf8Byte* signature
	)
	:
	JMDIServer(signature),
	itsQuitFlag(kJFalse)
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

JBoolean
TestMDIServer::CanAcceptMDIRequest()
{
	return kJTrue;
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
	const JSize argCount = argList.GetElementCount();
	if (argCount == 2 && *argList.GetElement(2) == "--quit")
		{
		itsQuitFlag = kJTrue;
		}
}
