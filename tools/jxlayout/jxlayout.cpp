/******************************************************************************
 jxlayout.cpp

	Program to generate JX code from an fdesign .fd file.

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JStringManager.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <JPtrArray-JString.h>
#include <JFontManager.h>
#include <jTextUtil.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jProcessUtil.h>
#include <jCommandLine.h>
#include <jVCSUtil.h>
#include <jGlobals.h>
#include <JRect.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jAssert.h>

// Constants

static const JUtf8Byte* kVersionStr =

	"jxlayout 5.0.0\n"
	"\n"
	"Copyright (C) 1996-2018 John Lindal.";

static const JUtf8Byte* kBackupSuffix = "~";

static const JUtf8Byte* kBeginCodeDelimiterPrefix = "// begin ";
static const JUtf8Byte* kEndCodeDelimiterPrefix   = "// end ";

static const JString kDefaultDelimTag("JXLayout", JString::kNoCopy);
static const JString kCustomTagMarker("__",       JString::kNoCopy);

static const JString kDefTopEnclVarName("window", JString::kNoCopy);

// Data files (search for at startup)

static const JString kMainConfigFileDir   (CONFIG_FILE_DIR, JString::kNoCopy);
static const JString kEnvUserConfigFileDir("JXLAYOUTDIR",   JString::kNoCopy);

static JString classMapFile      ("class_map",        JString::kNoCopy);
static JString optionMapFile     ("option_map",       JString::kNoCopy);
static JString needFontListFile  ("need_font_list",   JString::kNoCopy);
static JString needStringListFile("need_string_list", JString::kNoCopy);
static JString needCreateListFile("need_create_list", JString::kNoCopy);

// Form fields

static const JString kBeginFormLine     ("=============== FORM ===============", JString::kNoCopy);
static const JString kFormNameMarker    ("Name:",              JString::kNoCopy);
static const JString kFormWidthMarker   ("Width:",             JString::kNoCopy);
static const JString kFormHeightMarker  ("Height:",            JString::kNoCopy);
static const JString kFormObjCountMarker("Number of Objects:", JString::kNoCopy);

// Object fields

static const JString kBeginObjLine     ("--------------------", JString::kNoCopy);
static const JString kObjClassMarker   ("class:",     JString::kNoCopy);
static const JString kObjTypeMarker    ("type:",      JString::kNoCopy);
static const JString kObjRectMarker    ("box:",       JString::kNoCopy);
static const JString kObjBoxTypeMarker ("boxtype:",   JString::kNoCopy);
static const JString kObjColorsMarker  ("colors:",    JString::kNoCopy);
static const JString kObjLAlignMarker  ("alignment:", JString::kNoCopy);
static const JString kObjLStyleMarker  ("style:",     JString::kNoCopy);
static const JString kObjLSizeMarker   ("size:",      JString::kNoCopy);
static const JString kObjLColorMarker  ("lcol:",      JString::kNoCopy);
static const JString kObjLabelMarker   ("label:",     JString::kNoCopy);
static const JString kObjShortcutMarker("shortcut:",  JString::kNoCopy);
static const JString kObjResizeMarker  ("resize:",    JString::kNoCopy);
static const JString kObjGravityMarker ("gravity:",   JString::kNoCopy);
static const JString kObjNameMarker    ("name:",      JString::kNoCopy);
static const JString kObjCallbackMarker("callback:",  JString::kNoCopy);
static const JString kObjCBArgMarker   ("argument:",  JString::kNoCopy);

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
	const JUtf8Byte* flSize;
	const JUtf8Byte* jxSize;
};

static const FontSizeConversion kFontSizeTable[] =
{
	{"FL_DEFAULT_SIZE", "JFontManager::GetDefaultFontSize()-2"},
	{"FL_TINY_SIZE",    "JFontManager::GetDefaultFontSize()-4"},
	{"FL_SMALL_SIZE",   "JFontManager::GetDefaultFontSize()-2"},
	{"FL_NORMAL_SIZE",  "JFontManager::GetDefaultFontSize()"},
	{"FL_MEDIUM_SIZE",  "JFontManager::GetDefaultFontSize()+2"},
	{"FL_LARGE_SIZE",   "JFontManager::GetDefaultFontSize()+4"},
	{"FL_HUGE_SIZE",    "JFontManager::GetDefaultFontSize()+8"}
};

const JSize kFontSizeTableSize = sizeof(kFontSizeTable)/sizeof(FontSizeConversion);

// color conversion

struct ColorConversion
{
	const JUtf8Byte* flColor;
	const JUtf8Byte* jxColor;
};

static const ColorConversion kColorTable[] =
{
	{"FL_BLACK",        "JColorManager::GetBlackColor()"},
	{"FL_RED",          "JColorManager::GetRedColor()"},
	{"FL_GREEN",        "JColorManager::GetGreenColor()"},
	{"FL_YELLOW",       "JColorManager::GetYellowColor()"},
	{"FL_BLUE",         "JColorManager::GetBlueColor()"},
	{"FL_MAGENTA",      "JColorManager::GetMagentaColor()"},
	{"FL_CYAN",         "JColorManager::GetCyanColor()"},
	{"FL_WHITE",        "JColorManager::GetWhiteColor()"},
	{"FL_LCOL",         "JColorManager::GetBlackColor()"},
	{"FL_COL1",         "JColorManager::GetDefaultBackColor()"},
	{"FL_MCOL",         "JColorManager::GetDefaultFocusColor()"},
	{"FL_RIGHT_BCOL",   "JColorManager::Get3DShadeColor()"},
	{"FL_BOTTOM_BCOL",  "JColorManager::Get3DShadeColor()"},
	{"FL_TOP_BCOL",     "JColorManager::Get3DLightColor()"},
	{"FL_LEFT_BCOL",    "JColorManager::Get3DLightColor()"},
	{"FL_INACTIVE",     "JColorManager::GetInactiveLabelColor()"},
	{"FL_INACTIVE_COL", "JColorManager::GetInactiveLabelColor()"}
};

const JSize kColorTableSize = sizeof(kColorTable)/sizeof(ColorConversion);

// Prototypes

bool GenerateForm(std::istream& input, const JString& formName,
					  const JString& tagName, const JString& enclName,
					  const JString& codePath, const JString& stringPath,
					  const JString& codeSuffix, const JString& headerSuffix,
					  const bool requireObjectNames,
					  JPtrArray<JString>* backupList);
bool GenerateCode(std::istream& input, std::ostream& output, const JString& stringPath,
					  const JString& formName, const JString& tagName,
					  const JString& userTopEnclVarName, const JUtf8Byte* indent,
					  const bool requireObjectNames,
					  JPtrArray<JString>* objTypes, JPtrArray<JString>* objNames);
void GenerateHeader(std::ostream& output, const JPtrArray<JString>& objTypes,
					const JPtrArray<JString>& objNames, const JUtf8Byte* indent);

bool ParseGravity(const JString& gravity, JString* hSizing, JString* vSizing);
void GetTempVarName(const JString& tagName, JString* varName,
					const JPtrArray<JString>& objNames);
bool GetEnclosure(const JArray<JRect>& rectList, const JIndex rectIndex, JIndex* enclIndex);
bool GetConstructor(const JString& flClass, const JString& flType,
						JString* label, JString* className, JString* argList);
bool SplitClassNameAndArgs(const JString& str, JString* className, JString* args);
void ApplyOptions(std::ostream& output, const JString& className,
				  const JString& formName, const JString& tagName, const JString& varName,
				  const JPtrArray<JString>& values, const JString& flSize,
				  const JString& flStyle, const JString& flColor,
				  const JUtf8Byte* indent, JStringManager* stringMgr);
bool AcceptsFontSpec(const JString& className);
bool NeedsStringArg(const JString& className);
bool NeedsCreateFunction(const JString& className);
bool FindClassName(const JString& fileName, const JString& className);
bool ConvertXFormsFontSize(const JString& flSize, JString* jxSize);
bool ConvertXFormsColor(const JString& flColor, JString* jxColor);

bool CopyBeforeCodeDelimiter(const JString& tag, std::istream& input, std::ostream& output,
								 JString* indent);
bool CopyAfterCodeDelimiter(const JString& tag, std::istream& input, std::ostream& output);
void RemoveIdentifier(const JString& id, JString* line);

void GetOptions(const JSize argc, char* argv[], JString* inputName,
				JString* codePath, JString* stringPath,
				JString* codeSuffix, JString* headerSuffix,
				bool* requireObjectNames, JString* postCmd);
bool FindConfigFile(JString* configFileName);

void PrintHelp(const JString& codePath,
			   const JString& codeSuffix, const JString& headerSuffix,
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
		!FindConfigFile(&needStringListFile) ||
		!FindConfigFile(&needCreateListFile))
		{
		return 1;
		}

	// parse the command line options

	JString inputName, codePath, stringPath, codeSuffix, headerSuffix, postCmd;
	bool requireObjectNames;
	JPtrArray<JString> backupList(JPtrArrayT::kDeleteAll);		// forms that have been backed up
	GetOptions(argc, argv, &inputName, &codePath, &stringPath,
			   &codeSuffix, &headerSuffix, &requireObjectNames, &postCmd);

	// generate each requested form

	bool changed = false;

	std::ifstream input(inputName.GetBytes());
	while (!input.eof() && !input.fail())
		{
		const JString line = JReadLine(input);
		if (line != kBeginFormLine)
			{
			continue;
			}

		// get form name

		JString formName = JReadLine(input);
		RemoveIdentifier(kFormNameMarker, &formName);

		// look for custom tag

		JString tagName = kDefaultDelimTag;
		JString enclName;
		JStringIterator fnIter(&formName);
		if (fnIter.Next(kCustomTagMarker) && !fnIter.AtEnd())
			{
			fnIter.RemoveLastMatch();

			fnIter.BeginMatch();
			fnIter.MoveTo(kJIteratorStartAtEnd, 0);
			tagName = fnIter.FinishMatch().GetString();

			fnIter.RemoveLastMatch();

			// get enclosure name

			JStringIterator tnIter(&tagName);
			if (tnIter.Next(kCustomTagMarker) && !tnIter.AtEnd())
				{
				tnIter.RemoveLastMatch();

				tnIter.BeginMatch();
				tnIter.MoveTo(kJIteratorStartAtEnd, 0);
				enclName = tnIter.FinishMatch().GetString();

				tnIter.RemoveLastMatch();
				}
			tnIter.Invalidate();

			// report errors

			if (tagName != kDefaultDelimTag && enclName.IsEmpty())
				{
				std::cerr << formName << ", " << tagName;
				std::cerr << ": no enclosure specified" << std::endl;
				}
			else if (tagName == kDefaultDelimTag &&
					 !enclName.IsEmpty() && enclName != kDefTopEnclVarName)
				{
				std::cerr << formName << ", " << tagName;
				std::cerr << ": not allowed to specify enclosure other than ";
				std::cerr << kDefTopEnclVarName << std::endl;
				}
			}
		fnIter.Invalidate();

		if (GenerateForm(input, formName, tagName, enclName,
						 codePath, stringPath, codeSuffix, headerSuffix,
						 requireObjectNames, &backupList))
			{
			changed = true;
			}
		}

	if (changed && !postCmd.IsEmpty())
		{
		const JError err = JExecute(postCmd, nullptr);
		err.ReportIfError();
		}

	return 0;
}

/******************************************************************************
 GenerateForm

 ******************************************************************************/

bool
GenerateForm
	(
	std::istream&		input,
	const JString&		formName,
	const JString&		tagName,
	const JString&		enclName,
	const JString&		codePath,
	const JString&		stringPath,
	const JString&		codeSuffix,
	const JString&		headerSuffix,
	const bool		requireObjectNames,
	JPtrArray<JString>*	backupList
	)
{
	const JString codeFileName    = codePath + formName + codeSuffix;
	const JString codeFileBakName = codeFileName + kBackupSuffix;

	const JString headerFileName    = codePath + formName + headerSuffix;
	const JString headerFileBakName = headerFileName + kBackupSuffix;

	if (!JFileExists(codeFileName))
		{
		std::cerr << codeFileName << " not found" << std::endl;
		return false;
		}
	if (!JFileExists(headerFileName))
		{
		std::cerr << headerFileName << " not found" << std::endl;
		return false;
		}

	std::cout << "Generating: " << formName << ", " << tagName << std::endl;

	bool changed = false;

	// copy source file contents before start delimiter

	JString tempCodeFileName;
	JError err = JCreateTempFile(&codePath, nullptr, &tempCodeFileName);
	if (!err.OK())
		{
		std::cerr << "Unable to create temporary file in " << codePath << std::endl;
		std::cerr << "  (" << err.GetMessage() << ')' << std::endl;
		return false;
		}

	JString indent;

	std::ifstream origCode(codeFileName.GetBytes());
	std::ofstream outputCode(tempCodeFileName.GetBytes());
	if (!outputCode.good())
		{
		std::cerr << "Unable to open temporary file in " << codePath << std::endl;
		JRemoveFile(tempCodeFileName);
		return false;
		}
	if (!CopyBeforeCodeDelimiter(tagName, origCode, outputCode, &indent))
		{
		std::cerr << "No starting delimiter in " << codeFileName << std::endl;
		outputCode.close();
		JRemoveFile(tempCodeFileName);
		return false;
		}

	// generate code for each object in the form

	JPtrArray<JString> objTypes(JPtrArrayT::kDeleteAll),
					   objNames(JPtrArrayT::kDeleteAll);
	if (!GenerateCode(input, outputCode, stringPath, formName, tagName, enclName,
					  indent.GetBytes(), requireObjectNames, &objTypes, &objNames))
		{
		outputCode.close();
		JRemoveFile(tempCodeFileName);
		return false;
		}

	// copy source file contents after end delimiter

	bool done = CopyAfterCodeDelimiter(tagName, origCode, outputCode);
	origCode.close();
	outputCode.close();

	if (!done)
		{
		std::cerr << "No ending delimiter in " << codeFileName << std::endl;
		JRemoveFile(tempCodeFileName);
		return false;
		}

	// check if source file actually changed

	JString origCodeText, newCodeText;
	JReadFile(codeFileName, &origCodeText);
	JReadFile(tempCodeFileName, &newCodeText);
	if (newCodeText != origCodeText)
		{
		JEditVCS(codeFileName);
		JRenameFile(tempCodeFileName, codeFileName, true);
		changed = true;
		}
	else
		{
		JRemoveFile(tempCodeFileName);
		}

	// copy header file contents before start delimiter

	JString tempHeaderFileName;
	err = JCreateTempFile(&codePath, nullptr, &tempHeaderFileName);
	if (!err.OK())
		{
		std::cerr << "Unable to create temporary file in " << codePath << std::endl;
		std::cerr << "  (" << err.GetMessage() << ')' << std::endl;
		return false;
		}

	std::ifstream origHeader(headerFileName.GetBytes());
	std::ofstream outputHeader(tempHeaderFileName.GetBytes());
	if (!outputHeader.good())
		{
		std::cerr << "Unable to open temporary file in " << codePath << std::endl;
		JRemoveFile(tempHeaderFileName);
		return false;
		}
	if (!CopyBeforeCodeDelimiter(tagName, origHeader, outputHeader, &indent))
		{
		std::cerr << "No starting delimiter in " << headerFileName << std::endl;
		outputHeader.close();
		JRemoveFile(tempHeaderFileName);
		return false;
		}

	// generate instance variable for each object in the form

	GenerateHeader(outputHeader, objTypes, objNames, indent.GetBytes());

	// copy header file contents after end delimiter

	done = CopyAfterCodeDelimiter(tagName, origHeader, outputHeader);
	origHeader.close();
	outputHeader.close();

	if (!done)
		{
		std::cerr << "No ending delimiter in " << headerFileName << std::endl;
		JRemoveFile(tempHeaderFileName);
		return false;
		}

	// check if header file actually changed

	JString origHeaderText, newHeaderText;
	JReadFile(headerFileName, &origHeaderText);
	JReadFile(tempHeaderFileName, &newHeaderText);
	if (newHeaderText != origHeaderText)
		{
		JEditVCS(headerFileName);
		JRenameFile(tempHeaderFileName, headerFileName, true);
		changed = true;
		}
	else
		{
		JRemoveFile(tempHeaderFileName);
		}

	return changed;
}

/******************************************************************************
 GenerateCode

 ******************************************************************************/

bool
GenerateCode
	(
	std::istream&		input,
	std::ostream&		output,
	const JString&		stringPath,
	const JString&		formName,
	const JString&		tagName,
	const JString&		userTopEnclVarName,
	const JUtf8Byte*	indent,
	const bool		requireObjectNames,
	JPtrArray<JString>*	objTypes,
	JPtrArray<JString>*	objNames
	)
{
JIndex i;

	// width

	input >> std::ws;
	JString line = JReadUntilws(input);
	assert( line == kFormWidthMarker );
	JSize formWidth;
	input >> formWidth;

	// height

	input >> std::ws;
	line = JReadUntilws(input);
	assert( line == kFormHeightMarker );
	JSize formHeight;
	input >> formHeight;

	// object count (marker contains whitespace)

	input >> std::ws;
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

		output << indent << "auto* window = jnew JXWindow(this, ";
		output << formWidth << ',' << formHeight;
		output << ", JString::empty);" << std::endl;
		output << indent << "assert( window != nullptr );" << std::endl;
		output << std::endl;
		}
	else
		{
		assert( !userTopEnclVarName.IsEmpty() );
		topEnclVarName = userTopEnclVarName;

		output << indent << "const JRect ";
		topEnclFrameName.Print(output);
		output << "    = ";
		topEnclVarName.Print(output);
		output << "->GetFrame();" << std::endl;

		output << indent << "const JRect ";
		topEnclApName.Print(output);
		output << " = ";
		topEnclVarName.Print(output);
		output << "->GetAperture();" << std::endl;

		output << indent;
		topEnclVarName.Print(output);
		output << "->AdjustSize(" << formWidth << " - ";
		topEnclApName.Print(output);
		output << ".width(), " << formHeight << " - ";
		topEnclApName.Print(output);
		output << ".height());" << std::endl;

		output << std::endl;
		}

	// We need to calculate the enclosure for each object.  Since objects
	// are drawn in the order added, an object must come after its enclosure
	// in the list in order to be visible.

	JArray<JRect>    rectList(10);
	JArray<bool> isInstanceVar(10);

	// This array is used to send the options to ApplyOptions.
	// It does not own the pointers that it contains.

	JPtrArray<JString> optionValues(JPtrArrayT::kForgetAll, kOptionCount);
	for (i=1; i<=kOptionCount; i++)
		{
		optionValues.Append(nullptr);
		}

	// generate code for each object

	JStringManager stringMgr;

	JIndex objCount = 1;
	for (i=1; i<=itemCount; i++)
		{
		// check for start-of-object

		input >> std::ws;
		line = JReadLine(input);
		assert( line == kBeginObjLine );

		// object class

		JString flClass = JReadLine(input);
		RemoveIdentifier(kObjClassMarker, &flClass);

		// object type

		JString flType = JReadLine(input);
		RemoveIdentifier(kObjTypeMarker, &flType);

		// object frame

		input >> std::ws;
		line = JReadUntilws(input);
		assert( line == kObjRectMarker );
		JCoordinate x,y,w,h;
		input >> x >> y >> w >> h >> std::ws;
		const JRect frame(y, x, y+h, x+w);
		rectList.AppendElement(frame);

		// box type

		JString boxType = JReadLine(input);
		RemoveIdentifier(kObjBoxTypeMarker, &boxType);

		// for actual boxes, use boxType instead of type

		if (flClass == "FL_BOX")
			{
			flType = boxType;
			}

		// colors

		input >> std::ws;
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

		input >> std::ws;
		line = JReadUntilws(input);
		assert( line == kObjGravityMarker );
		const JString nwGravity = JReadUntilws(input);
		const JString seGravity = JReadUntilws(input);

		// variable name

		bool isLocal = false;
		auto* varName = jnew JString(JReadLine(input));
		assert( varName != nullptr );
		RemoveIdentifier(kObjNameMarker, varName);

		// callback (ignored)

		JIgnoreLine(input);

		// callback argument

		JString cbArg = JReadLine(input);
		RemoveIdentifier(kObjCBArgMarker, &cbArg);

		JString cbArgExtra;
		do
			{
			cbArgExtra = JReadLine(input);
			cbArgExtra.TrimWhitespace();
			}
			while (!cbArgExtra.IsEmpty());

		// don't bother to generate code for initial box
		// if it is FL_BOX, FL_FLAT_BOX, FL_COL1

		if (i==1 && flClass == "FL_BOX" && flType == "FL_FLAT_BOX" && col1 == "FL_COL1")
			{
			rectList.RemoveElement(objCount);
			continue;
			}

		// variable name

		if (varName->IsEmpty() && requireObjectNames)
			{
			std::cerr << "FAILED - Names are required for all objects" << std::endl;
			return false;
			}
		else if (varName->IsEmpty())
			{
			isInstanceVar.AppendElement(false);
			GetTempVarName(tagName, varName, *objNames);
			isLocal = true;
			}
		else if ((varName->GetFirstCharacter() == '(' &&
				  varName->GetLastCharacter()  == ')') ||
				 (varName->GetFirstCharacter() == '<' &&
				  varName->GetLastCharacter()  == '>'))
			{
			isInstanceVar.AppendElement(false);
			isLocal  = varName->GetFirstCharacter() == '(';

			JStringIterator iter(varName);
			iter.SkipNext();
			iter.RemoveAllPrev();
			iter.MoveTo(kJIteratorStartAtEnd, 0);
			iter.SkipPrev();
			iter.RemoveAllNext();
			}
		else
			{
			isInstanceVar.AppendElement(true);
			}
		objNames->Append(varName);

		// check for errors -- safe since we have read in entire object

		JString hSizing, vSizing;
		if (!ParseGravity(nwGravity, &hSizing, &vSizing))
			{
			std::cerr << "Illegal sizing specification ";
			std::cerr << nwGravity << ',' << seGravity;
			std::cerr << " for '" << *varName << '\'' << std::endl;
			rectList.RemoveElement(objCount);
			isInstanceVar.RemoveElement(objCount);
			objNames->DeleteElement(objCount);
			continue;
			}

		if (*varName == topEnclVarName)
			{
			std::cerr << "Cannot use reserved name '" << topEnclVarName << '\'' << std::endl;
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
			enclName = *(objNames->GetElement(enclIndex));
			const JRect enclFrame = rectList.GetElement(enclIndex);
			localFrame.Shift(-enclFrame.topLeft());
			}
		else
			{
			enclName = topEnclVarName;
			}

		// get the class name and additional arguments

		auto* className = jnew JString;
		assert( className != nullptr );
		objTypes->Append(className);

		JString argList;
		if (!GetConstructor(flClass, flType, &label, className, &argList))
			{
			std::cerr << "Unsupported class: " << flClass << ", " << flType << std::endl;
			rectList.RemoveElement(objCount);
			isInstanceVar.RemoveElement(objCount);
			objNames->DeleteElement(objCount);
			objTypes->DeleteElement(objCount);
			continue;
			}

		// generate the actual code

		const bool needCreate = NeedsCreateFunction(*className);

		output << indent;
		if (isLocal)
			{
			output << "auto* ";
			}
		varName->Print(output);
		output << " =" << std::endl;
		output << indent << indent;
		if (!needCreate)
			{
			output << "jnew ";
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
			NeedsStringArg(*className))
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
		output << ',' << std::endl;
		output << indent << indent << indent << indent << indent << "JXWidget::";
		hSizing.Print(output);
		output << ", JXWidget::";
		vSizing.Print(output);
		output << ", " << localFrame.left << ',' << localFrame.top << ", ";
		output << localFrame.width() << ',' << localFrame.height() << ");" << std::endl;

		output << indent << "assert( ";
		varName->Print(output);
		output << " != nullptr );" << std::endl;

		ApplyOptions(output, *className, formName, tagName, *varName, optionValues,
					 lSize, lStyle, lColor, indent, &stringMgr);

		const bool isLabel = *className == "JXStaticText" &&
			cbArg.IsEmpty() &&
			localFrame.height() <= 20;

		if (isLabel && lAlign.Contains("FL_ALIGN_CENTER"))
			{
			output << indent;
			varName->Print(output);
			output << "->SetToLabel(true);" << std::endl;
			}
		else if (isLabel && !lAlign.Contains("FL_ALIGN_TOP"))
			{
			output << indent;
			varName->Print(output);
			output << "->SetToLabel();" << std::endl;
			}

		output << std::endl;

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
		std::ofstream dbOutput(dbFileName.GetBytes());
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
		output << indent;
		topEnclVarName.Print(output);
		output << "->SetSize(";
		topEnclFrameName.Print(output);
		output << ".width(), ";
		topEnclFrameName.Print(output);
		output << ".height());" << std::endl;
		output << std::endl;
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

	return true;
}

/******************************************************************************
 GenerateHeader

 ******************************************************************************/

void
GenerateHeader
	(
	std::ostream&				output,
	const JPtrArray<JString>&	objTypes,
	const JPtrArray<JString>&	objNames,
	const JUtf8Byte*			indent
	)
{
	JIndex i;
	const JSize count = objTypes.GetElementCount();
	assert( count == objNames.GetElementCount() );

	// get width of longest type

	JSize maxLen = 0;
	for (i=1; i<=count; i++)
		{
		const JString* type = objTypes.GetElement(i);
		const JSize len     = type->GetCharacterCount();
		if (len > maxLen)
			{
			maxLen = len;
			}
		}

	// declare each object

	for (i=1; i<=count; i++)
		{
		output << indent;
		const JString* type = objTypes.GetElement(i);
		type->Print(output);
		output << '*';
		const JSize len = type->GetCharacterCount();
		for (JIndex j=len+1; j<=maxLen+1; j++)
			{
			output << ' ';
			}
		(objNames.GetElement(i))->Print(output);
		output << ';' << std::endl;
		}

	// need blank line to conform to expectations of CopyAfterCodeDelimiter

	output << std::endl;
}

/******************************************************************************
 ParseGravity

	Convert X gravity values into JXWidget sizing values.
	Returns false if user specified an invalid combination.

 ******************************************************************************/

static const JUtf8Byte* kGravityMap[] =
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

const JSize kGravityCount = sizeof(kGravityMap) / (3*sizeof(JUtf8Byte*));

bool
ParseGravity
	(
	const JString&	gravity,
	JString*		hSizing,
	JString*		vSizing
	)
{
	for (JUnsignedOffset i=0; i<kGravityCount; i++)
		{
		const JIndex j = 3*i;
		if (gravity == kGravityMap[j])
			{
			*hSizing = kGravityMap[j+1];
			*vSizing = kGravityMap[j+2];
			return true;
			}
		}

	return false;
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
	const JString&				tagName,
	JString*					varName,
	const JPtrArray<JString>&	objNames
	)
{
	JString suffix = tagName;
	JStringIterator iter(&suffix);
	while (iter.Next(illegalCChar))
		{
		iter.RemoveLastMatch();
		}
	iter.Invalidate();

	suffix.Prepend("_");

	const JString prefix("obj", JString::kNoCopy);
	const JSize count = objNames.GetElementCount();
	for (JIndex i=1; i<=INT_MAX; i++)
		{
		*varName = prefix + JString(i, 0) + suffix;
		bool unique = true;
		for (JIndex j=1; j<=count; j++)
			{
			const JString* usedName = objNames.GetElement(j);
			if (*varName == *usedName)
				{
				unique = false;
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

	Returns true if it finds a rectangle that encloses the rectangle
	at the specified index.  If it finds more than one enclosing rectangle,
	it returns the smallest one.

	If no enclosure is found, returns false, and sets *enclIndex to zero.

 ******************************************************************************/

bool
GetEnclosure
	(
	const JArray<JRect>&	rectList,
	const JIndex			rectIndex,
	JIndex*					enclIndex
	)
{
	const JRect theRect = rectList.GetElement(rectIndex);
	bool found = false;
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
				found      = true;
				*enclIndex = i;
				}
			}
		}
	return found;
}

/******************************************************************************
 GetConstructor

	Convert the XForms type into a JX object constructor.
	Returns true if successful.

 ******************************************************************************/

bool
GetConstructor
	(
	const JString&	flClass,
	const JString&	flType,
	JString*		label,
	JString*		className,
	JString*		argList
	)
{
	if (flClass == "FL_BOX" && flType == "FL_NO_BOX" && !label->IsEmpty())
		{
		const bool ok = SplitClassNameAndArgs(*label, className, argList);
		label->Clear();
		return ok;
		}

	std::ifstream classMap(classMapFile.GetBytes());
	classMap >> std::ws;
	while (true)
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
		classMap >> std::ws;
		}

	// falling through means that nothing matched

	return false;
}

/******************************************************************************
 SplitClassNameAndArgs

	Returns false if there is no class name.

 ******************************************************************************/

bool
SplitClassNameAndArgs
	(
	const JString&	str,
	JString*		name,
	JString*		args
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	str.Split("(", &list, 2);

	const bool hasArgs = list.GetElementCount() > 1;
	if (hasArgs &&
		!list.GetFirstElement()->IsEmpty() &&
		!list.GetLastElement()->IsEmpty())
		{
		*name = *(list.GetFirstElement());
		*args = *(list.GetLastElement());

		name->TrimWhitespace();
		args->TrimWhitespace();
		return true;
		}
	else if (hasArgs && list.GetFirstElement()->IsEmpty())
		{
		std::cerr << "No class name in " << str << std::endl;
		name->Clear();
		args->Clear();
		return false;
		}
	else
		{
		if (hasArgs)
			{
			*name = *(list.GetFirstElement());
			}
		else
			{
			*name = str;
			}
		name->TrimWhitespace();
		args->Clear();
		return true;
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
	std::ostream&				output,
	const JString&				className,
	const JString&				formName,
	const JString&				tagName,
	const JString&				varName,
	const JPtrArray<JString>&	values,
	const JString&				flSize,
	const JString&				flStyle,
	const JString&				flColor,
	const JUtf8Byte*			indent,
	JStringManager*				stringMgr
	)
{
	std::ifstream optionMap(optionMapFile.GetBytes());
	optionMap >> std::ws;
	while (true)
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
				bool supported;

				// shortcuts

				optionMap >> std::ws >> JBoolFromString(supported);
				if (supported)
					{
					optionMap >> std::ws;
					const JString function = JReadUntilws(optionMap);
					const JString* value   = values.GetElement(kShortcutsIndex);
					if (!value->IsEmpty())
						{
						JString id = varName;
						id += "::";
						id += formName;
						id += "::shortcuts::";
						id += tagName;		// last since it is almost always the same

						output << indent;
						varName.Print(output);
						output << "->";
						function.Print(output);
						output << "(JGetString(\"";
						id.Print(output);
						output << "\"));" << std::endl;

						auto* s = jnew JString(*value);
						assert( s != nullptr );
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
					optionMap >> std::ws >> JBoolFromString(supported);
					if (supported)
						{
						optionMap >> std::ws;
						const JString defValue = JReadUntilws(optionMap);
						const JString function = JReadUntilws(optionMap);
						const JString* value   = values.GetElement(i);
						if (*value != defValue)
							{
							JString jxColor;
							if (ConvertXFormsColor(*value, &jxColor))
								{
								output << indent;
								varName.Print(output);
								output << "->";
								function.Print(output);
								output << '(';
								jxColor.Print(output);
								output << ");" << std::endl;
								}
							else
								{
								std::cerr << "Unknown color: " << *value << std::endl;
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
		optionMap >> std::ws;
		}

	// For some objects, we have to decode the XForms font spec.

	if (AcceptsFontSpec(className))
		{
		JString fontName = JFontManager::GetDefaultFontName();
		if (flStyle.Contains("FIXED"))
			{
			output << indent;
			varName.Print(output);
			output << "->SetFont(JGetMonospaceFontName(), JGetDefaultMonoFontSize(), JFontStyle());" << std::endl;
			}
		else if (flStyle.Contains("TIMES"))
			{
			JString id;
			id += "TimesFontName::";
			id += formName;
			id += "::";
			id += tagName;		// last since it is almost always the same

			output << indent;
			varName.Print(output);
			output << "->SetFontName(JGetString(\"";
			id.Print(output);
			output << "\"));" << std::endl;

			stringMgr->SetElement(id, JString("Times", JString::kNoCopy), JPtrArrayT::kDelete);
			}

		if (flSize != "FL_NORMAL_SIZE")
			{
			JString jxSize;
			if (ConvertXFormsFontSize(flSize, &jxSize))
				{
				output << indent;
				varName.Print(output);
				output << "->SetFontSize(";
				jxSize.Print(output);
				output << ");" << std::endl;
				}
			else
				{
				std::cerr << "Unknown font size: " << flSize << std::endl;
				}
			}

		JFontStyle style;
		if (flStyle.Contains("BOLD"))
			{
			style.bold = true;
			}
		if (flStyle.Contains("ITALIC"))
			{
			style.italic = true;
			}
		if (style.bold || style.italic || flColor != "FL_BLACK")
			{
			JString jxColor;
			if (ConvertXFormsColor(flColor, &jxColor))
				{
				output << indent << "const JFontStyle ";
				varName.Print(output);
				output << "_style(";

				if (style.bold)
					{
					output << "true, ";
					}
				else
					{
					output << "false, ";
					}

				if (style.italic)
					{
					output << "true, ";
					}
				else
					{
					output << "false, ";
					}

				output << "0, false, ";
				jxColor.Print(output);
				output << ");" << std::endl;

				output << indent;
				varName.Print(output);
				output << "->SetFontStyle(";
				varName.Print(output);
				output << "_style);" << std::endl;
				}
			else
				{
				std::cerr << "Unknown color: " << flColor << std::endl;
				}
			}
		}
}

/******************************************************************************
 AcceptsFontSpec

	Returns true if the given class accepts SetFontName(), SetFontSize(),
	and SetFontStyle().

 ******************************************************************************/

bool
AcceptsFontSpec
	(
	const JString& className
	)
{
	return FindClassName(needFontListFile, className);
}

/******************************************************************************
 NeedsStringArg

	Returns true if the given class requires a text string as the first arg.

 ******************************************************************************/

bool
NeedsStringArg
	(
	const JString& className
	)
{
	return FindClassName(needStringListFile, className);
}

/******************************************************************************
 NeedsCreateFunction

	Returns true if the given class needs to constructed via Create().

 ******************************************************************************/

bool
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

bool
FindClassName
	(
	const JString& fileName,
	const JString& className
	)
{
	std::ifstream list(fileName.GetBytes());
	list >> std::ws;
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
				return true;
				}
			}
		list >> std::ws;
		}

	// falling through means that nothing matched

	return false;
}

/******************************************************************************
 ConvertXFormsFontSize

 ******************************************************************************/

bool
ConvertXFormsFontSize
	(
	const JString&	flSize,
	JString*		jxSize
	)
{
	for (JUnsignedOffset i=0; i<kFontSizeTableSize; i++)
		{
		if (kFontSizeTable[i].flSize == flSize)
			{
			*jxSize = kFontSizeTable[i].jxSize;
			return true;
			}
		}

	return false;
}

/******************************************************************************
 ConvertXFormsColor

 ******************************************************************************/

bool
ConvertXFormsColor
	(
	const JString&	flColor,
	JString*		jxColor
	)
{
	for (JUnsignedOffset i=0; i<kColorTableSize; i++)
		{
		if (kColorTable[i].flColor == flColor)
			{
			*jxColor = kColorTable[i].jxColor;
			return true;
			}
		}

	return false;
}

/******************************************************************************
 CopyBeforeCodeDelimiter

 ******************************************************************************/

bool
CopyBeforeCodeDelimiter
	(
	const JString&	tag,
	std::istream&	input,
	std::ostream&	output,
	JString*		indent
	)
{
	JString buffer;

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

		buffer += line;
		buffer += "\n";
		}

	bool useSpaces, isMixed;
	JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	*indent = useSpaces ? "    " : "\t";

	return !input.eof() && !input.fail();
}

/******************************************************************************
 CopyAfterCodeDelimiter

	Skips everything before end delimiter and then copies the rest.

 ******************************************************************************/

bool
CopyAfterCodeDelimiter
	(
	const JString&	tag,
	std::istream&	input,
	std::ostream&	output
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
		return false;
		}

	// include end delimiter

	delim.Print(output);
	output << std::endl;

	// copy lines after end delimiter

	while (true)
		{
		const JString line = JReadLine(input);
		if ((input.eof() || input.fail()) && line.IsEmpty())
			{
			break;	// avoid creating extra empty lines
			}
		line.Print(output);
		output << std::endl;
		}

	return true;
}

/******************************************************************************
 RemoveIdentifier

	Checks that the given id is at the start of the line, and then removes it.

 ******************************************************************************/

void
RemoveIdentifier
	(
	const JString&	id,
	JString*		line
	)
{
	assert( line->BeginsWith(id) );

	JStringIterator iter(line);
	iter.Next(id);
	iter.RemoveAllPrev();
	line->TrimWhitespace();	// invalidates iter
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
	bool*			requireObjectNames,
	JString*			postCmd
	)
{
	inputName->Clear();
	postCmd->Clear();

	*codePath           = "./code/";
	*stringPath         = "./strings/";
	*codeSuffix         = ".cpp";
	*headerSuffix       = ".h";
	*requireObjectNames = false;

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
			PrintHelp(*codePath, *codeSuffix, *headerSuffix, *stringPath);
			exit(0);
			}

		else if (strcmp(argv[index], "-cp") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			*codePath = argv[index];
			JAppendDirSeparator(codePath);
			if (!JDirectoryExists(*codePath))
				{
				std::cerr << argv[0] << ": specified code path does not exist" << std::endl;
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
				std::cerr << argv[0] << ": specified string database path does not exist" << std::endl;
				exit(1);
				}
			}

		else if (strcmp(argv[index], "--require-obj-names") == 0)
			{
			*requireObjectNames = true;
			}

		else if (strcmp(argv[index], "--post-cmd") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			*postCmd = argv[index];
			}

		else if (argv[index][0] == '-')
			{
			std::cerr << argv[0] << ": unknown command line option: " << argv[index] << std::endl;
			}

		else if (inputName->IsEmpty())
			{
			*inputName = argv[index];
			if (!inputName->EndsWith(".fd"))
				{
				std::cerr << argv[0] << ": not an fdesign (.fd) file: " << argv[index] << std::endl;
				exit(1);
				}
			}

		else
			{
			std::cerr << argv[0] << ": unknown command line argument: " << argv[index] << std::endl;
			}

		index++;
		}

	if (inputName->IsEmpty())
		{
		std::cerr << argv[0] << ": fdesign input file not specified" << std::endl;
		exit(1);
		}
	if (!JFileExists(*inputName))
		{
		std::cerr << argv[0] << ": fdesign input file not found" << std::endl;
		exit(1);
		}
	if (!JDirectoryExists(*codePath))
		{
		std::cerr << argv[0] << ": source code directory does not exist" << std::endl;
		exit(1);
		}
	if (!JDirectoryExists(*stringPath))
		{
		std::cerr << argv[0] << ": directory for string data files does not exist" << std::endl;
		exit(1);
		}
}

/******************************************************************************
 FindConfigFile

	Searches for the specified config file.  Returns true if successful.

 ******************************************************************************/

bool
FindConfigFile
	(
	JString* configFileName
	)
{
	if (JFileExists(*configFileName))
		{
		return true;
		}

	const JString mainFileName = JCombinePathAndName(kMainConfigFileDir, *configFileName);
	if (JFileExists(mainFileName))
		{
		*configFileName = mainFileName;
		return true;
		}

	char* envStr = getenv(kEnvUserConfigFileDir.GetBytes());
	if (envStr != nullptr && envStr[0] != '\0')
		{
		JString otherFileName = JCombinePathAndName(JString(envStr, JString::kNoCopy), *configFileName);
		if (JFileExists(otherFileName))
			{
			*configFileName = otherFileName;
			return true;
			}
		}

	std::cerr << "Unable to find " << *configFileName << std::endl;
	std::cerr << "  please install it in " << kMainConfigFileDir << std::endl;
	std::cerr << "  or setenv " << kEnvUserConfigFileDir << " to point to it" << std::endl;
	return false;
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp
	(
	const JString& codePath,
	const JString& codeSuffix,
	const JString& headerSuffix,
	const JString& stringPath
	)
{
	std::cout << std::endl;
	std::cout << "This program generates subclasses of JXWindowDirector from" << std::endl;
	std::cout << "the forms stored in an fdesign .fd file." << std::endl;
	std::cout << std::endl;
	std::cout << "The source and header files are assumed to have the same" << std::endl;
	std::cout << "name as the associated form.  Everything delimited by" << std::endl;
	std::cout << std::endl;
	std::cout << "    // begin JXLayout" << std::endl;
	std::cout << "    // end JXLayout" << std::endl;
	std::cout << std::endl;
	std::cout << "will be replaced." << std::endl;
	std::cout << std::endl;
	std::cout << "FL_BOX objects with type NO_BOX count as overloaded objects." << std::endl;
	std::cout << "The label will be used as the class name.  Extra arguments" << std::endl;
	std::cout << "can be included as follows:  'MyWidget(arg1,arg2,'" << std::endl;
	std::cout << "and the standard arguments will be appended at the end." << std::endl;
	std::cout << std::endl;
	std::cout << "Named objects will be instantiated in the header file." << std::endl;
	std::cout << "To designate it as a local variable, put () around it." << std::endl;
	std::cout << "To designate it as a variable that is defined elsewhere in" << std::endl;
	std::cout << "the function, put <> around it." << std::endl;
	std::cout << std::endl;
	std::cout << "Usage:  <options> <fdesign file>" << std::endl;
	std::cout << std::endl;
	std::cout << "-h   prints help" << std::endl;
	std::cout << "-v   prints version information" << std::endl;
	std::cout << "-cp  <code path>     - default: " << codePath << std::endl;
	std::cout << "-cs  <code suffix>   - default: " << codeSuffix << std::endl;
	std::cout << "-hs  <header suffix> - default: " << headerSuffix << std::endl;
	std::cout << "-sp  <string path>   - default: " << stringPath << std::endl;
	std::cout << std::endl;
	std::cout << "--post-cmd           <cmd> - command to exec after all files have been modified" << std::endl;
	std::cout << "--require-obj-names  Fail if any object is missing a name" << std::endl;
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
