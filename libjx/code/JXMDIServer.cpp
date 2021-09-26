/******************************************************************************
 JXMDIServer.cpp

	Implements CanAcceptMDIRequest() for use with JXApplication.

	BASE CLASS = JMDIServer

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXMDIServer.h"
#include "jx-af/jx/jXGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMDIServer::JXMDIServer()
	:
	JMDIServer(JXGetApplication()->GetSignature().GetBytes())
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

bool
JXMDIServer::CanAcceptMDIRequest()
{
	JXApplication* app = JXGetApplication();
	return !app->HasBlockingWindow() && !app->IsSuspended();
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
		if (*(argList->GetElement(i)) == JMDIServer::kQuitOptionName)
		{
			JXGetApplication()->Quit();
			argList->CleanOut();	// don't invoke HandleMDIRequest()
			break;
		}
	}
}
