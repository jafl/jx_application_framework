/******************************************************************************
 TestMDIServer.h

	Interface for the TestMDIServer class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestMDIServer
#define _H_TestMDIServer

#include <JXMDIServer.h>

class TestMDIServer : public JXMDIServer
{
public:

	TestMDIServer();

	virtual ~TestMDIServer();

	static void	PrintCommandLineHelp();

protected:

	virtual void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList);

private:

	// not allowed

	TestMDIServer(const TestMDIServer& source);
	const TestMDIServer& operator=(const TestMDIServer& source);
};

#endif
