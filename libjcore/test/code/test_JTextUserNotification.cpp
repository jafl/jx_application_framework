#include <JTextUserNotification.h>
#include <jCommandLine.h>
#include <jGlobals.h>
#include <ace/OS.h>
#include <jAssert.h>

int
main
	(
	int argc,
	char** argv
	)
{
	JInitCore();

	JUserNotification* un = JGetUserNotification();

	un->DisplayMessage("Hello there!");
	un->ReportError("Bus Error!");
	cout << un->AskUserNo("Quit?") << endl;
	cout << un->AskUserYes("Quit?") << endl;

	return 0;
}
