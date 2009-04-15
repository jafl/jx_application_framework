/******************************************************************************
 JXMDIServer.h

	Interface for the JXMDIServer class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMDIServer
#define _H_JXMDIServer

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JMDIServer.h>

class JXMDIServer : public JMDIServer
{
public:

	JXMDIServer();

	virtual ~JXMDIServer();

protected:

	virtual JBoolean	CanAcceptMDIRequest();
	virtual void		PreprocessArgList(JPtrArray<JString>* argList);

private:

	// not allowed

	JXMDIServer(const JXMDIServer& source);
	const JXMDIServer& operator=(const JXMDIServer& source);
};

#endif
