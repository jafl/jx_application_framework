/******************************************************************************
 CBProjectDocument.h

	Interface for the CBProjectDocument class

	Copyright (C) 1996-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBProjectDocument
#define _H_CBProjectDocument

#include <JXFileDocument.h>
#include "CBDirList.h"
#include "CBTextFileType.h"
#include "CBProjectTable.h"			// need defn of DropFileAction
#include "CBRelPathCSF.h"			// need defn of PathType
#include "CBBuildManager.h"			// need defn of MakefileMethod
#include "CBExecOutputDocument.h"	// need defn of RecordLink
#include "CBCommandManager.h"		// need defn of CmdList

class JProcess;
class JPTPrinter;
class JXTextMenu;
class JXTextButton;
class JXToolBar;
class JXWidgetSet;
class JXPTPrinter;
class JXTimerTask;
class JXProgressDisplay;
class JXProgressIndicator;
class CBProjectTree;
class CBSymbolDirector;
class CBTreeDirector;
class CBCTreeDirector;
class CBJavaTreeDirector;
class CBPHPTreeDirector;
class CBCommandMenu;
class CBFileListDirector;
class CBFileListTable;
class CBRelPathCSF;
class CBEditSearchPathsDialog;
class CBWaitForSymbolUpdateTask;
class CBDelaySymbolUpdateTask;

class CBProjectDocument : public JXFileDocument
{
public:

	enum UpdateMessageType
	{
		kFixedLengthStart,
		kVariableLengthStart,
		kProgressIncrement,
		kLockSymbolTable,
		kSymbolTableLocked,
		kSymbolTableWritten,
		kDoItYourself
	};

public:

	static JBoolean		Create(CBProjectDocument** doc);
	static FileStatus	Create(const JString& fullName, const JBoolean silent,
							   CBProjectDocument** doc);
	static FileStatus	CanReadFile(const JString& fullName);
	static FileStatus	CanReadFile(std::istream& input, JFileVersion* actualFileVersion);

	virtual ~CBProjectDocument();

	virtual void			Activate() override;
	virtual const JString&	GetName() const override;
	virtual JBoolean		GetMenuIcon(const JXImage** icon) const override;
	virtual JBoolean		Close() override;

	CBProjectTree*	GetFileTree() const;
	CBProjectTable*	GetFileTable() const;
	void			AddFile(const JString& fullName,
							const CBRelPathCSF::PathType pathType);
	void			EditMakeConfig();

	CBCommandManager*	GetCommandManager() const;
	CBBuildManager*		GetBuildManager() const;
	const JString&		GetBuildTargetName() const;
	CBFileListDirector*	GetFileListDirector() const;
	CBFileListTable*	GetAllFileList() const;
	CBSymbolDirector*	GetSymbolDirector() const;
	CBCTreeDirector*	GetCTreeDirector() const;
	CBJavaTreeDirector*	GetJavaTreeDirector() const;
	CBPHPTreeDirector*	GetPHPTreeDirector() const;
	CBRelPathCSF*		GetRelPathCSF() const;

	JBoolean					HasDirectories() const;
	const CBDirList&			GetDirectories() const;
	CBEditSearchPathsDialog*	EditSearchPaths(JXDirector* owner);

	void	DelayUpdateSymbolDatabase();
	void	UpdateSymbolDatabase();
	void	CancelUpdateSymbolDatabase();
	void	RefreshVCSStatus();

	void	SetProjectPrefs(const JBoolean reopenTextFiles,
							const JBoolean doubleSpaceCompile,
							const JBoolean rebuildMakefileDaily,
							const CBProjectTable::DropFileAction dropFileAction);

	static void	ReadStaticGlobalPrefs(std::istream& input, const JFileVersion vers);
	static void	WriteStaticGlobalPrefs(std::ostream& output);

	static JBoolean	WillReopenTextFiles();
	static void		ShouldReopenTextFiles(const JBoolean reopen);

	static JBoolean	WillAskOKToOpenOldVersion();
	static void		ShouldAskOKToOpenOldVersion(const JBoolean ask);

	static const JString&	GetAddFilesFilter();
	static void				SetAddFilesFilter(const JString& filter);

	static const JUtf8Byte*	GetProjectFileSuffix();

	void	ConvertCompileRunDialogs(std::istream& projInput, const JFileVersion vers);

	// called by CBNewProjectSaveFileDialog

	static const JUtf8Byte*	GetTemplateDirectoryName();
	static JBoolean			GetProjectTemplateType(const JString& fullName,
												   JString* type);

	// called by CBEditTreePrefsDialog

	void	SetTreePrefs(const JSize fontSize, const JBoolean showInheritedFns,
						 const JBoolean autoMinMILinks, const JBoolean drawMILinksOnTop,
						 const JBoolean raiseWhenSingleMatch, const JBoolean writePrefs);

	// called by CBCommandTable

	static JBoolean	ReadTasksFromProjectFile(std::istream& input, CBCommandManager::CmdList* cmdList);

protected:

	CBProjectDocument(const JString& fullName,
					  const CBBuildManager::MakefileMethod makefileMethod,
					  const JBoolean fromTemplate, const JString& tmplFile);
	CBProjectDocument(std::istream& input, const JString& projName,
					  const JString& setName, const JString& symName,
					  const JBoolean silent);

	virtual void	DiscardChanges() override;
	virtual JError	WriteFile(const JString& fullName, const JBoolean safetySave) const override;
	virtual void	WriteTextFile(std::ostream& output, const JBoolean safetySave) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	CBProjectTree*	itsFileTree;
	CBProjectTable*	itsFileTable;
	JBoolean		itsProcessNodeMessageFlag;		// kJTrue => process messages from itsFileTree

	CBCommandManager*	itsCmdMgr;
	CBBuildManager*		itsBuildMgr;
	CBRelPathCSF*		itsCSF;
	JXTimerTask*		itsSaveTask;
	JString				itsPrintName;
	mutable JString		itsDocName;					// so GetName() can return JString&

	CBDirList*			itsDirList;
	CBFileListDirector*	itsAllFileDirector;
	CBSymbolDirector*	itsSymbolDirector;
	CBCTreeDirector*	itsCTreeDirector;
	CBJavaTreeDirector*	itsJavaTreeDirector;
	CBPHPTreeDirector*	itsPHPTreeDirector;

	JProcess*							itsUpdateProcess;
	CBExecOutputDocument::RecordLink*	itsUpdateLink;
	std::ostream*						itsUpdateStream;
	JXProgressDisplay*					itsUpdatePG;
	CBWaitForSymbolUpdateTask*			itsWaitForUpdateTask;
	CBDelaySymbolUpdateTask*			itsDelaySymbolUpdateTask;
	JFloat								itsLastSymbolLoadTime;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsTreeMenu;
	JXTextMenu*		itsProjectMenu;
	JXTextMenu*		itsSourceMenu;
	CBCommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	CBEditSearchPathsDialog*	itsEditPathsDialog;

	static JBoolean	theReopenTextFilesFlag;
	static JBoolean	theWarnOpenOldVersionFlag;
	static JString	theAddFilesFilter;

// begin JXLayout

	JXToolBar*           itsToolBar;
	JXTextButton*        itsConfigButton;
	JXWidgetSet*         itsUpdateContainer;
	JXStaticText*        itsUpdateLabel;
	JXProgressIndicator* itsUpdateCleanUpIndicator;
	JXStaticText*        itsUpdateCounter;

// end JXLayout

private:

	void	CBProjectDocumentX(CBProjectTree* fileList);
	void	BuildWindow(CBProjectTree* fileList);

	void	WriteFiles(std::ostream& projOutput,
					   const JString& setName, std::ostream* setOutput,
					   const JString& symName, std::ostream* symOutput) const;

	void	SaveAsTemplate() const;
	void	ReadTemplate(std::istream& input, const JFileVersion tmplVers,
						 const JFileVersion projVers);
	void	WriteTemplate(const JString& fileName) const;

	static JString	GetSettingFileName(const JString& fullName);
	static JString	GetSymbolFileName(const JString& fullName);

	void	ProcessNodeMessage(const Message& message);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	Print(JPTPrinter& p) const;

	void	UpdateTreeMenu();
	void	HandleTreeMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	UpdateSourceMenu();
	void	HandleSourceMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);
	void	EditProjectPrefs();

	void	SetTreePrefs(CBTreeDirector* director,
						 const JSize fontSize, const JBoolean showInheritedFns,
						 const JBoolean autoMinMILinks, const JBoolean drawMILinksOnTop,
						 const JBoolean raiseWhenSingleMatch, const JBoolean writePrefs);

	void	SymbolUpdateProgress();
	void	SymbolUpdateFinished();
	void	ShowUpdatePG(const JBoolean visible);
	void	DeleteUpdateLink();

	int		StartSymbolLoadTimer();
	void	StopSymbolLoadTimer(const int timerStatus);

	// not allowed

	CBProjectDocument(const CBProjectDocument& source);
	const CBProjectDocument& operator=(const CBProjectDocument& source);
};


/******************************************************************************
 GetFileTree

 ******************************************************************************/

inline CBProjectTree*
CBProjectDocument::GetFileTree()
	const
{
	return itsFileTree;
}

/******************************************************************************
 GetFileTable

 ******************************************************************************/

inline CBProjectTable*
CBProjectDocument::GetFileTable()
	const
{
	return itsFileTable;
}

/******************************************************************************
 GetCommandManager

 ******************************************************************************/

inline CBCommandManager*
CBProjectDocument::GetCommandManager()
	const
{
	return itsCmdMgr;
}

/******************************************************************************
 GetBuildManager

 ******************************************************************************/

inline CBBuildManager*
CBProjectDocument::GetBuildManager()
	const
{
	return itsBuildMgr;
}

/******************************************************************************
 GetBuildTargetName

 ******************************************************************************/

inline const JString&
CBProjectDocument::GetBuildTargetName()
	const
{
	return itsBuildMgr->GetBuildTargetName();
}

/******************************************************************************
 HasDirectories

 ******************************************************************************/

inline JBoolean
CBProjectDocument::HasDirectories()
	const
{
	return !itsDirList->IsEmpty();
}

/******************************************************************************
 GetDirectories

 ******************************************************************************/

inline const CBDirList&
CBProjectDocument::GetDirectories()
	const
{
	return *itsDirList;
}

/******************************************************************************
 GetFileListDirector

 ******************************************************************************/

inline CBFileListDirector*
CBProjectDocument::GetFileListDirector()
	const
{
	return itsAllFileDirector;
}

/******************************************************************************
 GetSymbolDirector

 ******************************************************************************/

inline CBSymbolDirector*
CBProjectDocument::GetSymbolDirector()
	const
{
	return itsSymbolDirector;
}

/******************************************************************************
 GetCTreeDirector

 ******************************************************************************/

inline CBCTreeDirector*
CBProjectDocument::GetCTreeDirector()
	const
{
	return itsCTreeDirector;
}

/******************************************************************************
 GetJavaTreeDirector

 ******************************************************************************/

inline CBJavaTreeDirector*
CBProjectDocument::GetJavaTreeDirector()
	const
{
	return itsJavaTreeDirector;
}

/******************************************************************************
 GetPHPTreeDirector

 ******************************************************************************/

inline CBPHPTreeDirector*
CBProjectDocument::GetPHPTreeDirector()
	const
{
	return itsPHPTreeDirector;
}

/******************************************************************************
 GetRelPathCSF

 ******************************************************************************/

inline CBRelPathCSF*
CBProjectDocument::GetRelPathCSF()
	const
{
	return itsCSF;
}

/******************************************************************************
 Reopen text files when open project (static)

 ******************************************************************************/

inline JBoolean
CBProjectDocument::WillReopenTextFiles()
{
	return theReopenTextFilesFlag;
}

inline void
CBProjectDocument::ShouldReopenTextFiles
	(
	const JBoolean reopen
	)
{
	theReopenTextFilesFlag = reopen;
}

/******************************************************************************
 Reopen text files when open project (static)

 ******************************************************************************/

inline JBoolean
CBProjectDocument::WillAskOKToOpenOldVersion()
{
	return theWarnOpenOldVersionFlag;
}

inline void
CBProjectDocument::ShouldAskOKToOpenOldVersion
	(
	const JBoolean ask
	)
{
	theWarnOpenOldVersionFlag = ask;
}

/******************************************************************************
 Filter for adding new files (static)

 ******************************************************************************/

inline const JString&
CBProjectDocument::GetAddFilesFilter()
{
	return theAddFilesFilter;
}

inline void
CBProjectDocument::SetAddFilesFilter
	(
	const JString& filter
	)
{
	theAddFilesFilter = filter;
}

#endif
