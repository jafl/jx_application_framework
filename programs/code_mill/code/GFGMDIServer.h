/******************************************************************************
 GFGMDIServer.h

	Copyright (C) 2002 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_GFGMDIServer
#define _H_GFGMDIServer

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
