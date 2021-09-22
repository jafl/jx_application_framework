/******************************************************************************
 CBSymbolTypeList.cpp

	BASE CLASS = JContainer, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBSymbolTypeList.h"
#include "cbGlobals.h"
#include <JXImage.h>
#include <JXImageCache.h>
#include <JColorManager.h>
#include <jStreamUtil.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

// JBroadcaster message types

const JUtf8Byte* CBSymbolTypeList::kVisibilityChanged = "VisibilityChanged::CBSymbolTypeList";
const JUtf8Byte* CBSymbolTypeList::kStyleChanged      = "StyleChanged::CBSymbolTypeList";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSymbolTypeList::CBSymbolTypeList
	(
	JXDisplay* display
	)
	:
	JContainer(),
	JPrefObject(CBGetPrefsManager(), kCBSymbolTypeListID)
{
	CreateSymTypeList(display);
	JPrefObject::ReadPrefs();
	InstallCollection(itsSymbolTypeList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSymbolTypeList::~CBSymbolTypeList()
{
	JPrefObject::WritePrefs();

	jdelete itsSymbolTypeList;
}

/******************************************************************************
 SkipSetup (static)

	Ignore the old data in the project file.

 ******************************************************************************/

void
CBSymbolTypeList::SkipSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	if (vers >= 74)
		{
		return;
		}

	if (vers >= 71)
		{
		input >> std::ws;
		JIgnoreLine(input);
		}

	if (vers <= 71)
		{
		JSize typeCount;
		input >> typeCount;

		for (JIndex i=1; i<=typeCount; i++)
			{
			long type;
			input >> type;

			bool visible;
			input >> JBoolFromString(visible);

			JFontStyle style;
			input >> JBoolFromString(style.bold);
			input >> JBoolFromString(style.italic);
			input >> style.underlineCount;
			input >> JBoolFromString(style.strike);

			JRGB color;
			input >> color;
			}
		}
	else
		{
		while (true)
			{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
			if (!keepGoing)
				{
				break;
				}

			JIgnoreLine(input);
			}
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

	We don't broadcast because this is called long before CBSymbolList is
	loaded.

 ******************************************************************************/

void
CBSymbolTypeList::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JSize typeCount;
	input >> typeCount;

	for (JIndex i=1; i<=typeCount; i++)
		{
		long type;
		input >> type;

		const JIndex j      = FindType((CBSymbolList::Type) type);
		SymbolTypeInfo info = itsSymbolTypeList->GetElement(j);

		input >> JBoolFromString(info.visible);
		input >> JBoolFromString(info.style.bold);
		input >> JBoolFromString(info.style.italic);
		input >> info.style.underlineCount;
		input >> JBoolFromString(info.style.strike);

		JRGB color;
		input >> color;
		info.style.color = JColorManager::GetColorID(color);

		itsSymbolTypeList->SetElement(j, info);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBSymbolTypeList::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	const JSize typeCount = itsSymbolTypeList->GetElementCount();
	output << ' ' << typeCount;

	for (JIndex i=1; i<=typeCount; i++)
		{
		const SymbolTypeInfo info = itsSymbolTypeList->GetElement(i);
		output << ' ' << (long) info.type;
		output << ' ' << JBoolToString(info.visible);

		output << ' ' << JBoolToString(info.style.bold);
		output << ' ' << JBoolToString(info.style.italic);
		output << ' ' << info.style.underlineCount;
		output << ' ' << JBoolToString(info.style.strike);
		output << ' ' << JColorManager::GetRGB(info.style.color);
		}
}

/******************************************************************************
 FindType (private)

	This does not return bool because the list must contain all types.

 ******************************************************************************/

JIndex
CBSymbolTypeList::FindType
	(
	const CBSymbolList::Type type
	)
	const
{
	const SymbolTypeInfo* info = itsSymbolTypeList->GetCArray();

	const JSize count = itsSymbolTypeList->GetElementCount();
	for (JUnsignedOffset i=0; i<count; i++)
		{
		if (info[i].type == type)
			{
			return i+1;
			}
		}

	assert_msg( 0, "CBSymbolTypeList::FindType couldn't find type" );
	return 0;
}

/******************************************************************************
 CreateSymTypeList (private)

 ******************************************************************************/

#define ADD(C,L) \
	itsSymbolTypeList->AppendElement(SymbolTypeInfo( \
		CBSymbolList::C, L, JFontStyle(), nullptr));

#define ADD_S(C,L,S) \
	itsSymbolTypeList->AppendElement(SymbolTypeInfo( \
		CBSymbolList::C, L, S, nullptr));

#define ADD_I(C,L,I) \
	itsSymbolTypeList->AppendElement(SymbolTypeInfo( \
		CBSymbolList::C, L, JFontStyle(), I));

#define ADD_SI(C,L,S,I) \
	itsSymbolTypeList->AppendElement(SymbolTypeInfo( \
		CBSymbolList::C, L, S, I));

void
CBSymbolTypeList::CreateSymTypeList
	(
	JXDisplay* display
	)
{
	LoadIcons(display);

	const JFontStyle bold(true, false, 0, false),
					 italic(false, true, 0, false),
					 bold_gray(true, false, 0, false, JColorManager::GetGrayColor(50));

	itsSymbolTypeList = jnew JArray<SymbolTypeInfo>(512);
	assert( itsSymbolTypeList != nullptr );

	ADD_S (kUnknownST, kCBOtherLang, JFontStyle(JColorManager::GetGrayColor(50)))

	ADD_I(kAdobeFlexFunctionST,       kCBAdobeFlexLang, itsFunctionIcon)
	ADD_S(kAdobeFlexClassST,          kCBAdobeFlexLang, bold)
	ADD_I(kAdobeFlexMethodST,         kCBAdobeFlexLang, itsFunctionIcon)
	ADD_I(kAdobeFlexGlobalVariableST, kCBAdobeFlexLang, itsVariableIcon)
	ADD  (kAdobeFlexMxTagST,          kCBAdobeFlexLang)

	ADD(kAntTargetST, kCBAntLang)

	ADD_I(kAssemblyLabelST,  kCBAssemblyLang, itsAssemblyLabelIcon)
	ADD_I(kAssemblyDefineST, kCBAssemblyLang, itsCMacroIcon)
	ADD_I(kAssemblyMacroST,  kCBAssemblyLang, itsCMacroIcon)
	ADD  (kAssemblyTypeST,   kCBAssemblyLang)

	ADD_I(kASPConstantST,   kCBASPLang, itsCMacroIcon)
	ADD_S(kASPClassST,      kCBASPLang, bold)
	ADD_I(kASPFunctionST,   kCBASPLang, itsFunctionIcon)
	ADD_I(kASPSubroutineST, kCBASPLang, itsFunctionIcon)
	ADD_I(kASPVariableST,   kCBASPLang, itsVariableIcon)

	ADD_I(kAWKFunctionST, kCBAWKLang, itsFunctionIcon)

	ADD_I(kBasicConstantST,    kCBBasicLang, itsCMacroIcon)
	ADD_I(kBasicFunctionST,    kCBBasicLang, itsFunctionIcon)
	ADD_I(kBasicLabelST,       kCBBasicLang, itsAssemblyLabelIcon)
	ADD  (kBasicTypeST,        kCBBasicLang)
	ADD_I(kBasicVariableST,    kCBBasicLang, itsVariableIcon)
	ADD_I(kBasicEnumerationST, kCBBasicLang, itsCEnumIcon)

	ADD(kBetaFragmentST, kCBBetaLang)
	ADD(kBetaPatternST,  kCBBetaLang)
	ADD(kBetaSlotST,     kCBBetaLang)

	ADD_I(kBisonNonterminalDefST,  kCBBisonLang, itsBisonNontermDefIcon)
	ADD_I(kBisonNonterminalDeclST, kCBBisonLang, itsBisonNontermDeclIcon)
	ADD_I(kBisonTerminalDeclST,    kCBBisonLang, itsBisonTermDeclIcon)

	ADD_SI(kCClassST,          kCBCLang, bold, itsCClassIcon)
	ADD_SI(kCMacroST,          kCBCLang, JFontStyle(JColorManager::GetBlueColor()), itsCMacroIcon)
	ADD_I (kCEnumValueST,      kCBCLang, itsMemberIcon)
	ADD_I (kCFunctionST,       kCBCLang, itsFunctionIcon)
	ADD_I (kCEnumST,           kCBCLang, itsCEnumIcon)
	ADD_I (kCNamespaceST,      kCBCLang, itsCNamespaceIcon)
	ADD_SI(kCStructST,         kCBCLang, bold_gray, itsCStructIcon)
	ADD_SI(kCTypedefST,        kCBCLang, italic, itsCTypedefIcon)
	ADD_I (kCUnionST,          kCBCLang, itsCUnionIcon)
	ADD_I (kCVariableST,       kCBCLang, itsVariableIcon)
	ADD_SI(kCPrototypeST,      kCBCLang, italic, itsPrototypeIcon)
	ADD_I (kCMemberST,         kCBCLang, itsMemberIcon)
	ADD_I (kCExternVariableST, kCBCLang, itsVariableIcon)

	ADD_SI(kCSharpClassST,      kCBCSharpLang, bold, itsCSharpClassIcon)
	ADD_SI(kCSharpMacroST,      kCBCSharpLang, JFontStyle(JColorManager::GetBlueColor()), itsCMacroIcon)
	ADD_I (kCSharpEnumValueST,  kCBCSharpLang, itsMemberIcon)
	ADD_I (kCSharpEventST,      kCBCSharpLang, itsCSharpEventIcon)
	ADD_I (kCSharpFieldST,      kCBCSharpLang, itsVariableIcon)
	ADD_I (kCSharpEnumNameST,   kCBCSharpLang, itsCEnumIcon)
	ADD_SI(kCSharpInterfaceST,  kCBCSharpLang, bold_gray, itsCSharpInterfaceIcon)
	ADD_I (kCSharpMethodST,     kCBCSharpLang, itsFunctionIcon)
	ADD_I (kCSharpNamespaceST,  kCBCSharpLang, itsCSharpNamespaceIcon)
	ADD_I (kCSharpPropertyST,   kCBCSharpLang, itsMemberIcon)
	ADD_I (kCSharpStructNameST, kCBCSharpLang, itsCSharpStructIcon)
	ADD_SI(kCSharpTypedefST,    kCBCSharpLang, italic, itsCTypedefIcon)

	ADD_I(kCobolParagraphST, kCBCobolLang, itsFunctionIcon)
	ADD_I(kCobolDataST,      kCBCobolLang, itsVariableIcon)
	ADD  (kCobolFileST,      kCBCobolLang)
	ADD  (kCobolGroupST,     kCBCobolLang)
	ADD  (kCobolProgramST,   kCBCobolLang)
	ADD  (kCobolSectionST,   kCBCobolLang)

	ADD_SI(kEiffelClassST,   kCBEiffelLang, bold, itsEiffelClassIcon)
	ADD_I (kEiffelFeatureST, kCBEiffelLang, itsFunctionIcon)

	ADD_SI(kErlangMacroST,    kCBErlangLang, JFontStyle(JColorManager::GetBlueColor()), itsCMacroIcon)
	ADD_I (kErlangFunctionST, kCBErlangLang, itsFunctionIcon)
	ADD   (kErlangModuleST,   kCBErlangLang)
	ADD   (kErlangRecordST,   kCBErlangLang)

	ADD   (kFortranBlockDataST,      kCBFortranLang)
	ADD_I (kFortranCommonBlockST,    kCBFortranLang, itsFortranCommonBlockIcon)
	ADD   (kFortranEntryPointST,     kCBFortranLang)
	ADD_I (kFortranFunctionST,       kCBFortranLang, itsFunctionIcon)
	ADD   (kFortranInterfaceST,      kCBFortranLang)
	ADD   (kFortranModuleST,         kCBFortranLang)
	ADD   (kFortranNamelistST,       kCBFortranLang)
	ADD_I (kFortranProgramST,        kCBFortranLang, itsFunctionIcon)
	ADD_I (kFortranSubroutineST,     kCBFortranLang, itsFunctionIcon)
	ADD   (kFortranDerivedTypeST,    kCBFortranLang)
	ADD   (kFortranTypeComponentST,  kCBFortranLang)
	ADD   (kFortranLabelST,          kCBFortranLang)
	ADD_I (kFortranLocalVariableST,  kCBFortranLang, itsVariableIcon)
	ADD_I (kFortranModuleVariableST, kCBFortranLang, itsVariableIcon)

	ADD_SI(kJavaClassST,     kCBJavaLang, bold, itsJavaClassIcon)
	ADD_I (kJavaEnumValueST, kCBJavaLang, itsMemberIcon)
	ADD_I (kJavaFieldST,     kCBJavaLang, itsMemberIcon)
	ADD_I (kJavaEnumST,      kCBJavaLang, itsCEnumIcon)
	ADD_SI(kJavaInterfaceST, kCBJavaLang, bold_gray, itsJavaInterfaceIcon)
	ADD_I (kJavaMethodST,    kCBJavaLang, itsFunctionIcon)
	ADD_SI(kJavaPrototypeST, kCBJavaLang, italic, itsPrototypeIcon)
	ADD_I (kJavaPackageST,   kCBJavaLang, itsJavaPackageIcon)

	ADD_I (kJavaScriptFunctionST,       kCBJavaScriptLang, itsFunctionIcon)
	ADD_SI(kJavaScriptClassST,          kCBJavaScriptLang, bold, itsJavaScriptClassIcon)
	ADD_I (kJavaScriptMethodST,         kCBJavaScriptLang, itsFunctionIcon)
	ADD_I (kJavaScriptGlobalVariableST, kCBJavaScriptLang, itsVariableIcon)

	ADD(kLexStateST, kCBLexLang)

	ADD_I(kLispFunctionST, kCBLispLang, itsFunctionIcon)

	ADD_I(kLuaFunctionST, kCBLuaLang, itsFunctionIcon)

	ADD(kMakeVariableST, kCBMakeLang)
	ADD(kMakeTargetST,   kCBMakeLang)

	ADD_I(kMatlabFunctionST, kCBMatlabLang, itsFunctionIcon)

	ADD_I(kPascalFunctionST,  kCBPascalLang, itsFunctionIcon)
	ADD_I(kPascalProcedureST, kCBPascalLang, itsFunctionIcon)

	ADD_I(kPerlSubroutineST, kCBPerlLang, itsFunctionIcon)
	ADD  (kPerlPackageST,    kCBPerlLang)
	ADD_I(kPerlConstantST,   kCBPerlLang, itsVariableIcon)
	ADD  (kPerlLabelST,      kCBPerlLang)
	ADD  (kPerlFormatST,     kCBPerlLang)

	ADD_SI(kPHPClassST,     kCBPHPLang, bold, itsPHPClassIcon)
	ADD_SI(kPHPInterfaceST, kCBPHPLang, bold_gray, itsPHPInterfaceIcon)
	ADD_I (kPHPFunctionST,  kCBPHPLang, itsFunctionIcon)
	ADD_SI(kPHPDefineST,    kCBPHPLang, JFontStyle(JColorManager::GetBlueColor()), itsCMacroIcon)

	ADD_SI(kPythonClassST,       kCBPythonLang, bold, itsPythonClassIcon)
	ADD_I (kPythonFunctionST,    kCBPythonLang, itsFunctionIcon)
	ADD_I (kPythonClassMemberST, kCBPythonLang, itsMemberIcon)

	ADD_I(kREXXSubroutineST, kCBREXXLang, itsFunctionIcon)

	ADD_S(kRubyClassST,           kCBRubyLang, bold)
	ADD_I(kRubyMethodST,          kCBRubyLang, itsFunctionIcon)
	ADD_I(kRubySingletonMethodST, kCBRubyLang, itsFunctionIcon)
	ADD_S(kRubyMixinST,           kCBRubyLang, bold)

	ADD_I(kSchemeFunctionST, kCBSchemeLang, itsFunctionIcon)
	ADD  (kSchemeSetST,      kCBSchemeLang)

	ADD_I(kBourneShellFunctionST, kCBBourneShellLang, itsFunctionIcon)

	ADD_I(kSLangFunctionST,  kCBSLangLang, itsFunctionIcon)
	ADD  (kSLangNamespaceST, kCBSLangLang)

	ADD   (kSMLExceptionST,   kCBSMLLang)
	ADD_I (kSMLFunctionST,    kCBSMLLang, itsFunctionIcon)
	ADD_SI(kSMLFunctorST,     kCBSMLLang, JFontStyle(JColorManager::GetGrayColor(50)), itsFunctionIcon)
	ADD   (kSMLSignatureST,   kCBSMLLang)
	ADD   (kSMLStructureST,   kCBSMLLang)
	ADD   (kSMLTypeST,        kCBSMLLang)
	ADD_I (kSMLValueST,       kCBSMLLang, itsVariableIcon)

	ADD  (kSQLCursorST,              kCBSQLLang)
	ADD_I(kSQLPrototypeST,           kCBSQLLang, itsPrototypeIcon)
	ADD_I(kSQLFunctionST,            kCBSQLLang, itsFunctionIcon)
	ADD  (kSQLRecordFieldST,         kCBSQLLang)
	ADD_I(kSQLLocalVariableST,       kCBSQLLang, itsVariableIcon)
	ADD  (kSQLLabelST,               kCBSQLLang)
	ADD  (kSQLPackageST,             kCBSQLLang)
	ADD_I(kSQLProcedureST,           kCBSQLLang, itsFunctionIcon)
	ADD  (kSQLRecordST,              kCBSQLLang)
	ADD  (kSQLSubtypeST,             kCBSQLLang)
	ADD  (kSQLTableST,               kCBSQLLang)
	ADD  (kSQLTriggerST,             kCBSQLLang)
	ADD_I(kSQLVariableST,            kCBSQLLang, itsVariableIcon)
	ADD  (kSQLIndexST,               kCBSQLLang)
	ADD  (kSQLEventST,               kCBSQLLang)
	ADD  (kSQLPublicationST,         kCBSQLLang)
	ADD  (kSQLServiceST,             kCBSQLLang)
	ADD  (kSQLDomainST,              kCBSQLLang)
	ADD  (kSQLViewST,                kCBSQLLang)
	ADD  (kSQLSynonymST,             kCBSQLLang)
	ADD  (kSQLMobiLinkTableScriptST, kCBSQLLang)
	ADD  (kSQLMobiLinkConnScriptST,  kCBSQLLang)

	ADD  (kVeraClassST,	           kCBVeraLang)
	ADD  (kVeraMacroST,            kCBVeraLang)
	ADD_I(kVeraEnumValueST,        kCBVeraLang, itsMemberIcon)
	ADD_I(kVeraFunctionST,         kCBVeraLang, itsFunctionIcon)
	ADD_I(kVeraEnumST,             kCBVeraLang, itsCEnumIcon)
	ADD_I(kVeraMemberST,           kCBVeraLang, itsFunctionIcon)
	ADD  (kVeraProgramST,          kCBVeraLang)
	ADD_I(kVeraPrototypeST,        kCBVeraLang, itsPrototypeIcon)
	ADD  (kVeraTaskST,             kCBVeraLang)
	ADD_I(kVeraTypedefST,          kCBVeraLang, itsCTypedefIcon)
	ADD_I(kVeraVariableST,         kCBVeraLang, itsVariableIcon)
	ADD_I(kVeraExternalVariableST, kCBVeraLang, itsVariableIcon)

	ADD_I(kVerilogFunctionST, kCBVerilogLang, itsFunctionIcon)
	ADD  (kVerilogModuleST,   kCBVerilogLang)
	ADD_I(kVerilogConstantST, kCBVerilogLang, itsVariableIcon)
	ADD  (kVerilogPortST,     kCBVerilogLang)
	ADD  (kVerilogRegisterST, kCBVerilogLang)
	ADD  (kVerilogTaskST,     kCBVerilogLang)
	ADD_I(kVerilogVariableST, kCBVerilogLang, itsVariableIcon)
	ADD  (kVerilogWireST,     kCBVerilogLang)
	ADD  (kVerilogEventST,    kCBVerilogLang)
	ADD  (kVerilogNetST,      kCBVerilogLang)

	ADD_I(kTCLProcedureST, kCBTCLLang, itsFunctionIcon)
	ADD  (kTCLClassST,     kCBTCLLang)
	ADD_I(kTCLMethodST,    kCBTCLLang, itsFunctionIcon)

	ADD_I(kVimFunctionST,         kCBVimLang, itsFunctionIcon)
	ADD_I(kVimVariableST,         kCBVimLang, itsVariableIcon)
	ADD  (kVimAutocommandGroupST, kCBVimLang)
	ADD_I(kVimUserCommandST,      kCBVimLang, itsCMacroIcon)
	ADD  (kVimMapST,              kCBVimLang)
}

/******************************************************************************
 LoadIcons (private)

 ******************************************************************************/

#include "jcc_sym_bison_nonterm_def.xpm"
#include "jcc_sym_bison_nonterm_decl.xpm"
#include "jcc_sym_bison_term_decl.xpm"
#include "jcc_sym_c_class.xpm"
#include "jcc_sym_c_struct.xpm"
#include "jcc_sym_c_enum.xpm"
#include "jcc_sym_c_union.xpm"
#include "jcc_sym_c_macro.xpm"
#include "jcc_sym_c_typedef.xpm"
#include "jcc_sym_c_namespace.xpm"
#include "jcc_sym_c_sharp_class.xpm"
#include "jcc_sym_c_sharp_event.xpm"
#include "jcc_sym_c_sharp_interface.xpm"
#include "jcc_sym_c_sharp_namespace.xpm"
#include "jcc_sym_c_sharp_struct.xpm"
#include "jcc_sym_eiffel_class.xpm"
#include "jcc_sym_fortran_common_blk.xpm"
#include "jcc_sym_java_class.xpm"
#include "jcc_sym_java_interface.xpm"
#include "jcc_sym_java_package.xpm"
#include "jcc_sym_java_script_class.xpm"
#include "jcc_sym_asm_label.xpm"
#include "jcc_sym_python_class.xpm"
#include "jcc_sym_php_class.xpm"
#include "jcc_sym_php_interface.xpm"
#include "jcc_sym_prototype.xpm"
#include "jcc_sym_function.xpm"
#include "jcc_sym_variable.xpm"
#include "jcc_sym_member.xpm"

void
CBSymbolTypeList::LoadIcons
	(
	JXDisplay* display
	)
{
	JXImageCache* c = display->GetImageCache();

	// C

	itsCClassIcon     = c->GetImage(jcc_sym_c_class);
	itsCStructIcon    = c->GetImage(jcc_sym_c_struct);
	itsCEnumIcon      = c->GetImage(jcc_sym_c_enum);
	itsCUnionIcon     = c->GetImage(jcc_sym_c_union);
	itsCMacroIcon     = c->GetImage(jcc_sym_c_macro);
	itsCTypedefIcon   = c->GetImage(jcc_sym_c_typedef);
	itsCNamespaceIcon = c->GetImage(jcc_sym_c_namespace);

	// C#

	itsCSharpClassIcon     = c->GetImage(jcc_sym_c_sharp_class);
	itsCSharpEventIcon     = c->GetImage(jcc_sym_c_sharp_event);
	itsCSharpInterfaceIcon = c->GetImage(jcc_sym_c_sharp_interface);
	itsCSharpNamespaceIcon = c->GetImage(jcc_sym_c_sharp_namespace);
	itsCSharpStructIcon    = c->GetImage(jcc_sym_c_sharp_struct);

	// Eiffel

	itsEiffelClassIcon = c->GetImage(jcc_sym_eiffel_class);

	// FORTRAN

	itsFortranCommonBlockIcon = c->GetImage(jcc_sym_fortran_common_block);

	// Java

	itsJavaClassIcon     = c->GetImage(jcc_sym_java_class);
	itsJavaInterfaceIcon = c->GetImage(jcc_sym_java_interface);
	itsJavaPackageIcon   = c->GetImage(jcc_sym_java_package);

	// JavaScript

	itsJavaScriptClassIcon = c->GetImage(jcc_sym_java_script_class);

	// Assembly

	itsAssemblyLabelIcon = c->GetImage(jcc_sym_asm_label);

	// Python

	itsPythonClassIcon = c->GetImage(jcc_sym_python_class);

	// PHP

	itsPHPClassIcon     = c->GetImage(jcc_sym_php_class);
	itsPHPInterfaceIcon = c->GetImage(jcc_sym_php_interface);

	// Bison

	itsBisonNontermDefIcon  = c->GetImage(jcc_sym_bison_nonterm_def);
	itsBisonNontermDeclIcon = c->GetImage(jcc_sym_bison_nonterm_decl);
	itsBisonTermDeclIcon    = c->GetImage(jcc_sym_bison_term_decl);

	// shared

	itsPrototypeIcon = c->GetImage(jcc_sym_prototype);
	itsFunctionIcon  = c->GetImage(jcc_sym_function);
	itsVariableIcon  = c->GetImage(jcc_sym_variable);
	itsMemberIcon    = c->GetImage(jcc_sym_member);
}
