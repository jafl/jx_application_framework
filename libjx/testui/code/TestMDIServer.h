/******************************************************************************
 TestMDIServer.h

	Interface for the TestMDIServer class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestMDIServer
#define _H_TestMDIServer

#include <jx-af/jx/JXMDIServer.h>

class TestMDIServer : public JXMDIServer
{
public:

	TestMDIServer();

	virtual ~TestMDIServer();

	static void	PrintCommandLineHelp();

protected:

	void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) override;
};

#endif
