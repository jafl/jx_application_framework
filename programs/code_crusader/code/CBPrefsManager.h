/******************************************************************************
 CBPrefsManager.h

	Copyright © 1997-2001 John Lindal.

 ******************************************************************************/

#ifndef _H_CBPrefsManager
#define _H_CBPrefsManager

#include <JXPrefsManager.h>
#include "CBTextFileType.h"
#include "CBEmulator.h"
#include <JStyledText.h>		// need definition of CRMRule

class JRegex;
class JPoint;

class JXWindow;
class JXChooseSaveFile;

class CBTextDocument;
class CBTextEditor;
class CBCharActionManager;
class CBMacroManager;

class CBEditFileTypesDialog;
class CBEditMacroDialog;
class CBEditCRMDialog;

const JUtf8Byte kCBContentRegexMarker = '^';

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
	kCBGoStyleID,
	kCBGoTreeToolBarID,
	kCBDStyleID,
	kCBDTreeToolBarID,

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

	CBPrefsManager(bool* isNew);

	virtual ~CBPrefsManager();

	JString	GetJCCVersionStr() const;

	bool	GetExpirationTimeStamp(time_t* t) const;
	void	SetExpirationTimeStamp(const time_t t);

	void	EditFileTypes();
	void	EditMacros(CBMacroManager* macroMgr);
	void	EditCRMRuleLists(JStyledText::CRMRuleList* ruleList);

	bool	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
						  JCoordinate* width, JCoordinate* height) const;
	void	SaveWindowSize(const JPrefID& id, JXWindow* window);

	bool	RestoreProgramState();
	void	SaveProgramState();
	void	ForgetProgramState();

	// text editor

	void	GetDefaultFont(JString* name, JSize* size) const;
	void	SetDefaultFont(const JString& name, const JSize size);

	JColorID	GetColor(const JIndex index) const;
	void		SetColor(const JIndex index, const JColorID color);
	static bool	ColorIndexValid(const JIndex index);

	CBEmulator	GetEmulator() const;
	void		SetEmulator(const CBEmulator type);

	// file types

	CBTextFileType	GetFileType(const JString& fileName) const;
	CBTextFileType	GetFileType(const CBTextDocument& doc,
								CBCharActionManager** actionMgr, CBMacroManager** macroMgr,
								JStyledText::CRMRuleList** crmRuleList,
								JString* scriptPath, bool* wordWrap) const;

	bool	EditWithOtherProgram(const JString& fileName, JString* cmd) const;

	void	GetFileSuffixes(const CBTextFileType type,
							JPtrArray<JString>* list) const;
	void	GetAllFileSuffixes(JPtrArray<JString>* list) const;

	JString	GetDefaultComplementSuffix(const JString& name, const CBTextFileType type,
									   JIndex* index) const;
	void	SetDefaultComplementSuffix(const JIndex index, const JString& name);

	static bool	FileMatchesSuffix(const JString& fileName,
								  const JPtrArray<JString>& suffixList);

	static bool	GetScriptPaths(JString* sysDir, JString* userDir);

protected:

	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	virtual void	SaveAllBeforeDestruct() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

public:

	struct FileTypeInfo
	{
		JString*		suffix;
		JRegex*			nameRegex;		// can be nullptr
		JRegex*			contentRegex;	// can be nullptr
		JUtf8ByteRange	literalRange;	// initially nothing
		CBTextFileType	type;
		JIndex			macroID;		// MacroSetInfo::id
		JIndex			crmID;			// CRMRuleListInfo::id
		bool			isUserScript;	// true if relative to ~/.jxcb/scripts/
		JString*		scriptPath;		// nullptr if none; *relative*
		bool			wordWrap;
		JString*		complSuffix;
		JString*		editCmd;		// nullptr if type != kCBExternalFT

		FileTypeInfo()
			:
			suffix(nullptr), nameRegex(nullptr), contentRegex(nullptr), type(kCBUnknownFT),
			macroID(kCBEmptyMacroID), crmID(kCBEmptyCRMRuleListID),
			isUserScript(true), scriptPath(nullptr),
			wordWrap(true), complSuffix(nullptr), editCmd(nullptr)
		{ };

		FileTypeInfo(JString* s, JRegex* nr, JRegex* cr, const CBTextFileType t,
					 const JIndex macro, const JIndex crm,
					 const bool us, JString* sp,
					 const bool wrap, JString* cs, JString* ec)
			:
			suffix(s), nameRegex(nr), contentRegex(cr), type(t),
			macroID(macro), crmID(crm), isUserScript(us), scriptPath(sp),
			wordWrap(wrap), complSuffix(cs), editCmd(ec)
		{ };

		bool	IsPlainSuffix() const;
		void	CreateRegex();
		void	Free();
	};

	static JUtf8ByteRange	GetLiteralPrefixRange(const JString& regexStr);

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
			id(kCBEmptyMacroID), name(nullptr),
			action(nullptr), macro(nullptr)
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

	static bool		FindMacroID(const JArray<MacroSetInfo>& list,
								const JIndex id, JIndex* index);
	static JIndex	FindMacroName(const JUtf8Byte* macroName,
								  JArray<MacroSetInfo>* macroList,
								  const bool create);

	struct CRMRuleListInfo
	{
		JIndex						id;
		JString*					name;
		JStyledText::CRMRuleList*	list;

		CRMRuleListInfo()
			:
			id(kCBEmptyCRMRuleListID), name(nullptr), list(nullptr)
		{ };

		CRMRuleListInfo(const JPrefID& i, JString* n,
						JStyledText::CRMRuleList* l)
			:
			id(i.GetID()), name(n), list(l)
		{ };

		void	CreateAndRead(std::istream& input, const JFileVersion vers);
		void	Write(std::ostream& output);
		void	Free();
	};

	static bool		FindCRMRuleListID(const JArray<CRMRuleListInfo>& list,
									  const JIndex id, JIndex* index);
	static JIndex	FindCRMRuleListName(const JUtf8Byte* crmName,
										const JArray<CRMRuleListInfo>& crmList);

private:

	JArray<FileTypeInfo>*		itsFileTypeList;
	JArray<MacroSetInfo>*		itsMacroList;
	JArray<CRMRuleListInfo>*	itsCRMList;
	bool						itsExecOutputWordWrapFlag;
	bool						itsUnknownTypeWordWrapFlag;

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
	void	CreateDCRMRuleList();
	void	CreateAndReadData(JArray<CRMRuleListInfo>** list, const JFileVersion vers);
	void	WriteData(const JArray<CRMRuleListInfo>& list);

	JArray<FileTypeInfo>*		CreateFileTypeList();
	JArray<MacroSetInfo>*		CreateMacroList();
	JArray<CRMRuleListInfo>*	CreateCRMList();

	CBTextFileType	GetFileType(const JString& fileName, JIndex* index) const;
	bool			CalcFileType(const CBTextDocument& doc, JIndex* index) const;
	JString			CleanFileName(const JString& name) const;

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

	static const JUtf8Byte* kFileTypesChanged;
	static const JUtf8Byte* kTextColorChanged;

	class FileTypesChanged : public JBroadcaster::Message
		{
		public:

			FileTypesChanged()
				:
				JBroadcaster::Message(kFileTypesChanged)
			{
				for (JUnsignedOffset i=0; i<kCBFTCount; i++)
					{
					itsStatus[i] = false;
					}
			};

			bool
			Changed(const CBTextFileType fileType)
				const
			{
				return itsStatus[ fileType ];
			}

			bool
			Changed(bool (*typeCheckFn)(const CBTextFileType type))
				const
			{
				for (JUnsignedOffset i=0; i<kCBFTCount; i++)
					{
					if (itsStatus[i] && typeCheckFn((CBTextFileType) i))
						{
						return true;
						}
					}
				return false;
			}

			bool
			AnyChanged()
				const
			{
				for (JUnsignedOffset i=0; i<kCBFTCount; i++)
					{
					if (itsStatus[i])
						{
						return true;
						}
					}
				return false;
			}

			void
			SetChanged(const CBTextFileType fileType, const bool status)
			{
				itsStatus[ fileType ] = status;
			}

		private:

			bool itsStatus[ kCBFTCount ];
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

inline bool
CBPrefsManager::ColorIndexValid
	(
	const JIndex index
	)
{
	return 1 <= index && index <= kColorCount;
}

#endif
