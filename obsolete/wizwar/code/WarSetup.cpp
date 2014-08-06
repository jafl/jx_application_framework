/******************************************************************************
 WarSetup.cpp

	Class to store setup for WizWar server.

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
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
				cerr << argv[0] << ": invalid setup file" << endl;
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
				cerr << argv[0] << ": invalid port index" << endl;
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
				cerr << argv[0] << ": number of players must be between ";
				cerr << kWWMinPlayerCount << " and " << kWWMaxPlayerCount << endl;
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
			cerr << argv[0] << ": unknown command line option: " << argv[index] << endl;
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
	ifstream input(fileName);

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
	ofstream output(fileName);

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
	cout << endl;
	cout << versionStr << endl;
	cout << endl;
	cout << "Protocol version:  " << kWWCurrentProtocolVersion << endl;
	cout << endl;
	cout << "Usage:" << endl;
	cout << endl;
	cout << "-h, --help" << endl;
	cout << "  Prints help.  Can be used after other options to check values." << endl;
	cout << endl;
	cout << "-v, --version" << endl;
	cout << "  Prints version information." << endl;
	cout << endl;
	cout << "--file <setup file>" << endl;
	cout << "  Reads setup from specified file." << endl;
	cout << endl;
	cout << "--port <port number> (" << itsServerPort << ')' << endl;
	cout << "  Sets port to which clients can connect." << endl;
	cout << endl;
	cout << "--players <number of players> (" << itsMaxPlayerCount << ')' << endl;
	cout << "  Sets maximum number of players who can connect. (";
	cout << kWWMinPlayerCount << '-' << kWWMaxPlayerCount << ')' << endl;
	cout << endl;
	cout << "--(no-)anon (" << (itsIsAnonymousFlag ? "anonymous" : "not anonymous") << ')' << endl;
	cout << "  Choose whether or not players will be anonymous." << endl;
	cout << endl;
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
	cout << endl;
	cout << versionStr << endl;
	cout << endl;
	cout << "Protocol version:  " << kWWCurrentProtocolVersion << endl;
	cout << endl;
}
