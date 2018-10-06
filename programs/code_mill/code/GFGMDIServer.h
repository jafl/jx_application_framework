/******************************************************************************
 GFGMDIServer.h

	Copyright (C) 2002 by Glenn W. Bach.

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

	virtual void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) override;

private:

	// not allowed

	GFGMDIServer(const GFGMDIServer& source);
	const GFGMDIServer& operator=(const GFGMDIServer& source);
};

#endif
