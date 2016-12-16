/******************************************************************************
 CBTextFileType.cpp

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBTextFileType.h"

#include "CBCStyler.h"
#include "CBJavaStyler.h"
#include "CBHTMLStyler.h"

#ifdef CODE_CRUSADER

#include "CBCSharpStyler.h"
#include "CBEiffelStyler.h"
#include "CBPerlStyler.h"
#include "CBPythonStyler.h"
#include "CBBourneShellStyler.h"
#include "CBCShellStyler.h"
#include "CBTCLStyler.h"
#include "CBJavaScriptStyler.h"
#include "CBRubyStyler.h"
#include "CBINIStyler.h"
#include "CBPropertiesStyler.h"

#include "CBBisonCompleter.h"
#include "CBCCompleter.h"
#include "CBEiffelCompleter.h"
#include "CBFortranCompleter.h"
#include "CBHTMLCompleter.h"
#include "CBJavaCompleter.h"
#include "CBLexCompleter.h"
#include "CBLuaCompleter.h"
#include "CBMakeCompleter.h"
#include "CBPascalCompleter.h"
#include "CBPerlCompleter.h"
#include "CBPHPCompleter.h"
#include "CBPythonCompleter.h"
#include "CBREXXCompleter.h"
#include "CBRubyCompleter.h"
#include "CBBourneShellCompleter.h"
#include "CBCShellCompleter.h"
#include "CBSQLCompleter.h"
#include "CBTCLCompleter.h"
#include "CBCSharpCompleter.h"
#include "CBJavaScriptCompleter.h"
#include "CBJSPCompleter.h"

#else

#include "CMStylerStubs.h"
#include <JString.h>

#endif

#include <jAssert.h>

/******************************************************************************
 Is character in word

	Returns kJTrue if the character is a special part of a word for the
	given language.

 ******************************************************************************/

JBoolean
CBIsCharacterInWord
	(
	const CBTextFileType	type,
	const JString&			text,
	const JIndex			charIndex
	)
{
	const JCharacter c = text.GetCharacter(charIndex);
	if (type == kCBPerlFT)
		{
		return JI2B(c == '$' || c == '%' || c == '@');
		}
	else if (type == kCBAWKFT         ||
			 type == kCBPHPFT         ||
			 type == kCBPythonFT      ||
			 type == kCBBourneShellFT ||
			 type == kCBCShellFT      ||
			 type == kCBSQLFT         ||
			 type == kCBTCLFT)
		{
		return JI2B(c == '$');
		}
	else if (type == kCBJavaSourceFT)
		{
		return JI2B(c == '@');
		}
	else
		{
		return kJFalse;
		}
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

JBoolean
CBGetComplementType
	(
	const CBTextFileType	inputType,
	CBTextFileType*			outputType
	)
{
	for (JIndex i=0; i<kComplMapCount; i++)
		{
		if (inputType == kComplMap[i].t1)
			{
			*outputType = kComplMap[i].t2;
			return kJTrue;
			}
		if (inputType == kComplMap[i].t2)
			{
			*outputType = kComplMap[i].t1;
			return kJTrue;
			}
		}

	*outputType = kCBUnknownFT;
	return kJFalse;
}

struct CBComplName
{
	CBTextFileType		type;
	const JCharacter*	name;
};

static const CBComplName kComplName[] =
{
	{ kCBCSourceFT,          "Header"    },
	{ kCBCHeaderFT,          "Source"    },
	{ kCBModula2ModuleFT,    "Interface" },
	{ kCBModula2InterfaceFT, "Module"    },
	{ kCBModula3ModuleFT,    "Interface" },
	{ kCBModula3InterfaceFT, "Module"    },
	{ kCBVeraSourceFT,       "Header"    },
	{ kCBVeraHeaderFT,       "Source"    },
};

const JSize kComplNameCount = sizeof(kComplName) / sizeof(CBComplName);

const JCharacter*
CBGetComplementFileTypeName
	(
	const CBTextFileType type
	)
{
	for (JIndex i=0; i<kComplNameCount; i++)
		{
		if (type == kComplName[i].type)
			{
			return kComplName[i].name;
			}
		}

	return "";
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
	{ kCBOtherLang,       NULL, NULL },
	{ kCBCLang,           &CBCStyler::Instance,           &CBCStyler::Shutdown },
	{ kCBEiffelLang,      &CBEiffelStyler::Instance,      &CBEiffelStyler::Shutdown },
	{ kCBFortranLang,     NULL, NULL },
	{ kCBJavaLang,        &CBJavaStyler::Instance,        &CBJavaStyler::Shutdown },
	{ kCBAssemblyLang,    NULL, NULL },
	{ kCBAWKLang,         NULL, NULL },
	{ kCBCobolLang,       NULL, NULL },
	{ kCBLispLang,        NULL, NULL },
	{ kCBPerlLang,        &CBPerlStyler::Instance,        &CBPerlStyler::Shutdown },
	{ kCBPHPLang,         &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBPythonLang,      &CBPythonStyler::Instance,      &CBPythonStyler::Shutdown },
	{ kCBSchemeLang,      NULL, NULL },
	{ kCBBourneShellLang, &CBBourneShellStyler::Instance, &CBBourneShellStyler::Shutdown },
	{ kCBTCLLang,         &CBTCLStyler::Instance,         &CBTCLStyler::Shutdown },
	{ kCBVimLang,         NULL, NULL },
	{ kCBHTMLLang,        &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBASPLang,         NULL, NULL },
	{ kCBMakeLang,        NULL, NULL },
	{ kCBPascalLang,      NULL, NULL },
	{ kCBREXXLang,        NULL, NULL },
	{ kCBRubyLang,        &CBRubyStyler::Instance,        &CBRubyStyler::Shutdown },
	{ kCBLexLang,         NULL, NULL },
	{ kCBCShellLang,      &CBCShellStyler::Instance,      &CBCShellStyler::Shutdown },
	{ kCBBisonLang,       &CBCStyler::Instance,           &CBCStyler::Shutdown },
	{ kCBBetaLang,        NULL, NULL },
	{ kCBLuaLang,         NULL, NULL },
	{ kCBSLangLang,       NULL, NULL },
	{ kCBSQLLang,         NULL, NULL },
	{ kCBVeraLang,        NULL, NULL },
	{ kCBVerilogLang,     NULL, NULL },
	{ kCBCSharpLang,      &CBCSharpStyler::Instance,      &CBCSharpStyler::Shutdown },
	{ kCBErlangLang,      NULL, NULL },
	{ kCBSMLLang,         NULL, NULL },
	{ kCBJavaScriptLang,  &CBJavaScriptStyler::Instance,  &CBJavaScriptStyler::Shutdown },
	{ kCBAntLang,         &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBJSPLang,         &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBXMLLang,         &CBHTMLStyler::Instance,        &CBHTMLStyler::Shutdown },
	{ kCBBasicLang,       NULL, NULL },
	{ kCBMatlabLang,      NULL, NULL },
	{ kCBAdobeFlexLang,   NULL, NULL },
	{ kCBINILang,         &CBINIStyler::Instance,         &CBINIStyler::Shutdown },
	{ kCBPropertiesLang,  &CBPropertiesStyler::Instance,  &CBPropertiesStyler::Shutdown },
};

const JSize kLang2StylerCount = sizeof(kLang2Styler) / sizeof(CBLang2Styler);

JBoolean
CBGetStyler
	(
	const CBLanguage	lang,
	CBStylerBase**		styler
	)
{
	assert( kLang2StylerCount == kCBLangCount );
	assert( kLang2Styler[lang].lang == lang );

	if (kLang2Styler[lang].create != NULL)
		{
		*styler = kLang2Styler[lang].create();
		return JI2B( *styler != NULL );
		}
	else
		{
		*styler = NULL;
		return kJFalse;
		}
}

void
CBShutdownStylers()
{
	for (JIndex i=0; i<kLang2StylerCount; i++)
		{
		if (kLang2Styler[i].destroy != NULL)
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
	{ kCBOtherLang,       NULL, NULL },
	{ kCBCLang,           &CBCCompleter::Instance,           &CBCCompleter::Shutdown },
	{ kCBEiffelLang,      &CBEiffelCompleter::Instance,      &CBEiffelCompleter::Shutdown },
	{ kCBFortranLang,     &CBFortranCompleter::Instance,     &CBFortranCompleter::Shutdown },
	{ kCBJavaLang,        &CBJavaCompleter::Instance,        &CBJavaCompleter::Shutdown },
	{ kCBAssemblyLang,    NULL, NULL },
	{ kCBAWKLang,         NULL, NULL },
	{ kCBCobolLang,       NULL, NULL },
	{ kCBLispLang,        NULL, NULL },
	{ kCBPerlLang,        &CBPerlCompleter::Instance,        &CBPerlCompleter::Shutdown },
	{ kCBPHPLang,         &CBPHPCompleter::Instance,         &CBPHPCompleter::Shutdown },
	{ kCBPythonLang,      &CBPythonCompleter::Instance,      &CBPythonCompleter::Shutdown },
	{ kCBSchemeLang,      NULL, NULL },
	{ kCBBourneShellLang, &CBBourneShellCompleter::Instance, &CBBourneShellCompleter::Shutdown },
	{ kCBTCLLang,         &CBTCLCompleter::Instance,         &CBTCLCompleter::Shutdown },
	{ kCBVimLang,         NULL, NULL },
	{ kCBHTMLLang,        &CBHTMLCompleter::Instance,        &CBHTMLCompleter::Shutdown },
	{ kCBASPLang,         NULL, NULL },
	{ kCBMakeLang,        &CBMakeCompleter::Instance,        &CBMakeCompleter::Shutdown },
	{ kCBPascalLang,      &CBPascalCompleter::Instance,      &CBPascalCompleter::Shutdown },
	{ kCBREXXLang,        &CBREXXCompleter::Instance,        &CBREXXCompleter::Shutdown },
	{ kCBRubyLang,        &CBRubyCompleter::Instance,        &CBRubyCompleter::Shutdown },
	{ kCBLexLang,         &CBLexCompleter::Instance,         &CBLexCompleter::Shutdown },
	{ kCBCShellLang,      &CBCShellCompleter::Instance,      &CBCShellCompleter::Shutdown },
	{ kCBBisonLang,       &CBBisonCompleter::Instance,       &CBBisonCompleter::Shutdown },
	{ kCBBetaLang,        NULL, NULL },
	{ kCBLuaLang,         &CBLuaCompleter::Instance,         &CBLuaCompleter::Shutdown },
	{ kCBSLangLang,       NULL, NULL },
	{ kCBSQLLang,         &CBSQLCompleter::Instance,         &CBSQLCompleter::Shutdown },
	{ kCBVeraLang,        NULL, NULL },
	{ kCBVerilogLang,     NULL, NULL },
	{ kCBCSharpLang,      &CBCSharpCompleter::Instance,      &CBCSharpCompleter::Shutdown },
	{ kCBErlangLang,      NULL, NULL },
	{ kCBSMLLang,         NULL, NULL },
	{ kCBJavaScriptLang,  &CBJavaScriptCompleter::Instance,  &CBJavaScriptCompleter::Shutdown },
	{ kCBAntLang,         NULL, NULL },
	{ kCBJSPLang,         &CBJSPCompleter::Instance,         &CBJSPCompleter::Shutdown },
	{ kCBXMLLang,         &CBHTMLCompleter::Instance,        &CBHTMLCompleter::Shutdown },
	{ kCBBasicLang,       NULL, NULL },
	{ kCBMatlabLang,      NULL, NULL },
	{ kCBAdobeFlexLang,   NULL, NULL },
	{ kCBINILang,         NULL, NULL },
	{ kCBPropertiesLang,  NULL, NULL },
};

const JSize kLang2CompleterCount = sizeof(kLang2Completer) / sizeof(CBLang2Completer);

JBoolean
CBGetCompleter
	(
	const CBLanguage	lang,
	CBStringCompleter**	completer
	)
{
	assert( kLang2CompleterCount == kCBLangCount );
	assert( kLang2Completer[lang].lang == lang );

	if (kLang2Completer[lang].create != NULL)
		{
		*completer = kLang2Completer[lang].create();
		return JI2B( *completer != NULL );
		}
	else
		{
		*completer = NULL;
		return kJFalse;
		}
}

void
CBShutdownCompleters()
{
	for (JIndex i=0; i<kLang2CompleterCount; i++)
		{
		if (kLang2Completer[i].destroy != NULL)
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
