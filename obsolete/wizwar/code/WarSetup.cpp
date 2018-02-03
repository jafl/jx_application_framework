/******************************************************************************
 WarSetup.cpp

	Class to store setup for WizWar server.

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "WarSetup.h"
#include "warFileVersions.h"
#include "wwUtil.h"
#include "wwMessages.h"
#include "wwConstants.h"
#include <JString.h>
#include <jStreamUtil.h>
#include <jCommandLine.h>
#include <jErrno.h>
#include <stdlib.h>
#include <jAssert.h>

static const JCharacter* kDefaultWarSetupFileName = "server_setup";

static const JCharacter* kWarSetupFileSignature = "jx_wizwar_server_setup_data";
const JSize kWarSetupFileSignatureLength        = strlen(kWarSetupFileSignature);

/******************************************************************************
 Constructor

 ******************************************************************************/

WarSetup::WarSetup
	(
	const JSize			argc,
	char*				argv[],
	const JCharacter*	versionStr
	)
{
	// initialize to default values

	itsServerPort      = 2000;
	itsMaxPlayerCount  = 4;
	itsIsAnonymousFlag = kJFalse;

	// read default setup file

	JString fullName;
	if (WWGetDataFileName(kDefaultWarSetupFileName, &fullName))
		{
		ReadSetup(fullName);
		}

	// parse command line options

	Configure(argc, argv, versionStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WarSetup::~WarSetup()
{
}

/******************************************************************************
 Configure (private)

	Modify the defaults based on the command line options.

 ******************************************************************************/

void
WarSetup::Configure
	(
	const JSize			argc,
	char*				argv[],
	const JCharacter*	versionStr
	)
{
	JIndex index = 1;
	while (index < argc)
		{
		if (strcmp(argv[index], "-h") == 0 ||
			strcmp(argv[index], "--help") == 0)
			{
			PrintHelp(versionStr);
			exit(0);
			}
		else if (strcmp(argv[index], "-v") == 0 ||
				 strcmp(argv[index], "--version") == 0)
			{
			PrintVersion(versionStr);
			exit(0);
			}

		else if (strcmp(argv[index], "--file") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			if (ReadSetup(argv[index]) != kFileReadable)
				{
				std::cerr << argv[0] << ": invalid setup file" << std::endl;
				exit(1);
				}
			}

		else if (strcmp(argv[index], "--port") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			jclear_errno();
			itsServerPort = atol(argv[index]);
			if (!jerrno_is_clear())
				{
				std::cerr << argv[0] << ": invalid port index" << std::endl;
				exit(1);
				}
			}

		else if (strcmp(argv[index], "--players") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			jclear_errno();
			itsMaxPlayerCount = atol(argv[index]);
			if (!jerrno_is_clear() ||
				itsMaxPlayerCount < kWWMinPlayerCount ||
				itsMaxPlayerCount > kWWMaxPlayerCount)
				{
				std::cerr << argv[0] << ": number of players must be between ";
				std::cerr << kWWMinPlayerCount << " and " << kWWMaxPlayerCount << std::endl;
				exit(1);
				}
			}

		else if (strcmp(argv[index], "--anon") == 0)
			{
			itsIsAnonymousFlag = kJTrue;
			}
		else if (strcmp(argv[index], "--no-anon") == 0)
			{
			itsIsAnonymousFlag = kJFalse;
			}

		else
			{
			std::cerr << argv[0] << ": unknown command line option: " << argv[index] << std::endl;
			}

		index++;
		}
}

/******************************************************************************
 ReadSetup (private)

 ******************************************************************************/

WarSetup::FileStatus
WarSetup::ReadSetup
	(
	const JCharacter* fileName
	)
{
	std::ifstream input(fileName);

	const JString filePrefix = JRead(input, kWarSetupFileSignatureLength);
	if (filePrefix != kWarSetupFileSignature)
		{
		return kNotMyFile;
		}

	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentWarSetupFileVersion)
		{
		return kNeedNewerVersion;
		}

	return kFileReadable;
}

/******************************************************************************
 WriteSetup (private)

 ******************************************************************************/

void
WarSetup::WriteSetup
	(
	const JCharacter* fileName
	)
	const
{
	std::ofstream output(fileName);

	output << kWarSetupFileSignatureLength;
	output << ' ' << kCurrentWarSetupFileVersion;
}

/******************************************************************************
 PrintHelp (private)

 ******************************************************************************/

void
WarSetup::PrintHelp
	(
	const JCharacter* versionStr
	)
	const
{
	std::cout << std::endl;
	std::cout << versionStr << std::endl;
	std::cout << std::endl;
	std::cout << "Protocol version:  " << kWWCurrentProtocolVersion << std::endl;
	std::cout << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << std::endl;
	std::cout << "-h, --help" << std::endl;
	std::cout << "  Prints help.  Can be used after other options to check values." << std::endl;
	std::cout << std::endl;
	std::cout << "-v, --version" << std::endl;
	std::cout << "  Prints version information." << std::endl;
	std::cout << std::endl;
	std::cout << "--file <setup file>" << std::endl;
	std::cout << "  Reads setup from specified file." << std::endl;
	std::cout << std::endl;
	std::cout << "--port <port number> (" << itsServerPort << ')' << std::endl;
	std::cout << "  Sets port to which clients can connect." << std::endl;
	std::cout << std::endl;
	std::cout << "--players <number of players> (" << itsMaxPlayerCount << ')' << std::endl;
	std::cout << "  Sets maximum number of players who can connect. (";
	std::cout << kWWMinPlayerCount << '-' << kWWMaxPlayerCount << ')' << std::endl;
	std::cout << std::endl;
	std::cout << "--(no-)anon (" << (itsIsAnonymousFlag ? "anonymous" : "not anonymous") << ')' << std::endl;
	std::cout << "  Choose whether or not players will be anonymous." << std::endl;
	std::cout << std::endl;
}

/******************************************************************************
 PrintVersion (private)

 ******************************************************************************/

void
WarSetup::PrintVersion
	(
	const JCharacter* versionStr
	)
	const
{
	std::cout << std::endl;
	std::cout << versionStr << std::endl;
	std::cout << std::endl;
	std::cout << "Protocol version:  " << kWWCurrentProtocolVersion << std::endl;
	std::cout << std::endl;
}
