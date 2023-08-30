#include "JTextProgressDisplay.h"
#include "JStopWatch.h"
#include "jTime.h"
#include "JString.h"
#include "jCommandLine.h"
#include "jAssert.h"

int
main
	(
	int argc,
	char** argv
	)
{
	JTextProgressDisplay pg;

	JStopWatch timer;

	pg.FixedLengthProcessBeginning(6, "Calculating scores...", true, false);

	timer.StartTimer();

	long i;
	for (i=1;i<=6;i++)
	{
		JWait(2.0);

		if (!pg.IncrementProgress())
		{
			break;
		}
	}

	timer.StopTimer();

	pg.ProcessFinished();

	JString timeString = timer.PrintTimeInterval();
	std::cout << timeString;

	pg.VariableLengthProcessBeginning("Processing examples...", true, false);

	for (i=1;i<=6;i++)
	{
		JWait(2.0);

		if (!pg.IncrementProgress(JString(i)))
		{
			break;
		}
	}

	pg.ProcessFinished();

	return 0;
}
