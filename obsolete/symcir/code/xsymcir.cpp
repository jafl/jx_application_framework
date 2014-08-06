/******************************************************************************
 xsymcir.cpp

	X Windows interface to the SCCircuit class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SymcirApp.h"
#include "SCCircuitDocument.h"
#include <jXGlobals.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jCommandLine.h>
#include <stdlib.h>
#include <jAssert.h>

// Constants

const JCharacter* kSymcirVersionStr =

	"Symcir 1.0.1\n"
	"\n"
	"A program for symbolic analysis of linear circuits\n"
	"\n"
	"Copyright © 1996-99 California Institute of Technology.\n"
	"Written by John Lindal.\n"
	"\n"
	"DO NOT DISTRIBUTE\n"
	"\n"
	"jafl@micro.caltech.edu";

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);
void ParseXOptions(const int argc, char* argv[], JString* inputFileName);

void PrintHelp();
void PrintVersion();

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	ParseTextOptions(argc, argv);

	SymcirApp* app = new SymcirApp(&argc, argv);
	assert( app != NULL );

	JString inputFileName;
	ParseXOptions(argc, argv, &inputFileName);

	if (!inputFileName.IsEmpty() ||
		(JGetChooseSaveFile())->ChooseFile("Netlist to analyze:", NULL, &inputFileName))
		{
		SCCircuitDocument* mainDir = new SCCircuitDocument(app, inputFileName);
		assert( mainDir != NULL );

		mainDir->Activate();
		app->Run();
		}

	return 0;
}

/******************************************************************************
 ParseTextOptions

	Handle the command line options that don't require opening an X display.

	Options that don't call exit() should use JXApplication::RemoveCmdLineOption()
	so ParseXOptions won't gag.

 ******************************************************************************/

void
ParseTextOptions
	(
	const int	argc,
	char*		argv[]
	)
{
	long index = 1;
	while (index < argc)
		{
		if (JIsVersionRequest(argv[index]))
			{
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			PrintHelp();
			exit(0);
			}
		index++;
		}
}

/******************************************************************************
 ParseXOptions

	Modify the defaults based on the command line options.

	Since JXApplication has already removed all of its options, we can
	safely assume that whatever we don't understand is an error.

 ******************************************************************************/

void
ParseXOptions
	(
	const int	argc,
	char*		argv[],
	JString*	inputFileName
	)
{
	inputFileName->Clear();

	long index = 1;
	while (index < argc)
		{
		if (argv[index][0] == '-')
			{
			cerr << argv[0] << ": unknown option " << argv[index] << endl;
			}
		else if (inputFileName->IsEmpty())
			{
			*inputFileName = argv[index];
			}
		index++;
		}

	if (!inputFileName->IsEmpty() && !JFileExists(*inputFileName))
		{
		cerr << argv[0] << ": file not found" << endl;
		exit(1);
		}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp()
{
	cout << endl;
	cout << "This X application allows one to symbolically analyze linear" << endl;
	cout << "circuits stored in the Spice netlist format." << endl;
	cout << endl;
	cout << "Mathematica is required in order to use this program." << endl;
	cout << endl;
	cout << "Usage:  <options> <symcir file or netlist file (optional)>" << endl;
	cout << endl;
	cout << "-h  prints help" << endl;
	cout << "-v  prints version information" << endl;
	cout << endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	cout << endl;
	cout << kSymcirVersionStr << endl;
	cout << endl;
}
