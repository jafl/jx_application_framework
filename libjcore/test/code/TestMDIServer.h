/******************************************************************************
 TestMDIServer.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestMDIServer
#define _H_TestMDIServer

#include <JMDIServer.h>

class TestMDIServer : public JMDIServer
{
public:

	TestMDIServer(const JUtf8Byte* signature);

	virtual ~TestMDIServer();

	JBoolean	ShouldQuit() const;

protected:

	virtual JBoolean	CanAcceptMDIRequest() override;
	virtual void		HandleMDIRequest(const JString& dir,
										 const JPtrArray<JString>& argList) override;

private:

	JBoolean itsQuitFlag;

private:

	// not allowed

	TestMDIServer(const TestMDIServer& source);
	const TestMDIServer& operator=(const TestMDIServer& source);
};


/******************************************************************************
 ShouldQuit

 ******************************************************************************/

inline JBoolean
TestMDIServer::ShouldQuit()
	const
{
	return itsQuitFlag;
}

#endif
