/******************************************************************************
 TestMDIServer.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestMDIServer
#define _H_TestMDIServer

#include <jx-af/jcore/JMDIServer.h>

class TestMDIServer : public JMDIServer
{
public:

	TestMDIServer(const JUtf8Byte* signature);

	virtual ~TestMDIServer();

	bool	ShouldQuit() const;

protected:

	bool	CanAcceptMDIRequest() override;
	void	HandleMDIRequest(const JString& dir,
							 const JPtrArray<JString>& argList) override;

private:

	bool itsQuitFlag;
};


/******************************************************************************
 ShouldQuit

 ******************************************************************************/

inline bool
TestMDIServer::ShouldQuit()
	const
{
	return itsQuitFlag;
}

#endif
