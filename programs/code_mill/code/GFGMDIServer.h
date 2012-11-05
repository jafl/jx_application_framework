/******************************************************************************
 GFGMDIServer.h

	Copyright © 2002 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_GFGMDIServer
#define _H_GFGMDIServer

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMDIServer.h>

class GFGMDIServer : public JXMDIServer
{
public:

	GFGMDIServer();

	virtual	~GFGMDIServer();

	static void	PrintCommandLineHelp();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	// not allowed

	GFGMDIServer(const GFGMDIServer& source);
	const GFGMDIServer& operator=(const GFGMDIServer& source);
};

#endif
