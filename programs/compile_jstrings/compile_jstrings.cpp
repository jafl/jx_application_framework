/******************************************************************************
 compile_jstrings.cpp

	Program to create a source file from a set of JStringManager data files.

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JStringManager.h>
#include <JPtrArray-JString.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jCommandLine.h>
#include <sstream>
#include <stdlib.h>
#include <jAssert.h>

// Constants

static const JCharacter* kVersionStr =

	"compile_jstrings 1.1.0\n"
	"\n"
	"Copyright © 2000-05 John Lindal.  All rights reserved.\n"
	"\n"
	"http://www.newplanetsoftware.com/jx/";

// Prototypes

void GetOptions(const JSize argc, char* argv[],
				JPtrArray<JString>* inputFileList,
				JString* dataVarName, JString* outputFileName,
				JString* databaseFileName, JBoolean* debug);

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
	// parse the command line options

	JPtrArray<JString> inputFileList(JPtrArrayT::kDeleteAll);
	JString dataVarName, outputFileName, databaseFileName;
	JBoolean debug;
	GetOptions(argc, argv, &inputFileList,
			   &dataVarName, &outputFileName, &databaseFileName, &debug);

	const JSize inputCount = inputFileList.GetElementCount();

	// check mod times of input files
/*
	This doesn't work because compiling different versions of the program
	requires different sets of string files, none of which may have been
	modified in a long time.  The output file still needs to be re-built,
	however!

	time_t outputTime;
	if ((JGetModificationTime(outputFileName, &outputTime)).OK())
		{
		JBoolean changed = kJFalse;

		for (JIndex i=1; i<=inputCount; i++)
			{
			const JString* inputFileName = inputFileList.NthElement(i);
			time_t t;
			if (!(JGetModificationTime(*inputFileName, &t)).OK())
				{
				cerr << argv[0] << ":  " << *inputFileName << " does not exist" << endl;
				return 1;
				}
			else if (t >= outputTime)
				{
				changed = kJTrue;
				break;
				}
			}

		if (!changed)
			{
			return 0;
			}
		}
*/
	// process the input files

	JStringManager mgr;

	for (JIndex i=1; i<=inputCount; i++)
		{
		const JString* inputFileName = inputFileList.NthElement(i);
		if (JDirectoryExists(*inputFileName))
			{
			continue;
			}

		ifstream input(*inputFileName);
		if (!input.good())
			{
			cerr << argv[0] << ":  unable to open " << *inputFileName << endl;
			return 1;
			}
		mgr.MergeFile(input, debug);
		if (input.fail())
			{
			cerr << argv[0] << ":  error while reading " << *inputFileName << endl;
			return 1;
			}
		}

	// generate the output file

	std::ostringstream data1;
	mgr.WriteFile(data1);

	JString data1Str = data1.str();
	if (!databaseFileName.IsEmpty())
		{
		ofstream dbOutput(databaseFileName);
		data1Str.Print(dbOutput);
		}

	if (!outputFileName.IsEmpty())
		{
		JIndex i = 1;
		while (data1Str.LocateNextSubstring("\\", &i))
			{
			data1Str.ReplaceSubstring(i,i, "\\\\");
			i += 2;
			}
		i = 1;
		while (data1Str.LocateNextSubstring("\"", &i))
			{
			data1Str.ReplaceSubstring(i,i, "\\\"");
			i += 2;
			}
		i = 1;
		while (data1Str.LocateNextSubstring("\n", &i))
			{
			data1Str.ReplaceSubstring(i,i, "\\n");
			i += 2;
			}

		std::ostringstream data2;
		data2 << "#include <jTypes.h>" << endl;
		data2 << "static const JCharacter* ";
		dataVarName.Print(data2);
		data2 << "[] = {" << endl;

		// Visual C++ cannot handle file with more than 2048 characters on a line
		// and cannot compile string constant more than 2048 characters!

		const JSize l1 = data1Str.GetLength();
		for (i=0; i<l1; )
			{
			JSize l2 = JMin((JSize) 2040, l1 - i);
			while (l2 > 0 && data1Str.GetCharacter(i+l2) == '\\')
				{
				l2--;
				}
			assert( l2 > 0 );

			data2 << "\"";
			data2.write(((const char*) data1Str) + i, l2);
			data2 << "\"," << endl;

			i += l2;
			}

		data2 << "NULL };" << endl;

		// if the file won't change, don't re-write it

		const JString s2 = data2.str();
		if (JFileExists(outputFileName))
			{
			JString origData;
			JReadFile(outputFileName, &origData);
			if (origData == s2)
				{
				JUpdateCVSIgnore(outputFileName);
				return 0;
				}
			}

		// write file

		ofstream output(outputFileName);
		s2.Print(output);

		if (!output.good())
			{
			cerr << argv[0] << ":  unable to write to " << outputFileName << endl;
			return 1;
			}

		JUpdateCVSIgnore(outputFileName);
		}

	return 0;
}

/******************************************************************************
 GetOptions

	Modify the defaults based on the command line options.

 ******************************************************************************/

void
GetOptions
	(
	const JSize			argc,
	char*				argv[],
	JPtrArray<JString>*	inputFileList,
	JString*			dataVarName,
	JString*			outputFileName,
	JString*			databaseFileName,
	JBoolean*			debug
	)
{
	inputFileList->CleanOut();
	dataVarName->Clear();
	outputFileName->Clear();
	databaseFileName->Clear();
	*debug = kJFalse;

	JIndex index = 1;
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

		else if (strcmp(argv[index], "--code") == 0)
			{
			JCheckForValues(2, &index, argc, argv);
			*dataVarName    = argv[index];
			*outputFileName = argv[index+1];
			index++;
			}

		else if (strcmp(argv[index], "--db") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			*databaseFileName = argv[index];
			}

		else if (strcmp(argv[index], "--debug") == 0)
			{
			*debug = kJTrue;
			}

		else if (argv[index][0] == '-')
			{
			cerr << argv[0] << ": unknown command line option: " << argv[index] << endl;
			}

		else
			{
			JString* inputFileName = new JString(argv[index]);
			assert( inputFileName != NULL );

			if (inputFileName->EndsWith("~") ||
				inputFileName->EndsWith("#"))
				{
				delete inputFileName;
				}
			else
				{
				inputFileList->Append(inputFileName);
				}
			}

		index++;
		}

	if (inputFileList->IsEmpty())
		{
		cerr << argv[0] << ": no input files" << endl;
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
	cout << kVersionStr << endl;
	cout << endl;
	cout << "This program builds a source file defining a char* that can be used" << endl;
	cout << "as the default data for JStringManager." << endl;
	cout << endl;
	cout << "Usage:  [options] input_file [input_file ...] " << endl;
	cout << endl;
	cout << "-h      prints help" << endl;
	cout << "-v      prints version information" << endl;
	cout << "--code  var_name output_file: generates source file defining var_name" << endl;
	cout << "--db    output_file: generates string database file from input files" << endl;
	cout << endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	cout << endl;
	cout << kVersionStr << endl;
	cout << endl;
}
