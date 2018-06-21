/******************************************************************************
 test_JPipe.cpp

	Program to test UNIX pipes.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JOutPipeStream.h>
#include <JThisProcess.h>
#include <JStringIterator.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jDirUtil.h>
#include <jSysUtil.h>
#include <jGlobals.h>
#include <signal.h>
#include <jAssert.h>

int main()
{
	JInitCore();

	std::cout << "Hello " << JGetUserName() << std::endl;

	// test directory functions

	JString dir;
	if (JGetHomeDirectory(&dir))
		{
		std::cout << "Your home directory is: " << dir << std::endl;
		}

	return JTestManager::Execute();
}

JTEST(Input)
{
	const JString dir = JGetCurrentDirectory();

	JString cmd = "ls -l " + JPrepArgForExec(dir);

	int fromFD;
	const JError err = JExecute(cmd, nullptr,
								kJIgnoreConnection, nullptr,
								kJCreatePipe, &fromFD);
	JAssertOK(err);

	JString data;
	JReadAll(fromFD, &data, kJTrue);

	JStringIterator iter(data);
	JAssertTrue(iter.Next("test_JPipe"));
}

JTEST(Output)
{
	pid_t childPID;
	int toFD;
	int fromFD;
	JError err = JExecute(JString("grep junk", kJFalse), &childPID,
						  kJCreatePipe, &toFD,
						  kJCreatePipe, &fromFD);
	JAssertOK(err);

	JOutPipeStream output(toFD, kJTrue);
	output << "This is line 1" << std::endl;
	output << "This is line 2" << std::endl;
	output << "This line contains 'junk'" << std::endl;
	output.close();

	err = JWaitForChild(childPID);
	JAssertOK(err);

	JString data;
	JReadAll(fromFD, &data, kJTrue);

	JStringIterator iter(data);
	JAssertTrue(iter.Next("This line contains 'junk'"));
}

JTEST(ChildInput)
{
	const JUtf8Byte* kMessage = "This is a message from the child process:  Hi!";

	int fd[2];
	JError err = JCreatePipe(fd);
	JAssertOK(err);

	pid_t pid;
	err = JThisProcess::Fork(&pid);
	JAssertOK(err);

	// child

	if (pid == 0)
		{
		close(fd[0]);

		JOutPipeStream output(fd[1], kJTrue);
		output << kMessage;
		output.flush();
		exit(0);
		}

	// parent

	else
		{
		close(fd[1]);

		JWait(1);	// required to receive data on OS X

		JString data;
		JReadAll(fd[0], &data, kJTrue);

		JAssertStringsEqual(kMessage, data);
		}
}
