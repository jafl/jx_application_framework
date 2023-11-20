/******************************************************************************
 makemake.cpp

	Program to create a Makefile given the source file names and a
	Makefile header.

	Copyright (C) 1994-2016 by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/jCommandLine.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jx-af/jcore/JMemoryManager.h>
#include <jx-af/jcore/jAssert.h>

// Turn this on to generate a Makefile that allows #include loops.
// Unfortunately, GNU Make doesn't understand the dependencies generated
// when this is turned off.
#define ALLOW_INCLUDE_LOOPS			1	// boolean

// On some systems, one cannot pass an arbitrarily long list of arguments
// to a process, so we store the information in a temp file instead.
#define USE_TEMP_FILE_FOR_DEPEND	1	// boolean

// Constants

static const JUtf8Byte* kVersionStr =

	"makemake 4.1.0\n"
	"\n"
	"Copyright (C) 1994-2021 by John Lindal.";

static const JUtf8Byte* kMakeBinary         = J_MAKE_BINARY_NAME;
static const JUtf8Byte* kDependTargetName   = "jdepend";
static const JUtf8Byte* kMakemakeBinaryVar  = "MAKEMAKE";
static const JUtf8Byte* kMakemakeBinary     = "${MAKEMAKE}";
static const JUtf8Byte* kDependInputFileVar = "JDEPEND_INPUT_FILE";
static const JUtf8Byte* kDependInputFile    = "${JDEPEND_INPUT_FILE}";

static const JUtf8Byte* kMakeFooterMarkerStr =
	"### makemake inserts here ###";

static const JUtf8Byte* kMakedependMarkerStr =
	"# DO NOT DELETE THIS LINE -- makemake depends on it.";

static const JUtf8Byte* kObjDirArg   = "--obj-dir";
static const JUtf8Byte* kNoStdIncArg = "--no-std-inc";

static const JString kCurrentDir   ("./", JString::kNoCopy);
static const JString kSysIncludeDir("/usr/include/", JString::kNoCopy);

static const JUtf8Byte* kDontInterpretFlag = "literal: ";
const JSize kDontInterpretFlagLen          = strlen(kDontInterpretFlag);

static const JUtf8Byte* kDefOutputSuffix = ".o";

static JRegex* globalIgnorePattern = nullptr;

static const JRegex objFileSuffix     = "^\\.(o|a|so|class|jar) ";
static       JRegex noParseFileSuffix = "^\\.(java|e|m[23])$";		// modified by GetOptions()
static const JRegex javaObjFileSuffix = "^\\.(java|class|jar)$";
static const JRegex suffixMapPattern  = "^(\\.[^.]+)(\\.[^.]+)$";
static const JRegex noParsePattern    = "\\.([^.]+)";

// .in_suffix [.out_suffix ]name

static const JRegex linePattern =
	"^(?P<inSuffix>\\.[^ ]+) +(?:(?P<outSuffix>\\.[^ ]+) +)?(?P<target>.+)$";

// Prototypes

bool ShouldMakeTarget(const JString& target, const JPtrArray<JString>& list);
void PrintForMake(std::ostream& output, const JString& str);
JString GetOutputSuffix(const JString& inputSuffix,
						const JPtrArray<JString>& suffixMapIn,
						const JPtrArray<JString>& suffixMapOut);

void GetOptions(const JSize argc, char* argv[], JString* defSuffix,
				JString* defineText, JString* headerName,
				JString* inputName, JString* outputName, JString* outputDirName,
				JPtrArray<JString>* userTargetList, bool* searchSysDir,
				JPtrArray<JString>* suffixMapIn, JPtrArray<JString>* suffixMapOut);
bool FindFile(const JString& fileName, const JPtrArray<JString>& pathList,
			  JString* fullName);
void PickTargets(const JString& fileName, JPtrArray<JString>* list);

void PrintHelp(const JString& headerName, const JString& inputName,
			   const JString& outputName);
void PrintVersion();

void CalcDepend(const JSize startArg, const JSize argc, char* argv[]);

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
	// check if we are being invoked to build the dependency graph

	if (argc > 2 && strcmp(argv[1], "--depend") == 0)
	{
		CalcDepend(2, argc, argv);
		return 0;
	}

	// parse the command line options

	JString defSuffix, defineText, headerName, inputName, outputName, outputDirName;
	JPtrArray<JString> userTargetList(JPtrArrayT::kDeleteAll);	// empty => include all targets
	bool searchSysDir;

	JPtrArray<JString> suffixMapIn(JPtrArrayT::kDeleteAll),
					   suffixMapOut(JPtrArrayT::kDeleteAll);

	GetOptions(argc, argv, &defSuffix, &defineText, &headerName, &inputName,
			   &outputName, &outputDirName, &userTargetList, &searchSysDir,
			   &suffixMapIn, &suffixMapOut);

	// process the input file

	JPtrArray<JString> mainTargetList(JPtrArrayT::kDeleteAll),
					   mainTargetObjsList(JPtrArrayT::kDeleteAll);
	JArray<bool>	   javaTargetList;
	JPtrArray<JString> targetList(JPtrArrayT::kDeleteAll),
					   prefixList(JPtrArrayT::kDeleteAll),
					   suffixList(JPtrArrayT::kDeleteAll);
	JPtrArray<JString> outPrefixList(JPtrArrayT::kDeleteAll),
					   outSuffixList(JPtrArrayT::kDeleteAll);

	targetList.SetCompareFunction(JCompareStringsCaseSensitive);

	// skip comments and check for possible first suffix instruction

	std::ifstream input(inputName.GetBytes());
	input >> std::ws;
	while (!input.eof() && !input.fail())
	{
		if (input.peek() == '-')
		{
			input.ignore(1);
			JString cmd   = JReadUntil(input, ' ');
			JString value = JReadUntil(input, '\n');
			input >> std::ws;
			value.TrimWhitespace();
			if (cmd == "suffix")
			{
				defSuffix = value;
			}
			else
			{
				std::cerr << argv[0] << ": unknown parameter: " << cmd << '\n';
			}
		}
		else if (input.peek() == '#')
		{
			JIgnoreLine(input);
			input >> std::ws;
		}
		else
		{
			break;
		}
	}

	// If a target has no dependencies, we assume that they are the
	// same as the following target.  This counter keeps track of
	// how many previous targets need to be updated once a set of
	// dependencies is found. 

	JSize prevEmptyTargets = 0;

	// process each target

	while (input.get() == '@' && !input.eof() && !input.fail())
	{
		JString prefix;
		bool usesJava = false;

		// get the name of the target for make

		auto* mainTargetName = jnew JString(JReadUntil(input,'\n'));
		assert( mainTargetName != nullptr );

		// If we aren't supposed to include this target, and
		// there aren't any other targets that need our dependency
		// list, we can take a quick exit.

		input >> std::ws;
		const bool shouldMakeTarget =
			ShouldMakeTarget(*mainTargetName, userTargetList);
		if (!shouldMakeTarget && prevEmptyTargets == 0)
		{
			jdelete mainTargetName;
			JIgnoreUntil(input, '@');
			input.putback('@');
			continue;
		}

		auto* mainTargetObjs = jnew JString;

		// get the names of the files that the main target depends on

		while (!input.eof() && !input.fail() && input.peek() != '@')
		{
			// set option

			if (input.peek() == '-')
			{
				input.ignore(1);
				JString cmd   = JReadUntil(input, ' ');
				JString value = JReadUntil(input, '\n');
				input >> std::ws;
				value.TrimWhitespace();
				if (cmd == "prefix")
				{
					prefix = value;
				}
				else if (cmd == "suffix")
				{
					defSuffix = value;
				}
				else
				{
					std::cerr << argv[0] << ": unknown parameter: " << cmd << '\n';
				}
				continue;
			}

			// comment

			if (input.peek() == '#')
			{
				JIgnoreLine(input);
				input >> std::ws;
				continue;
			}

			JString fullName = JReadUntil(input, '\n');
			input >> std::ws;

			fullName.TrimWhitespace();
			if (fullName.IsEmpty())
			{
				continue;
			}

			// binary object file

			JStringIterator fnIter(&fullName);
			if (fnIter.Next(objFileSuffix))
			{
				const JStringMatch& m = fnIter.GetLastMatch();
				JString suffix(m.GetString());
				suffix.TrimWhitespace();

				fnIter.RemoveLastMatch();
				fullName.TrimWhitespace();	// invalidates fnIter

				*mainTargetObjs += " ";
				*mainTargetObjs += prefix;
				*mainTargetObjs += fullName;
				*mainTargetObjs += suffix;

				usesJava = usesJava || javaObjFileSuffix.Match(suffix);
				continue;
			}

			// raw text

			fnIter.MoveTo(JStringIterator::kStartAtBeginning, 0);
			if (fnIter.Next(kDontInterpretFlag))
			{
				fnIter.RemoveAllPrev();
				fullName.TrimWhitespace();	// invalidates fnIter

				*mainTargetObjs += " ";
				*mainTargetObjs += fullName;

				continue;
			}

			// analyze 

			fnIter.MoveTo(JStringIterator::kStartAtBeginning, 0);

			JString* targetName    = nullptr;
			JString* suffixName    = nullptr;
			JString* outSuffixName = nullptr;
			if (fnIter.Next(linePattern))
			{
				const JStringMatch& m = fnIter.GetLastMatch();

				targetName = jnew JString(m.GetSubstring("target"));
				assert( targetName != nullptr );
				targetName->TrimWhitespace();
				suffixName = jnew JString(m.GetSubstring("inSuffix"));
				assert( suffixName != nullptr );

				outSuffixName = jnew JString(m.GetSubstring("outSuffix"));
				assert( outSuffixName != nullptr );

				if (outSuffixName->IsEmpty())
				{
					*outSuffixName = GetOutputSuffix(*suffixName, suffixMapIn, suffixMapOut);
				}
			}
			else if (fullName.GetFirstCharacter() == '.')
			{
				std::cerr << argv[0] << ": invalid dependency \"";
				std::cerr << fullName << "\"\n";
				continue;
			}
			else
			{
				targetName = jnew JString(fullName);
				suffixName = jnew JString(defSuffix);
				outSuffixName = jnew JString(
					GetOutputSuffix(*suffixName, suffixMapIn, suffixMapOut));
				assert( outSuffixName != nullptr );
			}

			fnIter.Invalidate();

			auto* prefixName = jnew JString(prefix);

			usesJava = (usesJava ||
						javaObjFileSuffix.Match(*suffixName) ||
						javaObjFileSuffix.Match(*outSuffixName));

			// We want all the path information to be in the prefix.

			JString targetPrefix, targetSuffix;
			JSplitPathAndName(*targetName, &targetPrefix, &targetSuffix);
			if (targetSuffix.GetCharacterCount() < targetName->GetCharacterCount())
			{
				*prefixName += targetPrefix;
				*targetName  = targetSuffix;
			}

			auto* outPrefixName = jnew JString;

			*mainTargetObjs += " ";
			if (!outputDirName.IsEmpty())
			{
				*outPrefixName = "${" + outputDirName + "}/";
			}
			else
			{
				*outPrefixName = *prefixName;
			}
			*mainTargetObjs += *outPrefixName;
			*mainTargetObjs += *targetName;
			*mainTargetObjs += *outSuffixName;

			targetList.Append(targetName);
			prefixList.Append(prefixName);
			suffixList.Append(suffixName);
			outPrefixList.Append(outPrefixName);
			outSuffixList.Append(outSuffixName);
		}

		// If the dependency list is empty, remember it for later.

		if (mainTargetObjs->IsEmpty() && shouldMakeTarget)
		{
			prevEmptyTargets++;
		}

		// If the dependency list is not empty, fill in the previous
		// empty ones.

		else if (prevEmptyTargets > 0)
		{
			const JSize mainTargetCount = mainTargetObjsList.GetItemCount();
			for (JIndex i=1; i<=prevEmptyTargets; i++)
			{
				const JIndex j = mainTargetCount-i+1;
				*(mainTargetObjsList.GetItem(j)) = *mainTargetObjs;
				javaTargetList.SetItem(j, usesJava);
			}
			prevEmptyTargets = 0;
		}

		// Now that we have filled in any previous targets that
		// needed our dependencies, we can finish this target.

		if (shouldMakeTarget)
		{
			mainTargetList.Append(mainTargetName);
			mainTargetObjsList.Append(mainTargetObjs);
			javaTargetList.AppendItem(usesJava);
		}
		else
		{
			jdelete mainTargetName;
			jdelete mainTargetObjs;
		}
	}

	// check that no trailing targets were empty

	if (prevEmptyTargets != 0)
	{
		std::cerr << argv[0] << ": empty target found at end of " << inputName << '\n';
		return 1;
	}

// build the output file: we start with defineText + a copy of the header file
// and then append the make rules to the output file

	std::ofstream output(outputName.GetBytes());
	if (output.fail())
	{
		std::cerr << argv[0] << ": unable to write to " << outputName << std::endl;
		return 1;
	}

	output << "# This file was automatically generated by makemake.\n";
	output << "# Do not edit it directly!\n";
	output << "# Any changes you make will be silently overwritten.\n\n";

	defineText.Print(output);
	output << "\n";

	JString footerText;
	{
	JString s;
	JReadFile(headerName, &s);

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	s.Split(kMakeFooterMarkerStr, &list, 2);

	JString* headerText = list.GetFirstItem();
	headerText->TrimWhitespace();
	headerText->Print(output);

	if (list.GetItemCount() > 1)
	{
		footerText = *(list.GetLastItem());
		footerText.TrimWhitespace();
	}
	}

	output << "\n\n\n# This is what makemake added\n\n";

// write the dependencies and link command for each main target

	const JSize mainTargetCount = mainTargetList.GetItemCount();
	const JSize targetCount     = targetList.GetItemCount();

	for (JIndex i=1; i<=mainTargetCount; i++)
	{
		JString* mainTargetName = mainTargetList.GetItem(i);
		JString* mainTargetObjs = mainTargetObjsList.GetItem(i);

		// mark it with a comment

		output << "\n# ";
		mainTargetName->Print(output);
		output << "\n\n";

		// append the updating rule (linker) for the main target

		mainTargetName->Print(output);
		output << " ::";
		mainTargetObjs->Print(output);
		output << '\n';

		// append the actual link command

		if (javaTargetList.GetItem(i))
		{
			output << "\t${JAVAC} ${JFLAGS} "
					  "-classpath ${JPATH}:${filter %.jar %.zip %.class, $^} "
					  "${filter-out %.jar %.zip %.class, $^}\n";

			output << "  ifeq (${suffix ";
			mainTargetName->Print(output);
			output << "}, .jar)\n";

				output << "\t${JAR} -cf $@ ${filter-out %.jar %.zip, $^}\n";

			output << "  else\n";
			output << "  ifeq (${suffix ";
			mainTargetName->Print(output);
			output << "}, .zip)\n";

				output << "\tzip $@ ${filter-out %.jar %.zip, $^}\n";

			output << "  endif\n";
			output << "  endif\n";
		}
		else
		{
			output << "  ifeq (${suffix ";
			mainTargetName->Print(output);
			output << "}, .a)\n";

				output << "\t@${RM} $@\n";

				#ifdef _J_RANLIB_VIA_AR_S
					output << "\t${AR} crs $@ ${filter-out %.a, $^}\n";
				#else
					output << "\t${AR} cr $@ ${filter-out %.a, $^}\n";
					output << "\tranlib $@\n";
				#endif

			output << "  else\n";

				output << "\t${LINKER} ${LDFLAGS} -o $@ $^ ${LDLIBS}\n";

			output << "  endif\n";
		}
	}
	output << '\n';

// define a variable to contain all targets

	output << "# list of all targets\n\n";
	output << "MM_ALL_TARGETS :=";

	for (JIndex i=1; i<=mainTargetCount; i++)
	{
		JString* mainTargetName = mainTargetList.GetItem(i);
		output << ' ';
		mainTargetName->Print(output);
	}
	output << "\n\n";

// append a target so user can easily make all files

	output << "# target for making everything\n\n";
	output << ".PHONY : all\n";
	output << "all:: ${MM_ALL_TARGETS}\n\n";

// append a target so user can easily remove the object files

	output << "# target for removing all object files\n\n";
	output << ".PHONY : tidy\n";
	output << "tidy::\n";
	output << "\t@${RM} core";
{
	for (JIndex i=1; i<=targetCount; i++)
	{
		output << ' ';
		outPrefixList.GetItem(i)->Print(output);
		targetList.GetItem(i)->Print(output);
		outSuffixList.GetItem(i)->Print(output);
	}
}
	output << "\n\n";

// append a target so user can easily remove everything

	output << "# target for removing all object files\n\n";
	output << ".PHONY : clean\n";
	output << "clean:: tidy\n";
	output << "\t@${RM} ${MM_ALL_TARGETS}\n\n";

// define a variable to contain all sources

	output << "# list of all source files\n\n";
	output << "MM_ALL_SOURCES :=";
{
	for (JIndex i=1; i<=targetCount; i++)
	{
		output << ' ';
		prefixList.GetItem(i)->Print(output);
		targetList.GetItem(i)->Print(output);
		suffixList.GetItem(i)->Print(output);
	}
}
	output << "\n\n";

// append a target to allow checking of syntax of a particular file

	output << "# target for checking a source file\n\n";
	output << "CHECKSYNTAXFILE := ${basename ${filter %${CHECKSTRING}, ${MM_ALL_SOURCES}}}\n\n";
	output << ".PHONY : checksyntax\n";
	output << "checksyntax:\n";
	output << "  ifneq (${CHECKSYNTAXFILE},)\n";
	output << "\t@${MAKE} ${addsuffix " << kDefOutputSuffix <<", ${CHECKSYNTAXFILE}}\n";
	output << "  else\n";
	output << "\t@echo No target to make ${CHECKSTRING}\n";
	output << "  endif\n\n\n";

// append a target to allow touching all sources that contain a given string

	output << "# target for touching appropriate source files\n\n";
	output << ".PHONY : touch\n";
	output << "touch::\n";

	// can't use $$(...) because GNU make on Solaris 2.5 doesn't like it

	output << "\t@list=`grep -l ${TOUCHSTRING} ${MM_ALL_SOURCES}`; \\\n";
	output << "     for file in $$list; do { echo $$file; touch $$file; } done\n";

// append footer, if any

	if (!footerText.IsEmpty())
	{
		output << "# from Make.header\n\n";
		footerText.Print(output);
	}
	output << "\n\n\n";

// append a target to build the initial dependencies via makedepend

	output << "# target for calculating dependencies (";
	output << kMakemakeBinaryVar;
#if USE_TEMP_FILE_FOR_DEPEND
	output << ", " << kDependInputFileVar;
#endif
	output << ")\n\n";
	output << ".PHONY : " << kDependTargetName << '\n';
	output << kDependTargetName << ":\n";

#if USE_TEMP_FILE_FOR_DEPEND
	JString tempFileName;
	const JError err = JCreateTempFile(&tempFileName);
	if (!err.OK())
	{
		std::cerr << argv[0] << ": error creating temporary file: " << err.GetMessage() << std::endl;
		return 1;
	}
{
	for (JIndex i=1; i<=targetCount; i++)
	{
		if (!noParseFileSuffix.Match(*suffixList.GetItem(i)))
		{
			// write the file to parse

			output << "\t@echo ";
			prefixList.GetItem(i)->Print(output);
			targetList.GetItem(i)->Print(output);
			suffixList.GetItem(i)->Print(output);
			output << " >> " << kDependInputFile << '\n';

			// write the string that should be used in the Makefile

			output << "\t@echo ";
			PrintForMake(output, *(outPrefixList.GetItem(i)));
			PrintForMake(output, *(targetList.GetItem(i)));
			PrintForMake(output, *(outSuffixList.GetItem(i)));
			output << " >> " << kDependInputFile << '\n';
		}
	}
}
#endif

	output << "\t@" << kMakemakeBinary << " --depend ";
	outputName.Print(output);
	if (!outputDirName.IsEmpty())
	{
		output << ' ' << kObjDirArg << ' ';
		outputDirName.Print(output);
	}
	if (!searchSysDir)
	{
		output << ' ' << kNoStdIncArg;
	}
	output << " -- ${DEPENDFLAGS} -- ";

#if USE_TEMP_FILE_FOR_DEPEND

	output << kDependInputFile;

#else
{
	for (JIndex i=1; i<=targetCount; i++)
	{
		if (!noParseFileSuffix.Match(*(suffixList.GetItem(i))))
		{
			// write the file to parse

			output << ' ';
			(prefixList.GetItem(i))->Print(output);
			(targetList.GetItem(i))->Print(output);
			(suffixList.GetItem(i))->Print(output);

			// write the string that should be used in the Makefile

			output << ' ';
			PrintForMake(output, *(outPrefixList.GetItem(i)));
			PrintForMake(output, *(targetList.GetItem(i)));
			PrintForMake(output, *(outSuffixList.GetItem(i)));
		}
	}
}
#endif

	output << "\n\n\n";

	// place the marker for makedepend

	output << kMakedependMarkerStr << '\n';
	if (output.fail())
	{
		std::cerr << argv[0] << ": error while writing to " << outputName << std::endl;
		return 1;
	}
	output.close();

	// build the dependency graph

	JString makemakeBinaryDef(kMakemakeBinaryVar);
	makemakeBinaryDef += "=";
	makemakeBinaryDef += argv[0];

#if USE_TEMP_FILE_FOR_DEPEND

	JString tempFileDef(kDependInputFileVar);
	tempFileDef += "=";
	tempFileDef += tempFileName;

	const JUtf8Byte* depArgv[] =
	{ kMakeBinary, makemakeBinaryDef.GetBytes(), tempFileDef.GetBytes(),
		  "-f", outputName.GetBytes(), kDependTargetName, nullptr };

#else

	const JUtf8Byte* depArgv[] =
	{ kMakeBinary, makemakeBinaryDef.GetBytes(),
		  "-f", outputName.GetBytes(), kDependTargetName, nullptr };

#endif

	JProcess* p;
	const JError depErr =
		JProcess::Create(&p, depArgv, sizeof(depArgv));
	if (!depErr.OK())
	{
		std::cerr << argv[0] << ": " << depErr.GetMessage() << std::endl;
		return 1;
	}
	p->WaitUntilFinished();

#if USE_TEMP_FILE_FOR_DEPEND
	JRemoveFile(tempFileName);
#endif

	JUpdateCVSIgnore(outputName);

	if (p->SuccessfulFinish())
	{
		jdelete p;
		return 0;
	}
	else
	{
		std::cerr << argv[0] << ": error while calculating dependencies" << std::endl;
		jdelete p;
		return 1;
	}
}

/******************************************************************************
 ShouldMakeTarget

	Returns true if target is in the list.  If the list is empty, it means
	"make all targets" so we just return true.

 ******************************************************************************/

bool
ShouldMakeTarget
	(
	const JString&				target,
	const JPtrArray<JString>&	list
	)
{
	if (list.IsEmpty())
	{
		return true;
	}

	const JSize count = list.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (target == *(list.GetItem(i)))
		{
			return true;
		}
	}
	return false;
}

/******************************************************************************
 PrintForMake

	Replace $ with \$$ so neither make nor the shell will evaluate the variables.

 ******************************************************************************/

void
PrintForMake
	(
	std::ostream&	output,
	const JString&	str
	)
{
	JStringIterator iter(str);
	JUtf8Character c;
	while (iter.Next(&c))
	{
		if (c == '$')
		{
			output << "\\$$";
		}
		else
		{
			output << c;
		}
	}
}

/******************************************************************************
 GetOutputSuffix

	Returns the appropriate output suffix that results from compiling
	the given suffix.

 ******************************************************************************/

JString
GetOutputSuffix
	(
	const JString&				inputSuffix,
	const JPtrArray<JString>&	suffixMapIn,
	const JPtrArray<JString>&	suffixMapOut
	)
{
	const JSize count = suffixMapIn.GetItemCount();
	assert( count == suffixMapOut.GetItemCount() );

	for (JIndex i=1; i<=count; i++)
	{
		if (inputSuffix == *(suffixMapIn.GetItem(i)))
		{
			return *(suffixMapOut.GetItem(i));
		}
	}

	return JString(kDefOutputSuffix, JString::kNoCopy);
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
	JString*			defSuffix,
	JString*			defineText,
	JString*			headerName,
	JString*			inputName,
	JString*			outputName,
	JString*			outputDirName,
	JPtrArray<JString>*	userTargetList,
	bool*				searchSysDir,
	JPtrArray<JString>*	suffixMapIn,
	JPtrArray<JString>*	suffixMapOut
	)
{
	assert( userTargetList->IsEmpty() &&
			suffixMapIn->IsEmpty() &&
			suffixMapOut->IsEmpty() );

	*defSuffix    = ".c";
	*headerName   = "Make.header";
	*inputName    = "Make.files";
	*outputName   = "Makefile";
	*searchSysDir = true;

	{
	auto* s = jnew JString(".java");
	suffixMapIn->Append(s);
	s = jnew JString(".java");
	suffixMapOut->Append(s);
	}

	outputDirName->Clear();
	defineText->Clear();

	bool pickTargets   = false;
	bool checkModTimes = false;

	JPtrArray<JString> searchPaths(JPtrArrayT::kDeleteAll);
	searchPaths.Append(kCurrentDir);

	// command line options

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
			PrintHelp(*headerName, *inputName, *outputName);
			exit(0);
		}

		else if (strcmp(argv[index], "--define") == 0)
		{
			JCheckForValues(1, &index, argc, argv);
			*defineText += argv[index];
			defineText->Append("\n");
		}

		else if (strcmp(argv[index], "--search-path") == 0)
		{
			JCheckForValues(1, &index, argc, argv);
			auto* s = jnew JString(argv[index]);
			searchPaths.Append(s);
		}

		else if (strcmp(argv[index], "-hf") == 0)
		{
			JCheckForValues(1, &index, argc, argv);
			*headerName = argv[index];
		}
		else if (strcmp(argv[index], "-if") == 0)
		{
			JCheckForValues(1, &index, argc, argv);
			*inputName = argv[index];
		}
		else if (strcmp(argv[index], "-of") == 0)
		{
			JCheckForValues(1, &index, argc, argv);
			*outputName = argv[index];
		}

		else if (strcmp(argv[index], kObjDirArg) == 0)
		{
			JCheckForValues(1, &index, argc, argv);
			*outputDirName = argv[index];
		}

		else if (strcmp(argv[index], "--suffix-map") == 0)
		{
			JCheckForValues(1, &index, argc, argv);

			JString arg(argv[index], JString::kNoCopy);
			JStringIterator iter(arg);
			if (iter.Next(suffixMapPattern))
			{
				const JStringMatch& m = iter.GetLastMatch();
				auto* s = jnew JString(m.GetSubstring(1));
				assert( s != nullptr );
				suffixMapIn->Append(s);
				s = jnew JString(m.GetSubstring(2));
				assert( s != nullptr );
				suffixMapOut->Append(s);
			}
			else
			{
				std::cerr << argv[0] << ":  invalid argument to --suffix-map" << std::endl;
				exit(1);
			}
		}

		else if (strcmp(argv[index], "--no-parse") == 0)
		{
			JCheckForValues(1, &index, argc, argv);

			JString p = noParseFileSuffix.GetPattern();
			JStringIterator iter1(&p, JStringIterator::kStartAtEnd);
			iter1.SkipPrev(2);

			JStringIterator iter2(JString(argv[index], JString::kNoCopy));
			while (iter2.Next(noParsePattern))
			{
				JString s = iter2.GetLastMatch().GetSubstring(1);
				s.Prepend("|");
				iter1.Insert(s);
			}

			noParseFileSuffix.SetPatternOrDie(p);
		}

		else if (strcmp(argv[index], kNoStdIncArg) == 0)
		{
			*searchSysDir = false;
		}

		else if (strcmp(argv[index], "--check") == 0)
		{
			checkModTimes = true;
		}

		else if (strcmp(argv[index], "--choose") == 0)
		{
			pickTargets = true;
		}

		else if (strcmp(argv[index], "--make-name") == 0)
		{
			JCheckForValues(1, &index, argc, argv);
			kMakeBinary = argv[index];
		}

		else if (argv[index][0] == '-')
		{
			std::cerr << argv[0] << ": unknown command line option: " << argv[index] << std::endl;
		}

		else
		{
			auto* userTarget = jnew JString(argv[index]);
			userTargetList->Append(userTarget);
		}

		index++;
	}

	JString s = *headerName;
	if (FindFile(s, searchPaths, headerName))
	{
		s = *headerName;
		const bool ok = JGetTrueName(s, headerName);
		assert( ok );
	}
	else
	{
		std::cerr << argv[0] << ": header file not found" << std::endl;
		exit(1);
	}

	s = *inputName;
	if (FindFile(s, searchPaths, inputName))
	{
		s = *inputName;
		const bool ok = JGetTrueName(s, inputName);
		assert( ok );
	}
	else
	{
		std::cerr << argv[0] << ": project file not found" << std::endl;
		exit(1);
	}

	if (outputName->Contains("/"))
	{
		JString path, name;
		JSplitPathAndName(*outputName, &path, &name);
		JString fullPath;
		if (!JGetTrueName(path, &fullPath))
		{
			std::cerr << argv[0] << ": invalid path for output file" << std::endl;
			exit(1);
		}
		const JError err = JChangeDirectory(fullPath);
		if (!err.OK())
		{
			std::cerr << argv[0] << ": " << err.GetMessage() << std::endl;
			exit(1);
		}
		*outputName = name;
	}

	// check mod times

	if (checkModTimes)
	{
		time_t headerTime, inputTime, outputTime;
		if (JGetModificationTime(*headerName, &headerTime) != kJNoError)
		{
			std::cerr << argv[0] << ": unable to get modification time for " << *headerName << std::endl;
			exit(1);
		}
		if (JGetModificationTime(*inputName, &inputTime) != kJNoError)
		{
			std::cerr << argv[0] << ": unable to get modification time for " << *inputName << std::endl;
			exit(1);
		}

		// don't build it if it exists and is newer

		if (JGetModificationTime(*outputName, &outputTime) == kJNoError &&
			outputTime > headerTime && outputTime > inputTime)
		{
			exit(0);
		}
	}

	// let the user pick targets to include

	if (pickTargets)
	{
		PickTargets(*inputName, userTargetList);
	}
}

/******************************************************************************
 PickTargets

	Show the user a list of the targets in the input file and let them
	choose which ones to include.  "All" is not an option because that
	is the default from the command line.

 ******************************************************************************/

void
PickTargets
	(
	const JString&		fileName,
	JPtrArray<JString>*	list
	)
{
	JPtrArray<JString> all(JPtrArrayT::kDeleteAll);
	JSize count = 0;
	std::cout << std::endl;

	std::ifstream input(fileName.GetBytes());
	JIgnoreUntil(input, '@');
	while (!input.eof() && !input.fail())
	{
		auto* targetName = jnew JString(JReadUntil(input, '\n'));
		assert( targetName != nullptr );

		all.Append(targetName);
		count++;
		std::cout << count << ") " << *targetName << std::endl;

		JIgnoreUntil(input, '@');
	}
	input.close();

	std::cout << std::endl;

	while (true)
	{
		JIndex choice;
		std::cout << "Target to include (0 to end): ";
		std::cin >> choice;
		JInputFinished();

		if (choice == 0 && list->IsEmpty())
		{
			exit(0);
		}
		else if (choice == 0)
		{
			break;
		}
		else if (choice > count)
		{
			std::cerr << "That is not a valid choice" << std::endl;
		}
		else
		{
			auto* targetName = jnew JString(*(all.GetItem(choice)));
			assert( targetName != nullptr );

			list->Append(targetName);
		}
	}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp
	(
	const JString& headerName,
	const JString& inputName,
	const JString& outputName
	)
{
	std::cout << std::endl;
	std::cout << "This program builds a Makefile from:" << std::endl;
	std::cout << std::endl;
	std::cout << "    1) Header file:  defines make variables" << std::endl;
	std::cout << "    2) Project file: contains a list of required code files" << std::endl;
	std::cout << std::endl;
	std::cout << "The format of the project file is:" << std::endl;
	std::cout << std::endl;
	std::cout << "    @<name of first target>" << std::endl;
	std::cout << "    <first object file>" << std::endl;
	std::cout << "    <second object file>" << std::endl;
	std::cout << "    ..." << std::endl;
	std::cout << "    @<name of second target>" << std::endl;
	std::cout << "    ..." << std::endl;
	std::cout << std::endl;
	std::cout << "If no objects files are listed for a target, it is assumed that" << std::endl;
	std::cout << "they are the same as those for the next target in the file." << std::endl;
	std::cout << std::endl;
	std::cout << "Embedded options:" << std::endl;
	std::cout << std::endl;
	std::cout << "    -prefix <prefix for following files>" << std::endl;
	std::cout << "            (reset to blank at start of each main target)" << std::endl;
	std::cout << "    -suffix <suffix for following files>" << std::endl;
	std::cout << "            (override for specific file by placing suffix in front of name)" << std::endl;
	std::cout << "    (you can redefine these as often as you want)" << std::endl;
	std::cout << std::endl;
	std::cout << "Comments can be included by beginning the line with #" << std::endl;
	std::cout << std::endl;
	std::cout << "Usage:  <options> <targets to generate>" << std::endl;
	std::cout << std::endl;
	std::cout << "-h                prints help" << std::endl;
	std::cout << "-v                prints version information" << std::endl;
	std::cout << "-hf               <header file name>  - default " << headerName << std::endl;
	std::cout << "-if               <project file name> - default " << inputName << std::endl;
	std::cout << "-of               <output file name>  - default " << outputName << std::endl;
	std::cout << kObjDirArg << "         <variable name>     - specifies directory for all .o files" << std::endl;
	std::cout << kNoStdIncArg << "      exclude dependencies on files in /usr/include" << std::endl;
	std::cout << "--check           only rebuild output file if input files are newer" << std::endl;
	std::cout << "--choose          interactively choose the targets" << std::endl;
	std::cout << "--make-name       <make binary> - default " << kMakeBinary << std::endl;
	std::cout << std::endl;
	std::cout << "The following options can be used multiple times:" << std::endl;
	std::cout << std::endl;
	std::cout << "--define      <code> - make code to prepend to Make.header" << std::endl;
	std::cout << "--search-path <path> - add to paths to search after . for inputs files" << std::endl;
	std::cout << "--suffix-map  .x.y   - file.y results from compiling file.x" << std::endl;
	std::cout << "--no-parse    .x.y.z - specifies suffixes of files which should not be parsed" << std::endl;
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

/******************************************************************************
 Dependency graph

 ******************************************************************************/

struct HeaderDep
{
	JString*			fileName;
	JPtrArray<JString>*	depList;	// list of files that fileName -explicitly- #includes

	HeaderDep()
		:
		fileName(nullptr), depList(nullptr)
{ };

	HeaderDep(JString* name, JPtrArray<JString>* list)
		:
		fileName(name), depList(list)
{ };
};

void		WriteDependencies(std::ostream& output, const JString& fileName,
							  const JString& makeName,
							  const JPtrArray<JString>& pathList1,
							  const JPtrArray<JString>& pathList2,
							  const JString& outputDirName,
							  JArray<HeaderDep>* headerList);
void		PrintDependencies(std::ostream& output, const JString& outputDirName,
							  const JString& makeName,
							  const JPtrArray<JString>& depList);
void		AddDependency(JPtrArray<JString>* depList, const JString& headerName,
						  const JPtrArray<JString>& pathList1,
						  const JPtrArray<JString>& pathList2,
						  JArray<HeaderDep>* headerList,
						  const bool addToDepList = true);
HeaderDep	ParseHeaderFile(const JString& fileName,
							const JPtrArray<JString>& pathList1,
							const JPtrArray<JString>& pathList2,
							JArray<HeaderDep>* headerList);
bool		GetNextIncludedFile(const JString& inputFileName, std::istream& input,
								const JPtrArray<JString>& pathList1,
								const JPtrArray<JString>& pathList2,
								JString* fileName);
void		TruncateMakefile(const JString& fileName);

std::weak_ordering CompareHeaderFiles(const HeaderDep& h1, const HeaderDep& h2);

/******************************************************************************
 CalcDepend

	Append the dependency graph to the specified file.

	*** This function does not return.

 ******************************************************************************/

void
CalcDepend
	(
	const JSize	startArg,
	const JSize	argc,
	char*		argv[]
	)
{
	// apply environment variables

	const JUtf8Byte* env = getenv("J_MAKEMAKE_IGNORE_PATTERN");
	if (!JString::IsEmpty(env))
	{
		globalIgnorePattern = jnew JRegex(env);
	}

	// parse command line arguments

	JString outputDirName;
	bool searchSysDir = true;

	JIndex i = startArg;

	const JString makefileName(argv[i], JString::kNoCopy);
	i++;

	while (i < argc-1 && strcmp(argv[i], "--") != 0)
	{
		if (strcmp(argv[i], kObjDirArg) == 0)
		{
			JCheckForValues(1, &i, argc, argv);
			outputDirName = argv[i];
		}
		else if (strcmp(argv[i], kNoStdIncArg) == 0)
		{
			searchSysDir = false;
		}
		else
		{
			std::cerr << "Unknown argument " << argv[i] << " in \"makemake --depend\"" << std::endl;
		}
		i++;
	}

	if (i >= argc || strcmp(argv[i], "--") != 0)
	{
		std::cerr << "Missing first \"--\" in \"makemake --depend\"" << std::endl;
		exit(1);
	}
	i++;	// move past "--"

	// find directories to search

	JPtrArray<JString> pathList1(JPtrArrayT::kForgetAll),
					   pathList2(JPtrArrayT::kForgetAll);
	JString* path;

	bool searchCurrDir = true;

	while (i < argc && strcmp(argv[i], "--") != 0)
	{
		if (strcmp(argv[i], "-nostdinc") == 0)
		{
			searchSysDir = false;
		}
		else if (strcmp(argv[i], "-I-") == 0)
		{
			searchCurrDir = false;
		}
		else if (argv[i][0] == '-' && argv[i][1] == 'I' &&
				 argv[i][2] != '\0' && argv[i][2] != '-')
		{
			if (JDirectoryReadable(JString(argv[i]+2, JString::kNoCopy)))
			{
				path = jnew JString(argv[i]+2);		// strip off "-I"
				assert( path != nullptr );
				if (searchCurrDir)
				{
					pathList1.Append(path);
				}
				else
				{
					pathList2.Append(path);
				}
			}
			else
			{
				std::cerr << argv[0] << ": invalid path " << argv[i] << std::endl;
			}
		}
		i++;
	}
	i++;	// skip second "--"

	if (i >= argc)
	{
#if USE_TEMP_FILE_FOR_DEPEND
//		std::cerr << "Missing transfer file name in \"makemake --depend\"" << std::endl;
#else
//		std::cerr << "Missing file list in \"makemake --depend\"" << std::endl;
#endif
		return;
	}

	if (searchCurrDir)
	{
		// don't include ./ for <...>
		pathList2.CopyPointers(pathList1, JPtrArrayT::kForgetAll, false);
		pathList1.Prepend(kCurrentDir);
	}

	if (searchSysDir)
	{
		pathList2.Append(kSysIncludeDir);
	}

	// append dependencies to input file

	TruncateMakefile(makefileName);

	std::ofstream output(makefileName.GetBytes(), std::ios::app);
	output << '\n';

	JArray<HeaderDep> headerList;		// header files that have been processed
	headerList.SetCompareFunction(CompareHeaderFiles);

#if USE_TEMP_FILE_FOR_DEPEND

	std::ifstream input(argv[i]);

	JString fileName, makeName;
	while (true)
	{
		fileName = JReadLine(input);
		if (input.eof() || input.fail())
		{
			break;
		}
		makeName = JReadLine(input);
		WriteDependencies(output, fileName, makeName, pathList1, pathList2,
						  outputDirName, &headerList);
	}

#else

	for ( ; i<argc; i+=2)
	{
		WriteDependencies(output, argv[i], argv[i+1], pathList1, pathList2,
						  outputDirName, &headerList);
	}

#endif

#if ! ALLOW_INCLUDE_LOOPS

	const JSize headerCount = headerList.GetItemCount();
	for (i=1; i<=headerCount; i++)
	{
		const HeaderDep info = headerList.GetItem(i);
		PrintDependencies(output, outputDirName, *(info.fileName), *(info.depList));
	}

#endif

	// We should clean up, but it's not worth the trouble.
	// (and pathList1 and pathList2 share objects)
}

/******************************************************************************
 WriteDependencies

	Write a line to the Makefile describing -all- the files that the given
	file depends on.

	Since Make uses string comparisons to find the source files among the
	rules, the string in front of the colon has to be -exactly- the same
	as in the source file list.  This is what makeName is.

 ******************************************************************************/

void
WriteDependencies
	(
	std::ostream&				output,
	const JString&				fileName,
	const JString&				makeName,
	const JPtrArray<JString>&	pathList1,
	const JPtrArray<JString>&	pathList2,
	const JString&				outputDirName,
	JArray<HeaderDep>*			headerList
	)
{
	if (!JFileExists(fileName))
	{
		std::cerr << "Source file \"" << fileName << "\" not found" << std::endl;
		return;
	}

	// build dependency list

	JPtrArray<JString> depList(JPtrArrayT::kForgetAll);
	depList.SetCompareFunction(JCompareStringsCaseSensitive);

	std::ifstream input(fileName.GetBytes());
	JString headerName;
	while (GetNextIncludedFile(fileName, input, pathList1, pathList2, &headerName))
	{
		AddDependency(&depList, headerName, pathList1, pathList2, headerList);
	}
	input.close();

	// write dependencies

	PrintDependencies(output, outputDirName, makeName, depList);
}

/******************************************************************************
 PrintDependencies

 ******************************************************************************/

void
PrintDependencies
	(
	std::ostream&				output,
	const JString&				outputDirName,
	const JString&				makeName,
	const JPtrArray<JString>&	depList
	)
{
	if (!depList.IsEmpty())
	{
		if (!outputDirName.IsEmpty())
		{
			JString p,f;
			JSplitPathAndName(makeName, &p, &f);
			output << "${" << outputDirName << "}/";
			f.Print(output);
		}
		else
		{
			makeName.Print(output);
		}

		output << ':';

		for (auto* dep : depList)
		{
			output << ' ';
			dep->Print(output);
		}

		output << "\n\n";
	}
}

/******************************************************************************
 AddDependency

	Add the given file and all files that it depends on to depList.

 ******************************************************************************/

void
AddDependency
	(
	JPtrArray<JString>*			depList,
	const JString&				headerName,
	const JPtrArray<JString>&	pathList1,
	const JPtrArray<JString>&	pathList2,
	JArray<HeaderDep>*			headerList,
	const bool					addToDepList
	)
{
	const HeaderDep info = ParseHeaderFile(headerName, pathList1, pathList2, headerList);

	bool isDuplicate;
	const JIndex index =
		depList->GetInsertionSortIndex(const_cast<JString*>(&headerName), &isDuplicate);
	if (!isDuplicate)
	{
		if (addToDepList)
		{
			// must use info.fileName so we have a valid JString*
			depList->InsertAtIndex(index, info.fileName);
		}

		#if ALLOW_INCLUDE_LOOPS
		const bool addSubToDepList = true;
		#else
		const bool addSubToDepList = false;
		#endif

		const JSize count = (info.depList)->GetItemCount();
		for (JIndex i=1; i<=count; i++)
		{
			const JString* includedFileName = (info.depList)->GetItem(i);

			#if ! ALLOW_INCLUDE_LOOPS

			// check if it already exists

			HeaderDep info(const_cast<JString*>(includedFileName), nullptr);
			JIndex j;
			if (headerList->SearchSorted(info, JListT::kAnyMatch, &j))
			{
				continue;
			}

			#endif

			AddDependency(depList, *includedFileName,
						  pathList1, pathList2,
						  headerList, addSubToDepList);
		}
	}
}

/******************************************************************************
 ParseHeaderFile

	Add a record to headerList describing the depencies of the given file.

 ******************************************************************************/

HeaderDep
ParseHeaderFile
	(
	const JString&				fileName,
	const JPtrArray<JString>&	pathList1,
	const JPtrArray<JString>&	pathList2,
	JArray<HeaderDep>*			headerList
	)
{
	// check if it already exists

	HeaderDep info(const_cast<JString*>(&fileName), (JPtrArray<JString>*) nullptr);
	bool found;
	const JIndex index =
		headerList->SearchSortedOTI(info, JListT::kAnyMatch, &found);
	if (found)
	{
		return headerList->GetItem(index);
	}

	// add it to headerList

	info.fileName = jnew JString(fileName);
	assert( info.fileName != nullptr );

	info.depList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( info.depList != nullptr );
	(info.depList)->SetCompareFunction(JCompareStringsCaseSensitive);

	headerList->InsertItemAtIndex(index, info);

	// extract dependencies only from non-system header files

	if (fileName.StartsWith(kSysIncludeDir))
	{
		return info;
	}

	// find the files that fileName depends on and add them to depList

	std::ifstream input(fileName.GetBytes());
	JString headerName;
	while (GetNextIncludedFile(fileName, input, pathList1, pathList2, &headerName))
	{
		bool isDuplicate;
		const JIndex i = (info.depList)->GetInsertionSortIndex(&headerName, &isDuplicate);
		if (!isDuplicate)
		{
			(info.depList)->InsertAtIndex(i, headerName);
		}
	}

	return info;
}

/******************************************************************************
 GetNextIncludedFile

	Get the next #include from the given input stream.
	Returns false if there are no more.

 ******************************************************************************/

static const JRegex includePattern = "^\\s*#\\s*include\\s*([<\"])(.+?)[>\"]";

bool
GetNextIncludedFile
	(
	const JString&				inputFileName,
	std::istream&				input,
	const JPtrArray<JString>&	pathList1,
	const JPtrArray<JString>&	pathList2,
	JString*					fileName
	)
{
	while (!input.eof() && !input.fail())
	{
		JString line = JReadLine(input);

		JStringIterator iter(&line);
		if (iter.Next(includePattern))
		{
			const JStringMatch& m     = iter.GetLastMatch();
			const JUtf8Character type = m.GetSubstring(1).GetFirstCharacter();
			const JString name        = m.GetSubstring(2);

			if (globalIgnorePattern != nullptr &&
				globalIgnorePattern->Match(name))
			{
				// skip
			}
			else if ((type == '"' && FindFile(name, pathList1, fileName)) ||
					 FindFile(name, pathList2, fileName))
			{
				if (globalIgnorePattern != nullptr &&
					globalIgnorePattern->Match(*fileName))
				{
					continue;
				}
				return true;
			}
			else if (type == '"')	// assume in same dir as including file
			{
				JString p, n;
				if (!JSplitPathAndName(inputFileName, &p, &n))
				{
					p = "./";
				}
				*fileName = JCombinePathAndName(p, name);
				return JFileExists(*fileName);
			}
		}
	}

	fileName->Clear();
	return false;
}

/******************************************************************************
 FindFile

	Search for the file in the given list of directories.

 ******************************************************************************/

bool
FindFile
	(
	const JString&				fileName,
	const JPtrArray<JString>&	pathList,
	JString*					fullName
	)
{
	// if full path is specified, use it

	if (fileName.GetFirstCharacter() == '/' ||
		fileName.StartsWith("./") ||
		fileName.StartsWith("../"))
	{
		if (JFileExists(fileName))
		{
			*fullName = fileName;
			return true;
		}
		else
		{
			fullName->Clear();
			return false;
		}
	}

	// otherwise, check search paths

	for (const auto* path : pathList)
	{
		*fullName = JCombinePathAndName(*path, fileName);
		if (JFileExists(*fullName))
		{
			return true;
		}
	}

	fullName->Clear();
	return false;
}

/******************************************************************************
 TruncateMakefile

	Chop off old dependency list.

 ******************************************************************************/

void
TruncateMakefile
	(
	const JString& fileName
	)
{
	std::fstream f(fileName.GetBytes(), kJTextFile);

	JString line;
	do
	{
		line = JReadLine(f);
	}
		while (!f.eof() && !f.fail() && line != kMakedependMarkerStr);

	if (!f.eof() && !f.fail())
	{
		std::fstream* newF = JSetFStreamLength(fileName, f, JTellg(f), kJTextFile);
		jdelete newF;
	}
}

/******************************************************************************
 CompareHeaderFiles

	UNIX file names are case sensitive.

 ******************************************************************************/

std::weak_ordering
CompareHeaderFiles
	(
	const HeaderDep& h1,
	const HeaderDep& h2
	)
{
	return JIntToWeakOrdering(JString::Compare(*h1.fileName, *h2.fileName));
}
