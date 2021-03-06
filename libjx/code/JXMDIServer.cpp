/******************************************************************************
 JXMDIServer.cpp

	Implements CanAcceptMDIRequest() for use with JXApplication.

	BASE CLASS = JMDIServer

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXMDIServer.h>
#include <jXGlobals.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMDIServer::JXMDIServer()
	:
	JMDIServer((JXGetApplication())->GetSignature())
{
	JXSetMDIServer(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMDIServer::~JXMDIServer()
{
}

/******************************************************************************
 CanAcceptMDIRequest (virtual protected)

 ******************************************************************************/

JBoolean
JXMDIServer::CanAcceptMDIRequest()
{
	JXApplication* app = JXGetApplication();
	return JNegate( app->HasBlockingWindow() || app->IsSuspended() );
}

/******************************************************************************
 PreprocessArgList (virtual protected)

 ******************************************************************************/

void
JXMDIServer::PreprocessArgList
	(
	JPtrArray<JString>* argList
	)
{
	JXApplication::StripBaseOptions(argList);

	const JSize count = argList->GetElementCount();
	for (JIndex i=2; i<=count; i++)
		{
		if (*(argList->NthElement(i)) == JMDIServer::kQuitOptionName)
			{
			(JXGetApplication())->Quit();
			argList->CleanOut();	// don't invoke HandleMDIRequest()
			break;
			}
		}
}
