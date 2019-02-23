/******************************************************************************
 CBTextFileType.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTextFileType
#define _H_CBTextFileType

#include <JUtf8Character.h>

class JString;
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
	kCBPropertiesFT,		// = kCBLastFT	// special

	// When you add new types, be sure to increment the prefs version!

	kCBFirstFT = kCBUnknownFT,
	kCBLastFT  = kCBPropertiesFT
};

const JSize kCBFTCount = kCBLastFT+1;

std::istream& operator>>(std::istream& input, CBTextFileType& type);
std::ostream& operator<<(std::ostream& output, const CBTextFileType type);

JBoolean			CBGetComplementType(const CBTextFileType inputType, CBTextFileType* outputType);
const JUtf8Byte*	CBGetComplementFileTypeName(const CBTextFileType type);

JBoolean	CBIsCharacterInWord(const CBTextFileType type, const JUtf8Character& c);

inline JBoolean
CBHasComplementType
	(
	const CBTextFileType type
	)
{
	CBTextFileType complType;
	return CBGetComplementType(type, &complType);
}

inline JBoolean
CBIsHeaderType
	(
	const CBTextFileType type
	)
{
	return JI2B(type == kCBCHeaderFT          ||
				type == kCBModula2InterfaceFT ||
				type == kCBModula3InterfaceFT ||
				type == kCBVeraHeaderFT);
}

inline JBoolean
CBCanCompile
	(
	const CBTextFileType type
	)
{
	return JI2B(type == kCBCSourceFT       ||
				type == kCBCSharpFT        ||
				type == kCBLexFT           ||
				type == kCBBisonFT         ||
				type == kCBEiffelFT        ||
				type == kCBErlangFT        ||
				type == kCBFortranFT       ||
				type == kCBRatforFT        ||
				type == kCBJavaSourceFT    ||
				type == kCBJSPFT           ||
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
				type == kCBOtherSourceFT );
}

inline JBoolean
CBIsLibrary
	(
	const CBTextFileType type
	)
{
	return JI2B(type == kCBStaticLibraryFT ||
				type == kCBSharedLibraryFT ||
				type == kCBJavaArchiveFT );
}

inline JBoolean
CBIncludeInMakeFiles
	(
	const CBTextFileType type
	)
{
	return JI2B( CBCanCompile(type) || CBIsLibrary(type) );
}

inline JBoolean
CBIncludeInCMakeSource
	(
	const CBTextFileType type
	)
{
	return JI2B( type == kCBCSourceFT || type == kCBLexFT || type == kCBBisonFT );
}

inline JBoolean
CBIncludeInCMakeHeader
	(
	const CBTextFileType type
	)
{
	return JI2B( type == kCBCHeaderFT );
}

inline JBoolean
CBIncludeInQMakeSource
	(
	const CBTextFileType type
	)
{
	return JI2B( type == kCBCSourceFT || type == kCBLexFT || type == kCBBisonFT );
}

inline JBoolean
CBIncludeInQMakeHeader
	(
	const CBTextFileType type
	)
{
	return JI2B( type == kCBCHeaderFT );
}

inline JBoolean
CBExcludeFromFileList
	(
	const CBTextFileType type
	)
{
	return CBIsLibrary(type);
}

inline JBoolean
CBUseCSourceWindowSize
	(
	const CBTextFileType type
	)
{
	return JI2B(CBCanCompile(type)         ||
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
				type == kCBVimFT );
}

inline JBoolean
CBCanDebug
	(
	const CBTextFileType type
	)
{
	return JI2B( CBCanCompile(type) || type == kCBCHeaderFT );
}

inline JBoolean
CBIsExecOutput
	(
	const CBTextFileType type
	)
{
	return JI2B( type == kCBExecOutputFT ||
				 type == kCBSearchOutputFT);
}

inline JBoolean
CBDrawRightMargin
	(
	const CBTextFileType type
	)
{
	return JNegate( CBIsExecOutput(type)    ||
					type == kCBManPageFT    ||
					type == kCBDiffOutputFT ||
					type == kCBShellOutputFT);
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
	kCBPropertiesLang,	// = kCBLastLang

	// When you add new types, be sure to increment the prefs version!

	kCBFirstLang = kCBOtherLang,
	kCBLastLang  = kCBPropertiesLang
};

const JSize kCBLangCount = kCBLastLang+1;

CBLanguage	CBGetLanguage(const CBTextFileType type);

JBoolean	CBGetStyler(const CBLanguage lang, CBStylerBase** styler);
JBoolean	CBGetCompleter(const CBLanguage lang, CBStringCompleter** completer);

void	CBShutdownStylers();
void	CBShutdownCompleters();

inline JBoolean
CBIsCaseSensitive
	(
	const CBLanguage lang
	)
{
	return JNegate( lang == kCBEiffelLang  ||
					lang == kCBFortranLang ||
					lang == kCBJavaScriptLang );
}

inline JBoolean
CBHasNamespace
	(
	const CBLanguage lang
	)
{
	return JI2B( lang == kCBCLang          ||	// C++, actually
				 lang == kCBJavaLang       ||
				 lang == kCBJavaScriptLang ||
				 lang == kCBPerlLang       ||
				 lang == kCBSQLLang );
}

inline JBoolean
CBGetStyler
	(
	const CBTextFileType	type,
	CBStylerBase**			styler
	)
{
	return CBGetStyler(CBGetLanguage(type), styler);
}

inline JBoolean
CBGetCompleter
	(
	const CBTextFileType	type,
	CBStringCompleter**		completer
	)
{
	return CBGetCompleter(CBGetLanguage(type), completer);
}

#endif
