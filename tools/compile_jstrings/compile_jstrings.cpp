/******************************************************************************
 compile_jstrings.cpp

	Program to create a source file from a set of JStringManager data files.

	Copyright (C) 2000-2017 by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JStringManager.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jCommandLine.h>
#include <jx-af/jcore/JMinMax.h>
#include <sstream>
#include <stdlib.h>
#include <jx-af/jcore/jAssert.h>

// Constants

static const JUtf8Byte* kVersionStr =

	"compile_jstrings 2.0.0\n"
	"\n"
	"Copyright (C) 2000-17 John Lindal.";

// Prototypes

void GetOptions(const JSize argc, char* argv[],
				JPtrArray<JString>* inputFileList,
				JString* dataVarName, JString* outputFileName,
				JString* databaseFileName, bool* debug);

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
	bool debug;
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
		bool changed = false;

		for (JIndex i=1; i<=inputCount; i++)
		{
			const JString* inputFileName = inputFileList.GetElement(i);
			time_t t;
			if (!(JGetModificationTime(*inputFileName, &t)).OK())
			{
				std::cerr << argv[0] << ":  " << *inputFileName << " does not exist" << std::endl;
				return 1;
			}
			else if (t >= outputTime)
			{
				changed = true;
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
		const JString* inputFileName = inputFileList.GetElement(i);
		if (JDirectoryExists(*inputFileName))
		{
			continue;
		}

		std::ifstream input(inputFileName->GetBytes());
		if (!input.good())
		{
			std::cerr << argv[0] << ":  unable to open " << *inputFileName << std::endl;
			return 1;
		}
		mgr.MergeFile(input, debug);
		if (input.fail())
		{
			std::cerr << argv[0] << ":  error while reading " << *inputFileName << std::endl;
			return 1;
		}
	}

	// generate the output file

	std::ostringstream data1;
	mgr.WriteFile(data1);

	JString data1Str(data1.str(), JUtf8ByteRange());
	if (!databaseFileName.IsEmpty())
	{
		std::ofstream dbOutput(databaseFileName.GetBytes());
		data1Str.Print(dbOutput);
	}

	if (!outputFileName.IsEmpty())
	{
		JStringIterator iter(&data1Str, kJIteratorStartAtEnd);
		JUtf8Character c;
		while (iter.Prev(&c))
		{
			if (c == "\\" || c == "\"")
			{
				iter.Insert("\\");
			}
			else if (c == "\n")
			{
				iter.SetNext(JUtf8Character('n'), kJIteratorStay);
				iter.Insert("\\");
			}
		}

		std::ostringstream data2;
		data2 << "#include <jx-af/jcore/jTypes.h>" << std::endl;
		data2 << "static const JUtf8Byte* ";
		dataVarName.Print(data2);
		data2 << "[] = {" << std::endl;

		// Visual C++ cannot handle file with more than 2048 characters on a line
		// and cannot compile string constant more than 2048 characters!
		// (2040 / 4bytes = 510 characters)

		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		iter.BeginMatch();
		while (!iter.AtEnd())
		{
			iter.SkipNext(510);
			while (iter.Prev(&c) && c == "\\")
			{
				// back up
			}
			iter.SkipNext();	// compensate

			data2 << "\"";
			iter.FinishMatch().GetString().Print(data2);
			data2 << "\"," << std::endl;

			iter.BeginMatch();
		}
		iter.Invalidate();

		data2 << "nullptr };" << std::endl;

		// if the file won't change, don't re-write it

		const JString s2(data2.str(), JUtf8ByteRange());
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

		std::ofstream output(outputFileName.GetBytes());
		s2.Print(output);

		if (!output.good())
		{
			std::cerr << argv[0] << ":  unable to write to " << outputFileName << std::endl;
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
	bool*				debug
	)
{
	inputFileList->CleanOut();
	dataVarName->Clear();
	outputFileName->Clear();
	databaseFileName->Clear();
	*debug = false;

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
			*debug = true;
		}

		else if (argv[index][0] == '-')
		{
			std::cerr << argv[0] << ": unknown command line option: " << argv[index] << std::endl;
		}

		else
		{
			auto* inputFileName = jnew JString(argv[index]);
			assert( inputFileName != nullptr );

			if (inputFileName->EndsWith("~") ||
				inputFileName->BeginsWith("#"))
			{
				jdelete inputFileName;
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
		std::cerr << argv[0] << ": no input files" << std::endl;
		exit(1);
	}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp()
{
	std::cout << std::endl;
	std::cout << kVersionStr << std::endl;
	std::cout << std::endl;
	std::cout << "This program builds a source file defining a char* that can be used" << std::endl;
	std::cout << "as the default data for JStringManager." << std::endl;
	std::cout << std::endl;
	std::cout << "Usage:  [options] input_file [input_file ...] " << std::endl;
	std::cout << std::endl;
	std::cout << "-h      prints help" << std::endl;
	std::cout << "-v      prints version information" << std::endl;
	std::cout << "--code  var_name output_file: generates source file defining var_name" << std::endl;
	std::cout << "--db    output_file: generates string database file from input files" << std::endl;
	std::cout << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << kVersionStr << std::endl;
	std::cout << std::endl;
}
