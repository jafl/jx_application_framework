#include <JOutPipeStream.h>
#include <JThisProcess.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jDirUtil.h>
#include <jSysUtil.h>
#include <jGlobals.h>
#include <signal.h>

const JCharacter* kFileName = "\"more junk\"";

int
main
	(
	int argc,
	char** argv
	)
{
	JInitCore();

	std::cout << "Hello " << JGetUserName() << std::endl << std::endl;

	// test directory functions

	JString dir;
	if (JGetHomeDirectory(&dir))
		{
		std::cout << "home dir   : " << dir << std::endl;
		}

	dir = JGetCurrentDirectory();
	std::cout << "current dir: " << dir << std::endl;

	// test input pipe

	JString cmd = "ls -l " + JPrepArgForExec(dir);

	std::cout << std::endl;
	std::cout << "Here is what is in your current directory:" << std::endl;
	std::cout << std::endl;

	int fromFD;
	JError err = JExecute(cmd, NULL,
						  kJIgnoreConnection, NULL,
						  kJCreatePipe, &fromFD);
	if (err.OK())
		{
		JString data;
		JReadAll(fromFD, &data, kJTrue);
		std::cout << data;
		}
	else
		{
		std::cerr << err.GetMessage() << std::endl;
		}
	std::cout << std::endl;

	// test output pipe

	std::cout << "Grepping for 'junk' in stuff that I print:" << std::endl;
	std::cout << std::endl;

	pid_t childPID;
	int toFD;
	err = JExecute("grep junk", &childPID, kJCreatePipe, &toFD);
	if (err.OK())
		{
		{
		JOutPipeStream output(toFD, kJTrue);
		output << "This is line 1" << std::endl;
		output << "This is line 2" << std::endl;
		output << "This line contains 'junk'" << std::endl;
		}

		err = JWaitForChild(childPID);
		if (!err.OK())
			{
			std::cerr << err.GetMessage() << std::endl;
			}
		}
	else
		{
		std::cerr << err.GetMessage() << std::endl;
		}

	// test JPrepArgForExec with screwy file name

	std::ofstream output(kFileName);
	output << "This is line 1" << std::endl;
	output << "This is line 2" << std::endl;
	output.close();

	std::cout << std::endl;
	std::cout << "Contents of " << kFileName << ':' << std::endl;
	std::cout << std::endl;

	cmd = "cat " + JPrepArgForExec(kFileName);

	err = JExecute(cmd, NULL);
	if (!err.OK())
		{
		std::cerr << err.GetMessage() << std::endl;
		}

	err = JRemoveFile(kFileName);
	if (!err.OK())
		{
		std::cerr << err.GetMessage() << std::endl;
		}

	// test reading from our own child process

	int fd[2];
	err = JCreatePipe(fd);
	if (!err.OK())
		{
		std::cerr << err.GetMessage() << std::endl;
		}

	pid_t pid;
	err = JThisProcess::Fork(&pid);
	if (!err.OK())
		{
		std::cerr << err.GetMessage() << std::endl;
		}

	// child

	else if (pid == 0)
		{
		close(fd[0]);

		JOutPipeStream output(fd[1], kJTrue);
		output << "This is a message from the child process:  Hi!" << std::endl;
		output.flush();
		exit(0);
		}

	// parent

	else
		{
		close(fd[1]);

		JWait(2);	// required to receive data on OS X

		JString data;
		JReadAll(fd[0], &data, kJTrue);

		std::cout << std::endl;
		std::cout << "Text received from child:" << std::endl;
		std::cout << std::endl;
		std::cout << data;
		}

	return 0;
}
