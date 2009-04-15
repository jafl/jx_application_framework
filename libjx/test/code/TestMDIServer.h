/******************************************************************************
 TestMDIServer.h

	Interface for the TestMDIServer class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestMDIServer
#define _H_TestMDIServer

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMDIServer.h>

class TestMDIServer : public JXMDIServer
{
public:

	TestMDIServer();

	virtual ~TestMDIServer();

	static void	PrintCommandLineHelp();

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

private:

	// not allowed

	TestMDIServer(const TestMDIServer& source);
	const TestMDIServer& operator=(const TestMDIServer& source);
};

#endif
