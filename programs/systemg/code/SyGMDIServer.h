/******************************************************************************
 SyGMDIServer.h

	Interface for the SyGMDIServer class

	Copyright � 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_SyGMDIServer
#define _H_SyGMDIServer

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMDIServer.h>

class SyGMDIServer : public JXMDIServer
{
public:

	SyGMDIServer();

	virtual	~SyGMDIServer();

	static void	PrintCommandLineHelp();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	void	OpenFiles();

	// not allowed

	SyGMDIServer(const SyGMDIServer& source);
	const SyGMDIServer& operator=(const SyGMDIServer& source);

};

#endif
