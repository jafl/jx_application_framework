#include <jx-af/jcore/JTextUserNotification.h>
#include <jx-af/jcore/jCommandLine.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

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
