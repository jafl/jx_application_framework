/******************************************************************************
 CBProjectDocument.h

	Interface for the CBProjectDocument class

	Copyright © 1996-2001 by John Lindal.

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
class CBDTreeDirector;
class CBGoTreeDirector;
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

	static bool		Create(CBProjectDocument** doc);
	static FileStatus	Create(const JString& fullName, const bool silent,
							   CBProjectDocument** doc);
	static FileStatus	CanReadFile(const JString& fullName);
	static FileStatus	CanReadFile(std::istream& input, JFileVersion* actualFileVersion);

	virtual ~CBProjectDocument();

	virtual void			Activate() override;
	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;
	virtual bool		Close() override;

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
	CBDTreeDirector*	GetDTreeDirector() const;
	CBGoTreeDirector*	GetGoTreeDirector() const;
	CBJavaTreeDirector*	GetJavaTreeDirector() const;
	CBPHPTreeDirector*	GetPHPTreeDirector() const;
	CBRelPathCSF*		GetRelPathCSF() const;

	bool					HasDirectories() const;
	const CBDirList&			GetDirectories() const;
	CBEditSearchPathsDialog*	EditSearchPaths(JXDirector* owner);

	void	DelayUpdateSymbolDatabase();
	void	UpdateSymbolDatabase();
	void	CancelUpdateSymbolDatabase();
	void	RefreshVCSStatus();

	void	SetProjectPrefs(const bool reopenTextFiles,
							const bool doubleSpaceCompile,
							const bool rebuildMakefileDaily,
							const CBProjectTable::DropFileAction dropFileAction);

	static void	ReadStaticGlobalPrefs(std::istream& input, const JFileVersion vers);
	static void	WriteStaticGlobalPrefs(std::ostream& output);

	static bool	WillReopenTextFiles();
	static void		ShouldReopenTextFiles(const bool reopen);

	static bool	WillAskOKToOpenOldVersion();
	static void		ShouldAskOKToOpenOldVersion(const bool ask);

	static const JString&	GetAddFilesFilter();
	static void				SetAddFilesFilter(const JString& filter);

	static const JUtf8Byte*	GetProjectFileSuffix();

	void	ConvertCompileRunDialogs(std::istream& projInput, const JFileVersion vers);

	// called by CBNewProjectSaveFileDialog

	static const JUtf8Byte*	GetTemplateDirectoryName();
	static bool			GetProjectTemplateType(const JString& fullName,
												   JString* type);

	// called by CBEditTreePrefsDialog

	void	SetTreePrefs(const JSize fontSize, const bool showInheritedFns,
						 const bool autoMinMILinks, const bool drawMILinksOnTop,
						 const bool raiseWhenSingleMatch, const bool writePrefs);

	// called by CBCommandTable

	static bool	ReadTasksFromProjectFile(std::istream& input, CBCommandManager::CmdList* cmdList);

protected:

	CBProjectDocument(const JString& fullName,
					  const CBBuildManager::MakefileMethod makefileMethod,
					  const bool fromTemplate, const JString& tmplFile);
	CBProjectDocument(std::istream& input, const JString& projName,
					  const JString& setName, const JString& symName,
					  const bool silent);

	virtual void	DiscardChanges() override;
	virtual JError	WriteFile(const JString& fullName, const bool safetySave) const override;
	virtual void	WriteTextFile(std::ostream& output, const bool safetySave) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	CBProjectTree*	itsFileTree;
	CBProjectTable*	itsFileTable;
	bool		itsProcessNodeMessageFlag;		// true => process messages from itsFileTree

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
	CBDTreeDirector*	itsDTreeDirector;
	CBGoTreeDirector*	itsGoTreeDirector;
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

	static bool	theReopenTextFilesFlag;
	static bool	theWarnOpenOldVersionFlag;
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
						 const JSize fontSize, const bool showInheritedFns,
						 const bool autoMinMILinks, const bool drawMILinksOnTop,
						 const bool raiseWhenSingleMatch, const bool writePrefs);

	void	SymbolUpdateProgress();
	void	SymbolUpdateFinished();
	void	ShowUpdatePG(const bool visible);
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

inline bool
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
 GetDTreeDirector

 ******************************************************************************/

inline CBDTreeDirector*
CBProjectDocument::GetDTreeDirector()
	const
{
	return itsDTreeDirector;
}

/******************************************************************************
 GetGoTreeDirector

 ******************************************************************************/

inline CBGoTreeDirector*
CBProjectDocument::GetGoTreeDirector()
	const
{
	return itsGoTreeDirector;
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

inline bool
CBProjectDocument::WillReopenTextFiles()
{
	return theReopenTextFilesFlag;
}

inline void
CBProjectDocument::ShouldReopenTextFiles
	(
	const bool reopen
	)
{
	theReopenTextFilesFlag = reopen;
}

/******************************************************************************
 Reopen text files when open project (static)

 ******************************************************************************/

inline bool
CBProjectDocument::WillAskOKToOpenOldVersion()
{
	return theWarnOpenOldVersionFlag;
}

inline void
CBProjectDocument::ShouldAskOKToOpenOldVersion
	(
	const bool ask
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
