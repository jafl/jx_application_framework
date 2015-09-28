/******************************************************************************
 CBCtagsUser.cpp

	Provides interface to ctags.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBCtagsUser.h"

#ifdef CODE_CRUSADER
#include "cbGlobals.h"
#elif defined CODE_MEDIC
#include "cmGlobals.h"
#endif

#include <JProcess.h>
#include <JOutPipeStream.h>
#include <JRegex.h>
#include <JString.h>
#include <jStreamUtil.h>
#include <ctype.h>
#include <jAssert.h>

CBCtagsUser::CtagsStatus CBCtagsUser::itsHasExuberantCtagsFlag =
	#ifdef _J_CTAGS_BUG
	CBCtagsUser::kFailure;
	#else
	CBCtagsUser::kUntested;
	#endif

static const JCharacter* kCheckVersionCmd = "ctags --version";
static const JRegex versionPattern =
	"^Exuberant Ctags [^0-9]*([0-9]+)(\\.[0-9]+)(\\.[0-9]+)?(pre[0-9]+)?";

static const JUInt kMinVersion[] = { 5, 8, 0, /* pre */ 0 };

static const JCharacter* kBaseExecCmd =
	"ctags --filter=yes --filter-terminator=\\\f --fields=kzafimsS ";

const JCharacter kDelimiter = '\f';

// language specific information

struct FTInfo
{
	CBTextFileType		fileType;
	CBLanguage			lang;
	const JCharacter*	cmd;
	const JCharacter*	fnTitle;
};

static const JCharacter* kOtherLangCmd = "--languages=all";
static const JCharacter* kDefFnTitle   = "Not parsed";

static const FTInfo kFTInfo[] =		// index on CBTextFileType
{
	{ kCBUnknownFT,          kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBCSourceFT,          kCBCLang,           "--language-force=c++"        , "Functions"   },
	{ kCBCHeaderFT,          kCBCLang,           "--language-force=c++"        , "Functions"   },
	{ kCBOtherSourceFT,      kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBDocumentationFT,    kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBHTMLFT,             kCBHTMLLang,        "--language-force=jhtml"      , "Ids"         },
	{ kCBEiffelFT,           kCBEiffelLang,      "--language-force=eiffel"     , "Features"    },
	{ kCBFortranFT,          kCBFortranLang,     "--language-force=fortran"    , "Functions"   },
	{ kCBJavaSourceFT,       kCBJavaLang,        "--language-force=java"       , "Functions"   },
	{ kCBStaticLibraryFT,    kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBSharedLibraryFT,    kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBExecOutputFT,       kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBManPageFT,          kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBDiffOutputFT,       kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBAssemblyFT,         kCBAssemblyLang,    "--language-force=asm"        , "Labels"      },
	{ kCBPascalFT,           kCBPascalLang,      "--language-force=pascal"     , "Functions"   },
	{ kCBRatforFT,           kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBExternalFT,         kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBBinaryFT,           kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBModula2ModuleFT,    kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBModula2InterfaceFT, kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBModula3ModuleFT,    kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBModula3InterfaceFT, kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBAWKFT,              kCBAWKLang,         "--language-force=awk"        , "Functions"   },
	{ kCBCobolFT,            kCBCobolLang,       "--language-force=cobol"      , "Paragraphs"  },
	{ kCBLispFT,             kCBLispLang,        "--language-force=lisp"       , "Functions"   },
	{ kCBPerlFT,             kCBPerlLang,        "--language-force=perl"       , "Subroutines" },
	{ kCBPythonFT,           kCBPythonLang,      "--language-force=python"     , "Functions"   },
	{ kCBSchemeFT,           kCBSchemeLang,      "--language-force=scheme"     , "Functions"   },
	{ kCBBourneShellFT,      kCBBourneShellLang, "--language-force=sh"         , "Functions"   },
	{ kCBTCLFT,              kCBTCLLang,         "--language-force=tcl"        , "Procedures"  },
	{ kCBVimFT,              kCBVimLang,         "--language-force=vim"        , "Functions"   },
	{ kCBJavaArchiveFT,      kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBPHPFT,              kCBPHPLang,         "--language-force=php"        , "Functions"   },
	{ kCBASPFT,              kCBASPLang,         "--language-force=asp"        , "Functions"   },
	{ kCBSearchOutputFT,     kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBMakeFT,             kCBMakeLang,        "--language-force=jmake"      , "Targets"     },
	{ kCBREXXFT,             kCBREXXLang,        "--language-force=rexx"       , "Subroutines" },
	{ kCBRubyFT,             kCBRubyLang,        "--language-force=ruby"       , "Functions"   },
	{ kCBLexFT,              kCBLexLang,         "--language-force=jlex"       , "States"      },
	{ kCBCShellFT,           kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBBisonFT,            kCBBisonLang,       "--language-force=jbison"     , "Symbols"     },
	{ kCBBetaFT,             kCBBetaLang,        "--language-force=beta"       , "Fragments"   },
	{ kCBLuaFT,              kCBLuaLang,         "--language-force=lua"        , "Functions"   },
	{ kCBSLangFT,            kCBSLangLang,       "--language-force=slang"      , "Functions"   },
	{ kCBSQLFT,              kCBSQLLang,         "--language-force=sql"        , "Functions"   },
	{ kCBVeraSourceFT,       kCBVeraLang,        "--language-force=vera"       , "Functions"   },
	{ kCBVeraHeaderFT,       kCBVeraLang,        "--language-force=vera"       , "Functions"   },
	{ kCBVerilogFT,          kCBVerilogLang,     "--language-force=verilog"    , "Functions"   },
	{ kCBCSharpFT,           kCBCSharpLang,      "--language-force=c#"         , "Functions"   },
	{ kCBErlangFT,           kCBErlangLang,      "--language-force=erlang"     , "Functions"   },
	{ kCBSMLFT,              kCBSMLLang,         "--language-force=sml"        , "Functions"   },
	{ kCBJavaScriptFT,       kCBJavaScriptLang,  "--language-force=javascript" , "Functions"   },
	{ kCBAntFT,              kCBAntLang,         "--language-force=ant"        , "Targets"     },
	{ kCBJSPFT,              kCBJSPLang,         "--language-force=javascript" , "Functions"   },
	{ kCBXMLFT,              kCBXMLLang,         kOtherLangCmd                 , kDefFnTitle   },
	{ kCBBasicFT,            kCBBasicLang,       "--language-force=basic"      , "Functions"   },
	{ kCBShellOutputFT,      kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBMatlabFT,           kCBMatlabLang,      "--language-force=matlab"     , "Functions"   },
	{ kCBAdobeFlexFT,        kCBAdobeFlexLang,   "--language-force=flex"       , "Functions"   },
	{ kCBINIFT,              kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
	{ kCBPropertiesFT,       kCBOtherLang,       kOtherLangCmd                 , kDefFnTitle   },
};

const JSize kFTCount = sizeof(kFTInfo) / sizeof(FTInfo);

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCtagsUser::CBCtagsUser
	(
	const JCharacter* args
	)
	:
	itsProcess(NULL),
	itsArgs(args),
	itsCmdPipe(NULL),
	itsResultFD(ACE_INVALID_HANDLE),
	itsIsActiveFlag(HasExuberantCtags()),
	itsTryRestartFlag(kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCtagsUser::~CBCtagsUser()
{
	DeleteProcess();
}

/******************************************************************************
 SetCtagsArgs (protected)

	Set the arguments passed to ctags.

 ******************************************************************************/

void
CBCtagsUser::SetCtagsArgs
	(
	const JCharacter* args
	)
{
	itsArgs = args;
	DeleteProcess();	// force restart
}

/******************************************************************************
 ProcessFile (protected)

	Runs the specified file through ctags.

 ******************************************************************************/

JBoolean
CBCtagsUser::ProcessFile
	(
	const JCharacter*		fileName,
	const CBTextFileType	fileType,
	JString*				result,
	CBLanguage*				lang
	)
{
	if (!StartProcess(fileType, lang))
		{
		result->Clear();
		return kJFalse;
		}

	*itsCmdPipe << fileName << endl;

	JBoolean found;
	*result = JReadUntil(itsResultFD, kDelimiter, &found);
	if (found)
		{
		return kJTrue;
		}
	else if (itsTryRestartFlag)
		{
		DeleteProcess();

		itsTryRestartFlag = kJFalse;
		const JBoolean ok = ProcessFile(fileName, fileType, result, lang);
		itsTryRestartFlag = kJTrue;
		return ok;
		}
	else
		{
		DeleteProcess();
		itsIsActiveFlag = kJFalse;

		if (!CBInUpdateThread())
			{
			(JGetUserNotification())->ReportError(
				"ctags is not responding correctly, "
				"so some features will not work.");
			}

		return kJFalse;
		}
}

/******************************************************************************
 StartProcess (private)

 ******************************************************************************/

JBoolean
CBCtagsUser::StartProcess
	(
	const CBTextFileType	fileType,
	CBLanguage*				lang
	)
{
	*lang = kCBOtherLang;

	if (itsIsActiveFlag && itsProcess == NULL)
		{
		JString cmd = kBaseExecCmd;
		cmd += itsArgs;

		int toFD, fromFD;
		const JError err =
			JProcess::Create(&itsProcess, cmd,
							 kJCreatePipe, &toFD,
							 kJCreatePipe, &fromFD);
		if (err.OK())
			{
			ListenTo(itsProcess);

			itsCmdPipe = new JOutPipeStream(toFD, kJTrue);
			assert( itsCmdPipe != NULL );

			itsResultFD = fromFD;
			assert( itsResultFD != ACE_INVALID_HANDLE );

			InitCtags(*itsCmdPipe);
			}
		else
			{
			itsIsActiveFlag = kJFalse;
			DeleteProcess();
			}
		}

	if (itsProcess != NULL)
		{
		assert( kFTCount == kCBFTCount && kFTInfo[fileType].fileType == fileType );

		*lang = kFTInfo[fileType].lang;
		*itsCmdPipe << kFTInfo[fileType].cmd << endl;
		}

	return itsIsActiveFlag;
}

/******************************************************************************
 InitCtags (virtual protected)

	Derived classes can pass additional info to ctags.

 ******************************************************************************/

void
CBCtagsUser::InitCtags
	(
	ostream& output
	)
{
}

/******************************************************************************
 DeleteProcess (protected)

	Closing itsCmdPipe causes ctags to quit automatically.

 ******************************************************************************/

void
CBCtagsUser::DeleteProcess()
{
	delete itsProcess;
	itsProcess = NULL;

	delete itsCmdPipe;
	itsCmdPipe = NULL;

	close(itsResultFD);
	itsResultFD = ACE_INVALID_HANDLE;
}

/******************************************************************************
 ReadExtensionFlags (protected)

	Parses the extension flags.

 ******************************************************************************/

void
CBCtagsUser::ReadExtensionFlags
	(
	istream&				input,
	JStringPtrMap<JString>*	flags
	)
	const
{
	flags->DeleteAll();

	JIgnoreUntil(input, ";\"\t");			// fluff

	JString data, key;
	JCharacter delimiter = '\t';
	while (delimiter != '\n')
		{
		if (!JReadUntil(input, 2, "\t\n", &data, &delimiter))
			{
			delimiter = '\n';
			}

		JString* value = new JString;
		assert( value != NULL );

		JIndex colonIndex;
		if (data.LocateSubstring(":", &colonIndex))
			{
			if (colonIndex > 1)
				{
				key = data.GetSubstring(1, colonIndex-1);
				}
			else
				{
				key.Clear();
				}

			if (colonIndex < data.GetLength())
				{
				*value = data.GetSubstring(colonIndex+1, data.GetLength());
				}
			}
		else
			{
			key    = "kind";
			*value = data;
			}

		flags->SetElement(key, value, JPtrArrayT::kDelete);
		}
}

/******************************************************************************
 DecodeSymbolType (protected)

	Adobe Flash
		f   functions
		c   classes
		m   methods
		p   properties
		v   global variables
		x   mxtags

	Ant
		t   target

	Assembly
		d   defines
		l   labels
		m   macros
		t	types

	ASP
		d	constants
		c	classes
		f   functions
		s   subroutines
		v	variables

	AWK
		f   functions

	Basic
		c	constants
		f   functions
		l   labels
		t   types
		v   variables
		g   enumerations

	Beta
		f	fragment definitions
		p	all patterns [off]
		s	slots (fragment uses)
		v	patterns (only virtual or rebound patterns are recorded)

	Bison (jbison defined in CBFnMenuUpdater,CBSymbolList)
		N   non-terminal symbol definition
		n   non-terminal symbol declaration
		t   terminal symbol (token) declaration

	C/C++
		c   classes
		d   macro definitions (and #undef names)
		e   enumerators
		f   function definitions
		g   enumeration names
		l   local variables [off]
		m   class, struct, or union members
		n   namespaces
		p   function prototypes and declarations
		s   structure names
		t   typedefs
		u   union names
		v   variable definitions
		x   extern and forward variable declarations [off]

	C#
		c	classes
		d	macro definitions
		e	enumerators (values inside an enumeration)
		E	events
		f	fields
		g	enumeration names
		i	interfaces
		l	local variables [off]
		m	methods
		n	namespaces
		p	properties
		s	structure names
		t	typedefs

	Cobol
		d	data
		f	file
		g	group
		p   paragraphs
		P	program
		s	section

	Eiffel
		c   classes
		f   features
		l   local entities [off]

	Erlang
		d	macro definitions
		f	functions
		m	modules
		r	record definitions

	FORTRAN
		b   block data
		c   common blocks
		e   entry points
		f   functions
		i   interfaces
		k   type components
		l   labels [off]
		L   local and common block variables [off]
		m   modules
		n   namelists
		p   programs
		s   subroutines
		t   derived types
		v   module variables

	Java
		c   classes
		e   enum constants
		f   fields
		g   enum types
		i   interfaces
		l   local variables [off]
		m   methods
		p   packages

	JavaScript
		f   functions
		c   classes
		m   methods
		v   global variables

	Lex (jlex defined in CBFnMenuUpdater,CBSymbolList)
		s   state

	Lisp
		f   functions

	Lua
		f	functions

	Makefile (jmake defined in CBFnMenuUpdater,CBSymbolList)
		t   target
		v   variable

	Matlab
		f   function

	Pascal
		f   functions
		p   procedures

	Perl
		c	constants
		f   formats
		l	labels
	#	p	packages
		s   subroutines
		d   subroutine declarations [off]

	PHP
		c   classes
		i   interfaces
		d   constant definitions
		f   functions
		j   javascript functions

	Python
		c   classes
		f   functions
		m	class member

	REXX
		s   subroutines

	Ruby
		c   classes
		f   methods
		F	singleton methods
		m   mixins

	Scheme
		f   functions
		s   sets

	Bourne Shell
		f   functions

	SLang
		f	functions
		n	namespaces

	SML
		e	exception declarations
		f	function definitions
		c	functor definitions
		s	signature declarations
		r	structure declarations
		t	type definitions
		v	value bindings

	SQL
		c	cursors
		d	prototypes [off]
		f	functions
		F	record fields
		l	local variables [off]
		L	block label
		P	packages
		p	procedures
		r	records
		s	subtypes
		t	tables
		T	triggers
		v	variables
		i   indexes
		e   events
		U   publications
		R   services
		D   domains
		V   views
		n   synonyms
		x   MobiLink Table Scripts
		y   MobiLink Conn Scripts

	TCL
		c	classes
		m	methods
		p   procedures

	Vera
		c	classes
		d	macro definitions
		e	enumerators
		f	function definitions
		g	enumeration names
		m	class, struct, and union members
		p	programs
		P	function prototypes [off]
		t	tasks
		T	typedefs
		v	variable definitions
		x	external variable declarations [off]

	Verilog
		c	constants (define, parameter, specparam)
		e	events
		f	functions
		m	modules
		n	net data types
		p	ports (input, output, inout)
		r	register data types
		t	tasks
	#	v	variables(integer, real, time)
	#	w	wires (supply, tri, wire, wand, ...)

	Vim
		a	autocommand group
		c   user-defined commands
		f   functions
		m   maps
		v	variables

 ******************************************************************************/

CBCtagsUser::Type
CBCtagsUser::DecodeSymbolType
	(
	const CBLanguage	lang,
	const JCharacter	c
	)
	const
{
	if (lang == kCBAdobeFlexLang)
		{
		switch (c)
			{
			case 'f':  return kAdobeFlexFunctionST;
			case 'c':  return kAdobeFlexClassST;
			case 'm':  return kAdobeFlexMethodST;
			case 'v':  return kAdobeFlexGlobalVariableST;
			case 'x':  return kAdobeFlexMxTagST;
			}
		}

	else if (lang == kCBAntLang)
		{
		switch (c)
			{
			case 't':  return kAntTargetST;
			}
		}

	else if (lang == kCBAssemblyLang)
		{
		switch (c)
			{
			case 'd':  return kAssemblyDefineST;
			case 'l':  return kAssemblyLabelST;
			case 'm':  return kAssemblyMacroST;
			case 't':  return kAssemblyTypeST;
			}
		}

	else if (lang == kCBASPLang)
		{
		switch (c)
			{
			case 'd':  return kASPConstantST;
			case 'c':  return kASPClassST;
			case 'f':  return kASPFunctionST;
			case 's':  return kASPSubroutineST;
			case 'v':  return kASPVariableST;
			}
		}

	else if (lang == kCBAWKLang)
		{
		switch (c)
			{
			case 'f':  return kAWKFunctionST;
			}
		}

	else if (lang == kCBBasicLang)
		{
		switch (c)
			{
			case 'c':  return kBasicConstantST;
			case 'f':  return kBasicFunctionST;
			case 'l':  return kBasicLabelST;
			case 't':  return kBasicTypeST;
			case 'v':  return kBasicVariableST;
			case 'g':  return kBasicEnumerationST;
			}
		}

	else if (lang == kCBBetaLang)
		{
		switch (c)
			{
			case 'f':  return kBetaFragmentST;
			case 'p':  return kBetaPatternST;
			case 's':  return kBetaSlotST;
			case 'v':  return kBetaPatternST;
			}
		}

	else if (lang == kCBBisonLang)
		{
		switch (c)
			{
			case 'N':  return kBisonNonterminalDefST;
			case 'n':  return kBisonNonterminalDeclST;
			case 't':  return kBisonTerminalDeclST;
			}
		}

	else if (lang == kCBCLang)
		{
		switch (c)
			{
			case 'c':  return kCClassST;
			case 'd':  return kCMacroST;
			case 'e':  return kCEnumValueST;
			case 'f':  return kCFunctionST;
			case 'g':  return kCEnumST;
			case 'm':  return kCMemberST;
			case 'n':  return kCNamespaceST;
			case 'p':  return kCPrototypeST;
			case 's':  return kCStructST;
			case 't':  return kCTypedefST;
			case 'u':  return kCUnionST;
			case 'v':  return kCVariableST;
			case 'x':  return kCExternVariableST;
			}
		}

	else if (lang == kCBCSharpLang)
		{
		switch (c)
			{
			case 'c':  return kCSharpClassST;
			case 'd':  return kCSharpMacroST;
			case 'e':  return kCSharpEnumValueST;
			case 'E':  return kCSharpEventST;
			case 'f':  return kCSharpFieldST;
			case 'g':  return kCSharpEnumNameST;
			case 'i':  return kCSharpInterfaceST;
			case 'm':  return kCSharpMethodST;
			case 'n':  return kCSharpNamespaceST;
			case 'p':  return kCSharpPropertyST;
			case 's':  return kCSharpStructNameST;
			case 't':  return kCSharpTypedefST;
			}
		}

	else if (lang == kCBCobolLang)
		{
		switch (c)
			{
			case 'd':  return kCobolDataST;
			case 'f':  return kCobolFileST;
			case 'g':  return kCobolGroupST;
			case 'p':  return kCobolParagraphST;
			case 'P':  return kCobolProgramST;
			case 's':  return kCobolSectionST;
			}
		}

	else if (lang == kCBEiffelLang)
		{
		switch (c)
			{
			case 'c':  return kEiffelClassST;
			case 'f':  return kEiffelFeatureST;
			}
		}

	else if (lang == kCBErlangLang)
		{
		switch (c)
			{
			case 'd':  return kErlangMacroST;
			case 'f':  return kErlangFunctionST;
			case 'm':  return kErlangModuleST;
			case 'r':  return kErlangRecordST;
			}
		}

	else if (lang == kCBFortranLang)
		{
		switch (c)
			{
			case 'b':  return kFortranBlockDataST;
			case 'c':  return kFortranCommonBlockST;
			case 'e':  return kFortranEntryPointST;
			case 'f':  return kFortranFunctionST;
			case 'i':  return kFortranInterfaceST;
			case 'k':  return kFortranTypeComponentST;
			case 'l':  return kFortranLabelST;
			case 'L':  return kFortranLocalVariableST;
			case 'm':  return kFortranModuleST;
			case 'n':  return kFortranNamelistST;
			case 'p':  return kFortranProgramST;
			case 's':  return kFortranSubroutineST;
			case 't':  return kFortranDerivedTypeST;
			case 'v':  return kFortranModuleVariableST;
			}
		}

	else if (lang == kCBJavaLang || lang == kCBJSPLang)
		{
		switch (c)
			{
			case 'c':  return kJavaClassST;
			case 'e':  return kJavaEnumValueST;
			case 'f':  return kJavaFieldST;
			case 'g':  return kJavaEnumST;
			case 'i':  return kJavaInterfaceST;
			case 'm':  return kJavaMethodST;
			case 'p':  return kJavaPackageST;
			}
		}

	else if (lang == kCBJavaScriptLang)
		{
		switch (c)
			{
			case 'f':  return kJavaScriptFunctionST;
			case 'c':  return kJavaScriptClassST;
			case 'm':  return kJavaScriptMethodST;
			case 'v':  return kJavaScriptGlobalVariableST;
			}
		}

	else if (lang == kCBLexLang)
		{
		switch (c)
			{
			case 's':  return kLexStateST;
			}
		}

	else if (lang == kCBLispLang)
		{
		switch (c)
			{
			case 'f':  return kLispFunctionST;
			}
		}

	else if (lang == kCBLuaLang)
		{
		switch (c)
			{
			case 'f':  return kLuaFunctionST;
			}
		}

	else if (lang == kCBMakeLang)
		{
		switch (c)
			{
			case 't':  return kMakeTargetST;
			case 'v':  return kMakeVariableST;
			}
		}

	else if (lang == kCBMatlabLang)
		{
		switch (c)
			{
			case 'f':  return kMatlabFunctionST;
			}
		}

	else if (lang == kCBPascalLang)
		{
		switch (c)
			{
			case 'f':  return kPascalFunctionST;
			case 'p':  return kPascalProcedureST;
			}
		}

	else if (lang == kCBPerlLang)
		{
		switch (c)
			{
			case 'c':  return kPerlConstantST;
			case 'f':  return kPerlFormatST;
			case 'l':  return kPerlLabelST;
			case 'p':  return kPerlPackageST;
			case 's':  return kPerlSubroutineST;
			}
		}

	else if (lang == kCBPHPLang)
		{
		switch (c)
			{
			case 'c':  return kPHPClassST;
			case 'i':  return kPHPInterfaceST;
			case 'd':  return kPHPDefineST;
			case 'f':  return kPHPFunctionST;
			case 'j':  return kJavaScriptFunctionST;
			}
		}

	else if (lang == kCBPythonLang)
		{
		switch (c)
			{
			case 'c':  return kPythonClassST;
			case 'f':  return kPythonFunctionST;
			case 'm':  return kPythonClassMemberST;
			}
		}

	else if (lang == kCBREXXLang)
		{
		switch (c)
			{
			case 's':  return kREXXSubroutineST;
			}
		}

	else if (lang == kCBRubyLang)
		{
		switch (c)
			{
			case 'c':  return kRubyClassST;
			case 'f':  return kRubyMethodST;
			case 'F':  return kRubySingletonMethodST;
			case 'm':  return kRubyMixinST;
			}
		}

	else if (lang == kCBSchemeLang)
		{
		switch (c)
			{
			case 'f':  return kSchemeFunctionST;
			case 's':  return kSchemeSetST;
			}
		}

	else if (lang == kCBBourneShellLang)
		{
		switch (c)
			{
			case 'f':  return kBourneShellFunctionST;
			}
		}

	else if (lang == kCBSLangLang)
		{
		switch (c)
			{
			case 'f':  return kSLangFunctionST;
			case 'n':  return kSLangNamespaceST;
			}
		}

	else if (lang == kCBSMLLang)
		{
		switch (c)
			{
			case 'e':  return kSMLExceptionST;
			case 'f':  return kSMLFunctionST;
			case 'c':  return kSMLFunctorST;
			case 's':  return kSMLSignatureST;
			case 'r':  return kSMLStructureST;
			case 't':  return kSMLTypeST;
			case 'v':  return kSMLValueST;
			}
		}

	else if (lang == kCBSQLLang)
		{
		switch (c)
			{
			case 'c':  return kSQLCursorST;
			case 'd':  return kSQLPrototypeST;
			case 'f':  return kSQLFunctionST;
			case 'F':  return kSQLRecordFieldST;
			case 'l':  return kSQLLocalVariableST;
			case 'L':  return kSQLLabelST;
			case 'P':  return kSQLPackageST;
			case 'p':  return kSQLProcedureST;
			case 'r':  return kSQLRecordST;
			case 's':  return kSQLSubtypeST;
			case 't':  return kSQLTableST;
			case 'T':  return kSQLTriggerST;
			case 'v':  return kSQLVariableST;
			case 'i':  return kSQLIndexST;
			case 'e':  return kSQLEventST;
			case 'U':  return kSQLPublicationST;
			case 'R':  return kSQLServiceST;
			case 'D':  return kSQLDomainST;
			case 'V':  return kSQLViewST;
			case 'n':  return kSQLSynonymST;
			case 'x':  return kSQLMobiLinkTableScriptST;
			case 'y':  return kSQLMobiLinkConnScriptST;
			}
		}

	else if (lang == kCBTCLLang)
		{
		switch (c)
			{
			case 'c':  return kTCLClassST;
			case 'm':  return kTCLMethodST;
			case 'p':  return kTCLProcedureST;
			}
		}

	else if (lang == kCBVeraLang)
		{
		switch (c)
			{
			case 'c':  return kVeraClassST;
			case 'd':  return kVeraMacroST;
			case 'e':  return kVeraEnumValueST;
			case 'f':  return kVeraFunctionST;
			case 'g':  return kVeraEnumST;
			case 'm':  return kVeraMemberST;
			case 'p':  return kVeraProgramST;
			case 'P':  return kVeraPrototypeST;
			case 't':  return kVeraTaskST;
			case 'T':  return kVeraTypedefST;
			case 'v':  return kVeraVariableST;
			case 'x':  return kVeraExternalVariableST;
			}
		}

	else if (lang == kCBVerilogLang)
		{
		switch (c)
			{
			case 'c':  return kVerilogConstantST;
			case 'e':  return kVerilogEventST;
			case 'f':  return kVerilogFunctionST;
			case 'm':  return kVerilogModuleST;
			case 'n':  return kVerilogNetST;
			case 'p':  return kVerilogPortST;
			case 'r':  return kVerilogRegisterST;
			case 't':  return kVerilogTaskST;
//			case 'v':  return kVerilogVariableST;
//			case 'w':  return kVerilogWireST;
			}
		}

	else if (lang == kCBVimLang)
		{
		switch (c)
			{
			case 'a':  return kVimAutocommandGroupST;
			case 'c':  return kVimUserCommandST;
			case 'f':  return kVimFunctionST;
			case 'm':  return kVimMapST;
			case 'v':  return kVimVariableST;
			}
		}

	return kUnknownST;
}

/******************************************************************************
 IgnoreSymbol (static)

	Fully qualified, global symbols (::qsort) are pointless.

 ******************************************************************************/

JBoolean
CBCtagsUser::IgnoreSymbol
	(
	const JString& s
	)
{
	return s.BeginsWith("::");
}

/******************************************************************************
 Receive (virtual protected)

	If the process dies, we delete the object so the process will
	be restarted next time.

 ******************************************************************************/

void
CBCtagsUser::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		DeleteProcess();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 HasExuberantCtags (static)

 ******************************************************************************/

// Interrupt routine

// prototypes

static void emptyHandler(int sig);

// functions

void emptyHandler(int sig)
{
}

JBoolean
CBCtagsUser::HasExuberantCtags()
{
	if (itsHasExuberantCtagsFlag == kUntested)
		{
		itsHasExuberantCtagsFlag = kFailure;

		// this hack is required on Linux kernel 2.3.x (4/19/2000)
		j_sig_func*	origHandler = signal(SIGCHLD, emptyHandler);

		pid_t pid;

		#if defined _J_SUNOS
		pid_t* ppid = NULL;
		#else
		pid_t* ppid = &pid;
		#endif

		int fromFD;
		JError err = JExecute(kCheckVersionCmd, ppid,
							  kJIgnoreConnection, NULL,
							  kJCreatePipe, &fromFD,
							  kJTossOutput, NULL);
		if (err.OK())
			{
			JString vers;
			JReadAll(fromFD, &vers);

			JArray<JIndexRange> matchList;
			if (versionPattern.Match(vers, &matchList))
				{
				matchList.RemoveElement(1);

				const JSize count = matchList.GetElementCount();
				JString s;
				for (JIndex i=1; i<=count; i++)
					{
					JUInt v = 0;
					const JIndexRange r = matchList.GetElement(i);
					if (!r.IsEmpty())
						{
						s = vers.GetSubstring(r);
						while (!isdigit(s.GetFirstCharacter()))
							{
							s.RemoveSubstring(1, 1);
							}
						const JBoolean ok = s.ConvertToUInt(&v);
						assert( ok );
						}

					if (v > kMinVersion[i-1] ||
						(i == count && v == kMinVersion[i-1]))
						{
						itsHasExuberantCtagsFlag = kSuccess;
						break;
						}
					else if (v < kMinVersion[i-1])
						{
						break;
						}
					}
				}
			}

		if (origHandler != SIG_ERR)
			{
			signal(SIGCHLD, origHandler);
			}
		}

	return JI2B( itsHasExuberantCtagsFlag == kSuccess );
}

/******************************************************************************
 IsParsed (static)

 ******************************************************************************/

JBoolean
CBCtagsUser::IsParsed
	(
	const CBTextFileType type
	)
{
	assert( kFTCount == kCBFTCount && kFTInfo[type].fileType == type );

	return JI2B( kFTInfo[type].lang != kCBOtherLang );
}

/******************************************************************************
 IsParsedForFunctionMenu (static)

 ******************************************************************************/

JBoolean
CBCtagsUser::IsParsedForFunctionMenu
	(
	const CBTextFileType type
	)
{
	assert( kFTCount == kCBFTCount && kFTInfo[type].fileType == type );

	return JI2B( kFTInfo[type].fnTitle != kDefFnTitle );
}

/******************************************************************************
 GetFunctionMenuTitle (static)

 ******************************************************************************/

const JCharacter*
CBCtagsUser::GetFunctionMenuTitle
	(
	const CBTextFileType type
	)
{
	assert( kFTCount == kCBFTCount && kFTInfo[type].fileType == type );

	return kFTInfo[type].fnTitle;
}
