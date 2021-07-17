/******************************************************************************
 CBTextFileType.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTextFileType
#define _H_CBTextFileType

#include <JString.h>

class CBStylerBase;
class CBStringCompleter;

// Do not change the file type values once they are assigned because
// they are stored in the prefs file.

enum CBTextFileType
{
	kCBUnknownFT = 0,
	kCBCSourceFT,
	kCBCHeaderFT,
	kCBOtherSourceFT,
	kCBDocumentationFT,
	kCBHTMLFT,
	kCBEiffelFT,
	kCBFortranFT,
	kCBJavaSourceFT,
	kCBStaticLibraryFT,
	kCBSharedLibraryFT,
	kCBExecOutputFT,		// special
	kCBManPageFT,			// special
	kCBDiffOutputFT,		// special
	kCBAssemblyFT,
	kCBPascalFT,
	kCBRatforFT,
	kCBExternalFT,			// use other program
	kCBBinaryFT,			// use binary file editor
	kCBModula2ModuleFT,
	kCBModula2InterfaceFT,
	kCBModula3ModuleFT,
	kCBModula3InterfaceFT,
	kCBAWKFT,
	kCBCobolFT,
	kCBLispFT,
	kCBPerlFT,
	kCBPythonFT,
	kCBSchemeFT,
	kCBBourneShellFT,
	kCBTCLFT,
	kCBVimFT,
	kCBJavaArchiveFT,
	kCBPHPFT,
	kCBASPFT,
	kCBSearchOutputFT,		// special
	kCBMakeFT,
	kCBREXXFT,
	kCBRubyFT,
	kCBLexFT,
	kCBCShellFT,
	kCBBisonFT,
	kCBBetaFT,
	kCBLuaFT,
	kCBSLangFT,
	kCBSQLFT,
	kCBVeraSourceFT,
	kCBVeraHeaderFT,
	kCBVerilogFT,
	kCBCSharpFT,
	kCBErlangFT,
	kCBSMLFT,
	kCBJavaScriptFT,
	kCBAntFT,
	kCBJSPFT,
	kCBXMLFT,
	kCBBasicFT,
	kCBShellOutputFT,
	kCBMatlabFT,
	kCBAdobeFlexFT,
	kCBINIFT,
	kCBPropertiesFT,
	kCBGoFT,
	kCBDFT,				// = kCBLastFT	// special

	// When you add new types, be sure to increment the prefs version!

	kCBFirstFT = kCBUnknownFT,
	kCBLastFT  = kCBDFT
};

const JSize kCBFTCount = kCBLastFT+1;

std::istream& operator>>(std::istream& input, CBTextFileType& type);
std::ostream& operator<<(std::ostream& output, const CBTextFileType type);

bool		CBGetComplementType(const CBTextFileType inputType, CBTextFileType* outputType);
const JString&	CBGetComplementFileTypeName(const CBTextFileType type);

bool	CBIsCharacterInWord(const CBTextFileType type, const JUtf8Character& c);

inline bool
CBHasComplementType
	(
	const CBTextFileType type
	)
{
	CBTextFileType complType;
	return CBGetComplementType(type, &complType);
}

inline bool
CBIsHeaderType
	(
	const CBTextFileType type
	)
{
	return type == kCBCHeaderFT          ||
				type == kCBModula2InterfaceFT ||
				type == kCBModula3InterfaceFT ||
				type == kCBVeraHeaderFT;
}

inline bool
CBCanCompile
	(
	const CBTextFileType type
	)
{
	return type == kCBCSourceFT       ||
				type == kCBCSharpFT        ||
				type == kCBDFT             ||
				type == kCBLexFT           ||
				type == kCBBisonFT         ||
				type == kCBEiffelFT        ||
				type == kCBErlangFT        ||
				type == kCBFortranFT       ||
				type == kCBRatforFT        ||
				type == kCBJavaSourceFT    ||
				type == kCBJSPFT           ||
				type == kCBGoFT            ||
				type == kCBPascalFT        ||
				type == kCBAssemblyFT      ||
				type == kCBModula2ModuleFT ||
				type == kCBModula3ModuleFT ||
				type == kCBCobolFT         ||
				type == kCBBetaFT          ||
				type == kCBLuaFT           ||
				type == kCBVeraSourceFT    ||
				type == kCBVerilogFT       ||
				type == kCBBasicFT         ||
				type == kCBOtherSourceFT;
}

inline bool
CBIsLibrary
	(
	const CBTextFileType type
	)
{
	return type == kCBStaticLibraryFT ||
				type == kCBSharedLibraryFT ||
				type == kCBJavaArchiveFT;
}

inline bool
CBIncludeInMakeFiles
	(
	const CBTextFileType type
	)
{
	return CBCanCompile(type) || CBIsLibrary(type);
}

inline bool
CBIncludeInCMakeSource
	(
	const CBTextFileType type
	)
{
	return type == kCBCSourceFT || type == kCBLexFT || type == kCBBisonFT;
}

inline bool
CBIncludeInCMakeHeader
	(
	const CBTextFileType type
	)
{
	return type == kCBCHeaderFT;
}

inline bool
CBIncludeInQMakeSource
	(
	const CBTextFileType type
	)
{
	return type == kCBCSourceFT || type == kCBLexFT || type == kCBBisonFT;
}

inline bool
CBIncludeInQMakeHeader
	(
	const CBTextFileType type
	)
{
	return type == kCBCHeaderFT;
}

inline bool
CBExcludeFromFileList
	(
	const CBTextFileType type
	)
{
	return CBIsLibrary(type);
}

inline bool
CBUseCSourceWindowSize
	(
	const CBTextFileType type
	)
{
	return CBCanCompile(type)         ||
				type == kCBDocumentationFT ||
				type == kCBAntFT           ||
				type == kCBASPFT           ||
				type == kCBAWKFT           ||
				type == kCBBisonFT         ||
				type == kCBINIFT           ||
				type == kCBJavaScriptFT    ||
				type == kCBJSPFT           ||
				type == kCBLexFT           ||
				type == kCBLispFT          ||
				type == kCBMakeFT          ||
				type == kCBPerlFT          ||
				type == kCBPHPFT           ||
				type == kCBPropertiesFT    ||
				type == kCBPythonFT        ||
				type == kCBREXXFT          ||
				type == kCBRubyFT          ||
				type == kCBSchemeFT        ||
				type == kCBBourneShellFT   ||
				type == kCBCShellFT        ||
				type == kCBSLangFT         ||
				type == kCBSMLFT           ||
				type == kCBSQLFT           ||
				type == kCBTCLFT           ||
				type == kCBVimFT;
}

inline bool
CBCanDebug
	(
	const CBTextFileType type
	)
{
	return CBCanCompile(type) || type == kCBCHeaderFT;
}

inline bool
CBIsExecOutput
	(
	const CBTextFileType type
	)
{
	return type == kCBExecOutputFT ||
				 type == kCBSearchOutputFT;
}

inline bool
CBDrawRightMargin
	(
	const CBTextFileType type
	)
{
	return !( CBIsExecOutput(type)    ||
			  type == kCBManPageFT    ||
			  type == kCBDiffOutputFT ||
			  type == kCBShellOutputFT );
}


// Do not change these values once they are assigned because
// they are stored in the project file.

enum CBLanguage
{
	kCBOtherLang = 0,
	kCBCLang,
	kCBEiffelLang,
	kCBFortranLang,
	kCBJavaLang,
	kCBAssemblyLang,
	kCBAWKLang,
	kCBCobolLang,
	kCBLispLang,
	kCBPerlLang,
	kCBPHPLang,
	kCBPythonLang,
	kCBSchemeLang,
	kCBBourneShellLang,
	kCBTCLLang,
	kCBVimLang,
	kCBHTMLLang,
	kCBASPLang,
	kCBMakeLang,
	kCBPascalLang,
	kCBREXXLang,
	kCBRubyLang,
	kCBLexLang,
	kCBCShellLang,
	kCBBisonLang,
	kCBBetaLang,
	kCBLuaLang,
	kCBSLangLang,
	kCBSQLLang,
	kCBVeraLang,
	kCBVerilogLang,
	kCBCSharpLang,
	kCBErlangLang,
	kCBSMLLang,
	kCBJavaScriptLang,
	kCBAntLang,
	kCBJSPLang,
	kCBXMLLang,
	kCBBasicLang,
	kCBMatlabLang,
	kCBAdobeFlexLang,
	kCBINILang,
	kCBPropertiesLang,
	kCBGoLang,
	kCBDLang,		// = kCBLastLang

	// When you add new types, be sure to increment the prefs version!

	kCBFirstLang = kCBOtherLang,
	kCBLastLang  = kCBDLang
};

const JSize kCBLangCount = kCBLastLang+1;

CBLanguage	CBGetLanguage(const CBTextFileType type);

bool	CBGetStyler(const CBLanguage lang, CBStylerBase** styler);
bool	CBGetCompleter(const CBLanguage lang, CBStringCompleter** completer);

void	CBShutdownStylers();
void	CBShutdownCompleters();

bool	CBNameIsQualified(const JString& s);

inline JString::Case
CBIsCaseSensitive
	(
	const CBLanguage lang
	)
{
	return (lang != kCBEiffelLang  &&
			lang != kCBFortranLang &&
			lang != kCBJavaScriptLang ? JString::kCompareCase : JString::kIgnoreCase);
}

inline bool
CBHasNamespace
	(
	const CBLanguage lang
	)
{
	return lang == kCBCLang          ||	// C++, actually
		   lang == kCBDLang          ||
		   lang == kCBGoLang         ||
		   lang == kCBJavaLang       ||
		   lang == kCBJavaScriptLang ||
		   lang == kCBPerlLang       ||
		   lang == kCBSQLLang;
}

inline bool
CBGetStyler
	(
	const CBTextFileType	type,
	CBStylerBase**			styler
	)
{
	return CBGetStyler(CBGetLanguage(type), styler);
}

inline bool
CBGetCompleter
	(
	const CBTextFileType	type,
	CBStringCompleter**		completer
	)
{
	return CBGetCompleter(CBGetLanguage(type), completer);
}

#endif
