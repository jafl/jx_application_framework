/******************************************************************************
 CBTextFileType.cpp

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBTextFileType.h"

#include "CBCStyler.h"
#include "CBDStyler.h"
#include "CBGoStyler.h"
#include "CBHTMLStyler.h"
#include "CBJavaStyler.h"

#ifdef CODE_CRUSADER

#include "CBBourneShellStyler.h"
#include "CBCSharpStyler.h"
#include "CBCShellStyler.h"
#include "CBEiffelStyler.h"
#include "CBINIStyler.h"
#include "CBJavaScriptStyler.h"
#include "CBPerlStyler.h"
#include "CBPropertiesStyler.h"
#include "CBPythonStyler.h"
#include "CBRubyStyler.h"
#include "CBSQLStyler.h"
#include "CBTCLStyler.h"

#include "CBBisonCompleter.h"
#include "CBBourneShellCompleter.h"
#include "CBCCompleter.h"
#include "CBCSharpCompleter.h"
#include "CBCShellCompleter.h"
#include "CBDCompleter.h"
#include "CBEiffelCompleter.h"
#include "CBFortranCompleter.h"
#include "CBGoCompleter.h"
#include "CBHTMLCompleter.h"
#include "CBJavaCompleter.h"
#include "CBJavaScriptCompleter.h"
#include "CBJSPCompleter.h"
#include "CBLexCompleter.h"
#include "CBLuaCompleter.h"
#include "CBMakeCompleter.h"
#include "CBPascalCompleter.h"
#include "CBPerlCompleter.h"
#include "CBPHPCompleter.h"
#include "CBPythonCompleter.h"
#include "CBREXXCompleter.h"
#include "CBRubyCompleter.h"
#include "CBSQLCompleter.h"
#include "CBTCLCompleter.h"

#else

#include "CMStylerStubs.h"
#include <JString.h>

#endif

#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Is character in word

	Returns true if the character is a special part of a word for the
	given language.

 ******************************************************************************/

bool
CBIsCharacterInWord
	(
	const CBTextFileType	type,
	const JUtf8Character&	c
	)
{
	if (type == kCBPerlFT)
		{
		return c == '$' || c == '%' || c == '@';
		}
	else if (type == kCBAWKFT         ||
			 type == kCBPHPFT         ||
			 type == kCBPythonFT      ||
			 type == kCBBourneShellFT ||
			 type == kCBCShellFT      ||
			 type == kCBTCLFT)
		{
		return c == '$';
		}
	else if (type == kCBJavaSourceFT ||
			 type == kCBSQLFT)
		{
		return c == '@';
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 Is character in word

	Returns true if the character is a special part of a word for the
	given language.

 ******************************************************************************/

bool
CBNameIsQualified
	(
	const JString& s
	)
{
	return s.Contains(":") || s.Contains(".") || s.Contains("\\");
}

/******************************************************************************
 Complement file mapping

 ******************************************************************************/

struct CBComplMap
{
	CBTextFileType t1;
	CBTextFileType t2;
};

static const CBComplMap kComplMap[] =
{
	{ kCBCSourceFT,       kCBCHeaderFT },
	{ kCBModula2ModuleFT, kCBModula2InterfaceFT },
	{ kCBModula3ModuleFT, kCBModula3InterfaceFT },
	{ kCBVeraSourceFT,    kCBVeraHeaderFT },
};

const JSize kComplMapCount = sizeof(kComplMap) / sizeof(CBComplMap);

bool
CBGetComplementType
	(
	const CBTextFileType	inputType,
	CBTextFileType*			outputType
	)
{
	for (JUnsignedOffset i=0; i<kComplMapCount; i++)
		{
		if (inputType == kComplMap[i].t1)
			{
			*outputType = kComplMap[i].t2;
			return true;
			}
		if (inputType == kComplMap[i].t2)
			{
			*outputType = kComplMap[i].t1;
			return true;
			}
		}

	*outputType = kCBUnknownFT;
	return false;
}

struct CBComplName
{
	CBTextFileType		type;
	const JUtf8Byte*	name;
};

static const CBComplName kComplName[] =
{
	{ kCBCSourceFT,          "ComplementHeader::CBGlobal"    },
	{ kCBCHeaderFT,          "ComplementSource::CBGlobal"    },
	{ kCBModula2ModuleFT,    "ComplementInterface::CBGlobal" },
	{ kCBModula2InterfaceFT, "ComplementModule::CBGlobal"    },
	{ kCBModula3ModuleFT,    "ComplementInterface::CBGlobal" },
	{ kCBModula3InterfaceFT, "ComplementModule::CBGlobal"    },
	{ kCBVeraSourceFT,       "ComplementHeader::CBGlobal"    },
	{ kCBVeraHeaderFT,       "ComplementSource::CBGlobal"    },
};

const JSize kComplNameCount = sizeof(kComplName) / sizeof(CBComplName);

const JString&
CBGetComplementFileTypeName
	(
	const CBTextFileType type
	)
{
	for (JUnsignedOffset i=0; i<kComplNameCount; i++)
		{
		if (type == kComplName[i].type)
			{
			return JGetString(kComplName[i].name);
			}
		}

	return JString::empty;
}

/******************************************************************************
 CBGetLanguage

 ******************************************************************************/

struct CBFT2Lang
{
	CBTextFileType	type;
	CBLanguage		lang;
};

static const CBFT2Lang kFT2Lang[] =
{
	{ kCBUnknownFT,          kCBOtherLang       },
	{ kCBCSourceFT,          kCBCLang           },
	{ kCBCHeaderFT,          kCBCLang           },
	{ kCBOtherSourceFT,      kCBOtherLang       },
	{ kCBDocumentationFT,    kCBOtherLang       },
	{ kCBHTMLFT,             kCBHTMLLang        },
	{ kCBEiffelFT,           kCBEiffelLang      },
	{ kCBFortranFT,          kCBFortranLang     },
	{ kCBJavaSourceFT,       kCBJavaLang        },
	{ kCBStaticLibraryFT,    kCBOtherLang       },
	{ kCBSharedLibraryFT,    kCBOtherLang       },
	{ kCBExecOutputFT,       kCBOtherLang       },
	{ kCBManPageFT,          kCBOtherLang       },
	{ kCBDiffOutputFT,       kCBOtherLang       },
	{ kCBAssemblyFT,         kCBAssemblyLang    },
	{ kCBPascalFT,           kCBPascalLang      },
	{ kCBRatforFT,           kCBFortranLang     },
	{ kCBExternalFT,         kCBOtherLang       },
	{ kCBBinaryFT,           kCBOtherLang       },
	{ kCBModula2ModuleFT,    kCBOtherLang       },
	{ kCBModula2InterfaceFT, kCBOtherLang       },
	{ kCBModula3ModuleFT,    kCBOtherLang       },
	{ kCBModula3InterfaceFT, kCBOtherLang       },
	{ kCBAWKFT,              kCBAWKLang         },
	{ kCBCobolFT,            kCBCobolLang       },
	{ kCBLispFT,             kCBLispLang        },
	{ kCBPerlFT,             kCBPerlLang        },
	{ kCBPythonFT,           kCBPythonLang      },
	{ kCBSchemeFT,           kCBSchemeLang      },
	{ kCBBourneShellFT,      kCBBourneShellLang },
	{ kCBTCLFT,              kCBTCLLang         },
	{ kCBVimFT,              kCBVimLang         },
	{ kCBJavaArchiveFT,      kCBOtherLang       },
	{ kCBPHPFT,              kCBPHPLang         },
	{ kCBASPFT,              kCBASPLang         },
	{ kCBSearchOutputFT,     kCBOtherLang       },
	{ kCBMakeFT,             kCBMakeLang        },
	{ kCBREXXFT,             kCBREXXLang        },
	{ kCBRubyFT,             kCBRubyLang        },
	{ kCBLexFT,              kCBLexLang         },
	{ kCBCShellFT,           kCBCShellLang      },
	{ kCBBisonFT,            kCBBisonLang       },
	{ kCBBetaFT,             kCBBetaLang        },
	{ kCBLuaFT,              kCBLuaLang         },
	{ kCBSLangFT,            kCBSLangLang       },
	{ kCBSQLFT,              kCBSQLLang         },
	{ kCBVeraSourceFT,       kCBVeraLang        },
	{ kCBVeraHeaderFT,       kCBVeraLang        },
	{ kCBVerilogFT,          kCBVerilogLang     },
	{ kCBCSharpFT,           kCBCSharpLang      },
	{ kCBErlangFT,           kCBErlangLang      },
	{ kCBSMLFT,              kCBSMLLang         },
	{ kCBJavaScriptFT,       kCBJavaScriptLang  },
	{ kCBAntFT,              kCBAntLang         },
	{ kCBJSPFT,              kCBJSPLang         },
	{ kCBXMLFT,              kCBXMLLang         },
	{ kCBBasicFT,            kCBBasicLang       },
	{ kCBShellOutputFT,      kCBOtherLang       },
	{ kCBMatlabFT,           kCBMatlabLang      },
	{ kCBAdobeFlexFT,        kCBAdobeFlexLang   },
	{ kCBINIFT,              kCBINILang         },
	{ kCBPropertiesFT,       kCBPropertiesLang  },
	{ kCBGoFT,               kCBGoLang          },
	{ kCBDFT,                kCBDLang           },
};

const JSize kFT2LangCount = sizeof(kFT2Lang) / sizeof(CBFT2Lang);

CBLanguage
CBGetLanguage
	(
	const CBTextFileType type
	)
{
	assert( kFT2LangCount == kCBFTCount );
	assert( kFT2Lang[type].type == type );
	return kFT2Lang[type].lang;
}

/******************************************************************************
 CBGetStyler

 ******************************************************************************/

struct CBLang2Styler
{
	CBLanguage		lang;
	CBStylerBase*	(*create)();
	void			(*destroy)();
};

static const CBLang2Styler kLang2Styler[] =
{
	{ kCBOtherLang,       nullptr, nullptr },
	{ kCBCLang,           &CBCStyler::Instance,           &CBCStyler::Shutdown },
	{ kCBEiffelLang,      &CBEiffelStyler::Instance,      &CBEiffelStyler::Shutdown },
	{ kCBFortranLang,     nullptr, nullptr },
	{ kCBJavaLang,        &CBJavaStyler::Instance,        &CBJavaStyler::Shutdown },
	{ kCBAssemblyLang,    nullptr, nullptr },
	{ kCBAWKLang,         nullptr, nullptr },
	{ kCBCobolLang,       nullptr, nullptr },
	{ kCBLispLang,        nullptr, nullptr },
	{ kCBPerlLang,        &CBPerlStyler::Instance,        &CBPerlStyler::Shutdown },
	{ kCBPHPLang,         &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBPythonLang,      &CBPythonStyler::Instance,      &CBPythonStyler::Shutdown },
	{ kCBSchemeLang,      nullptr, nullptr },
	{ kCBBourneShellLang, &CBBourneShellStyler::Instance, &CBBourneShellStyler::Shutdown },
	{ kCBTCLLang,         &CBTCLStyler::Instance,         &CBTCLStyler::Shutdown },
	{ kCBVimLang,         nullptr, nullptr },
	{ kCBHTMLLang,        &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBASPLang,         nullptr, nullptr },
	{ kCBMakeLang,        nullptr, nullptr },
	{ kCBPascalLang,      nullptr, nullptr },
	{ kCBREXXLang,        nullptr, nullptr },
	{ kCBRubyLang,        &CBRubyStyler::Instance,        &CBRubyStyler::Shutdown },
	{ kCBLexLang,         nullptr, nullptr },
	{ kCBCShellLang,      &CBCShellStyler::Instance,      &CBCShellStyler::Shutdown },
	{ kCBBisonLang,       &CBCStyler::Instance,           &CBCStyler::Shutdown },
	{ kCBBetaLang,        nullptr, nullptr },
	{ kCBLuaLang,         nullptr, nullptr },
	{ kCBSLangLang,       nullptr, nullptr },
	{ kCBSQLLang,         &CBSQLStyler::Instance,         &CBSQLStyler::Shutdown },
	{ kCBVeraLang,        nullptr, nullptr },
	{ kCBVerilogLang,     nullptr, nullptr },
	{ kCBCSharpLang,      &CBCSharpStyler::Instance,      &CBCSharpStyler::Shutdown },
	{ kCBErlangLang,      nullptr, nullptr },
	{ kCBSMLLang,         nullptr, nullptr },
	{ kCBJavaScriptLang,  &CBJavaScriptStyler::Instance,  &CBJavaScriptStyler::Shutdown },
	{ kCBAntLang,         &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBJSPLang,         &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBXMLLang,         &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBBasicLang,       nullptr, nullptr },
	{ kCBMatlabLang,      nullptr, nullptr },
	{ kCBAdobeFlexLang,   nullptr, nullptr },
	{ kCBINILang,         &CBINIStyler::Instance,         &CBINIStyler::Shutdown },
	{ kCBPropertiesLang,  &CBPropertiesStyler::Instance,  &CBPropertiesStyler::Shutdown },
	{ kCBGoLang,          &CBGoStyler::Instance,          &CBGoStyler::Shutdown },
	{ kCBDLang,           &CBDStyler::Instance,           &CBDStyler::Shutdown },
};

const JSize kLang2StylerCount = sizeof(kLang2Styler) / sizeof(CBLang2Styler);

bool
CBGetStyler
	(
	const CBLanguage	lang,
	CBStylerBase**		styler
	)
{
	assert( kLang2StylerCount == kCBLangCount );
	assert( kLang2Styler[lang].lang == lang );

	if (kLang2Styler[lang].create != nullptr)
		{
		*styler = kLang2Styler[lang].create();
		return *styler != nullptr;
		}
	else
		{
		*styler = nullptr;
		return false;
		}
}

void
CBShutdownStylers()
{
	for (JUnsignedOffset i=0; i<kLang2StylerCount; i++)
		{
		if (kLang2Styler[i].destroy != nullptr)
			{
			kLang2Styler[i].destroy();
			}
		}
}

#ifdef CODE_CRUSADER

/******************************************************************************
 CBGetCompleter

 ******************************************************************************/

struct CBLang2Completer
{
	CBLanguage			lang;
	CBStringCompleter*	(*create)();
	void				(*destroy)();
};

static const CBLang2Completer kLang2Completer[] =
{
	{ kCBOtherLang,       nullptr, nullptr },
	{ kCBCLang,           &CBCCompleter::Instance,           &CBCCompleter::Shutdown },
	{ kCBEiffelLang,      &CBEiffelCompleter::Instance,      &CBEiffelCompleter::Shutdown },
	{ kCBFortranLang,     &CBFortranCompleter::Instance,     &CBFortranCompleter::Shutdown },
	{ kCBJavaLang,        &CBJavaCompleter::Instance,        &CBJavaCompleter::Shutdown },
	{ kCBAssemblyLang,    nullptr, nullptr },
	{ kCBAWKLang,         nullptr, nullptr },
	{ kCBCobolLang,       nullptr, nullptr },
	{ kCBLispLang,        nullptr, nullptr },
	{ kCBPerlLang,        &CBPerlCompleter::Instance,        &CBPerlCompleter::Shutdown },
	{ kCBPHPLang,         &CBPHPCompleter::Instance,         &CBPHPCompleter::Shutdown },
	{ kCBPythonLang,      &CBPythonCompleter::Instance,      &CBPythonCompleter::Shutdown },
	{ kCBSchemeLang,      nullptr, nullptr },
	{ kCBBourneShellLang, &CBBourneShellCompleter::Instance, &CBBourneShellCompleter::Shutdown },
	{ kCBTCLLang,         &CBTCLCompleter::Instance,         &CBTCLCompleter::Shutdown },
	{ kCBVimLang,         nullptr, nullptr },
	{ kCBHTMLLang,        &CBHTMLCompleter::Instance,        &CBHTMLCompleter::Shutdown },
	{ kCBASPLang,         nullptr, nullptr },
	{ kCBMakeLang,        &CBMakeCompleter::Instance,        &CBMakeCompleter::Shutdown },
	{ kCBPascalLang,      &CBPascalCompleter::Instance,      &CBPascalCompleter::Shutdown },
	{ kCBREXXLang,        &CBREXXCompleter::Instance,        &CBREXXCompleter::Shutdown },
	{ kCBRubyLang,        &CBRubyCompleter::Instance,        &CBRubyCompleter::Shutdown },
	{ kCBLexLang,         &CBLexCompleter::Instance,         &CBLexCompleter::Shutdown },
	{ kCBCShellLang,      &CBCShellCompleter::Instance,      &CBCShellCompleter::Shutdown },
	{ kCBBisonLang,       &CBBisonCompleter::Instance,       &CBBisonCompleter::Shutdown },
	{ kCBBetaLang,        nullptr, nullptr },
	{ kCBLuaLang,         &CBLuaCompleter::Instance,         &CBLuaCompleter::Shutdown },
	{ kCBSLangLang,       nullptr, nullptr },
	{ kCBSQLLang,         &CBSQLCompleter::Instance,         &CBSQLCompleter::Shutdown },
	{ kCBVeraLang,        nullptr, nullptr },
	{ kCBVerilogLang,     nullptr, nullptr },
	{ kCBCSharpLang,      &CBCSharpCompleter::Instance,      &CBCSharpCompleter::Shutdown },
	{ kCBErlangLang,      nullptr, nullptr },
	{ kCBSMLLang,         nullptr, nullptr },
	{ kCBJavaScriptLang,  &CBJavaScriptCompleter::Instance,  &CBJavaScriptCompleter::Shutdown },
	{ kCBAntLang,         nullptr, nullptr },
	{ kCBJSPLang,         &CBJSPCompleter::Instance,         &CBJSPCompleter::Shutdown },
	{ kCBXMLLang,         &CBHTMLCompleter::Instance,        &CBHTMLCompleter::Shutdown },
	{ kCBBasicLang,       nullptr, nullptr },
	{ kCBMatlabLang,      nullptr, nullptr },
	{ kCBAdobeFlexLang,   nullptr, nullptr },
	{ kCBINILang,         nullptr, nullptr },
	{ kCBPropertiesLang,  nullptr, nullptr },
	{ kCBGoLang,          &CBGoCompleter::Instance,          &CBGoCompleter::Shutdown },
	{ kCBDLang,           &CBDCompleter::Instance,           &CBDCompleter::Shutdown },
};

const JSize kLang2CompleterCount = sizeof(kLang2Completer) / sizeof(CBLang2Completer);

bool
CBGetCompleter
	(
	const CBLanguage	lang,
	CBStringCompleter**	completer
	)
{
	assert( kLang2CompleterCount == kCBLangCount );
	assert( kLang2Completer[lang].lang == lang );

	if (kLang2Completer[lang].create != nullptr)
		{
		*completer = kLang2Completer[lang].create();
		return *completer != nullptr;
		}
	else
		{
		*completer = nullptr;
		return false;
		}
}

void
CBShutdownCompleters()
{
	for (JUnsignedOffset i=0; i<kLang2CompleterCount; i++)
		{
		if (kLang2Completer[i].destroy != nullptr)
			{
			kLang2Completer[i].destroy();
			}
		}
}

#endif

/******************************************************************************
 Stream operators

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&		input,
	CBTextFileType&	type
	)
{
	long temp;
	input >> temp;
	type = (CBTextFileType) temp;
	assert( kCBFirstFT <= type && type <= kCBLastFT );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&				output,
	const CBTextFileType	type
	)
{
	output << (long) type;
	return output;
}
