/******************************************************************************
 CBBuildManager.h

	Copyright © 1997-2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBBuildManager
#define _H_CBBuildManager

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
				   const bool needWriteMakeFiles = true,
				   const JString& targetName = JString::empty,
				   const JString& depListExpr = JString::empty);
	CBBuildManager(std::istream& projInput, const JFileVersion projVers,
				   std::istream* setInput, const JFileVersion setVers,
				   CBProjectDocument* doc);

	virtual ~CBBuildManager();

	void		CreateMakeFiles(const MakefileMethod method);
	bool	UpdateMakeFiles(const bool reportError = true);
	bool	UpdateMakefile(CBExecOutputDocument* compileDoc = nullptr,
							   CBCommand** cmd = nullptr,
							   const bool force = false);

	void		EditProjectConfig();
	bool	EditMakeConfig();

	const JString&	GetBuildTargetName() const;
	void			SetBuildTargetName(const JString& targetName);

	MakefileMethod	GetMakefileMethod() const;

	static const JString&	GetSubProjectBuildSuffix();

	static bool	WillRebuildMakefileDaily();
	static void		ShouldRebuildMakefileDaily(const bool rebuild);

	// stored in project

	void	StreamOut(std::ostream& projOutput, std::ostream* setOutput) const;

	// stored in project template

	void	ReadTemplate(std::istream& input, const JFileVersion tmplVers,
						 const JFileVersion projVers, const MakefileMethod method,
						 const JString& targetName, const JString& depListExpr);
	void	WriteTemplate(std::ostream& output) const;

	// called by CBProjectDocument

	void	ProjectChanged(const CBProjectNode* node = nullptr);

	// called by CBCommandManager

	void	ConvertCompileDialog(std::istream& input, const JFileVersion vers);

	// called by CBNewProjectSaveFileDialog

	static JString	GetMakefileMethodName(const MakefileMethod method);

	static JString	GetMakeFilesName(const JString& path);
	static JString	GetCMakeInputName(const JString& path, const JString& projName);
	static JString	GetQMakeInputName(const JString& path, const JString& projName);
	static void		GetMakefileNames(const JString& path, JPtrArray<JString>* list);

	// called by CBProjectConfigDialog

	static bool	UpdateMakeDependCmd(const MakefileMethod oldMethod,
										const MakefileMethod newMethod,
										JString* cmd);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
	bool		itsNeedWriteMakeFilesFlag;	// true => Make.files changed
	JString			itsTargetName;				// name of target in Make.files
	JString			itsDepListExpr;				// "literal:" at end of Make.files
	JString			itsSubProjectBuildCmd;
	JString			itsMakePath;				// path where we last looked for make files
	ModTime			itsModTime;					// last success
	ModTime			itsNewModTime;				// when process started
	time_t			itsLastMakefileUpdateTime;

	CBProjectDocument*		itsProjDoc;			// owns us
	CBCommand*				itsMakeDependCmd;	// not owned; nullptr unless running
	CBProjectConfigDialog*	itsProjectConfigDialog;

	static bool	itsRebuildMakefileDailyFlag;

private:

	void	UpdateProjectConfig();

	void		PrintTargetName(std::ostream& output) const;
	bool	SaveOpenFile(const JString& fileName);

	void		CreateMakemakeFiles(const JString& makeHeaderText,
									const JString& makeFilesText,
									const bool readingTemplate);
	void		CreateCMakeFiles(const JString& cmakeHeaderText,
								 const bool readingTemplate);
	void		CreateQMakeFiles(const JString& qmakeHeaderText,
								 const bool readingTemplate);
	void		RecreateMakeHeaderFile();

	bool	WriteSubProjectBuildFile(const bool reportError);

	void		UpdateMakeHeader(const JString& fileName,
								 const JPtrArray<JString>& libFileList,
								 const JPtrArray<JString>& libProjPathList) const;
	void		UpdateMakeFiles(const JString& fileName, const JString& text) const;

	void		WriteCMakeInput(const JString& inputFileName,
								const JString& src, const JString& hdr,
								const JString& outputFileName) const;
	void		WriteQMakeInput(const JString& inputFileName,
								const JString& src, const JString& hdr,
								const JString& outputFileName) const;

	bool	MakeFilesChanged() const;
	bool	CMakeHeaderChanged() const;
	bool	QMakeHeaderChanged() const;
	ModTime		SaveMakeFileModTimes();
	bool	MakefileExists() const;

	void		GetMakefileNames(JPtrArray<JString>* list) const;
	void		GetMakemakeFileNames(JString* makeHeaderName,
									 JString* makeFilesName) const;
	void		GetCMakeFileNames(JString* cmakeHeaderName,
								  JString* cmakeInputName) const;
	void		GetQMakeFileNames(JString* qmakeHeaderName,
								  JString* qmakeInputName) const;

	void	ReadSetup(std::istream& projInput, const JFileVersion projVers,
					  std::istream* setInput, const JFileVersion setVers,
					  const JString& projPath);

	// not allowed

	CBBuildManager(const CBBuildManager& source);
	const CBBuildManager& operator=(const CBBuildManager& source);
};

std::istream& operator>>(std::istream& input, CBBuildManager::MakefileMethod& method);
std::ostream& operator<<(std::ostream& output, const CBBuildManager::MakefileMethod method);


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
	const JString& targetName
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

inline bool
CBBuildManager::WillRebuildMakefileDaily()
{
	return itsRebuildMakefileDailyFlag;
}

inline void
CBBuildManager::ShouldRebuildMakefileDaily
	(
	const bool rebuild
	)
{
	itsRebuildMakefileDailyFlag = rebuild;
}

#endif
