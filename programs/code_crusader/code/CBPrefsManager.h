/******************************************************************************
 CBPrefsManager.h

	Copyright (C) 1997-2001 John Lindal.

 ******************************************************************************/

#ifndef _H_CBPrefsManager
#define _H_CBPrefsManager

#include <JXPrefsManager.h>
#include "CBTextFileType.h"
#include "CBEmulator.h"
#include <JTextEditor.h>		// need definition of CRMRule

class JRegex;

class JXWindow;
class JXChooseSaveFile;

class CBTextDocument;
class CBTextEditor;
class CBCharActionManager;
class CBMacroManager;

class CBEditFileTypesDialog;
class CBEditMacroDialog;
class CBEditCRMDialog;

const JCharacter kCBContentRegexMarker = '^';

// Preferences -- do not change ID's once they are assigned

enum
{
	kCBHTMLStyleID = 1,
	kCBMDIServerID,
	kCBViewManPagePrefID,
	kCBDocMgrID,
	kCBTreeWindSizeID,
	kCBSourceWindSizeID,
	kCBSearchTextID,
	kCBMacroSetListID,
	kCBHeaderWindSizeID,
	kCBOtherTextWindSizeID,
	kCBTextDocID,
	kCBgCSFSetupID,
	kCBExecOutputWindSizeID,
	kCBRunCommandDialogID,
	kCBDockPrefID,
	kCBPrintPlainTextID,
	kCBFindFilePrefID,
	kCBSymbolDirectorID,
	kCBTCLStyleID,
	kCBBourneShellStyleID,
	kCBDefFontID,
	kCBProgramVersionID,
	kCBAppID,
	kCBDocMgrStateID,
	kCBTreeSetupID,
	kCBEditSearchPathsDialogID,
	kCBUnused1ID,				// unused
	kCBFileListWindSizeID,
	kCBCRMRuleSetListID,
	kCBFileTypeListID,
	kCBMiscWordWrapID,
	kCBEditMacroDialogID,
	kCBEditFileTypesDialogID,
	kCBRunTEScriptDialogID,
	kCBEditCRMDialogID,
	kCBTextColorID,
	kCBStaticGlobalID,
	kCBEditCPPMacroDialogID,
	kCBPrintStyledTextID,
	kCBProjectWindSizeID,
	kCBJavaStyleID,
	kCBTEToolBarID,
	kCBProjectToolBarID,
	kCBCTreeToolBarID,
	kCBFnMenuUpdaterID,
	kCBDiffFileDialogID,
	kCBSymbolWindSizeID,
	kCBSymbolToolBarID,
	kCBJavaTreeToolBarID,
	kCBNewProjectCSFID,
	kCBCStyleID,
	kCBFileListToolBarID,
	kCBSearchOutputWindSizeID,
	kCBCShellStyleID,
	kCBGlobalCommandsID,
	kCBEditCommandsDialogID,
	kCBPerlStyleID,
	kCBExpireTimeStampID,
	kCBCSharpStyleID,
	kCBPythonStyleID,
	kCBEiffelStyleID,
	kCBJavaScriptStyleID,
	kCBSymbolTypeListID,
	kCBVersionCheckID,
	kCBEmulatorID,
	kCBRubyStyleID,
	kCBINIStyleID,
	kCBPHPTreeToolBarID,
	kCBPropertiesStyleID,
	kCBSQLStyleID,

	// 10000-19999 are reserved for action/macro

	kCBEmptyMacroID = 10000,			// reserved and empty
	kCBFirstMacroID = 10001,
	kCBLastMacroID  = 19999,

	// 20000-29999 are reserved for CRM rule lists

	kCBEmptyCRMRuleListID = 20000,		// reserved and empty
	kCBFirstCRMRuleListID = 20001,
	kCBLastCRMRuleListID  = 29999
};

class CBPrefsManager : public JXPrefsManager
{
public:

	// remember to increment shared prefs file version

	enum
	{
		kTextColorIndex = 1,
		kBackColorIndex,
		kCaretColorIndex,
		kSelColorIndex,
		kSelLineColorIndex,
		kRightMarginColorIndex,		// = kColorCount

		kColorCount = kRightMarginColorIndex
	};

public:

	CBPrefsManager(JBoolean* isNew);

	virtual ~CBPrefsManager();

	JString		GetJCCVersionStr() const;

	JBoolean	GetExpirationTimeStamp(time_t* t) const;
	void		SetExpirationTimeStamp(const time_t t);

	void		EditFileTypes();
	void		EditMacros(CBMacroManager* macroMgr);
	void		EditCRMRuleLists(JTextEditor::CRMRuleList* ruleList);

	JBoolean	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
							  JCoordinate* width, JCoordinate* height) const;
	void		SaveWindowSize(const JPrefID& id, JXWindow* window);

	JBoolean	RestoreProgramState();
	void		SaveProgramState();
	void		ForgetProgramState();

	// text editor

	void	GetDefaultFont(JString* name, JSize* size) const;
	void	SetDefaultFont(const JCharacter* name, const JSize size);

	JColorID		GetColor(const JIndex index) const;
	void			SetColor(const JIndex index, const JColorID color);
	static JBoolean	ColorIndexValid(const JIndex index);

	CBEmulator	GetEmulator() const;
	void		SetEmulator(const CBEmulator type);

	// file types

	CBTextFileType	GetFileType(const JString& fileName) const;
	CBTextFileType	GetFileType(const CBTextDocument& doc,
								CBCharActionManager** actionMgr, CBMacroManager** macroMgr,
								JTextEditor::CRMRuleList** crmRuleList,
								JString* scriptPath, JBoolean* wordWrap) const;

	JBoolean		EditWithOtherProgram(const JCharacter* fileName, JString* cmd) const;

	void	GetFileSuffixes(const CBTextFileType type,
							JPtrArray<JString>* list) const;
	void	GetAllFileSuffixes(JPtrArray<JString>* list) const;

	JString	GetDefaultComplementSuffix(const JString& name, const CBTextFileType type,
									   JIndex* index) const;
	void	SetDefaultComplementSuffix(const JIndex index, const JString& name);

	static JBoolean	FileMatchesSuffix(const JString& fileName,
									  const JPtrArray<JString>& suffixList);

	static JBoolean	GetScriptPaths(JString* sysDir, JString* userDir);

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	SaveAllBeforeDestruct();
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

public:

	struct FileTypeInfo
	{
		JString*				suffix;
		JRegex*					nameRegex;		// can be NULL
		JRegex*					contentRegex;	// can be NULL
		JIndexRange				literalRange;	// initially nothing
		CBTextFileType			type;
		JIndex					macroID;		// MacroSetInfo::id
		JIndex					crmID;			// CRMRuleListInfo::id
		JBoolean				isUserScript;	// kJTrue if relative to ~/.jxcb/scripts/
		JString*				scriptPath;		// NULL if none; *relative*
		JBoolean				wordWrap;
		JString*				complSuffix;
		JString*				editCmd;		// NULL if type != kCBExternalFT

		FileTypeInfo()
			:
			suffix(NULL), nameRegex(NULL), contentRegex(NULL), type(kCBUnknownFT),
			macroID(kCBEmptyMacroID), crmID(kCBEmptyCRMRuleListID),
			isUserScript(kJTrue), scriptPath(NULL),
			wordWrap(kJTrue), complSuffix(NULL), editCmd(NULL)
		{ };

		FileTypeInfo(JString* s, JRegex* nr, JRegex* cr, const CBTextFileType t,
					 const JIndex macro, const JIndex crm,
					 const JBoolean us, JString* sp,
					 const JBoolean wrap, JString* cs, JString* ec)
			:
			suffix(s), nameRegex(nr), contentRegex(cr), type(t),
			macroID(macro), crmID(crm), isUserScript(us), scriptPath(sp),
			wordWrap(wrap), complSuffix(cs), editCmd(ec)
		{ };

		JBoolean	IsPlainSuffix() const;
		void		CreateRegex();
		void		Free();
	};

	static JIndexRange	GetLiteralPrefixRange(const JCharacter* regexStr);

	static JListT::CompareResult
		CompareFileTypeSpec(const FileTypeInfo& i1, const FileTypeInfo& i2);

	struct MacroSetInfo
	{
		JIndex					id;
		JString*				name;
		CBCharActionManager*	action;
		CBMacroManager*			macro;

		MacroSetInfo()
			:
			id(kCBEmptyMacroID), name(NULL),
			action(NULL), macro(NULL)
		{ };

		MacroSetInfo(const JPrefID& i, JString* n,
					 CBCharActionManager* a, CBMacroManager* m)
			:
			id(i.GetID()), name(n), action(a), macro(m)
		{ };

		void	CreateAndRead(std::istream& input, const JFileVersion vers);
		void	Write(std::ostream& output);
		void	Free();
	};

	static JBoolean	FindMacroID(const JArray<MacroSetInfo>& list,
								const JIndex id, JIndex* index);
	static JIndex	FindMacroName(const JCharacter* macroName,
								  JArray<MacroSetInfo>* macroList,
								  const JBoolean create);

	struct CRMRuleListInfo
	{
		JIndex						id;
		JString*					name;
		JTextEditor::CRMRuleList*	list;

		CRMRuleListInfo()
			:
			id(kCBEmptyCRMRuleListID), name(NULL), list(NULL)
		{ };

		CRMRuleListInfo(const JPrefID& i, JString* n,
						JTextEditor::CRMRuleList* l)
			:
			id(i.GetID()), name(n), list(l)
		{ };

		void	CreateAndRead(std::istream& input, const JFileVersion vers);
		void	Write(std::ostream& output);
		void	Free();
	};

	static JBoolean	FindCRMRuleListID(const JArray<CRMRuleListInfo>& list,
									  const JIndex id, JIndex* index);
	static JIndex	FindCRMRuleListName(const JCharacter* crmName,
										const JArray<CRMRuleListInfo>& crmList);

private:

	JArray<FileTypeInfo>*		itsFileTypeList;
	JArray<MacroSetInfo>*		itsMacroList;
	JArray<CRMRuleListInfo>*	itsCRMList;
	JBoolean					itsExecOutputWordWrapFlag;
	JBoolean					itsUnknownTypeWordWrapFlag;

	JColorID	itsColor [ kColorCount ];

	CBEditFileTypesDialog*	itsFileTypesDialog;
	CBEditMacroDialog*		itsMacroDialog;
	CBEditCRMDialog*		itsCRMDialog;

private:

	void	UpdateFileTypes(const CBEditFileTypesDialog& dlog);
	void	UpdateMacros(const CBEditMacroDialog& dlog);
	void	UpdateCRMRuleLists(const CBEditCRMDialog& dlog);

	void	GetStringList(const JPrefID& id, JPtrArray<JString>* list) const;
	void	SetStringList(const JPrefID& id, const JPtrArray<JString>& list);

	void	ConvertFromSuffixLists();
	void	ConvertHTMLSuffixesToFileTypes(const std::string& data);
	void	AddDefaultCMacros(CBMacroManager* mgr) const;
	void	AddDefaultEiffelMacros(CBMacroManager* mgr) const;
	void	AddDefaultFortranMacros(CBMacroManager* mgr) const;
	void	AddDefaultHTMLMacros(CBMacroManager* mgr) const;
	void	AddDefaultJavaMacros(CBMacroManager* mgr) const;
	void	AddDefaultCSharpMacros(CBMacroManager* mgr) const;
	void	AddDefaultXMLMacros(CBMacroManager* mgr) const;
	void	AddDefaultXMLActions(CBCharActionManager* mgr) const;
	void	InitComplementSuffix(const JString& suffix, JString* complSuffix) const;

	void	ReadFileTypeInfo(const JFileVersion vers);
	void	WriteFileTypeInfo();

	void	CreateAndReadData(JArray<MacroSetInfo>** list, const JFileVersion vers);
	void	WriteData(const JArray<MacroSetInfo>& list);

	void	CreateCRMRuleLists();
	void	CreateAndReadData(JArray<CRMRuleListInfo>** list, const JFileVersion vers);
	void	WriteData(const JArray<CRMRuleListInfo>& list);

	JArray<FileTypeInfo>*		CreateFileTypeList();
	JArray<MacroSetInfo>*		CreateMacroList();
	JArray<CRMRuleListInfo>*	CreateCRMList();

	CBTextFileType	GetFileType(const JString& fileName, JIndex* index) const;
	JBoolean		CalcFileType(const CBTextDocument& doc, JIndex* index) const;
	JString			CleanFileName(const JCharacter* name) const;

	void	ReadColors();
	void	WriteColors();

	void	ReadStaticGlobalPrefs(const JFileVersion vers) const;
	void	WriteStaticGlobalPrefs();

	static JListT::CompareResult
		CompareFileTypeSpecAndLength(const FileTypeInfo& i1, const FileTypeInfo& i2);
	static JListT::CompareResult
		CompareMacroNames(const MacroSetInfo& i1, const MacroSetInfo& i2);
	static JListT::CompareResult
		CompareCRMNames(const CRMRuleListInfo& i1, const CRMRuleListInfo& i2);

	// not allowed

	CBPrefsManager(const CBPrefsManager& source);
	CBPrefsManager& operator=(const CBPrefsManager& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFileTypesChanged;
	static const JCharacter* kTextColorChanged;

	class FileTypesChanged : public JBroadcaster::Message
		{
		public:

			FileTypesChanged()
				:
				JBroadcaster::Message(kFileTypesChanged)
			{
				for (JIndex i=0; i<kCBFTCount; i++)
					{
					itsStatus[i] = kJFalse;
					}
			};

			JBoolean
			Changed(const CBTextFileType fileType)
				const
			{
				return itsStatus[ fileType ];
			}

			JBoolean
			Changed(JBoolean (*typeCheckFn)(const CBTextFileType type))
				const
			{
				for (JIndex i=0; i<kCBFTCount; i++)
					{
					if (itsStatus[i] && typeCheckFn((CBTextFileType) i))
						{
						return kJTrue;
						}
					}
				return kJFalse;
			}

			JBoolean
			AnyChanged()
				const
			{
				for (JIndex i=0; i<kCBFTCount; i++)
					{
					if (itsStatus[i])
						{
						return kJTrue;
						}
					}
				return kJFalse;
			}

			void
			SetChanged(const CBTextFileType fileType, const JBoolean status)
			{
				itsStatus[ fileType ] = status;
			}

		private:

			JBoolean itsStatus[ kCBFTCount ];
		};

	class TextColorChanged : public JBroadcaster::Message
		{
		public:

			TextColorChanged()
				:
				JBroadcaster::Message(kTextColorChanged)
				{ };
		};
};


/******************************************************************************
 ColorIndexValid (static)

 ******************************************************************************/

inline JBoolean
CBPrefsManager::ColorIndexValid
	(
	const JIndex index
	)
{
	return JI2B( 1 <= index && index <= kColorCount );
}

#endif
