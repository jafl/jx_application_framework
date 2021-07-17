/******************************************************************************
 CBDocumentManager.h

	Interface for the CBDocumentManager class

	Copyright © 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDocumentManager
#define _H_CBDocumentManager

#include <JXDocumentManager.h>
#include "CBTextFileType.h"
#include <JPrefObject.h>
#include <JPtrArray.h>

class JRegex;
class JSTStyler;
class JXWindow;
class CBProjectDocument;
class CBTextDocument;
class CBShellDocument;
class CBExecOutputDocument;
class CBExtEditorDialog;
class CBFileHistoryMenu;

class CBDocumentManager : public JXDocumentManager, public JPrefObject
{
public:

	enum FileHistoryType
	{
		kProjectFileHistory,
		kTextFileHistory
	};

	enum
	{
		kMinWarnFileSize = 5000000
	};

public:

	CBDocumentManager();

	virtual ~CBDocumentManager();

	bool	GetTemplateDirectory(const JUtf8Byte* dirName,
									 const bool create, JString* fullName);
	bool	GetTextTemplateDirectory(const bool create, JString* tmplDir);

	bool	NewProjectDocument(CBProjectDocument** doc = nullptr);
	void		NewTextDocument();
	void		NewTextDocumentFromTemplate();
	void		NewShellDocument();
	bool	OpenTextDocument(const JString& fileName,
								 const JIndex lineIndex = 0,
								 CBTextDocument** doc = nullptr,
								 const bool iconify = false,
								 const bool forceReload = false);
	bool	OpenTextDocument(const JString& fileName,
								 const JIndexRange& lineRange,
								 CBTextDocument** doc = nullptr,
								 const bool iconify = false,
								 const bool forceReload = false);
	static bool	WarnFileSize(const JString& fileName);

	void	OpenBinaryDocument(const JString& fileName);

	void	OpenSomething(const JString& fileName = JString::empty,
						  const JIndexRange lineRange = JIndexRange(),
						  const bool iconify = false,
						  const bool forceReload = false);
	void	OpenSomething(const JPtrArray<JString>& fileNameList);

	void	ReadFromProject(std::istream& input, const JFileVersion vers);
	void	WriteForProject(std::ostream& output) const;

	bool	HasProjectDocuments() const;
	bool	GetActiveProjectDocument(CBProjectDocument** doc) const;
	bool	ProjectDocumentIsOpen(const JString& fileName,
									  CBProjectDocument** doc) const;
	bool	CloseProjectDocuments();

	bool	HasTextDocuments() const;
	JSize		GetTextDocumentCount() const;
	bool	GetActiveTextDocument(CBTextDocument** doc) const;
	bool	TextDocumentsNeedSave();
	bool	SaveTextDocuments(const bool saveUntitled);
	void		ReloadTextDocuments(const bool force);
	bool	CloseTextDocuments();
	void		FileRenamed(const JString& origFullName, const JString& newFullName);
	void		StylerChanged(JSTStyler* styler);

	void	ProjDocCreated(CBProjectDocument* doc);
	void	ProjDocDeleted(CBProjectDocument* doc);
	void	SetActiveProjectDocument(CBProjectDocument* doc);

	void	TextDocumentCreated(CBTextDocument* doc);
	void	TextDocumentDeleted(CBTextDocument* doc);
	void	SetActiveTextDocument(CBTextDocument* doc);
	void	TextDocumentNeedsSave();

	bool	GetActiveListDocument(CBExecOutputDocument** doc) const;
	void		SetActiveListDocument(CBExecOutputDocument* doc);

	bool	OpenComplementFile(const JString& fullName, const CBTextFileType type,
								   CBProjectDocument* projDoc = nullptr,
								   const bool searchDirs = true);
	bool	GetComplementFile(const JString& inputName, const CBTextFileType inputType,
								  JString* outputName, CBProjectDocument* projDoc = nullptr,
								  const bool searchDirs = true) const;
	bool	GetOpenComplementFile(const JString& inputName,
									  const CBTextFileType inputType,
									  JXFileDocument** doc) const;

	JPtrArray<CBProjectDocument>*	GetProjectDocList() const;
	JPtrArray<CBTextDocument>*		GetTextDocList() const;

	void	AddToFileHistoryMenu(const FileHistoryType type,
								 const JString& fullName);

	void	UpdateSymbolDatabases();
	void	CancelUpdateSymbolDatabases();
	void	RefreshVCSStatus();

	void		ChooseEditors();
	bool	WillEditTextFilesLocally() const;
	void		ShouldEditTextFilesLocally(const bool local);
	bool	WillEditBinaryFilesLocally() const;
	void		ShouldEditBinaryFilesLocally(const bool local);

	// called by CBPrefsManager

	bool	RestoreState(std::istream& input);
	bool	SaveState(std::ostream& output) const;

	// called by CBEditGenPrefsDialog

	void	GetWarnings(bool* warnBeforeSaveAll, bool* warnBeforeCloseAll) const;
	void	SetWarnings(const bool warnBeforeSaveAll, const bool warnBeforeCloseAll);

	// called by CBCreateGlobals()

	void	CreateFileHistoryMenus(JXWindow* window);

	// called by CBFileHistoryMenu

	CBFileHistoryMenu*	GetFileHistoryMenu(const FileHistoryType type) const;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<CBProjectDocument>*	itsProjectDocuments;

	JPtrArray<CBTextDocument>*		itsTextDocuments;
	CBExecOutputDocument*			itsListDocument;
	bool						itsTextNeedsSaveFlag;

	bool	itsWarnBeforeSaveAllFlag;
	bool	itsWarnBeforeCloseAllFlag;

	// recent files

	CBFileHistoryMenu*	itsRecentProjectMenu;
	CBFileHistoryMenu*	itsRecentTextMenu;

	// external editors

	bool	itsEditTextLocalFlag;
	JString		itsEditTextFileCmd;
	JString		itsEditTextFileLineCmd;

	bool	itsEditBinaryLocalFlag;
	JString		itsEditBinaryFileCmd;

	CBExtEditorDialog*	itsExtEditorDialog;

private:

	void		PrivateOpenSomething(const JString& fileName,
									 const JIndexRange& lineRange,
									 const bool iconify,
									 const bool forceReload);
	bool	PrivateOpenTextDocument(const JString& fullName,
										const JIndexRange& lineRange,
										const bool iconify,
										const bool forceReload,
										CBTextDocument** doc) const;
	void		PrivateOpenBinaryDocument(const JString& fullName,
										  const bool iconify,
										  const bool forceReload) const;

	bool	FindComplementFile(const JString& inputName,
								   const CBTextFileType outputType,
								   JString* outputName,
								   CBProjectDocument* projDoc,
								   const bool searchDirs) const;
	bool	SearchForComplementFile(CBProjectDocument* projDoc,
										const JString& origFullName,
										const JString& baseFullName,
										const JPtrArray<JString>& suffixList,
										JString* outputName) const;

	bool	FindOpenComplementFile(const JString& inputName,
									   const CBTextFileType outputType,
									   JXFileDocument** doc) const;

	// not allowed

	CBDocumentManager(const CBDocumentManager& source);
	const CBDocumentManager& operator=(const CBDocumentManager& source);

private:

	// JBroadcaster message base class

	class ProjectDocumentMessage : public JBroadcaster::Message
		{
		public:

			ProjectDocumentMessage(const JUtf8Byte* type, CBProjectDocument* doc)
				:
				JBroadcaster::Message(type),
				itsDoc(doc)
				{ };

			CBProjectDocument*
			GetProjectDocument() const
			{
				return itsDoc;
			}

		private:

			CBProjectDocument*	itsDoc;
		};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kProjectDocumentCreated;
	static const JUtf8Byte* kProjectDocumentDeleted;
	static const JUtf8Byte* kProjectDocumentActivated;
	static const JUtf8Byte* kAddFileToHistory;

	class ProjectDocumentCreated : public ProjectDocumentMessage
		{
		public:

			ProjectDocumentCreated(CBProjectDocument* doc)
				:
				ProjectDocumentMessage(kProjectDocumentCreated, doc)
				{ };
		};

	class ProjectDocumentDeleted : public ProjectDocumentMessage
		{
		public:

			ProjectDocumentDeleted(CBProjectDocument* doc)
				:
				ProjectDocumentMessage(kProjectDocumentDeleted, doc)
				{ };
		};

	class ProjectDocumentActivated : public ProjectDocumentMessage
		{
		public:

			ProjectDocumentActivated(CBProjectDocument* doc)
				:
				ProjectDocumentMessage(kProjectDocumentActivated, doc)
				{ };

			ProjectDocumentActivated(JPtrArray<CBProjectDocument>* list)
				:
				ProjectDocumentMessage(kProjectDocumentActivated,
									   list->IsEmpty() ?
											(CBProjectDocument*) nullptr :
											list->GetFirstElement())
				{ };
		};

	class AddFileToHistory : public JBroadcaster::Message
		{
		public:

			AddFileToHistory(const FileHistoryType type,
							 const JString& fullName)
				:
				JBroadcaster::Message(kAddFileToHistory),
				itsType(type), itsFullName(fullName)
				{ };

			FileHistoryType
			GetFileHistoryType() const
			{
				return itsType;
			}

			const JString&
			GetFullName() const
			{
				return itsFullName;
			}

		private:

			const FileHistoryType	itsType;
			const JString&			itsFullName;
		};
};


/******************************************************************************
 HasTextDocuments

 ******************************************************************************/

inline bool
CBDocumentManager::HasTextDocuments()
	const
{
	return !itsTextDocuments->IsEmpty();
}

/******************************************************************************
 GetTextDocumentCount

 ******************************************************************************/

inline JSize
CBDocumentManager::GetTextDocumentCount()
	const
{
	return itsTextDocuments->GetElementCount();
}

/******************************************************************************
 GetTextDocList

 ******************************************************************************/

inline JPtrArray<CBTextDocument>*
CBDocumentManager::GetTextDocList()
	const
{
	return itsTextDocuments;
}

/******************************************************************************
 TextDocumentsNeedSave

 ******************************************************************************/

inline bool
CBDocumentManager::TextDocumentsNeedSave()
{
	return !itsTextDocuments->IsEmpty() && itsTextNeedsSaveFlag;
}

/******************************************************************************
 TextDocumentNeedsSave

	Called by CBTextDocument.

 ******************************************************************************/

inline void
CBDocumentManager::TextDocumentNeedsSave()
{
	itsTextNeedsSaveFlag = true;
}

/******************************************************************************
 HasProjectDocuments

 ******************************************************************************/

inline bool
CBDocumentManager::HasProjectDocuments()
	const
{
	return !itsProjectDocuments->IsEmpty();
}

/******************************************************************************
 GetProjectDocList

 ******************************************************************************/

inline JPtrArray<CBProjectDocument>*
CBDocumentManager::GetProjectDocList()
	const
{
	return itsProjectDocuments;
}

/******************************************************************************
 SetActiveProjectDocument

	Called by CBProjectDocument.

 ******************************************************************************/

inline void
CBDocumentManager::SetActiveProjectDocument
	(
	CBProjectDocument* doc
	)
{
	JIndex i;
	if (itsProjectDocuments->Find(doc, &i) && i != 1)
		{
		itsProjectDocuments->MoveElementToIndex(i, 1);
		}
}

/******************************************************************************
 Warnings

 ******************************************************************************/

inline void
CBDocumentManager::GetWarnings
	(
	bool* warnBeforeSaveAll,
	bool* warnBeforeCloseAll
	)
	const
{
	*warnBeforeSaveAll  = itsWarnBeforeSaveAllFlag;
	*warnBeforeCloseAll = itsWarnBeforeCloseAllFlag;
}

inline void
CBDocumentManager::SetWarnings
	(
	const bool warnBeforeSaveAll,
	const bool warnBeforeCloseAll
	)
{
	itsWarnBeforeSaveAllFlag  = warnBeforeSaveAll;
	itsWarnBeforeCloseAllFlag = warnBeforeCloseAll;
}

/******************************************************************************
 Edit files locally

 ******************************************************************************/

inline bool
CBDocumentManager::WillEditTextFilesLocally()
	const
{
	return itsEditTextLocalFlag;
}

inline void
CBDocumentManager::ShouldEditTextFilesLocally
	(
	const bool local
	)
{
	itsEditTextLocalFlag = local;
}

inline bool
CBDocumentManager::WillEditBinaryFilesLocally()
	const
{
	return itsEditBinaryLocalFlag;
}

inline void
CBDocumentManager::ShouldEditBinaryFilesLocally
	(
	const bool local
	)
{
	itsEditBinaryLocalFlag = local;
}

#endif
