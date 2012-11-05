/******************************************************************************
 CBBuildManager.h

	Copyright © 1997-2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBBuildManager
#define _H_CBBuildManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBroadcaster.h>
#include <JString.h>
#include <jTime.h>

class CBCommand;
class CBProjectDocument;
class CBExecOutputDocument;
class CBProjectConfigDialog;
class CBProjectNode;

class CBBuildManager : virtual public JBroadcaster
{
public:

	enum MakefileMethod		// remember to update kMakefileMethodName
	{
		// Do not change values once assigned because they are stored in files

		kManual,
		kMakemake,
		kQMake,
		kCMake
	};

public:

	CBBuildManager(CBProjectDocument* doc, const MakefileMethod method = kMakemake,
				   const JBoolean needWriteMakeFiles = kJTrue,
				   const JCharacter* targetName = "",
				   const JCharacter* depListExpr = "");
	CBBuildManager(istream& projInput, const JFileVersion projVers,
				   istream* setInput, const JFileVersion setVers,
				   CBProjectDocument* doc);

	virtual ~CBBuildManager();

	void		CreateMakeFiles(const MakefileMethod method);
	JBoolean	UpdateMakeFiles(const JBoolean reportError = kJTrue);
	JBoolean	UpdateMakefile(CBExecOutputDocument* compileDoc = NULL,
							   CBCommand** cmd = NULL,
							   const JBoolean force = kJFalse);

	void		EditProjectConfig();
	JBoolean	EditMakeConfig();

	const JString&	GetBuildTargetName() const;
	void			SetBuildTargetName(const JCharacter* targetName);

	MakefileMethod	GetMakefileMethod() const;

	static const JCharacter*	GetSubProjectBuildSuffix();

	static JBoolean	WillRebuildMakefileDaily();
	static void		ShouldRebuildMakefileDaily(const JBoolean rebuild);

	// stored in project

	void	StreamOut(ostream& projOutput, ostream* setOutput) const;

	// stored in project template

	void	ReadTemplate(istream& input, const JFileVersion tmplVers,
						 const JFileVersion projVers, const MakefileMethod method,
						 const JCharacter* targetName, const JCharacter* depListExpr);
	void	WriteTemplate(ostream& output) const;

	// called by CBProjectDocument

	void	ProjectChanged(const CBProjectNode* node = NULL);

	// called by CBCommandManager

	void	ConvertCompileDialog(istream& input, const JFileVersion vers);

	// called by CBNewProjectSaveFileDialog

	static const JCharacter*	GetMakefileMethodName(const MakefileMethod method);

	static JString	GetMakeFilesName(const JCharacter* path);
	static JString	GetCMakeInputName(const JCharacter* path, const JCharacter* projName);
	static JString	GetQMakeInputName(const JCharacter* path, const JCharacter* projName);
	static void		GetMakefileNames(const JCharacter* path, JPtrArray<JString>* list);

	// called by CBProjectConfigDialog

	static JBoolean	UpdateMakeDependCmd(const MakefileMethod oldMethod,
										const MakefileMethod newMethod,
										JString* cmd);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	struct ModTime
	{
		time_t	makeHeaderModTime;
		time_t	makeFilesModTime;
		time_t	cmakeHeaderModTime;
		time_t	cmakeInputModTime;
		time_t	qmakeHeaderModTime;
		time_t	qmakeInputModTime;

		ModTime()
			:
			makeHeaderModTime(0), makeFilesModTime(0),
			cmakeHeaderModTime(0), cmakeInputModTime(0),
			qmakeHeaderModTime(0), qmakeInputModTime(0)
		{ };
	};

private:

	MakefileMethod	itsMakefileMethod;
	JBoolean		itsNeedWriteMakeFilesFlag;	// kJTrue => Make.files changed
	JString			itsTargetName;				// name of target in Make.files
	JString			itsDepListExpr;				// "literal:" at end of Make.files
	JString			itsSubProjectBuildCmd;
	JString			itsMakePath;				// path where we last looked for make files
	ModTime			itsModTime;					// last success
	ModTime			itsNewModTime;				// when process started
	time_t			itsLastMakefileUpdateTime;

	CBProjectDocument*		itsProjDoc;			// owns us
	CBCommand*				itsMakeDependCmd;	// not owned; NULL unless running
	CBProjectConfigDialog*	itsProjectConfigDialog;

	static JBoolean	itsRebuildMakefileDailyFlag;

private:

	void	UpdateProjectConfig();

	void		PrintTargetName(ostream& output) const;
	JBoolean	SaveOpenFile(const JCharacter* fileName);

	void		CreateMakemakeFiles(const JCharacter* makeHeaderText,
									const JCharacter* makeFilesText,
									const JBoolean readingTemplate);
	void		CreateCMakeFiles(const JCharacter* cmakeHeaderText,
								 const JBoolean readingTemplate);
	void		CreateQMakeFiles(const JCharacter* qmakeHeaderText,
								 const JBoolean readingTemplate);
	void		RecreateMakeHeaderFile();

	JBoolean	WriteSubProjectBuildFile(const JBoolean reportError);

	void		UpdateMakeHeader(const JCharacter* fileName,
								 const JPtrArray<JString>& libFileList,
								 const JPtrArray<JString>& libProjPathList) const;
	void		UpdateMakeFiles(const JCharacter* fileName, const JCharacter* text) const;

	void		WriteCMakeInput(const JCharacter* inputFileName,
								const JCharacter* src, const JCharacter* hdr,
								const JCharacter* outputFileName) const;
	void		WriteQMakeInput(const JCharacter* inputFileName,
								const JCharacter* src, const JCharacter* hdr,
								const JCharacter* outputFileName) const;

	JBoolean	MakeFilesChanged() const;
	JBoolean	CMakeHeaderChanged() const;
	JBoolean	QMakeHeaderChanged() const;
	ModTime		SaveMakeFileModTimes();
	JBoolean	MakefileExists() const;

	void		GetMakefileNames(JPtrArray<JString>* list) const;
	void		GetMakemakeFileNames(JString* makeHeaderName,
									 JString* makeFilesName) const;
	void		GetCMakeFileNames(JString* cmakeHeaderName,
								  JString* cmakeInputName) const;
	void		GetQMakeFileNames(JString* qmakeHeaderName,
								  JString* qmakeInputName) const;

	void	ReadSetup(istream& projInput, const JFileVersion projVers,
					  istream* setInput, const JFileVersion setVers,
					  const JCharacter* projPath);

	// not allowed

	CBBuildManager(const CBBuildManager& source);
	const CBBuildManager& operator=(const CBBuildManager& source);
};

istream& operator>>(istream& input, CBBuildManager::MakefileMethod& method);
ostream& operator<<(ostream& output, const CBBuildManager::MakefileMethod method);


/******************************************************************************
 Build target name

 ******************************************************************************/

inline const JString&
CBBuildManager::GetBuildTargetName()
	const
{
	return itsTargetName;
}

inline void
CBBuildManager::SetBuildTargetName
	(
	const JCharacter* targetName
	)
{
	itsTargetName = targetName;
}

/******************************************************************************
 Build target name

 ******************************************************************************/

inline CBBuildManager::MakefileMethod
CBBuildManager::GetMakefileMethod()
	const
{
	return itsMakefileMethod;
}

/******************************************************************************
 Reopen text files when open project (static)

 ******************************************************************************/

inline JBoolean
CBBuildManager::WillRebuildMakefileDaily()
{
	return itsRebuildMakefileDailyFlag;
}

inline void
CBBuildManager::ShouldRebuildMakefileDaily
	(
	const JBoolean rebuild
	)
{
	itsRebuildMakefileDailyFlag = rebuild;
}

#endif
