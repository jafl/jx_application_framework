/******************************************************************************
 CBDocumentManager.h

	Interface for the CBDocumentManager class

	Copyright (C) 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBDocumentManager
#define _H_CBDocumentManager

#include <JXDocumentManager.h>
#include "CBTextFileType.h"
#include <JPrefObject.h>
#include <JPtrArray.h>

class JRegex;
class JTEStyler;
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
		kMinWarnFileSize = 1000000
	};

public:

	CBDocumentManager();

	virtual ~CBDocumentManager();

	JBoolean	GetTemplateDirectory(const JCharacter* dirName,
									 const JBoolean create, JString* fullName);
	JBoolean	GetTextTemplateDirectory(const JBoolean create, JString* tmplDir);

	JBoolean	NewProjectDocument(CBProjectDocument** doc = NULL);
	void		NewTextDocument();
	void		NewTextDocumentFromTemplate();
	void		NewShellDocument();
	JBoolean	OpenTextDocument(const JCharacter* fileName,
								 const JIndex lineIndex = 0,
								 CBTextDocument** doc = NULL,
								 const JBoolean iconify = kJFalse,
								 const JBoolean forceReload = kJFalse);
	JBoolean	OpenTextDocument(const JCharacter* fileName,
								 const JIndexRange& lineRange,
								 CBTextDocument** doc = NULL,
								 const JBoolean iconify = kJFalse,
								 const JBoolean forceReload = kJFalse);
	static JBoolean	WarnFileSize(const JCharacter* fileName);

	void	OpenBinaryDocument(const JCharacter* fileName);

	void	OpenSomething(const JCharacter* fileName = NULL,
						  const JIndexRange lineRange = JIndexRange(),
						  const JBoolean iconify = kJFalse,
						  const JBoolean forceReload = kJFalse);
	void	OpenSomething(const JPtrArray<JString>& fileNameList);

	void	ReadFromProject(std::istream& input, const JFileVersion vers);
	void	WriteForProject(std::ostream& output) const;

	JBoolean	HasProjectDocuments() const;
	JBoolean	GetActiveProjectDocument(CBProjectDocument** doc) const;
	JBoolean	ProjectDocumentIsOpen(const JCharacter* fileName,
									  CBProjectDocument** doc) const;
	JBoolean	CloseProjectDocuments();

	JBoolean	HasTextDocuments() const;
	JSize		GetTextDocumentCount() const;
	JBoolean	GetActiveTextDocument(CBTextDocument** doc) const;
	JBoolean	TextDocumentsNeedSave();
	JBoolean	SaveTextDocuments(const JBoolean saveUntitled);
	void		ReloadTextDocuments(const JBoolean force);
	JBoolean	CloseTextDocuments();
	void		FileRenamed(const JCharacter* origFullName, const JCharacter* newFullName);
	void		StylerChanged(JTEStyler* styler);

	void	ProjDocCreated(CBProjectDocument* doc);
	void	ProjDocDeleted(CBProjectDocument* doc);
	void	SetActiveProjectDocument(CBProjectDocument* doc);

	void	TextDocumentCreated(CBTextDocument* doc);
	void	TextDocumentDeleted(CBTextDocument* doc);
	void	SetActiveTextDocument(CBTextDocument* doc);
	void	TextDocumentNeedsSave();

	JBoolean	GetActiveListDocument(CBExecOutputDocument** doc) const;
	void		SetActiveListDocument(CBExecOutputDocument* doc);

	JBoolean	OpenComplementFile(const JString& fullName, const CBTextFileType type,
								   CBProjectDocument* projDoc = NULL,
								   const JBoolean searchDirs = kJTrue);
	JBoolean	GetComplementFile(const JString& inputName, const CBTextFileType inputType,
								  JString* outputName, CBProjectDocument* projDoc = NULL,
								  const JBoolean searchDirs = kJTrue) const;
	JBoolean	GetOpenComplementFile(const JString& inputName,
									  const CBTextFileType inputType,
									  JXFileDocument** doc) const;

	JBoolean	SearchFile(const JCharacter* fileName, const JCharacter* searchPattern,
						   const JBoolean caseSensitive, JIndex* lineIndex) const;

	JPtrArray<CBProjectDocument>*	GetProjectDocList() const;
	JPtrArray<CBTextDocument>*		GetTextDocList() const;

	void	AddToFileHistoryMenu(const FileHistoryType type,
								 const JCharacter* fullName);

	void	UpdateSymbolDatabases();
	void	CancelUpdateSymbolDatabases();
	void	RefreshCVSStatus();

	void		ChooseEditors();
	JBoolean	WillEditTextFilesLocally() const;
	void		ShouldEditTextFilesLocally(const JBoolean local);
	JBoolean	WillEditBinaryFilesLocally() const;
	void		ShouldEditBinaryFilesLocally(const JBoolean local);

	// called by CBPrefsManager

	JBoolean	RestoreState(std::istream& input);
	JBoolean	SaveState(std::ostream& output) const;

	// called by CBEditGenPrefsDialog

	void	GetWarnings(JBoolean* warnBeforeSaveAll, JBoolean* warnBeforeCloseAll) const;
	void	SetWarnings(const JBoolean warnBeforeSaveAll, const JBoolean warnBeforeCloseAll);

	// called by CBCreateGlobals()

	void	CreateFileHistoryMenus(JXWindow* window);

	// called by CBFileHistoryMenu

	CBFileHistoryMenu*	GetFileHistoryMenu(const FileHistoryType type) const;

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<CBProjectDocument>*	itsProjectDocuments;

	JPtrArray<CBTextDocument>*		itsTextDocuments;
	CBExecOutputDocument*			itsListDocument;
	JBoolean						itsTextNeedsSaveFlag;

	JBoolean	itsWarnBeforeSaveAllFlag;
	JBoolean	itsWarnBeforeCloseAllFlag;

	// recent files

	CBFileHistoryMenu*	itsRecentProjectMenu;
	CBFileHistoryMenu*	itsRecentTextMenu;

	// external editors

	JBoolean	itsEditTextLocalFlag;
	JString		itsEditTextFileCmd;
	JString		itsEditTextFileLineCmd;

	JBoolean	itsEditBinaryLocalFlag;
	JString		itsEditBinaryFileCmd;

	CBExtEditorDialog*	itsExtEditorDialog;

private:

	void		PrivateOpenSomething(const JCharacter* fileName,
									 const JIndexRange& lineRange,
									 const JBoolean iconify,
									 const JBoolean forceReload);
	JBoolean	PrivateOpenTextDocument(const JCharacter* fullName,
										const JIndexRange& lineRange,
										const JBoolean iconify,
										const JBoolean forceReload,
										CBTextDocument** doc) const;
	void		PrivateOpenBinaryDocument(const JCharacter* fullName,
										  const JBoolean iconify,
										  const JBoolean forceReload) const;

	JBoolean	FindComplementFile(const JString& inputName,
								   const CBTextFileType outputType,
								   JString* outputName,
								   CBProjectDocument* projDoc,
								   const JBoolean searchDirs) const;
	JBoolean	SearchForComplementFile(CBProjectDocument* projDoc,
										const JString& origFullName,
										const JString& baseFullName,
										const JPtrArray<JString>& suffixList,
										JString* outputName) const;

	JBoolean	FindOpenComplementFile(const JString& inputName,
									   const CBTextFileType outputType,
									   JXFileDocument** doc) const;

	JBoolean	SearchLine(const JString& text, const JRegex& pattern,
						   JIndex* charIndex) const;

	// not allowed

	CBDocumentManager(const CBDocumentManager& source);
	const CBDocumentManager& operator=(const CBDocumentManager& source);

private:

	// JBroadcaster message base class

	class ProjectDocumentMessage : public JBroadcaster::Message
		{
		public:

			ProjectDocumentMessage(const JCharacter* type, CBProjectDocument* doc)
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

	static const JCharacter* kProjectDocumentCreated;
	static const JCharacter* kProjectDocumentDeleted;
	static const JCharacter* kProjectDocumentActivated;
	static const JCharacter* kAddFileToHistory;

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
											(CBProjectDocument*) NULL :
											list->FirstElement())
				{ };
		};

	class AddFileToHistory : public JBroadcaster::Message
		{
		public:

			AddFileToHistory(const FileHistoryType type,
							 const JCharacter* fullName)
				:
				JBroadcaster::Message(kAddFileToHistory),
				itsType(type), itsFullName(fullName)
				{ };

			FileHistoryType
			GetType() const
			{
				return itsType;
			}

			const JCharacter*
			GetFullName() const
			{
				return itsFullName;
			}

		private:

			const FileHistoryType	itsType;
			const JCharacter*		itsFullName;
		};
};


/******************************************************************************
 HasTextDocuments

 ******************************************************************************/

inline JBoolean
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

inline JBoolean
CBDocumentManager::TextDocumentsNeedSave()
{
	return JConvertToBoolean( !itsTextDocuments->IsEmpty() && itsTextNeedsSaveFlag );
}

/******************************************************************************
 TextDocumentNeedsSave

	Called by CBTextDocument.

 ******************************************************************************/

inline void
CBDocumentManager::TextDocumentNeedsSave()
{
	itsTextNeedsSaveFlag = kJTrue;
}

/******************************************************************************
 HasProjectDocuments

 ******************************************************************************/

inline JBoolean
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
	JBoolean* warnBeforeSaveAll,
	JBoolean* warnBeforeCloseAll
	)
	const
{
	*warnBeforeSaveAll  = itsWarnBeforeSaveAllFlag;
	*warnBeforeCloseAll = itsWarnBeforeCloseAllFlag;
}

inline void
CBDocumentManager::SetWarnings
	(
	const JBoolean warnBeforeSaveAll,
	const JBoolean warnBeforeCloseAll
	)
{
	itsWarnBeforeSaveAllFlag  = warnBeforeSaveAll;
	itsWarnBeforeCloseAllFlag = warnBeforeCloseAll;
}

/******************************************************************************
 Edit files locally

 ******************************************************************************/

inline JBoolean
CBDocumentManager::WillEditTextFilesLocally()
	const
{
	return itsEditTextLocalFlag;
}

inline void
CBDocumentManager::ShouldEditTextFilesLocally
	(
	const JBoolean local
	)
{
	itsEditTextLocalFlag = local;
}

inline JBoolean
CBDocumentManager::WillEditBinaryFilesLocally()
	const
{
	return itsEditBinaryLocalFlag;
}

inline void
CBDocumentManager::ShouldEditBinaryFilesLocally
	(
	const JBoolean local
	)
{
	itsEditBinaryLocalFlag = local;
}

#endif
