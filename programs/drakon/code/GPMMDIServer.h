/******************************************************************************
 GPMMDIServer.h

	Copyright (C) 2001 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_GPMMDIServer
#define _H_GPMMDIServer

#include <JXMDIServer.h>

class GPMMainDirector;

class GPMMDIServer : public JXMDIServer
{
public:

	GPMMDIServer();

	virtual ~GPMMDIServer();

	static void	PrintCommandLineHelp();

	void	SetMainDirector(GPMMainDirector* dir);

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	GPMMainDirector*	itsMainDirector;

private:

	// not allowed

	GPMMDIServer(const GPMMDIServer& source);
	const GPMMDIServer& operator=(const GPMMDIServer& source);
};


/******************************************************************************
 SetMainDirector

 *****************************************************************************/

inline void
GPMMDIServer::SetMainDirector
	(
	GPMMainDirector* dir
	)
{
	itsMainDirector = dir;
}

#endif
