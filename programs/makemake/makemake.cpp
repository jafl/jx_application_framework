/******************************************************************************
 makemake.cpp

	Program to create a Makefile given the source file names and a
	Makefile header.

	Copyright (C) 1994-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JRegex.h>
#include <JPtrArray-JString.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <JProcess.h>
#include <jCommandLine.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <JMemoryManager.h>
#include <jAssert.h>

// Turn this on to generate a Makefile that allows #include loops.
// Unfortunately, GNU Make doesn't understand the dependencies generated
// when this is turned off.
#define ALLOW_INCLUDE_LOOPS			1	// boolean

// On some systems, one cannot pass an arbitrarily long list of arguments
// to a process, so we store the information in a temp file instead.
#define USE_TEMP_FILE_FOR_DEPEND	1	// boolean

// Constants

static const JCharacter* kVersionStr =

	"makemake 3.4.0\n"
	"\n"
	"Copyright (C) 1994-2010 by John Lindal.  All rights reserved.\n"
	"\n"
	"http://www.newplanetsoftware.com/";

static const JCharacter* kMakeBinary         = J_MAKE_BINARY_NAME;
static const JCharacter* kDependTargetName   = "jdepend";
static const JCharacter* kMakemakeBinaryVar  = "MAKEMAKE";
static const JCharacter* kMakemakeBinary     = "${MAKEMAKE}";
static const JCharacter* kDependInputFileVar = "JDEPEND_INPUT_FILE";
static const JCharacter* kDependInputFile    = "${JDEPEND_INPUT_FILE}";

static const JCharacter* kMakeFooterMarkerStr =
	"### makemake inserts here ###";

static const JCharacter* kMakedependMarkerStr =
	"# DO NOT DELETE THIS LINE -- makemake depends on it.";

static const JCharacter* kObjDirArg   = "--obj-dir";
static const JCharacter* kNoStdIncArg = "--no-std-inc";
static const JCharacter* kAutoGenArg  = "--assume-autogen";

static const JCharacter* kCurrentDir    = "./";
static const JCharacter* kSysIncludeDir = "/usr/include/";

static const JCharacter* kDontInterpretFlag = "literal: ";
const JSize kDontInterpretFlagLen           = strlen(kDontInterpretFlag);

static const JCharacter* kDefOutputSuffix = ".o";

#define J_LINUX_SHARED_LIB_SUFFIX	".so"
#define J_OSX_SHARED_LIB_SUFFIX		".dylib"

static const JCharacter* kSharedLibSuffix[] =
{
	J_LINUX_SHARED_LIB_SUFFIX,
	J_OSX_SHARED_LIB_SUFFIX
};

const JSize kSharedLibSuffixCount = sizeof(kSharedLibSuffix) / sizeof(JCharacter*);

#ifdef _J_OSX
#define J_SHARED_LIB_SUFFIX	J_OSX_SHARED_LIB_SUFFIX
#else
#define J_SHARED_LIB_SUFFIX	J_LINUX_SHARED_LIB_SUFFIX
#endif

static JRegex* globalIgnorePattern = NULL;

static const JRegex objFileSuffix     = "^\\.(o|a|so|class|jar) ";
static       JRegex noParseFileSuffix = "^\\.(java|e|m[23])$";		// modified by GetOptions()
static const JRegex javaObjFileSuffix = "^\\.(java|class|jar)$";
static const JRegex suffixMapPattern  = "^(\\.[^.]+)(\\.[^.]+)$";
static const JRegex noParsePattern    = "\\.([^.]+)";

// .in_suffix [.out_suffix ]name

static const JRegex fileSuffix = "^(\\.[^ ]+) +((\\.[^ ]+) +)?(.+)$";

// Prototypes

JBoolean ShouldMakeTarget(const JString& target, const JPtrArray<JString>& list);
JBoolean AddSubTarget(JPtrArray<JString>& targetList,
					  JPtrArray<JString>& prefixList, JPtrArray<JString>& suffixList,
					  JPtrArray<JString>& outPrefixList, JPtrArray<JString>& outSuffixList,
					  JString* targetName, JString* prefixName, JString* suffixName,
					  JString* outPrefix, JString* outSuffixName);
void PrintForMake(std::ostream& output, const JString& str);
const JCharacter* GetOutputSuffix(const JString& inputSuffix,
								  const JPtrArray<JString>& suffixMapIn,
								  const JPtrArray<JString>& suffixMapOut);

void GetOptions(const JSize argc, char* argv[], JString* defSuffix,
				JString* defineText, JString* headerName,
				JString* inputName, JString* outputName, JString* outputDirName,
				JPtrArray<JString>* userTargetList, JBoolean* searchSysDir,
				JBoolean* assumeAutoGen,
				JPtrArray<JString>* suffixMapIn, JPtrArray<JString>* suffixMapOut);
JBoolean FindFile(const JCharacter* fileName, const JPtrArray<JString>& pathList,
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
	JBoolean searchSysDir, assumeAutoGen;

	JPtrArray<JString> suffixMapIn(JPtrArrayT::kDeleteAll),
					   suffixMapOut(JPtrArrayT::kDeleteAll);

	GetOptions(argc, argv, &defSuffix, &defineText, &headerName, &inputName,
			   &outputName, &outputDirName, &userTargetList, &searchSysDir,
			   &assumeAutoGen, &suffixMapIn, &suffixMapOut);

	// process the input file

	JPtrArray<JString> mainTargetList(JPtrArrayT::kDeleteAll),
					   mainTargetObjsList(JPtrArrayT::kDeleteAll);
	JArray<JBoolean>   javaTargetList;
	JPtrArray<JString> targetList(JPtrArrayT::kDeleteAll),
					   prefixList(JPtrArrayT::kDeleteAll),
					   suffixList(JPtrArrayT::kDeleteAll);
	JPtrArray<JString> outPrefixList(JPtrArrayT::kDeleteAll),
					   outSuffixList(JPtrArrayT::kDeleteAll);

	JArray<JIndexRange> matchList;

	targetList.SetCompareFunction(JCompareStringsCaseSensitive);

	// skip comments and check for possible first suffix instruction

	std::ifstream input(inputName);
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
		JBoolean usesJava = kJFalse;

		// get the name of the target for make

		JString* mainTargetName = jnew JString(JReadUntil(input,'\n'));
		assert( mainTargetName != NULL );

		// If we aren't supposed to include this target, and
		// there aren't any other targets that need our dependency
		// list, we can take a quick exit.

		input >> std::ws;
		const JBoolean shouldMakeTarget =
			ShouldMakeTarget(*mainTargetName, userTargetList);
		if (!shouldMakeTarget && prevEmptyTargets == 0)
			{
			jdelete mainTargetName;
			JIgnoreUntil(input, '@');
			input.putback('@');
			continue;
			}

		for (JIndex i=1; i<=kSharedLibSuffixCount; i++)
			{
			if (mainTargetName->EndsWith(kSharedLibSuffix[i-1]))
				{
				const JSize len = mainTargetName->GetLength();
				mainTargetName->ReplaceSubstring(
					len - strlen(kSharedLibSuffix[i-1]) + 1, len,
					J_SHARED_LIB_SUFFIX);
				break;
				}
			}

		JString* mainTargetObjs = jnew JString;
		assert( mainTargetObjs != NULL );

		// get the names of the files that the main target depends on

		while (!input.eof() && !input.fail() && input.peek() != '@')
			{
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
				}
			else if (input.peek() == '#')
				{
				JIgnoreLine(input);
				input >> std::ws;
				}
			else
				{
				JString fullName = JReadUntil(input, '\n');
				input >> std::ws;

				JBoolean usesJava1 = kJFalse;
				JIndexRange suffixRange;
				if (objFileSuffix.Match(fullName, &suffixRange))
					{
					JString suffix(fullName, suffixRange);
					suffix.TrimWhitespace();
					fullName.RemoveSubstring(suffixRange);
					fullName.TrimWhitespace();

					*mainTargetObjs += " ";
					*mainTargetObjs += prefix;
					*mainTargetObjs += fullName;
					*mainTargetObjs += suffix;

					usesJava1 = javaObjFileSuffix.Match(suffix);
					}
				else if (fullName.BeginsWith(kDontInterpretFlag))
					{
					fullName.RemoveSubstring(1, kDontInterpretFlagLen);
					fullName.TrimWhitespace();

					*mainTargetObjs += " ";
					*mainTargetObjs += fullName;
					}
				else if (fullName.GetLength() > 0)
					{
					JString* targetName    = NULL;
					JString* suffixName    = NULL;
					JString* outSuffixName = NULL;
					if (fileSuffix.Match(fullName, &matchList))
						{
						targetName = jnew JString(fullName, matchList.GetElement(5));
						assert( targetName != NULL );
						targetName->TrimWhitespace();
						suffixName = jnew JString(fullName, matchList.GetElement(2));
						assert( suffixName != NULL );

						JIndexRange r = matchList.GetElement(4);
						if (!r.IsEmpty())
							{
							outSuffixName = jnew JString(fullName, r);
							}
						else
							{
							outSuffixName = jnew JString(
								GetOutputSuffix(*suffixName, suffixMapIn, suffixMapOut));
							}
						assert( outSuffixName != NULL );
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
						assert( targetName != NULL );
						suffixName = jnew JString(defSuffix);
						assert( suffixName != NULL );
						outSuffixName = jnew JString(
							GetOutputSuffix(*suffixName, suffixMapIn, suffixMapOut));
						assert( outSuffixName != NULL );
						}

					JString* prefixName = jnew JString(prefix);
					assert( prefixName != NULL );

					usesJava1 = JI2B(javaObjFileSuffix.Match(*suffixName) ||
									 javaObjFileSuffix.Match(*outSuffixName));

					// We want all the path information to be
					// in the prefix.

					JString targetPrefix, targetSuffix;
					JSplitPathAndName(*targetName, &targetPrefix, &targetSuffix);
					if (targetSuffix.GetLength() < targetName->GetLength())
						{
						*prefixName += targetPrefix;
						*targetName  = targetSuffix;
						}

					JString* outPrefixName = jnew JString;
					assert( outPrefixName != NULL );

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

					// may delete *Name objects

					AddSubTarget(targetList, prefixList, suffixList, outPrefixList, outSuffixList,
								 targetName, prefixName, suffixName, outPrefixName, outSuffixName);
					}

				usesJava = JI2B(usesJava || usesJava1);
				}
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
			const JSize mainTargetCount = mainTargetObjsList.GetElementCount();
			for (JIndex i=1; i<=prevEmptyTargets; i++)
				{
				const JIndex j = mainTargetCount-i+1;
				*(mainTargetObjsList.NthElement(j)) = *mainTargetObjs;
				javaTargetList.SetElement(j, usesJava);
				}
			prevEmptyTargets = 0;
			}

		// Now that we have filled in any previous targets that
		// needed our dependencies, we can finish this target.

		if (shouldMakeTarget)
			{
			mainTargetList.Append(mainTargetName);
			mainTargetObjsList.Append(mainTargetObjs);
			javaTargetList.AppendElement(usesJava);
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

	std::ofstream output(outputName);
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
	JString headerText;
	JReadFile(headerName, &headerText);

	JIndex i;
	if (headerText.LocateSubstring(kMakeFooterMarkerStr, &i))
		{
		footerText = headerText.GetSubstring(i+strlen(kMakeFooterMarkerStr),
											 headerText.GetLength());
		footerText.TrimWhitespace();
		headerText.RemoveSubstring(i, headerText.GetLength());
		}

	headerText.TrimWhitespace();
	headerText.Print(output);
	}

	output << "\n\n\n# This is what makemake added\n\n";

// write the dependencies and link command for each main target

	const JSize mainTargetCount = mainTargetList.GetElementCount();
	const JSize targetCount     = targetList.GetElementCount();
	{
	for (JIndex i=1; i<=mainTargetCount; i++)
		{
		JString* mainTargetName = mainTargetList.NthElement(i);
		JString* mainTargetObjs = mainTargetObjsList.NthElement(i);

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

		if (javaTargetList.GetElement(i))
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
					output << "\t${AR} crs $@ ${filter-out %.a %" J_SHARED_LIB_SUFFIX ", $^}\n";
				#else
					output << "\t${AR} cr $@ ${filter-out %.a %" J_SHARED_LIB_SUFFIX ", $^}\n";
					output << "\tranlib $@\n";
				#endif

			output << "  else\n";
			output << "  ifeq (${suffix ";
			mainTargetName->Print(output);
			output << "}, " J_SHARED_LIB_SUFFIX ")\n";

				output << "\t${LINKER} " J_SHARED_LIB_LINK_OPTION
						  " ${LDFLAGS} -o $@ ${filter-out %.a %" J_SHARED_LIB_SUFFIX ", $^}"
						  " ${LOADLIBES}\n";

			output << "  else\n";

				output << "\t${LINKER} ${LDFLAGS} -o $@"
						  " ${filter-out %" J_SHARED_LIB_SUFFIX ", $^}"
						  " ${LOADLIBES}\n";

			output << "  endif\n";
			output << "  endif\n";
			}
		}
	}
	output << '\n';

// define a variable to contain all targets

	output << "# list of all targets\n\n";
	output << "MM_ALL_TARGETS :=";
	{
	for (JIndex i=1; i<=mainTargetCount; i++)
		{
		JString* mainTargetName = mainTargetList.NthElement(i);
		output << ' ';
		mainTargetName->Print(output);
		}
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
		(outPrefixList.NthElement(i))->Print(output);
		(targetList.NthElement(i))->Print(output);
		(outSuffixList.NthElement(i))->Print(output);
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
		(prefixList.NthElement(i))->Print(output);
		(targetList.NthElement(i))->Print(output);
		(suffixList.NthElement(i))->Print(output);
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
	output << "ifdef TOUCHPATHS\n";
	output << "\t@for path in ${TOUCHPATHS}; do \\\n";
	output << "     ( if cd $$path; then ${MAKE} TOUCHSTRING=${TOUCHSTRING} touch; fi ) \\\n";
	output << "     done\n";
	output << "endif\n\n\n";

// append footer, if any

	if (!footerText.IsEmpty())
		{
		output << "# from Make.header\n\n";
		footerText.Print(output);
		output << "\n\n\n";
		}

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
		if (!noParseFileSuffix.Match(*(suffixList.NthElement(i))))
			{
			// write the file to parse

			output << "\t@echo ";
			(prefixList.NthElement(i))->Print(output);
			(targetList.NthElement(i))->Print(output);
			(suffixList.NthElement(i))->Print(output);
			output << " >> " << kDependInputFile << '\n';

			// write the string that should be used in the Makefile

			output << "\t@echo ";
			PrintForMake(output, *(outPrefixList.NthElement(i)));
			PrintForMake(output, *(targetList.NthElement(i)));
			PrintForMake(output, *(outSuffixList.NthElement(i)));
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
	if (assumeAutoGen)
		{
		output << ' ' << kAutoGenArg;
		}
	output << " -- ${DEPENDFLAGS} -- ";

#if USE_TEMP_FILE_FOR_DEPEND

	output << kDependInputFile;

#else
	{
	for (JIndex i=1; i<=targetCount; i++)
		{
		if (!noParseFileSuffix.Match(*(suffixList.NthElement(i))))
			{
			// write the file to parse

			output << ' ';
			(prefixList.NthElement(i))->Print(output);
			(targetList.NthElement(i))->Print(output);
			(suffixList.NthElement(i))->Print(output);

			// write the string that should be used in the Makefile

			output << ' ';
			PrintForMake(output, *(outPrefixList.NthElement(i)));
			PrintForMake(output, *(targetList.NthElement(i)));
			PrintForMake(output, *(outSuffixList.NthElement(i)));
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

	JString makemakeBinaryDef = kMakemakeBinaryVar;
	makemakeBinaryDef += "=";
	makemakeBinaryDef += argv[0];

#if USE_TEMP_FILE_FOR_DEPEND

	JString tempFileDef = kDependInputFileVar;
	tempFileDef += "=";
	tempFileDef += tempFileName;

	const JCharacter* depArgv[] =
		{ kMakeBinary, makemakeBinaryDef, tempFileDef,
		  "-f", outputName, kDependTargetName, NULL };

#else

	const JCharacter* depArgv[] =
		{ kMakeBinary, makemakeBinaryDef,
		  "-f", outputName, kDependTargetName, NULL };

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

	Returns kJTrue if target is in the list.  If the list is empty, it means
	"make all targets" so we just return kJTrue.

 ******************************************************************************/

JBoolean
ShouldMakeTarget
	(
	const JString&				target,
	const JPtrArray<JString>&	list
	)
{
	if (list.IsEmpty())
		{
		return kJTrue;
		}

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (target == *(list.NthElement(i)))
			{
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 AddSubTarget

	Add the given target name to the targetList if it is new.
	Returns kJTrue if the target was added.

	*** If it returns kJFalse, the JStrings were deleted.

 ******************************************************************************/

JBoolean
AddSubTarget
	(
	JPtrArray<JString>&	targetList,
	JPtrArray<JString>&	prefixList,
	JPtrArray<JString>&	suffixList,
	JPtrArray<JString>&	outPrefixList,
	JPtrArray<JString>&	outSuffixList,
	JString*			targetName,
	JString*			prefixName,
	JString*			suffixName,
	JString*			outPrefixName,
	JString*			outSuffixName
	)
{
	JBoolean found;
	const JIndex index =
		targetList.SearchSorted1(targetName, JOrderedSetT::kAnyMatch, &found);

	if (found)
		{
		jdelete targetName;
		jdelete prefixName;
		jdelete suffixName;
		jdelete outPrefixName;
		jdelete outSuffixName;
		return kJFalse;
		}
	else
		{
		targetList.InsertAtIndex(index, targetName);
		prefixList.InsertAtIndex(index, prefixName);
		suffixList.InsertAtIndex(index, suffixName);
		outPrefixList.InsertAtIndex(index, outPrefixName);
		outSuffixList.InsertAtIndex(index, outSuffixName);
		return kJTrue;
		}
}

/******************************************************************************
 PrintForMake

	Replace $ with \$$ so neither make nor the shell will evaluate the variables.

 ******************************************************************************/

void
PrintForMake
	(
	std::ostream&		output,
	const JString&	str
	)
{
	const JSize length = str.GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		const JCharacter c = str.GetCharacter(i);
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

const JCharacter*
GetOutputSuffix
	(
	const JString&				inputSuffix,
	const JPtrArray<JString>&	suffixMapIn,
	const JPtrArray<JString>&	suffixMapOut
	)
{
	const JSize count = suffixMapIn.GetElementCount();
	assert( count == suffixMapOut.GetElementCount() );

	for (JIndex i=1; i<=count; i++)
		{
		if (inputSuffix == *(suffixMapIn.NthElement(i)))
			{
			return *(suffixMapOut.NthElement(i));
			}
		}

	return kDefOutputSuffix;
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
	JBoolean*			searchSysDir,
	JBoolean*			assumeAutoGen,
	JPtrArray<JString>*	suffixMapIn,
	JPtrArray<JString>*	suffixMapOut
	)
{
	assert( userTargetList->IsEmpty() &&
			suffixMapIn->IsEmpty() &&
			suffixMapOut->IsEmpty() );

	*defSuffix     = ".c";
	*headerName    = "Make.header";
	*inputName     = "Make.files";
	*outputName    = "Makefile";
	*searchSysDir  = kJTrue;
	*assumeAutoGen = kJFalse;

	{
	JString* s = jnew JString(".java");
	assert( s != NULL );
	suffixMapIn->Append(s);
	s = jnew JString(".java");
	assert( s != NULL );
	suffixMapOut->Append(s);
	}

	outputDirName->Clear();
	defineText->Clear();

	JBoolean pickTargets   = kJFalse;
	JBoolean checkModTimes = kJFalse;

	JPtrArray<JString> searchPaths(JPtrArrayT::kDeleteAll);
	searchPaths.Append(kCurrentDir);

	// command line options

	JIndex index = 1;
	JArray<JIndexRange> matchList;
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
			defineText->AppendCharacter('\n');
			}

		else if (strcmp(argv[index], "--search-path") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			JString* s = jnew JString(argv[index]);
			assert( s != NULL );
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

			if (suffixMapPattern.Match(argv[index], &matchList))
				{
				JString* s = jnew JString(argv[index], matchList.GetElement(2));
				assert( s != NULL );
				suffixMapIn->Append(s);
				s = jnew JString(argv[index], matchList.GetElement(3));
				assert( s != NULL );
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

			JIndexRange r;
			while (noParsePattern.MatchAfter(argv[index], r, &matchList))
				{
				JString s(argv[index], matchList.GetElement(2));
				s.PrependCharacter('|');
				p.InsertSubstring(s, p.GetLength()-1);
				r = matchList.GetElement(1);
				}

			noParseFileSuffix.SetPatternOrDie(p);
			}

		else if (strcmp(argv[index], kNoStdIncArg) == 0)
			{
			*searchSysDir = kJFalse;
			}

		else if (strcmp(argv[index], kAutoGenArg) == 0)
			{
			*assumeAutoGen = kJTrue;
			}

		else if (strcmp(argv[index], "--check") == 0)
			{
			checkModTimes = kJTrue;
			}

		else if (strcmp(argv[index], "--choose") == 0)
			{
			pickTargets = kJTrue;
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
			JString* userTarget = jnew JString(argv[index]);
			assert( userTarget != NULL );
			userTargetList->Append(userTarget);
			}

		index++;
		}

	JString s = *headerName;
	if (FindFile(s, searchPaths, headerName))
		{
		s = *headerName;
		const JBoolean ok = JGetTrueName(s, headerName);
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
		const JBoolean ok = JGetTrueName(s, inputName);
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

	std::ifstream input(fileName);
	JIgnoreUntil(input, '@');
	while (!input.eof() && !input.fail())
		{
		JString* targetName = jnew JString(JReadUntil(input, '\n'));
		assert( targetName != NULL );

		all.Append(targetName);
		count++;
		std::cout << count << ") " << *targetName << std::endl;

		JIgnoreUntil(input, '@');
		}
	input.close();

	std::cout << std::endl;

	while (1)
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
			JString* targetName = jnew JString(*(all.NthElement(choice)));
			assert( targetName != NULL );

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
	std::cout << kAutoGenArg << "  assume unfound \"...\" files reside in includer's directory" << std::endl;
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
		fileName(NULL), depList(NULL)
	{ };

	HeaderDep(JString* name, JPtrArray<JString>* list)
		:
		fileName(name), depList(list)
	{ };
};

void		WriteDependencies(std::ostream& output, const JCharacter* fileName,
							  const JCharacter* makeName,
							  const JPtrArray<JString>& pathList1,
							  const JPtrArray<JString>& pathList2,
							  const JBoolean assumeAutoGen,
							  const JCharacter* outputDirName,
							  JArray<HeaderDep>* headerList);
void		PrintDependencies(std::ostream& output, const JCharacter* outputDirName,
							  const JCharacter* makeName,
							  const JPtrArray<JString>& depList);
void		AddDependency(JPtrArray<JString>* depList, const JString& headerName,
						  const JPtrArray<JString>& pathList1,
						  const JPtrArray<JString>& pathList2,
						  const JBoolean assumeAutoGen,
						  JArray<HeaderDep>* headerList,
						  const JBoolean addToDepList = kJTrue);
HeaderDep	ParseHeaderFile(const JString& fileName,
							const JPtrArray<JString>& pathList1,
							const JPtrArray<JString>& pathList2,
							const JBoolean assumeAutoGen,
							JArray<HeaderDep>* headerList);
JBoolean	GetNextIncludedFile(const JCharacter* inputFileName, std::istream& input,
								const JPtrArray<JString>& pathList1,
								const JPtrArray<JString>& pathList2,
								const JBoolean assumeAutoGen,
								JString* fileName);
void		TruncateMakefile(const JCharacter* fileName);

JOrderedSetT::CompareResult CompareHeaderFiles(const HeaderDep& h1, const HeaderDep& h2);

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
	// we don't bother to clean up at the end

	(JMemoryManager::Instance())->CancelRecordAllocated();
	(JMemoryManager::Instance())->CancelRecordDeallocated();
	(JMemoryManager::Instance())->SetPrintExitStats(kJFalse);

	// apply environment variables

	const JCharacter* env = getenv("J_MAKEMAKE_IGNORE_PATTERN");
	if (!JStringEmpty(env))
		{
		globalIgnorePattern = jnew JRegex(env);
		assert( globalIgnorePattern != NULL );
		}

	// parse command line arguments

	JString outputDirName;
	JBoolean searchSysDir  = kJTrue;
	JBoolean assumeAutoGen = kJFalse;

	JIndex i = startArg;

	const JCharacter* makefileName = argv[i];
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
			searchSysDir = kJFalse;
			}
		else if (strcmp(argv[i], kAutoGenArg) == 0)
			{
			assumeAutoGen = kJTrue;
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

	JBoolean searchCurrDir = kJTrue;

	while (i < argc && strcmp(argv[i], "--") != 0)
		{
		if (strcmp(argv[i], "-nostdinc") == 0)
			{
			searchSysDir = kJFalse;
			}
		else if (strcmp(argv[i], "-I-") == 0)
			{
			searchCurrDir = kJFalse;
			}
		else if (argv[i][0] == '-' && argv[i][1] == 'I' &&
				 argv[i][2] != '\0' && argv[i][2] != '-')
			{
			if (JDirectoryReadable(argv[i]+2))
				{
				path = jnew JString(argv[i]+2);		// strip off "-I"
				assert( path != NULL );
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
		pathList2.CopyPointers(pathList1, JPtrArrayT::kForgetAll, kJFalse);
		pathList1.Prepend(kCurrentDir);
		}

	if (searchSysDir)
		{
		pathList2.Append(kSysIncludeDir);
		}

	// append dependencies to input file

	TruncateMakefile(makefileName);

	std::ofstream output(makefileName, std::ios::app);
	output << '\n';

	JArray<HeaderDep> headerList;		// header files that have been processed
	headerList.SetCompareFunction(CompareHeaderFiles);

#if USE_TEMP_FILE_FOR_DEPEND

	std::ifstream input(argv[i]);

	JString fileName, makeName;
	while (1)
		{
		fileName = JReadLine(input);
		if (input.eof() || input.fail())
			{
			break;
			}
		makeName = JReadLine(input);
		WriteDependencies(output, fileName, makeName, pathList1, pathList2,
						  assumeAutoGen, outputDirName, &headerList);
		}

#else

	for ( ; i<argc; i+=2)
		{
		WriteDependencies(output, argv[i], argv[i+1], pathList1, pathList2,
						  assumeAutoGen, outputDirName, &headerList);
		}

#endif

#if ! ALLOW_INCLUDE_LOOPS

	const JSize headerCount = headerList.GetElementCount();
	for (i=1; i<=headerCount; i++)
		{
		const HeaderDep info = headerList.GetElement(i);
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
	std::ostream&					output,
	const JCharacter*			fileName,
	const JCharacter*			makeName,
	const JPtrArray<JString>&	pathList1,
	const JPtrArray<JString>&	pathList2,
	const JBoolean				assumeAutoGen,
	const JCharacter*			outputDirName,
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

	std::ifstream input(fileName);
	JString headerName;
	while (GetNextIncludedFile(fileName, input, pathList1, pathList2, assumeAutoGen, &headerName))
		{
		AddDependency(&depList, headerName, pathList1, pathList2, assumeAutoGen, headerList);
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
	std::ostream&					output,
	const JCharacter*			outputDirName,
	const JCharacter*			makeName,
	const JPtrArray<JString>&	depList
	)
{
	if (!depList.IsEmpty())
		{
		if (!JStringEmpty(outputDirName))
			{
			JString s = makeName, p,f;
			JSplitPathAndName(s, &p, &f);
			output << "${" << outputDirName << "}/";
			f.Print(output);
			}
		else
			{
			output << makeName;
			}

		output << ':';

		const JSize depCount = depList.GetElementCount();
		for (JIndex i=1; i<=depCount; i++)
			{
			output << ' ';
			(depList.NthElement(i))->Print(output);
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
	const JBoolean				assumeAutoGen,
	JArray<HeaderDep>*			headerList,
	const JBoolean				addToDepList
	)
{
	const HeaderDep info = ParseHeaderFile(headerName, pathList1, pathList2, assumeAutoGen, headerList);

	JBoolean isDuplicate;
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
		const JBoolean addSubToDepList = kJTrue;
		#else
		const JBoolean addSubToDepList = kJFalse;
		#endif

		const JSize count = (info.depList)->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JString* includedFileName = (info.depList)->NthElement(i);

			#if ! ALLOW_INCLUDE_LOOPS

			// check if it already exists

			HeaderDep info(const_cast<JString*>(includedFileName), NULL);
			JIndex j;
			if (headerList->SearchSorted(info, JOrderedSetT::kAnyMatch, &j))
				{
				continue;
				}

			#endif

			AddDependency(depList, *includedFileName,
						  pathList1, pathList2, assumeAutoGen,
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
	const JBoolean				assumeAutoGen,
	JArray<HeaderDep>*			headerList
	)
{
	// check if it already exists

	HeaderDep info(const_cast<JString*>(&fileName), (JPtrArray<JString>*) NULL);
	JBoolean found;
	const JIndex index =
		headerList->SearchSorted1(info, JOrderedSetT::kAnyMatch, &found);
	if (found)
		{
		return headerList->GetElement(index);
		}

	// add it to headerList

	info.fileName = jnew JString(fileName);
	assert( info.fileName != NULL );

	info.depList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( info.depList != NULL );
	(info.depList)->SetCompareFunction(JCompareStringsCaseSensitive);

	headerList->InsertElementAtIndex(index, info);

	// extract dependencies only from non-system header files

	if (fileName.BeginsWith(kSysIncludeDir))
		{
		return info;
		}

	// find the files that fileName depends on and add them to depList

	std::ifstream input(fileName);
	JString headerName;
	while (GetNextIncludedFile(fileName, input, pathList1, pathList2, assumeAutoGen, &headerName))
		{
		JBoolean isDuplicate;
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
	Returns kJFalse if there are no more.

 ******************************************************************************/

static const JCharacter* kIncludeMarker = "include";
const JSize kIncludeMarkerLength        = strlen(kIncludeMarker);

JBoolean
GetNextIncludedFile
	(
	const JCharacter*			inputFileName,
	std::istream&					input,
	const JPtrArray<JString>&	pathList1,
	const JPtrArray<JString>&	pathList2,
	const JBoolean				assumeAutoGen,
	JString*					fileName
	)
{
	while (!input.eof() && !input.fail())
		{
		input >> std::ws;
		if (input.peek() != '#')
			{
			JIgnoreLine(input);
			}
		else
			{
			input.ignore();
			input >> std::ws;
			JString line = JReadLine(input);
			if (line.BeginsWith(kIncludeMarker))
				{
				line.RemoveSubstring(1, kIncludeMarkerLength);
				line.TrimWhitespace();

				JCharacter c = line.GetFirstCharacter();
				if (c == '"' || c == '<')
					{
					if (c == '<')
						{
						c = '>';
						}

					const JSize length = line.GetLength();
					JIndex i = 2;
					while (i <= length && line.GetCharacter(i) != c)
						{
						i++;
						}

					if (i <= length)
						{
						const JString name = line.GetSubstring(2, i-1);
						if (globalIgnorePattern != NULL &&
							globalIgnorePattern->Match(name))
							{
							// skip
							}
						else if ((c == '"' && FindFile(name, pathList1, fileName)) ||
								 FindFile(name, pathList2, fileName))
							{
							return kJTrue;
							}
						else if (c == '"' && assumeAutoGen)	// assume in same dir as including file
							{
							JString p, n;
							if (!JSplitPathAndName(inputFileName, &p, &n))
								{
								p = "./";
								}
							*fileName = JCombinePathAndName(p, name);
							return kJTrue;
							}
						}
					}
				}
			}
		}

	fileName->Clear();
	return kJFalse;
}

/******************************************************************************
 FindFile

	Search for the file in the given list of directories.

 ******************************************************************************/

JBoolean
FindFile
	(
	const JCharacter*			fileName,
	const JPtrArray<JString>&	pathList,
	JString*					fullName
	)
{
	// if full path is specified, use it

	if (fileName[0] == '/' ||
		JCompareMaxN(fileName, "./", 2, kJTrue) ||
		JCompareMaxN(fileName, "../", 3, kJTrue))
		{
		if (JFileExists(fileName))
			{
			*fullName = fileName;
			return kJTrue;
			}
		else
			{
			fullName->Clear();
			return kJFalse;
			}
		}

	// otherwise, check search paths

	const JSize count = pathList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* path = pathList.NthElement(i);
		*fullName           = JCombinePathAndName(*path, fileName);
		if (JFileExists(*fullName))
			{
			return kJTrue;
			}
		}

	fullName->Clear();
	return kJFalse;
}

/******************************************************************************
 TruncateMakefile

	Chop off old dependency list.

 ******************************************************************************/

void
TruncateMakefile
	(
	const JCharacter* fileName
	)
{
	std::fstream f(fileName, kJTextFile);

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

JOrderedSetT::CompareResult
CompareHeaderFiles
	(
	const HeaderDep& h1,
	const HeaderDep& h2
	)
{
	const int r = JStringCompare(*(h1.fileName), *(h2.fileName), kJTrue);

	if (r > 0)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}
