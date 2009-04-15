/******************************************************************************
 jxlayout.cpp

	Program to generate JX code from an fdesign .fd file.

	Copyright © 1996-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jXGlobals.h>
#include <JXLibVersion.h>
#include <JPtrArray-JString.h>
#include <JRegex.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jProcessUtil.h>
#include <jCommandLine.h>
#include <jVCSUtil.h>
#include <JRect.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jAssert.h>

// Constants

static const JCharacter* kVersionStr =

	"jxlayout 2.1.1\n"
	"\n"
	"For use with JX 2.5.0\n"
	"\n"
	"Copyright © 1996-2006 New Planet Software, Inc.  All rights reserved.\n"
	"\n"
	"http://www.newplanetsoftware.com/jx/\n"
	"jx@newplanetsoftware.com";

static const JCharacter* kBackupSuffix = "~";

static const JCharacter* kBeginCodeDelimiterPrefix = "// begin ";
static const JCharacter* kEndCodeDelimiterPrefix   = "// end ";

static const JCharacter* kDefaultDelimTag = "JXLayout";
static const JCharacter* kCustomTagMarker = "--";
const JSize kCustomTagMarkerLength        = strlen(kCustomTagMarker);

static const JCharacter* kDefTopEnclVarName = "window";

// Data files (search for at startup)

static const JCharacter* kMainConfigFileDir    = CONFIG_FILE_DIR;
static const JCharacter* kEnvUserConfigFileDir = "JXLAYOUTDIR";

static JString classMapFile       = "class_map";
static JString optionMapFile      = "option_map";
static JString needFontListFile   = "need_font_list";
static JString needCreateListFile = "need_create_list";

// Form fields

static const JCharacter* kBeginFormLine      = "=============== FORM ===============";
static const JCharacter* kFormNameMarker     = "Name:";
static const JCharacter* kFormWidthMarker    = "Width:";
static const JCharacter* kFormHeightMarker   = "Height:";
static const JCharacter* kFormObjCountMarker = "Number of Objects:";

// Object fields

static const JCharacter* kBeginObjLine      = "--------------------";
static const JCharacter* kObjClassMarker    = "class:";
static const JCharacter* kObjTypeMarker     = "type:";
static const JCharacter* kObjRectMarker     = "box:";
static const JCharacter* kObjBoxTypeMarker  = "boxtype:";
static const JCharacter* kObjColorsMarker   = "colors:";
static const JCharacter* kObjLAlignMarker   = "alignment:";
static const JCharacter* kObjLStyleMarker   = "style:";
static const JCharacter* kObjLSizeMarker    = "size:";
static const JCharacter* kObjLColorMarker   = "lcol:";
static const JCharacter* kObjLabelMarker    = "label:";
static const JCharacter* kObjShortcutMarker = "shortcut:";
static const JCharacter* kObjResizeMarker   = "resize:";
static const JCharacter* kObjGravityMarker  = "gravity:";
static const JCharacter* kObjNameMarker     = "name:";
static const JCharacter* kObjCallbackMarker = "callback:";
static const JCharacter* kObjCBArgMarker    = "argument:";

// Options for each object

const JSize kOptionCount = 3;

enum
{
	kShortcutsIndex = 1,
	kCol1Index,
	kCol2Index
};

// font size conversion

struct FontSizeConversion
{
	const JCharacter* flSize;
	const JCharacter* jxSize;
};

static const FontSizeConversion kFontSizeTable[] =
{
	{"FL_DEFAULT_SIZE", "10"},
	{"FL_TINY_SIZE",    "8"},
	{"FL_SMALL_SIZE",   "10"},
	{"FL_NORMAL_SIZE",  "kJDefaultFontSize"},
	{"FL_MEDIUM_SIZE",  "14"},
	{"FL_LARGE_SIZE",   "18"},
	{"FL_HUGE_SIZE",    "24"}
};

const JSize kFontSizeTableSize = sizeof(kFontSizeTable)/sizeof(FontSizeConversion);

// color conversion

struct ColorConversion
{
	const JCharacter* flColor;
	const JCharacter* jxColor;
};

static const ColorConversion kColorTable[] =
{
	{"FL_BLACK",        "(GetColormap())->GetBlackColor()"},
	{"FL_RED",          "(GetColormap())->GetRedColor()"},
	{"FL_GREEN",        "(GetColormap())->GetGreenColor()"},
	{"FL_YELLOW",       "(GetColormap())->GetYellowColor()"},
	{"FL_BLUE",         "(GetColormap())->GetBlueColor()"},
	{"FL_MAGENTA",      "(GetColormap())->GetMagentaColor()"},
	{"FL_CYAN",         "(GetColormap())->GetCyanColor()"},
	{"FL_WHITE",        "(GetColormap())->GetWhiteColor()"},
	{"FL_LCOL",         "(GetColormap())->GetBlackColor()"},
	{"FL_COL1",         "(GetColormap())->GetDefaultBackColor()"},
	{"FL_MCOL",         "(GetColormap())->GetDefaultFocusColor()"},
	{"FL_RIGHT_BCOL",   "(GetColormap())->Get3DShadeColor()"},
	{"FL_BOTTOM_BCOL",  "(GetColormap())->Get3DShadeColor()"},
	{"FL_TOP_BCOL",     "(GetColormap())->Get3DLightColor()"},
	{"FL_LEFT_BCOL",    "(GetColormap())->Get3DLightColor()"},
	{"FL_INACTIVE",     "(GetColormap())->GetInactiveLabelColor()"},
	{"FL_INACTIVE_COL", "(GetColormap())->GetInactiveLabelColor()"}
};

const JSize kColorTableSize = sizeof(kColorTable)/sizeof(ColorConversion);

// Prototypes

void GenerateForm(istream& input, const JString& formName,
				  const JString& tagName, const JString& windArgs,
				  const JString& enclName,
				  const JString& codePath, const JString& stringPath,
				  const JString& codeSuffix, const JString& headerSuffix,
				  JPtrArray<JString>* backupList);
JBoolean ShouldGenerateForm(const JString& form, const JPtrArray<JString>& list);
JBoolean ShouldBackupForm(const JString& form, JPtrArray<JString>* list);
void GenerateCode(istream& input, ostream& output, const JString& stringPath,
				  const JString& formName, const JString& tagName,
				  const JString& windowConstrArgs, const JString& userTopEnclVarName,
				  JPtrArray<JString>* objTypes, JPtrArray<JString>* objNames);
void GenerateHeader(ostream& output, const JPtrArray<JString>& objTypes,
					const JPtrArray<JString>& objNames);

JBoolean ParseGravity(const JString& gravity, JString* hSizing, JString* vSizing);
void GetTempVarName(const JCharacter* tagName, JString* varName,
					const JPtrArray<JString>& objNames);
JBoolean GetEnclosure(const JArray<JRect>& rectList, const JIndex rectIndex, JIndex* enclIndex);
JBoolean GetConstructor(const JString& flClass, const JString& flType,
						JString* label, JString* className, JString* argList);
JBoolean SplitClassNameAndArgs(const JString& str, JString* className, JString* args);
void ApplyOptions(ostream& output, const JString& className,
				  const JString& formName, const JString& tagName, const JString& varName,
				  const JPtrArray<JString>& values, const JString& flSize,
				  const JString& flStyle, const JString& flColor,
				  JStringManager* stringMgr);
JBoolean AcceptsFontSpec(const JString& className);
JBoolean NeedsCreateFunction(const JString& className);
JBoolean FindClassName(const JString& fileName, const JString& className);
JBoolean ConvertXFormsFontSize(const JString& flSize, JString* jxSize);
JBoolean ConvertXFormsColor(const JString& flColor, JString* jxColor);

JBoolean CopyBeforeCodeDelimiter(const JString& tag, istream& input, ostream& output);
JBoolean CopyAfterCodeDelimiter(const JString& tag, istream& input, ostream& output);
void RemoveIdentifier(const JCharacter* id, JString* line);

void GetOptions(const JSize argc, char* argv[], JString* inputName,
				JString* codePath, JString* stringPath,
				JString* codeSuffix, JString* headerSuffix,
				JString* postCmd,
				JPtrArray<JString>* userFormList);
void PickForms(const JString& fileName, JPtrArray<JString>* list);
JBoolean FindConfigFile(JString* configFileName);

void PrintHelp(const JString& codeSuffix, const JString& headerSuffix,
			   const JString& stringPath);
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
	// find the configuration files

	if (!FindConfigFile(&classMapFile) ||
		!FindConfigFile(&optionMapFile) ||
		!FindConfigFile(&needFontListFile) ||
		!FindConfigFile(&needCreateListFile))
		{
		return 1;
		}

	// parse the command line options

	JString inputName, codePath, stringPath, codeSuffix, headerSuffix;
	JString postCmd;
	JPtrArray<JString> userFormList(JPtrArrayT::kDeleteAll);	// empty => generate all forms
	JPtrArray<JString> backupList(JPtrArrayT::kDeleteAll);		// forms that have been backed up
	GetOptions(argc, argv, &inputName, &codePath, &stringPath,
			   &codeSuffix, &headerSuffix, &postCmd, &userFormList);

	// generate each requested form

	JBoolean changed = kJFalse;

	ifstream input(inputName);
	while (!input.eof() && !input.fail())
		{
		const JString line = JReadLine(input);
		if (line == kBeginFormLine)
			{
			// get form name

			JString formName = JReadLine(input);
			RemoveIdentifier(kFormNameMarker, &formName);

			// look for custom tag

			const JSize formNameLength = formName.GetLength();
			JString tagName            = kDefaultDelimTag;
			JString windArgs, enclName;
			JIndex tagMarkerIndex;
			if (formName.LocateSubstring(kCustomTagMarker, &tagMarkerIndex) &&
				tagMarkerIndex <= formNameLength - kCustomTagMarkerLength)
				{
				tagName = formName.GetSubstring(
					tagMarkerIndex + kCustomTagMarkerLength, formNameLength);
				formName.RemoveSubstring(tagMarkerIndex, formNameLength);

				// get enclosure name

				const JSize tagNameLength = tagName.GetLength();
				JIndex enclMarkerIndex;
				if (tagName.LocateSubstring(kCustomTagMarker, &enclMarkerIndex) &&
					enclMarkerIndex <= tagNameLength - kCustomTagMarkerLength)
					{
					enclName = tagName.GetSubstring(
						enclMarkerIndex + kCustomTagMarkerLength, tagNameLength);
					tagName.RemoveSubstring(enclMarkerIndex, tagNameLength);
					}

				// get window constructor arguments

				JIndex windArgsIndex;
				if (tagName.LocateSubstring("<", &windArgsIndex) &&
					tagName.GetLastCharacter() == '>')
					{
					const JSize tagLength = tagName.GetLength();
					if (windArgsIndex < tagLength-1)
						{
						windArgs = tagName.GetSubstring(windArgsIndex+1, tagLength-1);
						}
					tagName.RemoveSubstring(windArgsIndex, tagLength);
					}

				// report errors

				if (tagName != kDefaultDelimTag)
					{
					if (enclName.IsEmpty())
						{
						cerr << formName << ", " << tagName;
						cerr << ": no enclosure specified" << endl;
						}
					else if (!windArgs.IsEmpty())
						{
						cerr << formName << ", " << tagName;
						cerr << ": no window to which to send \"" << windArgs << '"' << endl;
						}
					}
				else if (!enclName.IsEmpty() && enclName != kDefTopEnclVarName)
					{
					cerr << formName << ", " << tagName;
					cerr << ": not allowed to specify enclosure other than ";
					cerr << kDefTopEnclVarName << endl;
					}
				}

			if (ShouldGenerateForm(formName, userFormList))
				{
				GenerateForm(input, formName, tagName, windArgs, enclName,
							 codePath, stringPath, codeSuffix, headerSuffix, &backupList);
				changed = kJTrue;
				}
			}
		}

	if (changed && !postCmd.IsEmpty())
		{
		const JError err = JExecute(postCmd, NULL);
		err.ReportIfError();
		}

	return 0;
}

/******************************************************************************
 ShouldGenerateForm

	Returns kJTrue if form is in the list.  If the list is empty, it means
	"make all forms" so we just return kJTrue.

 ******************************************************************************/

JBoolean
ShouldGenerateForm
	(
	const JString&				form,
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
		if (form == *(list.NthElement(i)))
			{
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 GenerateForm

 ******************************************************************************/

void
GenerateForm
	(
	istream&			input,
	const JString&		formName,
	const JString&		tagName,
	const JString&		windArgs,
	const JString&		enclName,
	const JString&		codePath,
	const JString&		stringPath,
	const JString&		codeSuffix,
	const JString&		headerSuffix,
	JPtrArray<JString>*	backupList
	)
{
	const JString codeFileName    = codePath + formName + codeSuffix;
	const JString codeFileBakName = codeFileName + kBackupSuffix;

	const JString headerFileName    = codePath + formName + headerSuffix;
	const JString headerFileBakName = headerFileName + kBackupSuffix;

	if (!JFileExists(codeFileName))
		{
		cerr << codeFileName << " not found" << endl;
		return;
		}
	if (!JFileExists(headerFileName))
		{
		cerr << headerFileName << " not found" << endl;
		return;
		}

	cout << "Generating: " << formName << ", " << tagName << endl;

	const JBoolean shouldBackup = ShouldBackupForm(formName, backupList);

	// copy code file contents before start delimiter

	JString tempCodeFileName;
	JError err = JCreateTempFile(codePath, NULL, &tempCodeFileName);
	if (!err.OK())
		{
		cerr << "Unable to create temporary file in " << codePath << endl;
		cerr << "  (" << err.GetMessage() << ')' << endl;
		return;
		}

	ifstream origCode(codeFileName);
	ofstream outputCode(tempCodeFileName);
	if (!outputCode.good())
		{
		cerr << "Unable to open temporary file in " << codePath << endl;
		remove(tempCodeFileName);
		return;
		}
	if (!CopyBeforeCodeDelimiter(tagName, origCode, outputCode))
		{
		cerr << "No starting delimiter in " << codeFileName << endl;
		outputCode.close();
		remove(tempCodeFileName);
		return;
		}

	// generate code for each object in the form

	JPtrArray<JString> objTypes(JPtrArrayT::kDeleteAll),
					   objNames(JPtrArrayT::kDeleteAll);
	GenerateCode(input, outputCode, stringPath, formName, tagName, windArgs, enclName,
				 &objTypes, &objNames);

	// copy code file contents after end delimiter

	JBoolean done = CopyAfterCodeDelimiter(tagName, origCode, outputCode);
	origCode.close();
	outputCode.close();

	if (!done)
		{
		cerr << "No ending delimiter in " << codeFileName << endl;
		remove(tempCodeFileName);
		return;
		}
	else if (shouldBackup && rename(codeFileName, codeFileBakName) != 0)
		{
		cerr << "Unable to rename original " << codeFileName << endl;
		remove(tempCodeFileName);
		return;
		}
	JEditVCS(codeFileName);
	rename(tempCodeFileName, codeFileName);

	// copy header file contents before start delimiter

	JString tempHeaderFileName;
	err = JCreateTempFile(codePath, NULL, &tempHeaderFileName);
	if (!err.OK())
		{
		cerr << "Unable to create temporary file in " << codePath << endl;
		cerr << "  (" << err.GetMessage() << ')' << endl;
		return;
		}

	ifstream origHeader(headerFileName);
	ofstream outputHeader(tempHeaderFileName);
	if (!outputHeader.good())
		{
		cerr << "Unable to open temporary file in " << codePath << endl;
		remove(tempHeaderFileName);
		return;
		}
	if (!CopyBeforeCodeDelimiter(tagName, origHeader, outputHeader))
		{
		cerr << "No starting delimiter in " << headerFileName << endl;
		outputHeader.close();
		remove(tempHeaderFileName);
		return;
		}

	// generate instance variable for each object in the form

	GenerateHeader(outputHeader, objTypes, objNames);

	// copy header file contents after end delimiter

	done = CopyAfterCodeDelimiter(tagName, origHeader, outputHeader);
	origHeader.close();
	outputHeader.close();

	if (!done)
		{
		cerr << "No ending delimiter in " << headerFileName << endl;
		remove(tempHeaderFileName);
		return;
		}

	// check if header file actually changed

	JString origHeaderText, newHeaderText;
	JReadFile(headerFileName, &origHeaderText);
	JReadFile(tempHeaderFileName, &newHeaderText);
	if (newHeaderText != origHeaderText)
		{
		if (shouldBackup && rename(headerFileName, headerFileBakName) != 0)
			{
			cerr << "Unable to rename original " << headerFileName << endl;
			remove(tempHeaderFileName);
			return;
			}
		JEditVCS(headerFileName);
		rename(tempHeaderFileName, headerFileName);
		}
	else
		{
		remove(tempHeaderFileName);
		}
}

/******************************************************************************
 ShouldBackupForm

	If form is not in the list, adds it and returns kJTrue.

 ******************************************************************************/

JBoolean
ShouldBackupForm
	(
	const JString&		form,
	JPtrArray<JString>*	list
	)
{
	const JSize count = list->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (form == *(list->NthElement(i)))
			{
			return kJFalse;
			}
		}

	// falling through means that the form hasn't been backed up yet

	JString* newForm = new JString(form);
	assert( newForm != NULL );
	list->Append(newForm);
	return kJTrue;
}

/******************************************************************************
 GenerateCode

 ******************************************************************************/

void
GenerateCode
	(
	istream&			input,
	ostream&			output,
	const JString&		stringPath,
	const JString&		formName,
	const JString&		tagName,
	const JString&		windowConstrArgs,
	const JString&		userTopEnclVarName,
	JPtrArray<JString>*	objTypes,
	JPtrArray<JString>*	objNames
	)
{
JIndex i;

	// width

	input >> ws;
	JString line = JReadUntilws(input);
	assert( line == kFormWidthMarker );
	JSize formWidth;
	input >> formWidth;

	// height

	input >> ws;
	line = JReadUntilws(input);
	assert( line == kFormHeightMarker );
	JSize formHeight;
	input >> formHeight;

	// object count (marker contains whitespace)

	input >> ws;
	line = JReadUntil(input, ':') + ":";
	assert( line == kFormObjCountMarker );
	JSize itemCount;
	input >> itemCount;

	// create window

	const JString topEnclFrameName = tagName + "_Frame";
	const JString topEnclApName    = tagName + "_Aperture";

	JString topEnclVarName;
	if (tagName == kDefaultDelimTag)
		{
		topEnclVarName = kDefTopEnclVarName;

		output << "    JXWindow* window = new JXWindow(this, ";
		output << formWidth << ',' << formHeight;
		if (!windowConstrArgs.IsEmpty())
			{
			output << ", ";
			windowConstrArgs.Print(output);
			}
		else
			{
			output << ", \"\"";
			}
		output << ");" << endl;
		output << "    assert( window != NULL );" << endl;
		output << "    SetWindow(window);" << endl;
		output << endl;
		}
	else
		{
		assert( !userTopEnclVarName.IsEmpty() );
		topEnclVarName = userTopEnclVarName;

		output << "    const JRect ";
		topEnclFrameName.Print(output);
		output << "    = ";
		topEnclVarName.Print(output);
		output << "->GetFrame();" << endl;

		output << "    const JRect ";
		topEnclApName.Print(output);
		output << " = ";
		topEnclVarName.Print(output);
		output << "->GetAperture();" << endl;

		output << "    ";
		topEnclVarName.Print(output);
		output << "->AdjustSize(" << formWidth << " - ";
		topEnclApName.Print(output);
		output << ".width(), " << formHeight << " - ";
		topEnclApName.Print(output);
		output << ".height());" << endl;

		output << endl;
		}

	// We need to calculate the enclosure for each object.  Since objects
	// are drawn in the order added, an object must come after its enclosure
	// in the list in order to be visible.

	JArray<JRect>    rectList(10);
	JArray<JBoolean> isInstanceVar(10);

	// This array is used to send the options to ApplyOptions.
	// It does not own the pointers that it contains.

	JPtrArray<JString> optionValues(JPtrArrayT::kForgetAll, kOptionCount);
	for (i=1; i<=kOptionCount; i++)
		{
		optionValues.Append(NULL);
		}

	// generate code for each object

	JStringManager stringMgr;

	JIndex objCount = 1;
	for (i=1; i<=itemCount; i++)
		{
		// check for start-of-object

		input >> ws;
		line = JReadLine(input);
		assert( line == kBeginObjLine );

		// object class

		JString flClass = JReadLine(input);
		RemoveIdentifier(kObjClassMarker, &flClass);

		// object type

		JString flType = JReadLine(input);
		RemoveIdentifier(kObjTypeMarker, &flType);

		// object frame

		input >> ws;
		line = JReadUntilws(input);
		assert( line == kObjRectMarker );
		JCoordinate x,y,w,h;
		input >> x >> y >> w >> h >> ws;
		const JRect frame(y, x, y+h, x+w);
		rectList.AppendElement(frame);

		// box type

		JString boxType = JReadLine(input);
		RemoveIdentifier(kObjBoxTypeMarker, &boxType);

		// colors

		input >> ws;
		line = JReadUntilws(input);
		assert( line == kObjColorsMarker );
		JString col1 = JReadUntilws(input);
		optionValues.SetElement(kCol1Index, &col1, JPtrArrayT::kForget);
		JString col2 = JReadUntilws(input);
		optionValues.SetElement(kCol2Index, &col2, JPtrArrayT::kForget);

		// label info

		JString lAlign = JReadLine(input);
		RemoveIdentifier(kObjLAlignMarker, &lAlign);
		JString lStyle = JReadLine(input);
		RemoveIdentifier(kObjLStyleMarker, &lStyle);
		JString lSize  = JReadLine(input);
		RemoveIdentifier(kObjLSizeMarker, &lSize);
		JString lColor = JReadLine(input);
		RemoveIdentifier(kObjLColorMarker, &lColor);
		JString label  = JReadLine(input);
		RemoveIdentifier(kObjLabelMarker, &label);

		// shortcuts

		JString shortcuts = JReadLine(input);
		RemoveIdentifier(kObjShortcutMarker, &shortcuts);
		optionValues.SetElement(kShortcutsIndex, &shortcuts, JPtrArrayT::kForget);

		// resizing (ignored)

		JIgnoreLine(input);

		// gravity

		input >> ws;
		line = JReadUntilws(input);
		assert( line == kObjGravityMarker );
		const JString nwGravity = JReadUntilws(input);
		const JString seGravity = JReadUntilws(input);

		// variable name

		JBoolean isLocal = kJFalse;
		JString* varName = new JString(JReadLine(input));
		assert( varName != NULL );
		RemoveIdentifier(kObjNameMarker, varName);
		if (varName->IsEmpty())
			{
			isInstanceVar.AppendElement(kJFalse);
			GetTempVarName(tagName, varName, *objNames);
			isLocal = kJTrue;
			}
		else if (varName->GetFirstCharacter() == '(' &&
				 varName->GetLastCharacter()  == ')')
			{
			isInstanceVar.AppendElement(kJFalse);
			isLocal  = kJTrue;
			*varName = varName->GetSubstring(2, varName->GetLength()-1);
			}
		else if (varName->GetFirstCharacter() == '<' &&
				 varName->GetLastCharacter()  == '>')
			{
			isInstanceVar.AppendElement(kJFalse);
			*varName = varName->GetSubstring(2, varName->GetLength()-1);
			}
		else
			{
			isInstanceVar.AppendElement(kJTrue);
			}
		objNames->Append(varName);

		// callback (ignored)

		JIgnoreLine(input);

		// callback argument

		JString cbArg = JReadLine(input);
		RemoveIdentifier(kObjCBArgMarker, &cbArg);

		// don't bother to generate code for initial box
		// if it is FL_BOX, FLAT_BOX, FL_COL1

		if (i==1 && flClass == "FL_BOX" && flType == "FLAT_BOX" && col1 == "FL_COL1")
			{
			rectList.RemoveElement(objCount);
			isInstanceVar.RemoveElement(objCount);
			objNames->DeleteElement(objCount);
			continue;
			}

		// check for errors -- safe since we have read in entire object

		JString hSizing, vSizing;
		if (!ParseGravity(nwGravity, &hSizing, &vSizing))
			{
			cerr << "Illegal sizing specification ";
			cerr << nwGravity << ',' << seGravity;
			cerr << " for '" << *varName << '\'' << endl;
			rectList.RemoveElement(objCount);
			isInstanceVar.RemoveElement(objCount);
			objNames->DeleteElement(objCount);
			continue;
			}

		if (*varName == topEnclVarName)
			{
			cerr << "Cannot use reserved name '" << topEnclVarName << '\'' << endl;
			rectList.RemoveElement(objCount);
			isInstanceVar.RemoveElement(objCount);
			objNames->DeleteElement(objCount);
			continue;
			}

		// get the object's enclosure

		JIndex enclIndex;
		JString enclName;
		JRect localFrame = frame;
		if (GetEnclosure(rectList, objCount, &enclIndex))
			{
			enclName = *(objNames->NthElement(enclIndex));
			const JRect enclFrame = rectList.GetElement(enclIndex);
			localFrame.Shift(-enclFrame.topLeft());
			}
		else
			{
			enclName = topEnclVarName;
			}

		// get the class name and additional arguments

		JString* className = new JString;
		assert( className != NULL );
		objTypes->Append(className);

		JString argList;
		if (!GetConstructor(flClass, flType, &label, className, &argList))
			{
			cerr << "Unsupported class: " << flClass << ", " << flType << endl;
			rectList.RemoveElement(objCount);
			isInstanceVar.RemoveElement(objCount);
			objNames->DeleteElement(objCount);
			objTypes->DeleteElement(objCount);
			continue;
			}

		// generate the actual code

		const JBoolean needCreate = NeedsCreateFunction(*className);

		output << "    ";
		if (isLocal)
			{
			className->Print(output);
			output << "* ";
			}
		varName->Print(output);
		output << " =" << endl;
		output << "        ";
		if (!needCreate)
			{
			output << "new ";
			}
		className->Print(output);
		if (needCreate)
			{
			output << "::Create";
			}
		output << '(';
		if (!argList.IsEmpty())
			{
			argList.Print(output);
			if (argList.GetLastCharacter() != ',')
				{
				output << ',';
				}
			output << ' ';
			}

		if (!cbArg.IsEmpty())
			{
			cbArg.Print(output);
			if (cbArg.GetLastCharacter() != ',')
				{
				output << ',';
				}
			output << ' ';
			}

		if ((*className == "JXStaticText" && cbArg.IsEmpty()) ||
			*className == "JXTextButton"      ||
			*className == "JXTextCheckbox"    ||
			*className == "JXTextRadioButton" ||
			*className == "JXTextMenu")
			{
			JString id = *varName;
			id += "::";
			id += formName;
			id += "::";
			id += tagName;		// last since it is almost always the same

			output << "JGetString(\"";
			id.Print(output);
			output << "\"), ";

			stringMgr.SetElement(id, label, JPtrArrayT::kDelete);
			}

		enclName.Print(output);
		output << ',' << endl;
		output << "                    JXWidget::";
		hSizing.Print(output);
		output << ", JXWidget::";
		vSizing.Print(output);
		output << ", " << localFrame.left << ',' << localFrame.top << ", ";
		output << localFrame.width() << ',' << localFrame.height() << ");" << endl;

		output << "    assert( ";
		varName->Print(output);
		output << " != NULL );" << endl;

		ApplyOptions(output, *className, formName, tagName, *varName, optionValues,
					 lSize, lStyle, lColor, &stringMgr);
		output << endl;

		// now we know the object is valid

		objCount++;
		}

	// write string database

	JString dbFileName = stringPath + formName;
	if (tagName != kDefaultDelimTag)
		{
		dbFileName += kCustomTagMarker + tagName;
		}
	dbFileName += "_layout";

	if (stringMgr.GetElementCount() > 0)
		{
		JEditVCS(dbFileName);
		ofstream dbOutput(dbFileName);
		stringMgr.WriteFile(dbOutput);
		}
	else
		{
		JRemoveVCS(dbFileName);
		JRemoveFile(dbFileName);
		}

	// reset enclosure size

	if (tagName != kDefaultDelimTag)
		{
		output << "    ";
		topEnclVarName.Print(output);
		output << "->SetSize(";
		topEnclFrameName.Print(output);
		output << ".width(), ";
		topEnclFrameName.Print(output);
		output << ".height());" << endl;
		output << endl;
		}

	// throw away temporary variables

	objCount--;
	assert( objCount == isInstanceVar.GetElementCount() );
	assert( objCount == objTypes->GetElementCount() );
	assert( objCount == objNames->GetElementCount() );
	for (i=objCount; i>=1; i--)
		{
		if (!isInstanceVar.GetElement(i))
			{
			objTypes->DeleteElement(i);
			objNames->DeleteElement(i);
			}
		}
}

/******************************************************************************
 GenerateHeader

 ******************************************************************************/

void
GenerateHeader
	(
	ostream&					output,
	const JPtrArray<JString>&	objTypes,
	const JPtrArray<JString>&	objNames
	)
{
	JIndex i;
	const JSize count = objTypes.GetElementCount();
	assert( count == objNames.GetElementCount() );

	// get width of longest type

	JSize maxLen = 0;
	for (i=1; i<=count; i++)
		{
		const JString* type = objTypes.NthElement(i);
		const JSize len     = type->GetLength();
		if (len > maxLen)
			{
			maxLen = len;
			}
		}

	// declare each object

	for (i=1; i<=count; i++)
		{
		output << "    ";
		const JString* type = objTypes.NthElement(i);
		type->Print(output);
		output << '*';
		const JSize len = type->GetLength();
		for (JIndex j=len+1; j<=maxLen+1; j++)
			{
			output << ' ';
			}
		(objNames.NthElement(i))->Print(output);
		output << ';' << endl;
		}

	// need blank line to conform to expectations of CopyAfterCodeDelimiter

	output << endl;
}

/******************************************************************************
 ParseGravity

	Convert X gravity values into JXWidget sizing values.
	Returns kJFalse if user specified an invalid combination.

 ******************************************************************************/

static const JCharacter* kGravityMap[] =
{
	"FL_NoGravity", "kHElastic",   "kVElastic",
	"FL_North",     "kHElastic",   "kFixedTop",
	"FL_NorthEast", "kFixedRight", "kFixedTop",
	"FL_East",      "kFixedRight", "kVElastic",
	"FL_SouthEast", "kFixedRight", "kFixedBottom",
	"FL_South",     "kHElastic",   "kFixedBottom",
	"FL_SouthWest", "kFixedLeft",  "kFixedBottom",
	"FL_West",      "kFixedLeft",  "kVElastic",
	"FL_NorthWest", "kFixedLeft",  "kFixedTop"
};

const JSize kGravityCount = sizeof(kGravityMap) / (3*sizeof(JCharacter*));

JBoolean
ParseGravity
	(
	const JString&	gravity,
	JString*		hSizing,
	JString*		vSizing
	)
{
	for (JIndex i=0; i<kGravityCount; i++)
		{
		const JIndex j = 3*i;
		if (gravity == kGravityMap[j])
			{
			*hSizing = kGravityMap[j+1];
			*vSizing = kGravityMap[j+2];
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetTempVarName

	Return a variable name that is not in the given list.

	We ignore the possibility of not finding a valid name because the
	code we are writing will run out of memory long before we run out
	of possibilities.

 ******************************************************************************/

static const JRegex illegalCChar = "[^_A-Za-z0-9]+";

void
GetTempVarName
	(
	const JCharacter*			tagName,
	JString*					varName,
	const JPtrArray<JString>&	objNames
	)
{
	JString suffix = tagName;
	JIndexRange r;
	while (illegalCChar.Match(suffix, &r))
		{
		suffix.RemoveSubstring(r);
		}
	suffix.PrependCharacter('_');

	const JString prefix = "obj";
	const JSize count    = objNames.GetElementCount();
	for (JIndex i=1; i<=INT_MAX; i++)
		{
		*varName = prefix + JString(i) + suffix;
		JBoolean unique = kJTrue;
		for (JIndex j=1; j<=count; j++)
			{
			const JString* usedName = objNames.NthElement(j);
			if (*varName == *usedName)
				{
				unique = kJFalse;
				break;
				}
			}
		if (unique)
			{
			break;
			}
		}
}

/******************************************************************************
 GetEnclosure

	Returns kJTrue if it finds a rectangle that encloses the rectangle
	at the specified index.  If it finds more than one enclosing rectangle,
	it returns the smallest one.

	If no enclosure is found, returns kJFalse, and sets *enclIndex to zero.

 ******************************************************************************/

JBoolean
GetEnclosure
	(
	const JArray<JRect>&	rectList,
	const JIndex			rectIndex,
	JIndex*					enclIndex
	)
{
	const JRect theRect = rectList.GetElement(rectIndex);
	JBoolean found = kJFalse;
	*enclIndex = 0;

	JSize minArea = 0;
	const JSize count = rectList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (i != rectIndex)
			{
			const JRect r = rectList.GetElement(i);
			const JSize a = r.area();
			if (r.Contains(theRect) && (a < minArea || minArea == 0))
				{
				minArea    = a;
				found      = kJTrue;
				*enclIndex = i;
				}
			}
		}
	return found;
}

/******************************************************************************
 GetConstructor

	Convert the XForms type into a JX object constructor.
	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
GetConstructor
	(
	const JString&	flClass,
	const JString&	flType,
	JString*		label,
	JString*		className,
	JString*		argList
	)
{
	if (flClass == "FL_BOX" && flType == "NO_BOX" && !label->IsEmpty())
		{
		const JBoolean ok = SplitClassNameAndArgs(*label, className, argList);
		label->Clear();
		return ok;
		}

	ifstream classMap(classMapFile);
	classMap >> ws;
	while (1)
		{
		if (classMap.peek() == '#')
			{
			JIgnoreLine(classMap);
			}
		else
			{
			const JString aClass = JReadUntilws(classMap);
			if (classMap.eof() || classMap.fail())
				{
				break;
				}

			const JString aType = JReadUntilws(classMap);
			if (aClass == flClass && (aType == "*" || aType == flType))
				{
				const JString s = JReadUntilws(classMap);
				return SplitClassNameAndArgs(s, className, argList);
				}
			else
				{
				JIgnoreLine(classMap);
				}
			}
		classMap >> ws;
		}

	// falling through means that nothing matched

	return kJFalse;
}

/******************************************************************************
 SplitClassNameAndArgs

	Returns kJFalse if there is no class name.

 ******************************************************************************/

JBoolean
SplitClassNameAndArgs
	(
	const JString&	str,
	JString*		name,
	JString*		args
	)
{
	JIndex i;
	const JBoolean hasArgs = str.LocateSubstring("(", &i);
	if (hasArgs && 1 < i && i < str.GetLength())
		{
		*name = str.GetSubstring(1, i-1);
		*args = str.GetSubstring(i+1, str.GetLength());

		name->TrimWhitespace();
		args->TrimWhitespace();
		return kJTrue;
		}
	else if (hasArgs && i == 1)
		{
		cerr << "No class name in " << str << endl;
		name->Clear();
		args->Clear();
		return kJFalse;
		}
	else
		{
		if (hasArgs)
			{
			*name = str.GetSubstring(1, i-1);
			}
		else
			{
			*name = str;
			}
		name->TrimWhitespace();
		args->Clear();
		return kJTrue;
		}
}

/******************************************************************************
 ApplyOptions

	Apply options to the specified object in an intelligent way.
	(i.e. If the value is the default value, don't explicitly set it.)

 ******************************************************************************/

void
ApplyOptions
	(
	ostream&					output,
	const JString&				className,
	const JString&				formName,
	const JString&				tagName,
	const JString&				varName,
	const JPtrArray<JString>&	values,
	const JString&				flSize,
	const JString&				flStyle,
	const JString&				flColor,
	JStringManager*				stringMgr
	)
{
	ifstream optionMap(optionMapFile);
	optionMap >> ws;
	while (1)
		{
		if (optionMap.peek() == '#')
			{
			JIgnoreLine(optionMap);
			}
		else
			{
			const JString aClassName = JReadUntilws(optionMap);
			if (optionMap.eof() || optionMap.fail())
				{
				break;
				}
			else if (aClassName != className)
				{
				for (JIndex i=1; i<=kOptionCount; i++)
					{
					JIgnoreLine(optionMap);
					}
				}
			else
				{
				JIndex i;
				JBoolean supported;

				// shortcuts

				optionMap >> ws >> supported;
				if (supported)
					{
					optionMap >> ws;
					const JString function = JReadUntilws(optionMap);
					const JString* value   = values.NthElement(kShortcutsIndex);
					if (!value->IsEmpty())
						{
						JString id = varName;
						id += "::";
						id += formName;
						id += "::shortcuts::";
						id += tagName;		// last since it is almost always the same

						output << "    ";
						varName.Print(output);
						output << "->";
						function.Print(output);
						output << "(JGetString(\"";
						id.Print(output);
						output << "\"));" << endl;

						JString* s = new JString(*value);
						assert( s != NULL );
						stringMgr->SetElement(id, s, JPtrArrayT::kDelete);
						}
					}
				else
					{
					JIgnoreLine(optionMap);
					}

				// colors

				for (i=2; i<=kOptionCount; i++)
					{
					optionMap >> ws >> supported;
					if (supported)
						{
						optionMap >> ws;
						const JString defValue = JReadUntilws(optionMap);
						const JString function = JReadUntilws(optionMap);
						const JString* value   = values.NthElement(i);
						if (*value != defValue)
							{
							JString jxColor;
							if (ConvertXFormsColor(*value, &jxColor))
								{
								output << "    ";
								varName.Print(output);
								output << "->";
								function.Print(output);
								output << '(';
								jxColor.Print(output);
								output << ");" << endl;
								}
							else
								{
								cerr << "Unknown color: " << *value << endl;
								}
							}
						}
					else
						{
						JIgnoreLine(optionMap);
						}
					}
				}
			}
		optionMap >> ws;
		}

	// For some objects, we have to decode the XForms font spec.

	if (AcceptsFontSpec(className))
		{
		JString fontName = JGetDefaultFontName();
		if (flStyle.Contains("FIXED"))
			{
			output << "    ";
			varName.Print(output);
			output << "->SetFontName(JXGetCourierFontName());" << endl;
			}
		else if (flStyle.Contains("TIMES"))
			{
			output << "    ";
			varName.Print(output);
			output << "->SetFontName(JXGetTimesFontName());" << endl;
			}

		if (flSize != "FL_NORMAL_SIZE")
			{
			JString jxSize;
			if (ConvertXFormsFontSize(flSize, &jxSize))
				{
				output << "    ";
				varName.Print(output);
				output << "->SetFontSize(";
				jxSize.Print(output);
				output << ");" << endl;
				}
			else
				{
				cerr << "Unknown font size: " << flSize << endl;
				}
			}

		JFontStyle style;
		if (flStyle.Contains("BOLD"))
			{
			style.bold = kJTrue;
			}
		if (flStyle.Contains("ITALIC"))
			{
			style.italic = kJTrue;
			}
		if (style.bold || style.italic || flColor != "FL_BLACK")
			{
			JString jxColor;
			if (ConvertXFormsColor(flColor, &jxColor))
				{
				output << "    const JFontStyle ";
				varName.Print(output);
				output << "_style(";

				if (style.bold)
					{
					output << "kJTrue, ";
					}
				else
					{
					output << "kJFalse, ";
					}

				if (style.italic)
					{
					output << "kJTrue, ";
					}
				else
					{
					output << "kJFalse, ";
					}

				output << "0, kJFalse, ";
				jxColor.Print(output);
				output << ");" << endl;

				output << "    ";
				varName.Print(output);
				output << "->SetFontStyle(";
				varName.Print(output);
				output << "_style);" << endl;
				}
			else
				{
				cerr << "Unknown color: " << flColor << endl;
				}
			}
		}
}

/******************************************************************************
 AcceptsFontSpec

	Returns kJTrue if the given class accepts SetFontName(), SetFontSize(),
	and SetFontStyle().

 ******************************************************************************/

JBoolean
AcceptsFontSpec
	(
	const JString& className
	)
{
	return FindClassName(needFontListFile, className);
}

/******************************************************************************
 NeedsCreateFunction

	Returns kJTrue if the given class needs to constructed via Create().

 ******************************************************************************/

JBoolean
NeedsCreateFunction
	(
	const JString& className
	)
{
	return FindClassName(needCreateListFile, className);
}

/******************************************************************************
 FindClassName

	Searches the specified file for the given class name.

 ******************************************************************************/

JBoolean
FindClassName
	(
	const JString& fileName,
	const JString& className
	)
{
	ifstream list(fileName);
	list >> ws;
	while (!list.eof() && !list.fail())
		{
		if (list.peek() == '#')
			{
			JIgnoreLine(list);
			}
		else
			{
			const JString name = JReadLine(list);
			if (name == className)
				{
				return kJTrue;
				}
			}
		list >> ws;
		}

	// falling through means that nothing matched

	return kJFalse;
}

/******************************************************************************
 ConvertXFormsFontSize

 ******************************************************************************/

JBoolean
ConvertXFormsFontSize
	(
	const JString&	flSize,
	JString*		jxSize
	)
{
	for (JIndex i=0; i<kFontSizeTableSize; i++)
		{
		if (kFontSizeTable[i].flSize == flSize)
			{
			*jxSize = kFontSizeTable[i].jxSize;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 ConvertXFormsColor

 ******************************************************************************/

JBoolean
ConvertXFormsColor
	(
	const JString&	flColor,
	JString*		jxColor
	)
{
	for (JIndex i=0; i<kColorTableSize; i++)
		{
		if (kColorTable[i].flColor == flColor)
			{
			*jxColor = kColorTable[i].jxColor;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 CopyBeforeCodeDelimiter

 ******************************************************************************/

JBoolean
CopyBeforeCodeDelimiter
	(
	const JString&	tag,
	istream&		input,
	ostream&		output
	)
{
	const JString delim = kBeginCodeDelimiterPrefix + tag;
	while (!input.eof() && !input.fail())
		{
		const JString line = JReadLine(input);
		line.Print(output);
		output << '\n';
		if (line == delim)
			{
			output << '\n';
			break;
			}
		}

	return JI2B( !input.eof() && !input.fail() );
}

/******************************************************************************
 CopyAfterCodeDelimiter

	Skips everything before end delimiter and then copies the rest.

 ******************************************************************************/

JBoolean
CopyAfterCodeDelimiter
	(
	const JString&	tag,
	istream&		input,
	ostream&		output
	)
{
	const JString delim = kEndCodeDelimiterPrefix + tag;

	// skip lines before end delimiter

	while (!input.eof() && !input.fail())
		{
		const JString line = JReadLine(input);
		if (line == delim)
			{
			break;
			}
		}

	if (input.eof() || input.fail())
		{
		return kJFalse;
		}

	// include end delimiter

	delim.Print(output);
	output << endl;

	// copy lines after end delimiter

	while (1)
		{
		const JString line = JReadLine(input);
		if ((input.eof() || input.fail()) && line.IsEmpty())
			{
			break;	// avoid creating extra empty lines
			}
		line.Print(output);
		output << endl;
		}

	return kJTrue;
}

/******************************************************************************
 RemoveIdentifier

	Checks that the given id is at the start of the line, and then removes it.

 ******************************************************************************/

void
RemoveIdentifier
	(
	const JCharacter*	id,
	JString*			line
	)
{
	const JSize lineLength = line->GetLength();
	const JSize idLength   = strlen(id);
	assert( lineLength >= idLength && line->GetSubstring(1,idLength) == id );

	line->RemoveSubstring(1,idLength);
	line->TrimWhitespace();
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
	JString*			inputName,
	JString*			codePath,
	JString*			stringPath,
	JString*			codeSuffix,
	JString*			headerSuffix,
	JString*			postCmd,
	JPtrArray<JString>*	userFormList
	)
{
	inputName->Clear();
	codePath->Clear();
	postCmd->Clear();

	*stringPath   = "./strings/";
	*codeSuffix   = ".cc";
	*headerSuffix = ".h";

	JBoolean pickForms = kJFalse;
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
			PrintHelp(*codeSuffix, *headerSuffix, *stringPath);
			exit(0);
			}

		else if (strcmp(argv[index], "-cp") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			*codePath = argv[index];
			JAppendDirSeparator(codePath);
			if (!JDirectoryExists(*codePath))
				{
				cerr << argv[0] << ": specified code path does not exist" << endl;
				exit(1);
				}
			}
		else if (strcmp(argv[index], "-cs") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			*codeSuffix = argv[index];
			}
		else if (strcmp(argv[index], "-hs") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			*headerSuffix = argv[index];
			}

		else if (strcmp(argv[index], "-sp") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			*stringPath = argv[index];
			JAppendDirSeparator(stringPath);
			if (!JDirectoryExists(*stringPath))
				{
				cerr << argv[0] << ": specified string database path does not exist" << endl;
				exit(1);
				}
			}

		else if (strcmp(argv[index], "--choose") == 0)
			{
			pickForms = kJTrue;
			}

		else if (strcmp(argv[index], "--post-cmd") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			*postCmd = argv[index];
			}

		else if (argv[index][0] == '-')
			{
			cerr << argv[0] << ": unknown command line option: " << argv[index] << endl;
			}

		else if (inputName->IsEmpty())
			{
			*inputName = argv[index];
			const JSize length = inputName->GetLength();
			if (!inputName->EndsWith(".fd"))
				{
				cerr << argv[0] << ": not an fdesign (.fd) file: " << argv[index] << endl;
				exit(1);
				}
			}

		else
			{
			JString* userForm = new JString(argv[index]);
			assert( userForm != NULL );
			userFormList->Append(userForm);
			}

		index++;
		}

	if (inputName->IsEmpty())
		{
		cerr << argv[0] << ": fdesign input file not specified" << endl;
		exit(1);
		}
	if (!JFileExists(*inputName))
		{
		cerr << argv[0] << ": fdesign input file not found" << endl;
		exit(1);
		}
	if (!JDirectoryExists(*codePath))
		{
		cerr << argv[0] << ": source code directory does not exist" << endl;
		exit(1);
		}
	if (!JDirectoryExists(*stringPath))
		{
		cerr << argv[0] << ": directory for string data files does not exist" << endl;
		exit(1);
		}

	// let the user pick forms to include

	if (pickForms)
		{
		PickForms(*inputName, userFormList);
		}
}

/******************************************************************************
 PickForms

	Show the user a list of the forms in the input file and let them
	choose which ones to generate.  "All" is not an option because that
	is the default from the command line.

 ******************************************************************************/

void
PickForms
	(
	const JString&		fileName,
	JPtrArray<JString>*	list
	)
{
	JPtrArray<JString> all(JPtrArrayT::kDeleteAll);
	JSize count = 0;
	cout << endl;

	ifstream input(fileName);
	while (!input.eof() && !input.fail())
		{
		const JString line = JReadLine(input);
		if (line == kBeginFormLine)
			{
			JString* formName = new JString(JReadLine(input));
			assert( formName != NULL );
			RemoveIdentifier(kFormNameMarker, formName);
			all.Append(formName);
			count++;
			cout << count << ") " << *formName << endl;
			}
		}
	input.close();

	cout << endl;

	while (1)
		{
		JIndex choice;
		cout << "Form to include (0 to end): ";
		cin >> choice;
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
			cout << "That is not a valid choice" << endl;
			}
		else
			{
			JString* formName = new JString(*(all.NthElement(choice)));
			assert( formName != NULL );

			JIndex tagIndex;
			if (formName->LocateSubstring(kCustomTagMarker, &tagIndex))
				{
				formName->RemoveSubstring(tagIndex, formName->GetLength());
				}

			list->Append(formName);
			}
		}
}

/******************************************************************************
 FindConfigFile

	Searches for the specified config file.  Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
FindConfigFile
	(
	JString* configFileName
	)
{
	if (JFileExists(*configFileName))
		{
		return kJTrue;
		}

	const JString mainFileName = JCombinePathAndName(kMainConfigFileDir, *configFileName);
	if (JFileExists(mainFileName))
		{
		*configFileName = mainFileName;
		return kJTrue;
		}

	char* envStr = getenv(kEnvUserConfigFileDir);
	if (envStr != NULL && envStr[0] != '\0')
		{
		JString otherFileName = JCombinePathAndName(envStr, *configFileName);
		if (JFileExists(otherFileName))
			{
			*configFileName = otherFileName;
			return kJTrue;
			}
		}

	cerr << "Unable to find " << *configFileName << endl;
	cerr << "  please install it in " << kMainConfigFileDir << endl;
	cerr << "  or setenv " << kEnvUserConfigFileDir << " to point to it" << endl;
	return kJFalse;
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp
	(
	const JString& codeSuffix,
	const JString& headerSuffix,
	const JString& stringPath
	)
{
	cout << endl;
	cout << "This program generates subclasses of JXWindowDirector from" << endl;
	cout << "the forms stored in an fdesign .fd file." << endl;
	cout << endl;
	cout << "The source and header files are assumed to have the same" << endl;
	cout << "name as the associated form.  Everything delimited by" << endl;
	cout << endl;
	cout << "    // begin JXLayout" << endl;
	cout << "    // end JXLayout" << endl;
	cout << endl;
	cout << "will be replaced." << endl;
	cout << endl;
	cout << "FL_BOX objects with type NO_BOX count as overloaded objects." << endl;
	cout << "The label will be used as the class name.  Extra arguments" << endl;
	cout << "can be included as follows:  'MyWidget(arg1,arg2,'" << endl;
	cout << "and the standard arguments will be appended at the end." << endl;
	cout << endl;
	cout << "Named objects will be instantiated in the header file." << endl;
	cout << "To designate it as a local variable, put () around it." << endl;
	cout << "To designate it as a variable that is defined elsewhere in" << endl;
	cout << "the function, put <> around it." << endl;
	cout << endl;
	cout << "Usage:  <options> <fdesign file> <forms to generate>" << endl;
	cout << endl;
	cout << "-h          prints help" << endl;
	cout << "-v          prints version information" << endl;
	cout << "-cp         <code path>     - default: current directory" << endl;
	cout << "-cs         <code suffix>   - default: " << codeSuffix << endl;
	cout << "-hs         <header suffix> - default: " << headerSuffix << endl;
	cout << "-sp         <string path>   - default: " << stringPath << endl;
	cout << "--choose    interactively choose the forms to generate" << endl;
	cout << "--post-cmd  <cmd> - command to exec after all files have been modified" << endl;
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
