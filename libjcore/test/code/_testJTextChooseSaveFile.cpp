#include <jx-af/jcore/JTextChooseSaveFile.h>
#include <jx-af/jcore/JUserNotification.h>
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

	JChooseSaveFile* csf = JGetChooseSaveFile();

	JString resultStr;

	if (csf->ChooseFile("Name of file:", nullptr, &resultStr))
	{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
	}

	if (csf->ChooseFile("Name of file:", "Please select a file...", &resultStr))
	{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
	}

	if (csf->ChooseRPath("", "Please select a directory...", nullptr, &resultStr))
	{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
	}

	if (csf->ChooseRWPath("", "Please select a writable directory:", nullptr, &resultStr))
	{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
	}

	if (csf->SaveFile("Save file as:", nullptr, "junk", &resultStr))
	{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
	}

	if (csf->SaveFile("Save file as:", "Please save the file...", "more junk", &resultStr))
	{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
	}

	return 0;
}
