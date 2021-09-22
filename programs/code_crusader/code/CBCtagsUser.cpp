/******************************************************************************
 CBCtagsUser.cpp

	Provides interface to ctags.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

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
#include <jAssert.h>

CBCtagsUser::CtagsStatus CBCtagsUser::itsHasExuberantCtagsFlag = CBCtagsUser::kUntested;

static const JString kCheckVersionCmd("ctags --version");
static const JRegex versionPattern =
	"^Exuberant Ctags [^0-9]*([0-9]+)(?:\\.([0-9]+))(?:\\.([0-9]+))?(?:pre([0-9]+))?";

static const JUInt kMinVersion[] = { 5, 8, 0, /* pre */ 0 };

static const JUtf8Byte* kBaseExecCmd =
	"ctags --filter=yes --filter-terminator=\\\f --fields=kzafimsS ";

const JUtf8Byte kDelimiter = '\f';

// language specific information

struct FTInfo
{
	CBTextFileType		fileType;
	CBLanguage			lang;
	const JUtf8Byte*	cmd;
	const JUtf8Byte*	fnTitleID;
};

static const JUtf8Byte* kOtherLangCmd = "--languages=all";

static const FTInfo kFTInfo[] =		// index on CBTextFileType
{
	{ kCBUnknownFT,          kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBCSourceFT,          kCBCLang,           "--language-force=c++"        , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBCHeaderFT,          kCBCLang,           "--language-force=c++"        , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBOtherSourceFT,      kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBDocumentationFT,    kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBHTMLFT,             kCBHTMLLang,        "--language-force=jhtml"      , "IdsMenuTitle::CBCtagsUser"         },
	{ kCBEiffelFT,           kCBEiffelLang,      "--language-force=eiffel"     , "FeaturesMenuTitle::CBCtagsUser"    },
	{ kCBFortranFT,          kCBFortranLang,     "--language-force=fortran"    , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBJavaSourceFT,       kCBJavaLang,        "--language-force=java"       , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBStaticLibraryFT,    kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBSharedLibraryFT,    kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBExecOutputFT,       kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBManPageFT,          kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBDiffOutputFT,       kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBAssemblyFT,         kCBAssemblyLang,    "--language-force=asm"        , "LabelsMenuTitle::CBCtagsUser"      },
	{ kCBPascalFT,           kCBPascalLang,      "--language-force=pascal"     , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBRatforFT,           kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBExternalFT,         kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBBinaryFT,           kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBModula2ModuleFT,    kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBModula2InterfaceFT, kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBModula3ModuleFT,    kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBModula3InterfaceFT, kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBAWKFT,              kCBAWKLang,         "--language-force=awk"        , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBCobolFT,            kCBCobolLang,       "--language-force=cobol"      , "ParagraphsMenuTitle::CBCtagsUser"  },
	{ kCBLispFT,             kCBLispLang,        "--language-force=lisp"       , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBPerlFT,             kCBPerlLang,        "--language-force=perl"       , "SubroutinesMenuTitle::CBCtagsUser" },
	{ kCBPythonFT,           kCBPythonLang,      "--language-force=python"     , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBSchemeFT,           kCBSchemeLang,      "--language-force=scheme"     , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBBourneShellFT,      kCBBourneShellLang, "--language-force=sh"         , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBTCLFT,              kCBTCLLang,         "--language-force=tcl"        , "ProceduresMenuTitle::CBCtagsUser"  },
	{ kCBVimFT,              kCBVimLang,         "--language-force=vim"        , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBJavaArchiveFT,      kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBPHPFT,              kCBPHPLang,         "--language-force=php"        , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBASPFT,              kCBASPLang,         "--language-force=asp"        , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBSearchOutputFT,     kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBMakeFT,             kCBMakeLang,        "--language-force=jmake"      , "TargetsMenuTitle::CBCtagsUser"     },
	{ kCBREXXFT,             kCBREXXLang,        "--language-force=rexx"       , "SubroutinesMenuTitle::CBCtagsUser" },
	{ kCBRubyFT,             kCBRubyLang,        "--language-force=ruby"       , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBLexFT,              kCBLexLang,         "--language-force=jlex"       , "StatesMenuTitle::CBCtagsUser"      },
	{ kCBCShellFT,           kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBBisonFT,            kCBBisonLang,       "--language-force=jbison"     , "SymbolsMenuTitle::CBCtagsUser"     },
	{ kCBBetaFT,             kCBBetaLang,        "--language-force=beta"       , "FragmentsMenuTitle::CBCtagsUser"   },
	{ kCBLuaFT,              kCBLuaLang,         "--language-force=lua"        , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBSLangFT,            kCBSLangLang,       "--language-force=slang"      , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBSQLFT,              kCBSQLLang,         "--language-force=sql"        , "ObjectsMenuTitle::CBCtagsUser"     },
	{ kCBVeraSourceFT,       kCBVeraLang,        "--language-force=vera"       , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBVeraHeaderFT,       kCBVeraLang,        "--language-force=vera"       , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBVerilogFT,          kCBVerilogLang,     "--language-force=verilog"    , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBCSharpFT,           kCBCSharpLang,      "--language-force=c#"         , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBErlangFT,           kCBErlangLang,      "--language-force=erlang"     , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBSMLFT,              kCBSMLLang,         "--language-force=sml"        , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBJavaScriptFT,       kCBJavaScriptLang,  "--language-force=javascript" , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBAntFT,              kCBAntLang,         "--language-force=ant"        , "TargetsMenuTitle::CBCtagsUser"     },
	{ kCBJSPFT,              kCBJSPLang,         "--language-force=javascript" , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBXMLFT,              kCBXMLLang,         kOtherLangCmd                 , nullptr                             },
	{ kCBBasicFT,            kCBBasicLang,       "--language-force=basic"      , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBShellOutputFT,      kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBMatlabFT,           kCBMatlabLang,      "--language-force=matlab"     , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBAdobeFlexFT,        kCBAdobeFlexLang,   "--language-force=flex"       , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBINIFT,              kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBPropertiesFT,       kCBOtherLang,       kOtherLangCmd                 , nullptr                             },
	{ kCBGoFT,               kCBGoLang,          "--language-force=go"         , "FunctionsMenuTitle::CBCtagsUser"   },
	{ kCBDFT,                kCBDLang,           "--language-force=d"          , "FunctionsMenuTitle::CBCtagsUser"   },
};

const JSize kFTCount = sizeof(kFTInfo) / sizeof(FTInfo);

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCtagsUser::CBCtagsUser
	(
	const JUtf8Byte* args
	)
	:
	itsProcess(nullptr),
	itsArgs(args),
	itsCmdPipe(nullptr),
	itsResultFD(ACE_INVALID_HANDLE),
	itsIsActiveFlag(HasExuberantCtags()),
	itsTryRestartFlag(true)
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
	const JUtf8Byte* args
	)
{
	itsArgs = args;
	DeleteProcess();	// force restart
}

/******************************************************************************
 ProcessFile (protected)

	Runs the specified file through ctags.

 ******************************************************************************/

bool
CBCtagsUser::ProcessFile
	(
	const JString&			fileName,
	const CBTextFileType	fileType,
	JString*				result,
	CBLanguage*				lang
	)
{
	if (!StartProcess(fileType, lang))
		{
		result->Clear();
		return false;
		}

	*itsCmdPipe << fileName.GetBytes() << std::endl;

	bool found;
	*result = JReadUntil(itsResultFD, kDelimiter, &found);
	if (found)
		{
		return true;
		}
	else if (itsTryRestartFlag)
		{
		DeleteProcess();

		itsTryRestartFlag = false;
		const bool ok = ProcessFile(fileName, fileType, result, lang);
		itsTryRestartFlag = true;
		return ok;
		}
	else
		{
		DeleteProcess();
		itsIsActiveFlag = false;

		if (!CBInUpdateThread())
			{
			JGetUserNotification()->ReportError(JGetString("ctagsFailure::CBCtagsUser"));
			}

		return false;
		}
}

/******************************************************************************
 StartProcess (private)

 ******************************************************************************/

bool
CBCtagsUser::StartProcess
	(
	const CBTextFileType	fileType,
	CBLanguage*				lang
	)
{
	*lang = kCBOtherLang;

	if (itsIsActiveFlag && itsProcess == nullptr)
		{
		JString cmd(kBaseExecCmd);
		cmd += itsArgs;

		int toFD, fromFD;
		const JError err =
			JProcess::Create(&itsProcess, cmd,
							 kJCreatePipe, &toFD,
							 kJCreatePipe, &fromFD);
		if (err.OK())
			{
			ListenTo(itsProcess);

			itsCmdPipe = jnew JOutPipeStream(toFD, true);
			assert( itsCmdPipe != nullptr );

			itsResultFD = fromFD;
			assert( itsResultFD != ACE_INVALID_HANDLE );

			InitCtags(*itsCmdPipe);
			}
		else
			{
			itsIsActiveFlag = false;
			DeleteProcess();
			}
		}

	if (itsProcess != nullptr)
		{
		assert( kFTCount == kCBFTCount && kFTInfo[fileType].fileType == fileType );

		*lang = kFTInfo[fileType].lang;
		*itsCmdPipe << kFTInfo[fileType].cmd << std::endl;
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
	std::ostream& output
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
	jdelete itsProcess;
	itsProcess = nullptr;

	jdelete itsCmdPipe;
	itsCmdPipe = nullptr;

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
	std::istream&			input,
	JStringPtrMap<JString>*	flags
	)
	const
{
	flags->DeleteAll();

	JIgnoreUntil(input, ";\"\t");			// fluff

	JPtrArray<JString> split(JPtrArrayT::kDeleteAll);
	JString data, key;
	JUtf8Byte delimiter = '\t';
	while (delimiter != '\n')
		{
		if (!JReadUntil(input, 2, "\t\n", &data, &delimiter))
			{
			delimiter = '\n';
			}

		auto* value = jnew JString;
		assert( value != nullptr );

		data.Split(":", &split, 2);
		if (split.GetElementCount() == 2)
			{
			key    = *split.GetElement(1);
			*value = *split.GetElement(2);
			}
		else
			{
			key    = "kind";
			*value = data;
			}

		if (!key.IsEmpty() && !value->IsEmpty())
			{
			flags->SetElement(key, value, JPtrArrayT::kDelete);
			}
		else
			{
			jdelete value;
			}
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
	const CBLanguage				lang,
	const JUtf8Byte					c,
	const JStringPtrMap<JString>&	flags
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
		const JString* value;
		if (c == 'm' && flags.GetElement("interface", &value))
			{
			return kJavaPrototypeST;
			}

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

bool
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

bool
CBCtagsUser::HasExuberantCtags()
{
	if (itsHasExuberantCtagsFlag == kUntested)
		{
		itsHasExuberantCtagsFlag = kFailure;

		// this hack is required on Linux kernel 2.3.x (4/19/2000)
		j_sig_func*	origHandler = signal(SIGCHLD, emptyHandler);

		pid_t pid;

		#if defined _J_SUNOS
		pid_t* ppid = nullptr;
		#else
		pid_t* ppid = &pid;
		#endif

		int fromFD;
		JError err = JExecute(kCheckVersionCmd, ppid,
							  kJIgnoreConnection, nullptr,
							  kJCreatePipe, &fromFD,
							  kJTossOutput, nullptr);
		if (err.OK())
			{
			JString vers;
			JReadAll(fromFD, &vers);

			const JStringMatch m = versionPattern.Match(vers, JRegex::kIncludeSubmatches);
			if (!m.IsEmpty())
				{
				const JSize count = m.GetSubstringCount();
				JString s;
				for (JIndex i=1; i<=count; i++)
					{
					JUInt v = 0;
					s       = m.GetSubstring(i);
					if (!s.IsEmpty())
						{
						const bool ok = s.ConvertToUInt(&v);
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

	return itsHasExuberantCtagsFlag == kSuccess;
}

/******************************************************************************
 IsParsed (static)

 ******************************************************************************/

bool
CBCtagsUser::IsParsed
	(
	const CBTextFileType type
	)
{
	assert( kFTCount == kCBFTCount && kFTInfo[type].fileType == type );

	return kFTInfo[type].lang != kCBOtherLang;
}

/******************************************************************************
 IsParsedForFunctionMenu (static)

 ******************************************************************************/

bool
CBCtagsUser::IsParsedForFunctionMenu
	(
	const CBTextFileType type
	)
{
	assert( kFTCount == kCBFTCount && kFTInfo[type].fileType == type );

	return kFTInfo[type].fnTitleID != nullptr;
}

/******************************************************************************
 GetFunctionMenuTitle (static)

 ******************************************************************************/

static const JString kDefFnTitle("Not parsed");

const JString&
CBCtagsUser::GetFunctionMenuTitle
	(
	const CBTextFileType type
	)
{
	assert( kFTCount == kCBFTCount && kFTInfo[type].fileType == type );

	if (kFTInfo[type].fnTitleID != nullptr)
		{
		return JGetString(kFTInfo[type].fnTitleID);
		}
	else
		{
		return kDefFnTitle;
		}
}
