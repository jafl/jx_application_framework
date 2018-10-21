#include <JTextUserNotification.h>
#include <jCommandLine.h>
#include <jGlobals.h>
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
	std::cout << un->AskUserNo("Quit?") << std::endl;
	std::cout << un->AskUserYes("Quit?") << std::endl;

	return 0;
}
