#include <JTextChooseSaveFile.h>
#include <JUserNotification.h>
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

	JChooseSaveFile* csf = JGetChooseSaveFile();

	JString resultStr;

	if (csf->ChooseFile("Name of file:", NULL, &resultStr))
		{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
		}

	if (csf->ChooseFile("Name of file:", "Please select a file...", &resultStr))
		{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
		}

	if (csf->ChooseRPath("", "Please select a directory...", NULL, &resultStr))
		{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
		}

	if (csf->ChooseRWPath("", "Please select a writable directory:", NULL, &resultStr))
		{
		std::cout << resultStr << std::endl;
		JWaitForReturn();
		}

	if (csf->SaveFile("Save file as:", NULL, "junk", &resultStr))
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
