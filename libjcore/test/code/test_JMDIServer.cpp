/******************************************************************************
 test_JMDIServer.cpp

	Program to test JMDIServer.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include "TestMDIServer.h"
#include <JThisProcess.h>
#include <jAssert.h>

static const JUtf8Byte* kSignature = "test-jmdiserver";

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	pid_t pid;
	const JError err = JThisProcess::Fork(&pid);

	if (pid == 0)	// child
	{
		TestMDIServer svr(kSignature);
		while (!svr.ShouldQuit())
		{
			svr.CheckForConnections();
		}
		exit(0);
	}

	sleep(2);

	const char* argv[] = { "test_JMDIServer", "--quit" };
	JAssertFalse(TestMDIServer::WillBeMDIServer(kSignature, 2, const_cast<char**>(argv)));

	ACE_exitcode status;
	JAssertOK(JWaitForChild(pid, &status));

	int result;
	JAssertEqual(kJChildFinished, JDecodeChildExitReason(status, &result));
	JAssertEqual(0, result);
}
