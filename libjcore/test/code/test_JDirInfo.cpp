/******************************************************************************
 test_JDirInfo.cc

	Program to test JDirInfo class.

	Written by John Lindal.

 ******************************************************************************/

#include <JDirInfo.h>
#include <jCommandLine.h>
#include <jAssert.h>

int main
	(
	int argc,
	char** argv
	)
{
	if (argc < 2)
		{
		std::cerr << "usage:  " << argv[0] << " path" << std::endl;
		return 1;
		}

	JDirInfo* info;
	if (!JDirInfo::Create(argv[1], &info))
		{
		std::cerr << "Create failed" << std::endl;
		return 1;
		}

	const JSize count = info->GetEntryCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry& entry = info->GetEntry(i);
		std::cout << entry.GetName() << std::endl;
		}

	return 0;
}
