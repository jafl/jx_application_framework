/******************************************************************************
 MDMDIServer.h

	Copyright © 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_MDMDIServer
#define _H_MDMDIServer

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMDIServer.h>

class MDMDIServer : public JXMDIServer
{
public:

	MDMDIServer();

	virtual	~MDMDIServer();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	// not allowed

	MDMDIServer(const MDMDIServer& source);
	const MDMDIServer& operator=(const MDMDIServer& source);
};

#endif
