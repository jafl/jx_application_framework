/******************************************************************************
 THXMDIServer.h

	Interface for the THXMDIServer class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXMDIServer
#define _H_THXMDIServer

#include <JXMDIServer.h>

class THXMDIServer : public JXMDIServer
{
public:

	THXMDIServer();

	virtual ~THXMDIServer();

	static void	PrintCommandLineHelp();

protected:

	virtual void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) override;

private:

	// not allowed

	THXMDIServer(const THXMDIServer& source);
	const THXMDIServer& operator=(const THXMDIServer& source);
};

#endif
