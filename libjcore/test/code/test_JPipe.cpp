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

	cout << "Hello " << JGetUserName() << endl << endl;

	// test directory functions

	JString dir;
	if (JGetHomeDirectory(&dir))
		{
		cout << "home dir   : " << dir << endl;
		}

	dir = JGetCurrentDirectory();
	cout << "current dir: " << dir << endl;

	// test input pipe
#ifdef _MSC_VER
	JString cmd = "c:\\windows\\system32\\cmd.exe /c dir " + JPrepArgForExec(dir);
#else
	JString cmd = "ls -l " + JPrepArgForExec(dir);
#endif

	cout << endl;
	cout << "Here is what is in your current directory:" << endl;
	cout << endl;

	int fromFD;
	JError err = JExecute(cmd, NULL,
						  kJIgnoreConnection, NULL,
						  kJCreatePipe, &fromFD);
	if (err.OK())
		{
		JString data;
		JReadAll(fromFD, &data, kJTrue);
		cout << data;
		}
	else
		{
		cerr << err.GetMessage() << endl;
		}
	cout << endl;

	// test output pipe

	cout << "Grepping for 'junk' in stuff that I print:" << endl;
	cout << endl;

	pid_t childPID;
	int toFD;
	err = JExecute("grep junk", &childPID, kJCreatePipe, &toFD);
	if (err.OK())
		{
		{
		JOutPipeStream output(toFD, kJTrue);
		output << "This is line 1" << endl;
		output << "This is line 2" << endl;
		output << "This line contains 'junk'" << endl;
		}

		err = JWaitForChild(childPID);
		if (!err.OK())
			{
			cerr << err.GetMessage() << endl;
			}
		}
	else
		{
		cerr << err.GetMessage() << endl;
		}

	// test JPrepArgForExec with screwy file name

	ofstream output(kFileName);
	output << "This is line 1" << endl;
	output << "This is line 2" << endl;
	output.close();

	cout << endl;
	cout << "Contents of " << kFileName << ':' << endl;
	cout << endl;

#ifdef _MSC_VER
	cmd = "c:\\windows\\system32\\cmd.exe /c type " + JPrepArgForExec(kFileName);
#else
	cmd = "cat " + JPrepArgForExec(kFileName);
#endif
	err = JExecute(cmd, NULL);
	if (!err.OK())
		{
		cerr << err.GetMessage() << endl;
		}

	err = JRemoveFile(kFileName);
	if (!err.OK())
		{
		cerr << err.GetMessage() << endl;
		}

	// test reading from our own child process

	int fd[2];
	err = JCreatePipe(fd);
	if (!err.OK())
		{
		cerr << err.GetMessage() << endl;
		}

	pid_t pid;
	err = JThisProcess::Fork(&pid);
	if (!err.OK())
		{
		cerr << err.GetMessage() << endl;
		}

	// child

	else if (pid == 0)
		{
		close(fd[0]);

		JOutPipeStream output(fd[1], kJTrue);
		output << "This is a message from the child process:  Hi!" << endl;
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

		cout << endl;
		cout << "Text received from child:" << endl;
		cout << endl;
		cout << data;
		}

	return 0;
}
