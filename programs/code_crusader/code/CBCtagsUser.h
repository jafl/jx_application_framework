/******************************************************************************
 CBCtagsUser.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCtagsUser
#define _H_CBCtagsUser

#include <JBroadcaster.h>
#include "CBTextFileType.h"
#include <JStringPtrMap.h>	// template
#include <JString.h>

class JProcess;
class JOutPipeStream;

class CBCtagsUser : virtual public JBroadcaster
{
public:

	// Do not change these values once they are assigned because
	// they are stored in the project file.

	// When you add new types, be sure to increment the project version!
	// Remember to update CBSymbolTypeList::CreateSymTypeList()!

	enum Type
	{
		kUnknownST = 0,

		kCClassST = 100,
		kCMacroST,
		kCEnumValueST,
		kCFunctionST,
		kCEnumST,
		kCNamespaceST,
		kCStructST,
		kCTypedefST,
		kCUnionST,
		kCVariableST,
		kCPrototypeST,
		kCMemberST,
		kCExternVariableST,

		kEiffelClassST = 200,
		kEiffelFeatureST,

		kFortranBlockDataST = 300,
		kFortranCommonBlockST,
		kFortranEntryPointST,
		kFortranFunctionST,
		kFortranInterfaceST,
		kFortranModuleST,
		kFortranNamelistST,
		kFortranProgramST,
		kFortranSubroutineST,
		kFortranDerivedTypeST,
		kFortranTypeComponentST,
		kFortranLabelST,
		kFortranLocalVariableST,
		kFortranModuleVariableST,

		kJavaClassST = 400,
		kJavaFieldST,
		kJavaInterfaceST,
		kJavaMethodST,
		kJavaPackageST,
		kJavaEnumValueST,
		kJavaEnumST,
		kJavaPrototypeST,

		kAssemblyLabelST = 500,
		kAssemblyDefineST,
		kAssemblyMacroST,
		kAssemblyTypeST,

		kAWKFunctionST = 600,

		kCobolParagraphST = 700,
		kCobolDataST,
		kCobolFileST,
		kCobolGroupST,
		kCobolProgramST,
		kCobolSectionST,

		kLispFunctionST = 800,

		kPerlSubroutineST = 900,
		kPerlPackageST,
		kPerlConstantST,
		kPerlLabelST,
		kPerlFormatST,

		kPythonClassST = 1000,
		kPythonFunctionST,
		kPythonClassMemberST,

		kSchemeFunctionST = 1100,
		kSchemeSetST,

		kBourneShellFunctionST = 1200,

		kTCLProcedureST = 1300,
		kTCLClassST,
		kTCLMethodST,

		kVimFunctionST = 1400,
		kVimVariableST,
		kVimAutocommandGroupST,
		kVimUserCommandST,
		kVimMapST,

		kPHPClassST = 1500,
		kPHPFunctionST,
		kPHPDefineST,
		kPHPInterfaceST,

		kASPFunctionST = 1600,
		kASPSubroutineST,
		kASPVariableST,
		kASPConstantST,
		kASPClassST,

		kMakeVariableST = 1700,
		kMakeTargetST,

		kREXXSubroutineST = 1800,

		kRubyClassST = 1900,
		kRubyMethodST,
		kRubyMixinST,
		kRubySingletonMethodST,

		kPascalFunctionST = 2000,
		kPascalProcedureST,

		kLexStateST = 2100,

		kBisonNonterminalDefST = 2200,
		kBisonNonterminalDeclST,
		kBisonTerminalDeclST,

		kBetaFragmentST = 2300,
		kBetaPatternST,
		kBetaSlotST,

		kLuaFunctionST = 2400,

		kSLangFunctionST = 2500,
		kSLangNamespaceST,

		kSQLCursorST = 2600,
		kSQLPrototypeST,
		kSQLFunctionST,
		kSQLRecordFieldST,
		kSQLLocalVariableST,
		kSQLLabelST,
		kSQLPackageST,
		kSQLProcedureST,
		kSQLRecordST,
		kSQLSubtypeST,
		kSQLTableST,
		kSQLTriggerST,
		kSQLVariableST,
		kSQLIndexST,
		kSQLEventST,
		kSQLPublicationST,
		kSQLServiceST,
		kSQLDomainST,
		kSQLViewST,
		kSQLSynonymST,
		kSQLMobiLinkTableScriptST,
		kSQLMobiLinkConnScriptST,

		kVeraClassST = 2700,
		kVeraMacroST,
		kVeraEnumValueST,
		kVeraFunctionST,
		kVeraEnumST,
		kVeraMemberST,
		kVeraProgramST,
		kVeraPrototypeST,
		kVeraTaskST,
		kVeraTypedefST,
		kVeraVariableST,
		kVeraExternalVariableST,

		kVerilogFunctionST = 2800,
		kVerilogModuleST,
		kVerilogConstantST,
		kVerilogPortST,
		kVerilogRegisterST,
		kVerilogTaskST,
		kVerilogVariableST,		// no longer used
		kVerilogWireST,			// no longer used
		kVerilogEventST,
		kVerilogNetST,

		kCSharpClassST = 2900,
		kCSharpMacroST,
		kCSharpEnumValueST,
		kCSharpEventST,
		kCSharpFieldST,
		kCSharpEnumNameST,
		kCSharpInterfaceST,
		kCSharpMethodST,
		kCSharpNamespaceST,
		kCSharpPropertyST,
		kCSharpStructNameST,
		kCSharpTypedefST,

		kErlangMacroST = 3000,
		kErlangFunctionST,
		kErlangModuleST,
		kErlangRecordST,

		kSMLExceptionST = 3100,
		kSMLFunctionST,
		kSMLFunctorST,
		kSMLSignatureST,
		kSMLStructureST,
		kSMLTypeST,
		kSMLValueST,

		kJavaScriptFunctionST = 3200,
		kJavaScriptClassST,
		kJavaScriptMethodST,
		kJavaScriptGlobalVariableST,

		kAntTargetST = 3300,

		kBasicConstantST = 3400,
		kBasicFunctionST,
		kBasicLabelST,
		kBasicTypeST,
		kBasicVariableST,
		kBasicEnumerationST,

		kMatlabFunctionST = 3500,

		kAdobeFlexFunctionST = 3600,
		kAdobeFlexClassST,
		kAdobeFlexMethodST,
		kAdobeFlexGlobalVariableST,
		kAdobeFlexMxTagST

		// When you add new types, be sure to increment the project version!
	};

public:

	CBCtagsUser(const JUtf8Byte* args);

	virtual ~CBCtagsUser();

	bool		IsActive() const;
	static bool	HasExuberantCtags();
	static bool	IsParsed(const CBTextFileType type);
	static bool	IsParsedForFunctionMenu(const CBTextFileType type);

	static const JString&	GetFunctionMenuTitle(const CBTextFileType type);

	// Type groups

	static bool	IsClass(const Type type);
	static bool	IsPrototype(const Type type);
	static bool	IsFunction(const Type type);
	static bool	IsFileScope(const Type type);

	static bool	ShouldSmartScroll(const Type type);

	static bool	IgnoreSymbol(const JString& s);

protected:

	void	SetCtagsArgs(const JUtf8Byte* args);

	bool	ProcessFile(const JString& fileName, const CBTextFileType fileType,
							JString* result, CBLanguage* lang);
	void		ReadExtensionFlags(std::istream& input, JStringPtrMap<JString>* flags) const;
	Type		DecodeSymbolType(const CBLanguage lang, const JUtf8Byte c,
								 const JStringPtrMap<JString>& flags) const;

	virtual void	InitCtags(std::ostream& output);
	void			DeleteProcess();

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum CtagsStatus
	{
		kUntested,
		kSuccess,
		kFailure
	};

private:

	JProcess*			itsProcess;
	JString				itsArgs;
	JOutPipeStream*		itsCmdPipe;
	int					itsResultFD;
	bool			itsIsActiveFlag;
	bool			itsTryRestartFlag;

	static CtagsStatus	itsHasExuberantCtagsFlag;

private:

	bool	StartProcess(const CBTextFileType fileType, CBLanguage* lang);

	// not allowed

	CBCtagsUser(const CBCtagsUser& source);
	const CBCtagsUser& operator=(const CBCtagsUser& source);
};


/******************************************************************************
 IsActive

 ******************************************************************************/

inline bool
CBCtagsUser::IsActive()
	const
{
	return itsIsActiveFlag;
}

/******************************************************************************
 Type groups (static)

 ******************************************************************************/

inline bool
CBCtagsUser::IsClass
	(
	const Type type
	)
{
	return (type == kCClassST           ||
			type == kCStructST          ||
			type == kCEnumST            ||
			type == kCSharpClassST      ||
			type == kCSharpInterfaceST  ||
			type == kCSharpStructNameST ||
			type == kCSharpEnumNameST   ||
			type == kEiffelClassST      ||
			type == kErlangModuleST     ||
			type == kJavaClassST        ||
			type == kPerlPackageST      ||
			type == kPythonClassST      ||
			type == kSMLStructureST     ||
			type == kTCLClassST         ||
			type == kVeraClassST);
}

inline bool
CBCtagsUser::IsPrototype
	(
	const Type type
	)
{
	return (type == kBisonNonterminalDeclST ||
			type == kBisonTerminalDeclST    ||
			type == kCPrototypeST           ||
			type == kJavaPrototypeST        ||
			type == kSQLPrototypeST         ||
			type == kVeraPrototypeST);
}

inline bool
CBCtagsUser::IsFunction
	(
	const Type type
	)
{
	return (type == kAWKFunctionST         ||
			type == kBisonNonterminalDefST ||
			type == kBourneShellFunctionST ||
			type == kCFunctionST           ||
			type == kCSharpMethodST        ||
			type == kCobolParagraphST      ||
			type == kEiffelFeatureST       ||
			type == kErlangFunctionST      ||
			type == kFortranProgramST      ||
			type == kFortranSubroutineST   ||
			type == kFortranFunctionST     ||
			type == kJavaMethodST          ||
			type == kJavaScriptFunctionST  ||
			type == kLispFunctionST        ||
			type == kLuaFunctionST         ||
			type == kPerlSubroutineST      ||
			type == kPHPFunctionST         ||
			type == kPythonFunctionST      ||
			type == kRubyMethodST          ||
			type == kRubySingletonMethodST ||
			type == kSchemeFunctionST      ||
			type == kSLangFunctionST       ||
			type == kSMLFunctionST         ||
			type == kSMLFunctorST          ||
			type == kSQLFunctionST         ||
			type == kSQLProcedureST        ||
			type == kTCLProcedureST        ||
			type == kTCLMethodST           ||
			type == kVeraFunctionST        ||
			type == kVerilogFunctionST     ||
			type == kVimFunctionST);
}

inline bool
CBCtagsUser::IsFileScope
	(
	const Type type
	)
{
	return (type == kAntTargetST           ||
			type == kAssemblyLabelST       ||
			type == kAWKFunctionST         ||
			type == kBourneShellFunctionST ||
			type == kCMacroST              ||
			type == kCSharpMacroST         ||
			type == kCVariableST           ||
			type == kErlangMacroST         ||
			type == kFortranLabelST        ||
			type == kJavaScriptFunctionST  ||
			type == kLexStateST            ||
			type == kMakeVariableST        ||
			type == kMakeTargetST          ||
			type == kPerlSubroutineST      ||
			type == kPerlConstantST        ||
			type == kPerlLabelST           ||
			type == kPHPClassST            ||
			type == kPHPInterfaceST        ||
			type == kPHPFunctionST         ||
			type == kPHPDefineST           ||
			type == kSQLLocalVariableST    ||
			type == kSQLLabelST            ||
			type == kTCLProcedureST        ||
			type == kVimFunctionST);
}

inline bool
CBCtagsUser::ShouldSmartScroll
	(
	const Type type
	)
{
	return IsClass(type) || IsFunction(type);
}

#endif
