/******************************************************************************
 CBProjectDocument.cpp

	BASE CLASS = JXFileDocument

	Copyright (C) 1996-2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBProjectDocument.h"
#include "CBProjectTable.h"
#include "CBProjectTree.h"
#include "CBProjectNode.h"
#include "CBWaitForSymbolUpdateTask.h"
#include "CBDelaySymbolUpdateTask.h"
#include "CBFileListDirector.h"
#include "CBFileListTable.h"
#include "CBSymbolDirector.h"
#include "CBSymbolList.h"
#include "CBCTreeDirector.h"
#include "CBCTree.h"
#include "CBCPreprocessor.h"
#include "CBJavaTreeDirector.h"
#include "CBPHPTreeDirector.h"
#include "CBFileHistoryMenu.h"
#include "CBCommandManager.h"
#include "CBCommandMenu.h"
#include "CBCompileDocument.h"
#include "CBEditSearchPathsDialog.h"
#include "CBEditProjPrefsDialog.h"
#include "CBViewManPageDialog.h"
#include "CBFindFileDialog.h"
#include "CBDiffFileDialog.h"
#include "CBSearchTextDialog.h"
#include "CBRelPathCSF.h"
#include "CBNewProjectCSF.h"
#include "CBPTPrinter.h"
#include "CBDirList.h"
#include "CBDocumentMenu.h"
#include "cbFileVersions.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"

#include <JXApplication.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXScrollbarSet.h>
#include <JXMenuBar.h>
#include <JXToolBar.h>
#include <JXChooseSaveFile.h>
#include <JXProgressDisplay.h>
#include <JXProgressIndicator.h>
#include <JXWebBrowser.h>
#include <JXTimerTask.h>
#include <JXImage.h>

#include <JNamedTreeList.h>
#include <JTreeNode.h>
#include <JThisProcess.h>
#include <JOutPipeStream.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jSysUtil.h>
#include <sstream>
#include <sys/time.h>
#include <signal.h>
#include <jErrno.h>
#include <jAssert.h>

JBoolean CBProjectDocument::theReopenTextFilesFlag    = kJTrue;
JBoolean CBProjectDocument::theWarnOpenOldVersionFlag = kJFalse;
JString CBProjectDocument::theAddFilesFilter;

static const JCharacter* kProjectFileSignature = "jx_browser_data";
const JSize kProjectFileSignatureLength        = strlen(kProjectFileSignature);
static const JCharacter* kProjectFileSuffix    = ".jcc";
const JSize kProjectFileSuffixLength           = strlen(kProjectFileSuffix);

static const JCharacter* kSettingFileSignature = "jx_browser_local_settings";
const JSize kSettingFileSignatureLength        = strlen(kSettingFileSignature);
static const JCharacter* kSettingFileSuffix    = ".jup";
const JSize kSettingFileSuffixLength           = strlen(kSettingFileSuffix);

static const JCharacter* kSymbolFileSignature  = "jx_browser_symbol_table";
const JSize kSymbolFileSignatureLength         = strlen(kSymbolFileSignature);
static const JCharacter* kSymbolFileSuffix     = ".jst";
const JSize kSymbolFileSuffixLength            = strlen(kSymbolFileSuffix);

static const JCharacter* kNewProjectFileName   = "Untitled.jcc";
static const JCharacter* kSaveNewFilePrompt    = "Save project as:";

static const JCharacter* kProjTemplateDir      = "project_templates";
static const JCharacter* kTmplFileSignature    = "jx_browser_project_template";
const JSize kTmplFileSignatureLength           = strlen(kTmplFileSignature);
static const JCharacter* kWizardFileSignature  = "jx_browser_project_wizard";
const JSize kWizardFileSignatureLength         = strlen(kWizardFileSignature);

const JSize kSafetySavePeriod = 600000;		// 10 minutes (milliseconds)

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    New text file                  %k Meta-N       %i" kCBNewTextFileAction
	"  | New text file from template... %k Meta-Shift-N %i" kCBNewTextFileFromTmplAction
	"  | New project...                                 %i" kCBNewProjectAction
	"  | New shell...                                   %i" kCBNewShellAction
	"%l| Open...                        %k Meta-O       %i" kCBOpenSomethingAction
	"  | Recent projects"
	"  | Recent text files"
	"%l| Save                                           %i" kCBSaveFileAction
	"  | Save as template...                            %i" kCBSaveAsTemplateAction
	"%l| Page setup...                                  %i" kJXPageSetupAction
	"  | Print...                       %k Meta-P       %i" kJXPrintAction
	"%l| Close                          %k Meta-W       %i" kJXCloseWindowAction
	"  | Quit                           %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewTextEditorCmd = 1, kNewTextTemplateCmd, kNewProjectCmd, kNewShellCmd,
	kOpenSomethingCmd,
	kRecentProjectMenuCmd, kRecentTextMenuCmd,
	kSaveCmd, kSaveAsTemplateCmd,
	kPageSetupCmd, kPrintCmd,
	kCloseCmd, kQuitCmd
};

// Project menu

static const JCharacter* kProjectMenuTitleStr = "Project";
static const JCharacter* kProjectMenuStr =
	"    Edit project configuration...                  %i" kCBEditMakeConfigAction
	"  | Update Makefile                                %i" kCBUpdateMakefileAction
	"%l| Update symbol database         %k Meta-U       %i" kCBUpdateClassTreeAction
	"  | Show symbol browser            %k Ctrl-F12     %i" kCBShowSymbolBrowserAction
	"  | Show C++ class tree                            %i" kCBShowCPPClassTreeAction
	"  | Show Java class tree                           %i" kCBShowJavaClassTreeAction
	"  | Show PHP class tree                            %i" kCBShowPHPClassTreeAction
	"  | Look up man page...            %k Meta-I       %i" kCBViewManPageAction
	"%l| Edit search paths...                           %i" kCBEditSearchPathsAction
	"  | Show file list                 %k Meta-Shift-F %i" kCBShowFileListAction
	"  | Find file...                   %k Meta-D       %i" kCBFindFileAction
	"  | Search files...                %k Meta-F       %i" kCBSearchFilesAction
	"  | Compare files...                               %i" kCBDiffFilesAction;

enum
{
	kOpenMakeConfigDlogCmd = 1, kUpdateMakefileCmd,
	kUpdateSymbolDBCmd, kShowSymbolBrowserCmd,
	kShowCTreeCmd, kShowJavaTreeCmd, kShowPHPTreeCmd,
	kViewManPageCmd,
	kEditSearchPathsCmd,
	kShowFileListCmd, kFindFileCmd, kSearchFilesCmd, kDiffFilesCmd
};

// Source menu

static const JCharacter* kSourceMenuTitleStr = "Source";
static const JCharacter* kSourceMenuStr =
	"    New group                                                      %i" kCBNewProjectGroupAction
	"  | Add files...                                                   %i" kCBAddFilesToProjectAction
	"  | Add directory tree...                                          %i" kCBAddDirectoryTreeToProjectAction
	"  | Remove selected items          %k Backspace.                   %i" kCBRemoveFilesAction
	"%l| Open selected files            %k Left-dbl-click or Return     %i" kCBOpenSelectedFilesAction
	"  | Open complement files          %k Middle-dbl-click or Meta-Tab %i" kCBOpenComplFilesAction
	"%l| Edit file path                 %k Meta-left-dbl-click          %i" kCBEditFilePathAction
	"  | Edit sub-project configuration                                 %i" kCBEditSubprojConfigAction
	"%l| Compare selected files with backup                             %i" kCBDiffSmartAction
	"  | Compare selected files with version control                    %i" kCBDiffVCSAction
	"  | Show selected files in file manager %k Meta-Return             %i" kCBOpenSelectedFileLocationsAction
	"%l| Save all                            %k Meta-Shift-S            %i" kCBSaveAllTextFilesAction
	"  | Close all                           %k Meta-Shift-W            %i" kCBCloseAllTextFilesAction;

enum
{
	kNewGroupCmd = 1, kAddFilesCmd, kAddDirTreeCmd, kRemoveSelCmd,
	kOpenFilesCmd, kOpenComplFilesCmd,
	kEditPathCmd, kEditSubprojConfigCmd,
	kDiffSmartCmd, kDiffVCSCmd, kShowLocationCmd,
	kSaveAllTextCmd, kCloseAllTextCmd
};

static const JCharacter* kOpenFilesItemStr     = "Open selected files";
static const JCharacter* kEditGroupNameItemStr = "Edit group name";

// Windows menu

static const JCharacter* kFileListMenuTitleStr = "Windows";

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    Project..."
	"  | Toolbar buttons..."
	"  | File types..."
	"  | External editors..."
	"  | File manager & web browser..."
	"  | Miscellaneous..."
	"%l| Save window size as default";

enum
{
	kProjectPrefsCmd = 1, kToolBarPrefsCmd,
	kEditFileTypesCmd, kChooseExtEditorsCmd,
	kShowLocationPrefsCmd, kMiscPrefsCmd,
	kSaveWindSizeCmd
};

// catch crashes during child process parsing

class ChildAssertHandler : public JAssertBase
{
	virtual int Assert(const JCharacter* expr, const JCharacter* file, const int line, const JCharacter* message)
	{
		return JAssertBase::DefaultAssert(expr, file, line, message);
	}

	virtual void Abort()
	{
		exit(1);
	}
};

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JBoolean
CBProjectDocument::Create
	(
	CBProjectDocument** doc
	)
{
	*doc = NULL;

	CBNewProjectCSF csf;
	JString fullName;
	if (!csf.SaveFile(kSaveNewFilePrompt, "", kNewProjectFileName, &fullName))
		{
		return kJFalse;
		}

	JString tmplFile, tmplType;
	const JBoolean fromTemplate = csf.GetProjectTemplate(&tmplFile);
	if (fromTemplate)
		{
		const JBoolean ok = GetProjectTemplateType(tmplFile, &tmplType);
		assert( ok );
		}

	if (!fromTemplate || tmplType == kTmplFileSignature)
		{
		std::ofstream temp(fullName);
		if (!temp.good())
			{
			(JGetUserNotification())->ReportError(
				"The project file could not be created.  "
				"Please check that the directory is writable.");
			return kJFalse;
			}
		temp.close();

		*doc = jnew CBProjectDocument(fullName, csf.GetMakefileMethod(),
									 fromTemplate, tmplFile);
		assert( *doc != NULL );
		(**doc).SaveInCurrentFile();
		(**doc).Activate();
		return kJTrue;
		}
	else
		{
		assert( tmplType == kWizardFileSignature );

		if (fullName.EndsWith(kProjectFileSuffix))
			{
			fullName.RemoveSubstring(fullName.GetLength()-kProjectFileSuffixLength+1,
									 fullName.GetLength());
			}

		std::ifstream input(tmplFile);
		input.ignore(kWizardFileSignatureLength);

		JFileVersion vers;
		input >> vers;
		assert( vers <= kCurrentProjectWizardVersion );

		JString cmd;
		JReadAll(input, &cmd);
		cmd.TrimWhitespace();

		JString path, name;
		JSplitPathAndName(fullName, &path, &name);
		const JCharacter* map[] =
			{
			"path", path,
			"name", name
			};
		(JGetStringManager())->Replace(&cmd, map, sizeof(map));

		pid_t pid;
		const JError err = JExecute(cmd, &pid);
		if (!err.OK())
			{
			(JGetStringManager())->ReportError("WizardExecError::CBProjectDocument", err);
			}
		return kJFalse;
		}
}

JXFileDocument::FileStatus
CBProjectDocument::Create
	(
	const JCharacter*	fullName,
	const JBoolean		silent,
	CBProjectDocument**	doc
	)
{
	assert( !JStringEmpty(fullName) );

	// make sure we use the project file, not the symbol file

	JString projName = fullName;
	JString setName  = GetSettingFileName(fullName);
	JString symName  = GetSymbolFileName(fullName);
	if (projName.EndsWith(kSymbolFileSuffix))
		{
		const JIndex length = projName.GetLength();
		projName.ReplaceSubstring(length - kSymbolFileSuffixLength + 1, length,
								  kProjectFileSuffix);
		}
	else if (projName.EndsWith(kSettingFileSuffix))
		{
		const JIndex length = projName.GetLength();
		projName.ReplaceSubstring(length - kSettingFileSuffixLength + 1, length,
								  kProjectFileSuffix);
		}

	const JCharacter* subProjBuildSuffix = CBBuildManager::GetSubProjectBuildSuffix();
	if (projName.EndsWith(subProjBuildSuffix))
		{
		const JIndex length = projName.GetLength();
		projName.ReplaceSubstring(length - strlen(subProjBuildSuffix) + 1, length,
								  kProjectFileSuffix);
		}

	JXFileDocument* baseDoc;
	if ((CBGetDocumentManager())->FileDocumentIsOpen(projName, &baseDoc))
		{
		*doc = dynamic_cast<CBProjectDocument*>(baseDoc);
		if (*doc != NULL && !silent)
			{
			(**doc).Activate();
			}
		return (*doc != NULL ? kFileReadable : kNotMyFile);
		}

	*doc = NULL;

	std::ifstream input(projName);
	JFileVersion vers;
	const FileStatus status = CanReadFile(input, &vers);
	if (status == kFileReadable &&
		(
		 !theWarnOpenOldVersionFlag ||
		 vers == kCurrentProjectFileVersion ||
		 (JGetUserNotification())->AskUserYes(JGetString("WarnOldVersion::CBProjectDocument"))))
		{
		JString testContent;
		JReadFile(projName, &testContent);
		if (testContent.Contains("<<<<<<<") &&
			testContent.Contains("=======") &&
			testContent.Contains(">>>>>>>"))
		{
			(JGetUserNotification())->ReportError(JGetString("VCSConflict::CBProjectDocument"));
			return kNotMyFile;
		}

		(CBGetApplication())->DisplayBusyCursor();

		*doc = jnew CBProjectDocument(input, projName, setName, symName, silent);
		assert( *doc != NULL );
		}
	else if (status == kNeedNewerVersion)
		{
		(JGetUserNotification())->ReportError(JGetString("NeedNewerVersion::CBProjectDocument"));
		}

	return status;
}

/******************************************************************************
 GetTemplateDirectoryName (static)

 ******************************************************************************/

const JCharacter*
CBProjectDocument::GetTemplateDirectoryName()
{
	return kProjTemplateDir;
}

/******************************************************************************
 GetProjectTemplateType (static)

 ******************************************************************************/

JBoolean
CBProjectDocument::GetProjectTemplateType
	(
	const JCharacter*	fullName,
	JString*			type
	)
{
	JFileVersion actualFileVersion;
	std::ifstream input(fullName);
	FileStatus status =
		DefaultCanReadASCIIFile(input, kTmplFileSignature, kCurrentProjectTmplVersion,
								&actualFileVersion);
	if (status == kFileReadable)
		{
		*type = kTmplFileSignature;
		return kJTrue;
		}
	else if (status == kNeedNewerVersion)
		{
		type->Clear();
		return kJFalse;
		}
	else
		{
		status = DefaultCanReadASCIIFile(input, kWizardFileSignature,
										 kCurrentProjectWizardVersion,
										 &actualFileVersion);
		if (status == kFileReadable)
			{
			*type = kWizardFileSignature;
			return kJTrue;
			}
		else
			{
			type->Clear();
			return kJFalse;
			}
		}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBProjectDocument::CBProjectDocument
	(
	const JCharacter*						fullName,
	const CBBuildManager::MakefileMethod	makefileMethod,
	const JBoolean							fromTemplate,
	const JCharacter*						tmplFile
	)
	:
	JXFileDocument(CBGetApplication(),
				   fullName, kJTrue, kJFalse, kProjectFileSuffix)
{
	std::ifstream* input = NULL;
	JFileVersion tmplVers, projVers;
	if (fromTemplate)
		{
		input = jnew std::ifstream(tmplFile);
		input->ignore(kTmplFileSignatureLength);

		*input >> tmplVers;
		assert( tmplVers <= kCurrentProjectTmplVersion );

		*input >> projVers;
		assert( projVers <= kCurrentProjectFileVersion );

		itsFileTree = jnew CBProjectTree(*input, projVers, this);
		}
	else
		{
		itsFileTree = jnew CBProjectTree(this);
		}
	assert( itsFileTree != NULL );

	CBProjectDocumentX(itsFileTree);

	itsBuildMgr = jnew CBBuildManager(this);
	assert( itsBuildMgr != NULL );

	itsAllFileDirector = jnew CBFileListDirector(this);
	assert( itsAllFileDirector != NULL );

	itsSymbolDirector = jnew CBSymbolDirector(this);
	assert( itsSymbolDirector != NULL );

	itsCTreeDirector = jnew CBCTreeDirector(this);
	assert( itsCTreeDirector != NULL );

	itsJavaTreeDirector = jnew CBJavaTreeDirector(this);
	assert( itsJavaTreeDirector != NULL );

	itsPHPTreeDirector = jnew CBPHPTreeDirector(this);
	assert( itsPHPTreeDirector != NULL );

	if (fromTemplate)
		{
		ReadTemplate(*input, tmplVers, projVers);
		}
	else
		{
		JString path, name, targetName, suffix;
		JSplitPathAndName(fullName, &path, &name);
		JSplitRootAndSuffix(name, &targetName, &suffix);
		itsBuildMgr->SetBuildTargetName(targetName);

		itsBuildMgr->CreateMakeFiles(makefileMethod);
		}

	UpdateSymbolDatabase();
	ListenTo(itsFileTree);

	(CBGetDocumentManager())->ProjDocCreated(this);

	jdelete input;
}

CBProjectDocument::CBProjectDocument
	(
	std::istream&			projInput,
	const JCharacter*	projName,
	const JCharacter*	setName,
	const JCharacter*	symName,
	const JBoolean		silent
	)
	:
	JXFileDocument(CBGetApplication(),
				   projName, kJTrue, kJFalse, kProjectFileSuffix)
{
	projInput.ignore(kProjectFileSignatureLength);

	JFileVersion projVers;
	projInput >> projVers;
	assert( projVers <= kCurrentProjectFileVersion );

	// open the setting file

	std::ifstream setStream(setName);

	const JString setSignature = JRead(setStream, kSettingFileSignatureLength);

	JFileVersion setVers;
	setStream >> setVers;

	std::istream* setInput = NULL;
	if (setSignature == kSettingFileSignature &&
		setVers <= kCurrentProjectFileVersion &&
		setVers >= projVers)
		{
		setInput = &setStream;
		}

	const JBoolean useProjSetData = JI2B( setInput == NULL || setVers < 71 );

	// open the symbol file

	std::ifstream symStream(symName);

	const JString symSignature = JRead(symStream, kSymbolFileSignatureLength);

	JFileVersion symVers;
	symStream >> symVers;

	std::istream* symInput = NULL;
	if (symSignature == kSymbolFileSignature &&
		symVers <= kCurrentProjectFileVersion &&
		symVers >= projVers)
		{
		symInput = &symStream;
		}

	const JBoolean useProjSymData = JI2B( symInput == NULL || symVers < 71 );

	// read in project file tree

	if (projVers >= 32)
		{
		if (projVers >= 71)
			{
			projInput >> std::ws;
			JIgnoreLine(projInput);
			}
		itsFileTree = jnew CBProjectTree(projInput, projVers, this);
		}
	else
		{
		itsFileTree = jnew CBProjectTree(this);
		}
	assert( itsFileTree != NULL );

	CBProjectDocumentX(itsFileTree);

	if (!useProjSetData)
		{
		GetWindow()->ReadGeometry(*setInput);
		}
	if (33 <= projVers && projVers < 71 && useProjSetData)
		{
		GetWindow()->ReadGeometry(projInput);
		}
	else if (33 <= projVers && projVers < 71)
		{
		JXWindow::SkipGeometry(projInput);
		}
	GetWindow()->Deiconify();

	itsFileTable->ReadSetup(projInput, projVers, setInput, setVers);

	if (projVers >= 62)
		{
		if (projVers >= 71)
			{
			projInput >> std::ws;
			JIgnoreLine(projInput);
			}
		itsCmdMgr->ReadSetup(projInput);
		}

	itsBuildMgr = jnew CBBuildManager(projInput, projVers, setInput, setVers, this);
	assert( itsBuildMgr != NULL );

	if (36 <= projVers && projVers < 71)
		{
		projInput >> itsPrintName;
		}
	if (!useProjSetData)	// overwrite
		{
		*setInput >> itsPrintName;
		}

	const int timerStatus = StartSymbolLoadTimer();

	// create file list

	itsAllFileDirector = jnew CBFileListDirector(projInput, projVers, setInput, setVers,
												symInput, symVers, this, silent);
	assert( itsAllFileDirector != NULL );

	// create symbol list

	itsSymbolDirector = jnew CBSymbolDirector(projInput, projVers, setInput, setVers,
											 symInput, symVers, this, silent);
	assert( itsSymbolDirector != NULL );

	// read C++ class tree

	itsCTreeDirector = jnew CBCTreeDirector(projInput, projVers, setInput, setVers,
										   symInput, symVers, this, silent, itsDirList);
	assert( itsCTreeDirector != NULL );
	// activates itself

	// read Java class tree

	if (projVers >= 48)
		{
		itsJavaTreeDirector = jnew CBJavaTreeDirector(projInput, projVers, setInput, setVers,
													 symInput, symVers, this, silent);
		assert( itsJavaTreeDirector != NULL );
		// activates itself
		}
	else
		{
		itsJavaTreeDirector = jnew CBJavaTreeDirector(this);
		assert( itsJavaTreeDirector != NULL );
		(itsJavaTreeDirector->GetTree())->NextUpdateMustReparseAll();
		}

	// read PHP class tree

	if (projVers >= 85)
		{
		itsPHPTreeDirector = jnew CBPHPTreeDirector(projInput, projVers, setInput, setVers,
												   symInput, symVers, this, silent);
		assert( itsPHPTreeDirector != NULL );
		// activates itself
		}
	else
		{
		itsPHPTreeDirector = jnew CBPHPTreeDirector(this);
		assert( itsPHPTreeDirector != NULL );
		(itsPHPTreeDirector->GetTree())->NextUpdateMustReparseAll();
		}

	StopSymbolLoadTimer(timerStatus);

	// this must be last so it can be cancelled

	CBDocumentManager* docMgr = CBGetDocumentManager();
	if (!silent && theReopenTextFilesFlag)
		{
		if (projVers >= 2)
			{
			docMgr->DocumentMustStayOpen(this, kJTrue);		// stay open if editor is closed

			if (projVers < 71 && useProjSetData)
				{
				(CBGetDocumentManager())->ReadFromProject(projInput, projVers);
				}
			else if (!useProjSetData)
				{
				(CBGetDocumentManager())->ReadFromProject(*setInput, setVers);
				}
			}

		Activate();		// stay open
		docMgr->DocumentMustStayOpen(this, kJFalse);
		}
	else if (!silent)
		{
		Activate();
		}

	UpdateSymbolDatabase();
	ListenTo(itsFileTree);

	docMgr->ProjDocCreated(this);
}

// private

void
CBProjectDocument::CBProjectDocumentX
	(
	CBProjectTree* fileList
	)
{
	itsProcessNodeMessageFlag = kJTrue;
	itsLastSymbolLoadTime     = 0.0;

	itsDirList = jnew CBDirList;
	assert( itsDirList != NULL );
	itsDirList->SetBasePath(GetFilePath());

	itsCSF = jnew CBRelPathCSF(this);
	assert( itsCSF != NULL );

	itsCmdMgr = jnew CBCommandManager;
	assert( itsCmdMgr != NULL );

	ListenTo(CBGetPrefsManager());

	itsSaveTask = jnew JXTimerTask(kSafetySavePeriod);
	assert( itsSaveTask != NULL );
	ListenTo(itsSaveTask);
	itsSaveTask->Start();

	itsUpdateProcess         = NULL;
	itsUpdateLink            = NULL;
	itsUpdateStream          = NULL;
	itsUpdatePG              = NULL;
	itsWaitForUpdateTask     = NULL;
	itsDelaySymbolUpdateTask = NULL;
	itsEditPathsDialog       = NULL;

	SetSaveNewFilePrompt(kSaveNewFilePrompt);

	BuildWindow(fileList);

	UpdateCVSIgnore();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBProjectDocument::~CBProjectDocument()
{
	(CBGetDocumentManager())->ProjDocDeleted(this);

	DeleteUpdateLink();
	jdelete itsUpdateStream;
	jdelete itsUpdateProcess;
	jdelete itsWaitForUpdateTask;
	jdelete itsDelaySymbolUpdateTask;

	if (itsUpdatePG->ProcessRunning())
		{
		itsUpdatePG->ProcessFinished();
		}
	jdelete itsUpdatePG;

	jdelete itsCmdMgr;
	jdelete itsBuildMgr;
	jdelete itsFileTree;
	jdelete itsDirList;
	jdelete itsCSF;
	jdelete itsSaveTask;
}

/******************************************************************************
 DeleteUpdateLink (private)

 ******************************************************************************/

void
CBProjectDocument::DeleteUpdateLink()
{
	delete itsUpdateLink;
	itsUpdateLink = NULL;
}

/******************************************************************************
 WriteFile (virtual protected)

	We override the default implementation because the project file might
	be read-only (e.g. CVS), but the symbol table still needs to be written.

 ******************************************************************************/

JError
CBProjectDocument::WriteFile
	(
	const JCharacter*	fullName,
	const JBoolean		safetySave
	)
	const
{
	std::ofstream output(fullName);
	WriteTextFile(output, safetySave);
	if (output.good())
		{
		return JNoError();
		}
	else
		{
		return WriteFailed();
		}
}

/******************************************************************************
 WriteTextFile (virtual protected)

 ******************************************************************************/

inline void
cbWriteSpace
	(
	std::ostream& projOutput,
	std::ostream* setOutput,
	std::ostream* symOutput
	)
{
	if (setOutput != NULL)
		{
		*setOutput << ' ';
		}
	if (symOutput != NULL)
		{
		*symOutput << ' ';
		}
}

void
CBProjectDocument::WriteTextFile
	(
	std::ostream&		projOutput,
	const JBoolean	safetySave
	)
	const
{
	(JXGetApplication())->DisplayBusyCursor();

	// create the setting and symbol files

	JString setName, symName;
	std::ostream* setOutput = NULL;
	std::ostream* symOutput = NULL;
	if (!safetySave)
		{
		JBoolean onDisk;
		setName = GetSettingFileName(GetFullName(&onDisk));

		setOutput = jnew std::ofstream(setName);
		assert( setOutput != NULL );

		symName = GetSymbolFileName(GetFullName(&onDisk));

		symOutput = jnew std::ofstream(symName);
		assert( symOutput != NULL );
		}

	WriteFiles(projOutput, setName, setOutput, symName, symOutput);
}

/******************************************************************************
 WriteFiles (private)

 ******************************************************************************/

void
CBProjectDocument::WriteFiles
	(
	std::ostream&			projOutput,
	const JCharacter*	setName,
	std::ostream*			setOutput,
	const JCharacter*	symName,
	std::ostream*			symOutput
	)
	const
{
	// headers

	projOutput << kProjectFileSignature << ' ';
	projOutput << kCurrentProjectFileVersion << '\n';

	if (setOutput != NULL)
		{
		*setOutput << kSettingFileSignature;
		*setOutput << ' ' << kCurrentProjectFileVersion;
		}

	if (symOutput != NULL)
		{
		*symOutput << kSymbolFileSignature;
		*symOutput << ' ' << kCurrentProjectFileVersion;
		}

	// data

	projOutput << "# project tree\n";
	itsFileTree->StreamOut(projOutput);

	if (setOutput != NULL)
		{
		*setOutput << ' ';
		GetWindow()->WriteGeometry(*setOutput);
		}

	cbWriteSpace(projOutput, setOutput, symOutput);
	itsFileTable->WriteSetup(projOutput, setOutput);

	projOutput << "# tasks\n";		// expected by ReadTasksFromProjectFile()
	itsCmdMgr->WriteSetup(projOutput);

	cbWriteSpace(projOutput, setOutput, symOutput);
	itsBuildMgr->StreamOut(projOutput, setOutput);

	if (setOutput != NULL)
		{
		*setOutput << ' ' << itsPrintName;
		}

	cbWriteSpace(projOutput, setOutput, symOutput);
	itsAllFileDirector->StreamOut(projOutput, setOutput, symOutput);

	cbWriteSpace(projOutput, setOutput, symOutput);
	itsSymbolDirector->StreamOut(projOutput, setOutput, symOutput);

	cbWriteSpace(projOutput, setOutput, symOutput);
	itsCTreeDirector->StreamOut(projOutput, setOutput, symOutput, itsDirList);

	cbWriteSpace(projOutput, setOutput, symOutput);
	itsJavaTreeDirector->StreamOut(projOutput, setOutput, symOutput, NULL);

	cbWriteSpace(projOutput, setOutput, symOutput);
	itsPHPTreeDirector->StreamOut(projOutput, setOutput, symOutput, NULL);

	// this must be last so it can be cancelled

	if (setOutput != NULL)
		{
		*setOutput << ' ';
		(CBGetDocumentManager())->WriteForProject(*setOutput);
		}

	// clean up -- no problem if settings or symbol table can't be written

	if (setOutput != NULL)
		{
		const JBoolean ok = JI2B( setOutput->good() );
		jdelete setOutput;
		if (!ok && setName != NULL)
			{
			JRemoveFile(setName);
			}
		}

	if (symOutput != NULL)
		{
		const JBoolean ok = JI2B( symOutput->good() );
		jdelete symOutput;
		if (!ok && symName != NULL)
			{
			JRemoveFile(symName);
			}
		}
}

/******************************************************************************
 ReadTasksFromProjectFile (static)

	Returns kJTrue if the file is a project file.

 ******************************************************************************/

JBoolean
CBProjectDocument::ReadTasksFromProjectFile
	(
	std::istream&					input,
	CBCommandManager::CmdList*	cmdList
	)
{
	JFileVersion vers;
	const FileStatus status = CanReadFile(input, &vers);
	if (status == kFileReadable && vers < 71)
		{
		(JGetUserNotification())->ReportError(JGetString("FileTooOld::CBProjectDocument"));
		return kJTrue;
		}
	else if (status == kFileReadable)
		{
		JBoolean foundDelimiter;
		JString makeDependCmd;

		JIgnoreUntil(input, "\n# tasks\n", &foundDelimiter);
		if (!foundDelimiter ||
			!CBCommandManager::ReadCommands(input, &makeDependCmd, 	cmdList))
			{
			(JGetUserNotification())->ReportError(JGetString("InvalidProjectFile::CBProjectDocument"));
			}
		return kJTrue;
		}
	else if (status == kNeedNewerVersion)
		{
		(JGetUserNotification())->ReportError(JGetString("NeedNewerVersion::CBProjectDocument"));
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ConvertCompileRunDialogs

 ******************************************************************************/

void
CBProjectDocument::ConvertCompileRunDialogs
	(
	std::istream&			projInput,
	const JFileVersion	projVers
	)
{
	itsCmdMgr->ConvertCompileDialog(projInput, projVers, itsBuildMgr, kJTrue);
	itsCmdMgr->ConvertRunDialog(projInput, projVers, kJTrue);
}

/******************************************************************************
 CanReadFile (static)

 ******************************************************************************/

JXFileDocument::FileStatus
CBProjectDocument::CanReadFile
	(
	const JCharacter* fullName
	)
{
	std::ifstream input(fullName);
	JFileVersion actualFileVersion;
	return CanReadFile(input, &actualFileVersion);
}

/******************************************************************************
 CanReadFile (static)

 ******************************************************************************/

JXFileDocument::FileStatus
CBProjectDocument::CanReadFile
	(
	std::istream&		input,
	JFileVersion*	actualFileVersion
	)
{
	return DefaultCanReadASCIIFile(input, kProjectFileSignature,
								   kCurrentProjectFileVersion, actualFileVersion);
}

/******************************************************************************
 ReadTemplate (private)

 ******************************************************************************/

void
CBProjectDocument::ReadTemplate
	(
	std::istream&			input,
	const JFileVersion	tmplVers,
	const JFileVersion	projVers
	)
{
	itsFileTree->CreateFilesForTemplate(input, tmplVers);
	itsFileTable->ReadSetup(input, projVers, NULL, 0);

	CBBuildManager::MakefileMethod makefileMethod = CBBuildManager::kManual;
	if (tmplVers == 2)
		{
		input >> makefileMethod;
		}
	else if (tmplVers < 2)
		{
		JBoolean shouldWriteMakeFilesFlag;
		input >> shouldWriteMakeFilesFlag;

		makefileMethod = (shouldWriteMakeFilesFlag ? CBBuildManager::kMakemake :
													 CBBuildManager::kManual);
		}

	JString targetName, depListExpr;
	if (tmplVers <= 2)
		{
		input >> targetName >> depListExpr;
		}

	itsDirList->ReadDirectories(input, projVers);
	((itsCTreeDirector->GetCTree())->GetCPreprocessor())->ReadSetup(input, projVers);
	itsBuildMgr->ReadTemplate(input, tmplVers, projVers, makefileMethod,
							  targetName, depListExpr);
	itsCmdMgr->ReadTemplate(input, tmplVers, projVers);
}

/******************************************************************************
 WriteTemplate (private)

 ******************************************************************************/

void
CBProjectDocument::WriteTemplate
	(
	const JCharacter* fileName
	)
	const
{
	std::ofstream output(fileName);
	output << kTmplFileSignature;
	output << ' ' << kCurrentProjectTmplVersion;
	output << ' ' << kCurrentProjectFileVersion;

	output << ' ';
	itsFileTree->StreamOut(output);

	// everything above this line is read in the constructor

	output << ' ';
	itsFileTree->SaveFilesInTemplate(output);

	output << ' ';
	itsFileTable->WriteSetup(output, NULL);

	output << ' ';
	itsDirList->WriteDirectories(output);

	output << ' ';
	((itsCTreeDirector->GetCTree())->GetCPreprocessor())->WriteSetup(output);

	output << ' ';
	itsBuildMgr->WriteTemplate(output);

	output << ' ';
	itsCmdMgr->WriteTemplate(output);
}

/******************************************************************************
 GetSettingFileName (static private)

 ******************************************************************************/

JString
CBProjectDocument::GetSettingFileName
	(
	const JCharacter* fullName
	)
{
	JString setName, suffix;
	JSplitRootAndSuffix(fullName, &setName, &suffix);
	setName += kSettingFileSuffix;
	return setName;
}

/******************************************************************************
 GetSymbolFileName (static private)

 ******************************************************************************/

JString
CBProjectDocument::GetSymbolFileName
	(
	const JCharacter* fullName
	)
{
	JString symName, suffix;
	JSplitRootAndSuffix(fullName, &symName, &suffix);
	symName += kSymbolFileSuffix;
	return symName;
}



/******************************************************************************
 Close

	Update Make.files and .pro so user can build without running Code Crusader.

 ******************************************************************************/

JBoolean
CBProjectDocument::Close()
{
	itsBuildMgr->UpdateMakeFiles(kJFalse);

	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (kJTrue)//NeedsSave() && onDisk && !JFileWritable(fullName))
		{
		JEditVCS(fullName);
		if (JFileWritable(fullName) && SaveInCurrentFile())
			{
			DataReverted(kJFalse);
			}
		else
			{
			DataModified();
			}
		}

	return JXFileDocument::Close();
}

/******************************************************************************
 DiscardChanges (virtual protected)

	We only set NeedsSilentSave(), so this will never be called.

 ******************************************************************************/

void
CBProjectDocument::DiscardChanges()
{
}

/******************************************************************************
 AddFile

 ******************************************************************************/

void
CBProjectDocument::AddFile
	(
	const JCharacter*				fullName,
	const CBRelPathCSF::PathType	pathType
	)
{
	JString name = fullName;
	JPtrArray<JString> list(JPtrArrayT::kForgetAll);
	list.Append(&name);
	itsFileTable->AddFiles(list, pathType);
}

/******************************************************************************
 GetAllFileList

 ******************************************************************************/

CBFileListTable*
CBProjectDocument::GetAllFileList()
	const
{
	return itsAllFileDirector->GetFileListTable();
}

/******************************************************************************
 RefreshCVSStatus

 ******************************************************************************/

void
CBProjectDocument::RefreshCVSStatus()
{
	itsFileTable->Refresh();
}

/******************************************************************************
 SetTreePrefs

 ******************************************************************************/

void
CBProjectDocument::SetTreePrefs
	(
	const JSize		fontSize,
	const JBoolean	showInheritedFns,
	const JBoolean	autoMinMILinks,
	const JBoolean	drawMILinksOnTop,
	const JBoolean	raiseWhenSingleMatch,
	const JBoolean	writePrefs
	)
{
	SetTreePrefs1(itsCTreeDirector,
				  fontSize, showInheritedFns,
				  autoMinMILinks, drawMILinksOnTop,
				  raiseWhenSingleMatch, writePrefs);

	SetTreePrefs1(itsJavaTreeDirector,
				  fontSize, showInheritedFns,
				  autoMinMILinks, drawMILinksOnTop,
				  raiseWhenSingleMatch, writePrefs);

	SetTreePrefs1(itsPHPTreeDirector,
				  fontSize, showInheritedFns,
				  autoMinMILinks, drawMILinksOnTop,
				  raiseWhenSingleMatch, writePrefs);
}

// private

void
CBProjectDocument::SetTreePrefs1
	(
	CBTreeDirector*	director,
	const JSize		fontSize,
	const JBoolean	showInheritedFns,
	const JBoolean	autoMinMILinks,
	const JBoolean	drawMILinksOnTop,
	const JBoolean	raiseWhenSingleMatch,
	const JBoolean	writePrefs
	)
{
	director->SetTreePrefs(fontSize, showInheritedFns,
						   autoMinMILinks, drawMILinksOnTop,
						   raiseWhenSingleMatch);

	if (writePrefs)
		{
		director->JPrefObject::WritePrefs();
		}
}

/******************************************************************************
 EditMakeConfig

 ******************************************************************************/

void
CBProjectDocument::EditMakeConfig()
{
	(CBGetDocumentManager())->SetActiveProjectDocument(this);
	itsBuildMgr->EditMakeConfig();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBProjectDocument::Activate()
{
	JXFileDocument::Activate();

	if (IsActive())
		{
		(CBGetDocumentManager())->SetActiveProjectDocument(this);
		}
}

/******************************************************************************
 GetName (virtual)

	Override of JXDocument::GetName().

 ******************************************************************************/

const JString&
CBProjectDocument::GetName()
	const
{
	itsDocName = GetFileName();
	if (itsDocName.EndsWith(kProjectFileSuffix))
		{
		itsDocName.RemoveSubstring(itsDocName.GetLength() - kProjectFileSuffixLength + 1,
								   itsDocName.GetLength());
		}
	return itsDocName;
}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

JBoolean
CBProjectDocument::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	CBProjectDocument* doc;
	*icon = CBGetProjectFileIcon(JI2B((CBGetDocumentManager())->GetActiveProjectDocument(&doc) &&
									  doc == const_cast<CBProjectDocument*>(this)));
	return kJTrue;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "jcc_project_window.xpm"

#include <jx_file_new.xpm>
#include <jx_file_open.xpm>
#include <jx_file_save.xpm>
#include <jx_file_save_all.xpm>
#include <jx_file_print.xpm>
#include "jcc_show_symbol_list.xpm"
#include "jcc_show_c_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"
#include "jcc_view_man_page.xpm"
#include "jcc_show_file_list.xpm"
#include "jcc_search_files.xpm"
#include "jcc_compare_files.xpm"
#include "jcc_compare_backup.xpm"
#include "jcc_compare_vcs.xpm"

void
CBProjectDocument::BuildWindow
	(
	CBProjectTree* fileList
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 510,430, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != NULL );

	itsToolBar =
		jnew JXToolBar(CBGetPrefsManager(), kCBProjectToolBarID, menuBar, 150,150, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 510,380);
	assert( itsToolBar != NULL );

	itsConfigButton =
		jnew JXTextButton(JGetString("itsConfigButton::CBProjectDocument::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 450,0, 60,30);
	assert( itsConfigButton != NULL );

	itsUpdateContainer =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 0,410, 510,20);
	assert( itsUpdateContainer != NULL );

	itsUpdateLabel =
		jnew JXStaticText(JGetString("itsUpdateLabel::CBProjectDocument::JXLayout"), itsUpdateContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,2, 130,16);
	assert( itsUpdateLabel != NULL );
	itsUpdateLabel->SetToLabel();

	itsUpdateCleanUpIndicator =
		jnew JXProgressIndicator(itsUpdateContainer,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,5, 380,10);
	assert( itsUpdateCleanUpIndicator != NULL );

	itsUpdateCounter =
		jnew JXStaticText(JGetString("itsUpdateCounter::CBProjectDocument::JXLayout"), itsUpdateContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,2, 90,16);
	assert( itsUpdateCounter != NULL );
	itsUpdateCounter->SetToLabel();

// end JXLayout

	AdjustWindowTitle();
	window->SetWMClass(CBGetWMClassInstance(), CBGetProjectWindowClass());

	JXDisplay* display = GetDisplay();
	JXImage* icon      = jnew JXImage(display, jcc_project_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (CBGetPrefsManager()->GetWindowSize(kCBProjectWindSizeID, &desktopLoc, &w, &h))
		{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
		}

	ListenTo(itsConfigButton);
	itsConfigButton->SetHint(JGetString("ConfigButtonHint::CBProjectDocument"));

	// file list

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure();

	JNamedTreeList* treeList = jnew JNamedTreeList(fileList);
	assert( treeList != NULL );

	itsFileTable =
		jnew CBProjectTable(this, menuBar, treeList,
						   scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic,
						   0,0, 10,10);
	assert( itsFileTable != NULL );
	itsFileTable->FitToEnclosure();

	// menus

	itsFileMenu = menuBar->PrependTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CBProjectDocument");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);
	itsFileMenu->SetItemImage(kSaveCmd,          jx_file_save);
	itsFileMenu->SetItemImage(kPrintCmd,         jx_file_print);

	CBFileHistoryMenu* recentProjectMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kProjectFileHistory,
							  itsFileMenu, kRecentProjectMenuCmd, menuBar);
	assert( recentProjectMenu != NULL );

	CBFileHistoryMenu* recentTextMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kTextFileHistory,
							  itsFileMenu, kRecentTextMenuCmd, menuBar);
	assert( recentTextMenu != NULL );

	itsProjectMenu = menuBar->AppendTextMenu(kProjectMenuTitleStr);
	itsProjectMenu->SetMenuItems(kProjectMenuStr, "CBProjectDocument");
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsProjectMenu);

	itsProjectMenu->SetItemImage(kShowSymbolBrowserCmd, jcc_show_symbol_list);
	itsProjectMenu->SetItemImage(kShowCTreeCmd,         jcc_show_c_tree);
	itsProjectMenu->SetItemImage(kShowJavaTreeCmd,      jcc_show_java_tree);
	itsProjectMenu->SetItemImage(kShowPHPTreeCmd,       jcc_show_php_tree);
	itsProjectMenu->SetItemImage(kViewManPageCmd,       jcc_view_man_page);
	itsProjectMenu->SetItemImage(kShowFileListCmd,      jcc_show_file_list);
	itsProjectMenu->SetItemImage(kSearchFilesCmd,       jcc_search_files);
	itsProjectMenu->SetItemImage(kDiffFilesCmd,         jcc_compare_files);

	itsSourceMenu = menuBar->AppendTextMenu(kSourceMenuTitleStr);
	itsSourceMenu->SetMenuItems(kSourceMenuStr, "CBProjectDocument");
	itsSourceMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSourceMenu);

	itsSourceMenu->SetItemImage(kSaveAllTextCmd, jx_file_save_all);
	itsSourceMenu->SetItemImage(kDiffSmartCmd,   jcc_compare_backup);
	itsSourceMenu->SetItemImage(kDiffVCSCmd,     jcc_compare_vcs);

	itsCmdMenu =
		jnew CBCommandMenu(this, NULL, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != NULL );
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	CBDocumentMenu* fileListMenu =
		jnew CBDocumentMenu(kFileListMenuTitleStr, menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != NULL );
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CBProjectDocument");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = (CBGetApplication())->CreateHelpMenu(menuBar, "CBProjectDocument");
	ListenTo(itsHelpMenu);

	// must do this after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kPrintCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsProjectMenu, kSearchFilesCmd);

		(CBGetApplication())->AppendHelpMenuToToolBar(itsToolBar, itsHelpMenu);
		}

	// update pg

	itsUpdatePG = jnew JXProgressDisplay();
	assert( itsUpdatePG != NULL );
	itsUpdatePG->SetItems(NULL, itsUpdateCounter, itsUpdateCleanUpIndicator, itsUpdateLabel);

	ShowUpdatePG(kJFalse);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBProjectDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsProjectMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateProjectMenu();
		}
	else if (sender == itsProjectMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleProjectMenu(selection->GetIndex());
		}

	else if (sender == itsSourceMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateSourceMenu();
		}
	else if (sender == itsSourceMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleSourceMenu(selection->GetIndex());
		}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		(CBGetApplication())->UpdateHelpMenu(itsHelpMenu);
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		(CBGetApplication())->HandleHelpMenu(itsHelpMenu, kCBProjectHelpName,
											 selection->GetIndex());
		}

	else if (sender == itsEditPathsDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			CBDirList pathList;
			itsEditPathsDialog->GetPathList(&pathList);
			if (pathList != *itsDirList)
				{
				*itsDirList = pathList;
				UpdateSymbolDatabase();
				}
			}
		itsEditPathsDialog = NULL;
		}

	else if (sender == CBGetPTTextPrinter() &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		CBPTPrinter* p = CBGetPTTextPrinter();
		if (info->Successful())
			{
			JBoolean onDisk;
			const JString fullName = GetFullName(&onDisk);
			p->SetHeaderName(fullName);

			itsPrintName = p->GetFileName();
			Print(*p);
			}
		StopListening(p);
		}

	else if (sender == CBGetPrefsManager() &&
			 message.Is(CBPrefsManager::kFileTypesChanged))
		{
		const CBPrefsManager::FileTypesChanged* info =
			dynamic_cast<const CBPrefsManager::FileTypesChanged*>(&message);
		assert( info != NULL );

		if (info->AnyChanged())
			{
			itsBuildMgr->ProjectChanged();
			itsSymbolDirector->FileTypesChanged(*info);
			itsCTreeDirector->FileTypesChanged(*info);
			itsJavaTreeDirector->FileTypesChanged(*info);
			itsPHPTreeDirector->FileTypesChanged(*info);
			UpdateSymbolDatabase();

			itsFileTable->Refresh();	// update icons
			}
		}

	else if (sender == itsFileTree &&
			 (message.Is(JTree::kNodeInserted) ||
			  message.Is(JTree::kNodeRemoved)))
		{
		if (itsProcessNodeMessageFlag)
			{
			ProcessNodeMessage(message);
			}
		}
	else if (sender == itsFileTree && message.Is(JTree::kNodeDeleted))
		{
		itsBuildMgr->ProjectChanged();
		}
	else if (sender == itsFileTree && message.Is(JTree::kNodeChanged))
		{
		ProcessNodeMessage(message);
		}
	else if (sender == itsFileTree && message.Is(JTree::kPrepareForNodeMove))
		{
		itsProcessNodeMessageFlag = kJFalse;
		}
	else if (sender == itsFileTree && message.Is(JTree::kNodeMoveFinished))
		{
		itsProcessNodeMessageFlag = kJTrue;
		itsBuildMgr->ProjectChanged();
		}

	else if (sender == itsConfigButton && message.Is(JXButton::kPushed))
		{
		EditMakeConfig();
		}

	else if (sender == itsSaveTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk && !JFileWritable(fullName))
			{
			JEditVCS(fullName);
			}
		WriteFile(fullName, kJFalse);	// always save .jst/.jup, even if .jcc not writable
		DataReverted(kJFalse);			// update file modification time
		}

	else if (sender == itsUpdateLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		SymbolUpdateProgress();
		}
	else if (sender == itsUpdateLink && message.Is(JMessageProtocolT::kReceivedDisconnect))
		{
		SymbolUpdateFinished();
		}

	else if (sender == itsUpdateProcess && message.Is(JProcess::kFinished))
		{
		SymbolUpdateFinished();
		}

	else
		{
		JXFileDocument::Receive(sender, message);
		}
}

/******************************************************************************
 ProcessNodeMessage (private)

 ******************************************************************************/

void
CBProjectDocument::ProcessNodeMessage
	(
	const Message& message
	)
{
	const JTree::NodeMessage* info =
		dynamic_cast<const JTree::NodeMessage*>(&message);
	assert( info != NULL );

	if ((info->GetNode())->GetDepth() == CBProjectTable::kFileDepth)
		{
		const CBProjectNode* node = dynamic_cast<const CBProjectNode*>(info->GetNode());
		assert( node != NULL );

		itsBuildMgr->ProjectChanged(node);

		if (message.Is(JTree::kNodeChanged))
			{
			DelayUpdateSymbolDatabase();
			}
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
CBProjectDocument::UpdateFileMenu()
{
	const JBoolean canPrint = JI2B( itsFileTable->GetRowCount() > 0 );
	itsFileMenu->SetItemEnable(kPrintCmd, canPrint);
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CBProjectDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(this);

	if (!itsFileTable->EndEditing())
		{
		return;
		}

	if (index == kNewTextEditorCmd)
		{
		(CBGetDocumentManager())->NewTextDocument();
		}
	else if (index == kNewTextTemplateCmd)
		{
		(CBGetDocumentManager())->NewTextDocumentFromTemplate();
		}
	else if (index == kNewProjectCmd)
		{
		(CBGetDocumentManager())->NewProjectDocument();
		}
	else if (index == kNewShellCmd)
		{
		(CBGetDocumentManager())->NewShellDocument();
		}
	else if (index == kOpenSomethingCmd)
		{
		(CBGetDocumentManager())->OpenSomething();
		}

	else if (index == kSaveCmd)
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk && !JFileWritable(fullName))
			{
			JEditVCS(fullName);
			}
		SaveInCurrentFile();
		}
	else if (index == kSaveAsTemplateCmd)
		{
		SaveAsTemplate();
		}

	else if (index == kPageSetupCmd)
		{
		(CBGetPTTextPrinter())->BeginUserPageSetup();
		}
	else if (index == kPrintCmd)
		{
		JXPTPrinter* p = CBGetPTTextPrinter();
		p->SetFileName(itsPrintName);
		p->BeginUserPrintSetup();
		ListenTo(p);
		}

	else if (index == kCloseCmd)
		{
		Close();
		}
	else if (index == kQuitCmd)
		{
		(JXGetApplication())->Quit();
		}
}

/******************************************************************************
 SaveAsTemplate (private)

 ******************************************************************************/

void
CBProjectDocument::SaveAsTemplate()
	const
{
	JString origName;
	if ((CBGetDocumentManager())->GetTemplateDirectory(kProjTemplateDir, kJTrue, &origName))
		{
		origName = JCombinePathAndName(origName, GetFileName());

		JString tmplName;
		if ((JGetChooseSaveFile())->SaveFile("Save project template as:", NULL,
											 origName, &tmplName))
			{
			WriteTemplate(tmplName);
			}
		}
}

/******************************************************************************
 Print (private)

 ******************************************************************************/

void
CBProjectDocument::Print
	(
	JPTPrinter& p
	)
	const
{
	JString s = "Project name:  ";
	s += GetFileName();
	s += "\nProject path:  ";
	s += GetFilePath();
	s += "\n";

	itsFileTree->Print(&s);

	p.Print(s);
}

/******************************************************************************
 UpdateProjectMenu (private)

 ******************************************************************************/

void
CBProjectDocument::UpdateProjectMenu()
{
	itsProjectMenu->SetItemEnable(kUpdateMakefileCmd,
		JI2B(itsBuildMgr->GetMakefileMethod() != CBBuildManager::kManual));
}

/******************************************************************************
 HandleProjectMenu (private)

 ******************************************************************************/

void
CBProjectDocument::HandleProjectMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(this);

	if (!itsFileTable->EndEditing())
		{
		return;
		}

	if (index == kOpenMakeConfigDlogCmd)
		{
		itsBuildMgr->EditProjectConfig();
		}
	else if (index == kUpdateMakefileCmd)
		{
		itsBuildMgr->UpdateMakefile(NULL, NULL, kJTrue);
		}

	else if (index == kUpdateSymbolDBCmd)
		{
		UpdateSymbolDatabase();
		}
	else if (index == kShowSymbolBrowserCmd)
		{
		itsSymbolDirector->Activate();
		}
	else if (index == kShowCTreeCmd)
		{
		itsCTreeDirector->Activate();
		}
	else if (index == kShowJavaTreeCmd)
		{
		itsJavaTreeDirector->Activate();
		}
	else if (index == kShowPHPTreeCmd)
		{
		itsPHPTreeDirector->Activate();
		}
	else if (index == kViewManPageCmd)
		{
		(CBGetViewManPageDialog())->Activate();
		}

	else if (index == kEditSearchPathsCmd)
		{
		EditSearchPaths(this);
		}
	else if (index == kShowFileListCmd)
		{
		itsAllFileDirector->Activate();
		}
	else if (index == kFindFileCmd)
		{
		(CBGetFindFileDialog())->Activate();
		}
	else if (index == kSearchFilesCmd)
		{
		(CBGetSearchTextDialog())->Activate();
		}
	else if (index == kDiffFilesCmd)
		{
		(CBGetDiffFileDialog())->Activate();
		}
}

/******************************************************************************
 EditSearchPaths

 ******************************************************************************/

CBEditSearchPathsDialog*
CBProjectDocument::EditSearchPaths
	(
	JXDirector* owner
	)
{
	assert( itsEditPathsDialog == NULL );

	itsEditPathsDialog = jnew CBEditSearchPathsDialog(owner, *itsDirList, itsCSF);
	assert( itsEditPathsDialog != NULL );
	itsEditPathsDialog->BeginDialog();
	ListenTo(itsEditPathsDialog);
	return itsEditPathsDialog;
}

/******************************************************************************
 UpdateSourceMenu (private)

 ******************************************************************************/

void
CBProjectDocument::UpdateSourceMenu()
{
	CBProjectTable::SelType selType;
	JBoolean single;
	JIndex index;
	const JBoolean hasSelection = itsFileTable->GetSelectionType(&selType, &single, &index);
	if (hasSelection && selType == CBProjectTable::kFileSelection)
		{
		itsSourceMenu->SetItemText(kOpenFilesCmd, kOpenFilesItemStr);

		itsSourceMenu->EnableItem(kRemoveSelCmd);
		itsSourceMenu->EnableItem(kOpenFilesCmd);
		itsSourceMenu->EnableItem(kOpenComplFilesCmd);
		itsSourceMenu->SetItemEnable(kEditPathCmd, single);
		itsSourceMenu->SetItemEnable(kEditSubprojConfigCmd,
			JI2B(single && (itsFileTable->GetProjectNode(index))->GetType() == kCBLibraryNT));
		itsSourceMenu->EnableItem(kDiffSmartCmd);
		itsSourceMenu->EnableItem(kDiffVCSCmd);
		itsSourceMenu->EnableItem(kShowLocationCmd);
		}
	else if (hasSelection)
		{
		itsSourceMenu->SetItemText(kOpenFilesCmd, kEditGroupNameItemStr);

		itsSourceMenu->EnableItem(kRemoveSelCmd);
		itsSourceMenu->SetItemEnable(kOpenFilesCmd, single);
		itsSourceMenu->DisableItem(kOpenComplFilesCmd);
		itsSourceMenu->DisableItem(kEditPathCmd);
		itsSourceMenu->DisableItem(kEditSubprojConfigCmd);
		itsSourceMenu->EnableItem(kDiffSmartCmd);
		itsSourceMenu->EnableItem(kDiffVCSCmd);
		itsSourceMenu->EnableItem(kShowLocationCmd);
		}
	else
		{
		itsSourceMenu->SetItemText(kOpenFilesCmd, kOpenFilesItemStr);

		itsSourceMenu->DisableItem(kRemoveSelCmd);
		itsSourceMenu->DisableItem(kOpenFilesCmd);
		itsSourceMenu->DisableItem(kOpenComplFilesCmd);
		itsSourceMenu->DisableItem(kEditPathCmd);
		itsSourceMenu->DisableItem(kEditSubprojConfigCmd);
		itsSourceMenu->DisableItem(kDiffSmartCmd);
		itsSourceMenu->DisableItem(kDiffVCSCmd);
		itsSourceMenu->DisableItem(kShowLocationCmd);
		}

	itsSourceMenu->SetItemEnable(kCloseAllTextCmd,
								 (CBGetDocumentManager())->HasTextDocuments());
	itsSourceMenu->SetItemEnable(kSaveAllTextCmd,
								 (CBGetDocumentManager())->TextDocumentsNeedSave());
}

/******************************************************************************
 HandleSourceMenu (private)

 ******************************************************************************/

void
CBProjectDocument::HandleSourceMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(this);

	if (!itsFileTable->EndEditing())
		{
		return;
		}

	if (index == kNewGroupCmd)
		{
		itsFileTable->NewGroup();
		}
	else if (index == kAddFilesCmd)
		{
		itsFileTable->AddFiles();
		}
	else if (index == kAddDirTreeCmd)
		{
		itsFileTable->AddDirectoryTree();
		}
	else if (index == kRemoveSelCmd)
		{
		itsFileTable->RemoveSelection();
		}

	else if (index == kOpenFilesCmd)
		{
		itsFileTable->OpenSelection();
		}
	else if (index == kOpenComplFilesCmd)
		{
		itsFileTable->OpenComplementFiles();
		}

	else if (index == kEditPathCmd)
		{
		itsFileTable->EditFilePath();
		}
	else if (index == kEditSubprojConfigCmd)
		{
		itsFileTable->EditSubprojectConfig();
		}

	else if (index == kDiffSmartCmd)
		{
		itsFileTable->PlainDiffSelection();
		}
	else if (index == kDiffVCSCmd)
		{
		itsFileTable->VCSDiffSelection();
		}
	else if (index == kShowLocationCmd)
		{
		itsFileTable->ShowSelectedFileLocations();
		}

	else if (index == kSaveAllTextCmd)
		{
		(CBGetDocumentManager())->SaveTextDocuments(kJTrue);
		}
	else if (index == kCloseAllTextCmd)
		{
		(CBGetDocumentManager())->CloseTextDocuments();
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
CBProjectDocument::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
CBProjectDocument::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kProjectPrefsCmd)
		{
		EditProjectPrefs();
		}
	else if (index == kToolBarPrefsCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kEditFileTypesCmd)
		{
		CBGetPrefsManager()->EditFileTypes();
		}
	else if (index == kChooseExtEditorsCmd)
		{
		(CBGetDocumentManager())->ChooseEditors();
		}
	else if (index == kShowLocationPrefsCmd)
		{
		(JXGetWebBrowser())->EditPrefs();
		}
	else if (index == kMiscPrefsCmd)
		{
		(CBGetApplication())->EditMiscPrefs();
		}

	else if (index == kSaveWindSizeCmd)
		{
		CBGetPrefsManager()->SaveWindowSize(kCBProjectWindSizeID, GetWindow());
		}
}

/******************************************************************************
 EditProjectPrefs (private)

 ******************************************************************************/

void
CBProjectDocument::EditProjectPrefs()
{
	CBEditProjPrefsDialog* dlog =
		jnew CBEditProjPrefsDialog(theReopenTextFilesFlag,
								  CBCompileDocument::WillDoubleSpace(),
								  CBBuildManager::WillRebuildMakefileDaily(),
								  CBProjectTable::GetDropFileAction());
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 SetProjectPrefs

 ******************************************************************************/

void
CBProjectDocument::SetProjectPrefs
	(
	const JBoolean							reopenTextFiles,
	const JBoolean							doubleSpaceCompile,
	const JBoolean							rebuildMakefileDaily,
	const CBProjectTable::DropFileAction	dropFileAction
	)
{
	theReopenTextFilesFlag = reopenTextFiles;

	CBCompileDocument::ShouldDoubleSpace(doubleSpaceCompile);
	CBBuildManager::ShouldRebuildMakefileDaily(rebuildMakefileDaily);
	CBProjectTable::SetDropFileAction(dropFileAction);
}

/******************************************************************************
 ReadStaticGlobalPrefs (static)

 ******************************************************************************/

void
CBProjectDocument::ReadStaticGlobalPrefs
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	if (vers >= 51)
		{
		input >> theWarnOpenOldVersionFlag;
		}

	if (vers >= 19)
		{
		CBProjectTable::DropFileAction action;
		input >> theReopenTextFilesFlag >> action;
		CBProjectTable::SetDropFileAction(action);
		}

	if (20 <= vers && vers <= 42)
		{
		JBoolean beepAfterMake, beepAfterCompile;
		input >> beepAfterMake >> beepAfterCompile;
		}

	if (37 <= vers && vers <= 42)
		{
		JBoolean raiseBeforeMake, raiseBeforeCompile, raiseBeforeRun;
		input >> raiseBeforeMake >> raiseBeforeCompile >> raiseBeforeRun;
		}

	if (vers >= 42)
		{
		JBoolean doubleSpaceCompile;
		input >> doubleSpaceCompile;
		CBCompileDocument::ShouldDoubleSpace(doubleSpaceCompile);
		}

	if (vers >= 46)
		{
		JBoolean rebuildMakefileDaily;
		input >> rebuildMakefileDaily;
		CBBuildManager::ShouldRebuildMakefileDaily(rebuildMakefileDaily);
		}

	if (vers >= 47)
		{
		input >> theAddFilesFilter;
		}
}

/******************************************************************************
 WriteStaticGlobalPrefs (static)

 ******************************************************************************/

void
CBProjectDocument::WriteStaticGlobalPrefs
	(
	std::ostream& output
	)
{
	output << theWarnOpenOldVersionFlag;
	output << ' ' << theReopenTextFilesFlag;
	output << ' ' << CBProjectTable::GetDropFileAction();
	output << ' ' << CBCompileDocument::WillDoubleSpace();
	output << ' ' << CBBuildManager::WillRebuildMakefileDaily();
	output << ' ' << theAddFilesFilter;
}

/******************************************************************************
 UpdateCVSIgnore (private)

 ******************************************************************************/

void
CBProjectDocument::UpdateCVSIgnore()
{
	JString ignoreFile = JCombinePathAndName(GetFilePath(), "*");
	ignoreFile        += kSymbolFileSuffix;
	JUpdateCVSIgnore(ignoreFile);

	ignoreFile  = JCombinePathAndName(GetFilePath(), "*");
	ignoreFile += kSettingFileSuffix;
	JUpdateCVSIgnore(ignoreFile);
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
CBProjectDocument::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsCmdMenu && message->Is(CBCommandMenu::kGetTargetInfo))
		{
		CBCommandMenu::GetTargetInfo* info =
			dynamic_cast<CBCommandMenu::GetTargetInfo*>(message);
		assert( info != NULL );
		itsFileTable->GetSelectedFileNames(info->GetFileList());
		}
	else
		{
		JXFileDocument::ReceiveWithFeedback(sender, message);
		}
}

/******************************************************************************
 GetProjectFileSuffix (static)

 ******************************************************************************/

const JCharacter*
CBProjectDocument::GetProjectFileSuffix()
{
	return kProjectFileSuffix;
}

/******************************************************************************
 GetSettingFileSuffix (static)

 ******************************************************************************/

const JCharacter*
CBProjectDocument::GetSettingFileSuffix()
{
	return kSettingFileSuffix;
}

/******************************************************************************
 GetSymbolFileSuffix (static)

 ******************************************************************************/

const JCharacter*
CBProjectDocument::GetSymbolFileSuffix()
{
	return kSymbolFileSuffix;
}

/******************************************************************************
 StartSymbolLoadTimer (private)

 ******************************************************************************/

const long kSymbolLoadTimerStart = 1000000;

int
CBProjectDocument::StartSymbolLoadTimer()
{
	itimerval t = { { 0, 0 }, { kSymbolLoadTimerStart, 0 } };
	return setitimer(ITIMER_PROF, &t, NULL);
}

/******************************************************************************
 StopSymbolLoadTimer (private)

 ******************************************************************************/

void
CBProjectDocument::StopSymbolLoadTimer
	(
	const int timerStatus
	)
{
	itimerval t;
	if (timerStatus == 0 && getitimer(ITIMER_PROF, &t) == 0)
		{
		itsLastSymbolLoadTime = kSymbolLoadTimerStart - (t.it_value.tv_sec + (t.it_value.tv_usec / 1.0e6));
		}

	bzero(&t, sizeof(t));
	setitimer(ITIMER_PROF, &t, NULL);
}

/******************************************************************************
 SymbolUpdateProgress (private)

 ******************************************************************************/

void
CBProjectDocument::SymbolUpdateProgress()
{
	JString msg;
	const JBoolean ok = itsUpdateLink->GetNextMessage(&msg);
	assert( ok );

	long type;

	const std::string s(msg.GetCString(), msg.GetLength());
	std::istringstream input(s);
	input >> type;

	if (type == kFixedLengthStart)
		{
		if (itsUpdatePG->ProcessRunning())
			{
			itsUpdatePG->ProcessFinished();
			}

		JIndex count;
		input >> count;

		JString msg;
		input >> msg;

		itsUpdatePG->FixedLengthProcessBeginning(count, msg, kJFalse, kJTrue);
		}
	else if (type == kVariableLengthStart)
		{
		if (itsUpdatePG->ProcessRunning())
			{
			itsUpdatePG->ProcessFinished();
			}

		JString msg;
		input >> msg;

		itsUpdatePG->VariableLengthProcessBeginning(msg, kJFalse, kJTrue);
		}
	else if (type == kProgressIncrement)
		{
		JIndex delta;
		input >> delta;
		itsUpdatePG->IncrementProgress(delta);
		}
	else if (type == kDoItYourself)
		{
		if (itsUpdatePG->ProcessRunning())
			{
			itsUpdatePG->ProcessFinished();
			}

		(JXGetApplication())->DisplayBusyCursor();

		itsUpdateCounter->Hide();
		itsUpdateCleanUpIndicator->Hide();
		itsUpdateLabel->SetText(JGetString("ReloadingSymbols::CBProjectDocument"));
		GetWindow()->Redraw();

		std::ostringstream pgOutput;
		itsAllFileDirector->GetFileListTable()->Update(
			pgOutput, itsFileTree, *itsDirList, itsSymbolDirector,
			itsCTreeDirector, itsJavaTreeDirector, itsPHPTreeDirector);

		itsAllFileDirector->GetFileListTable()->UpdateFinished();
		SymbolUpdateFinished();
		}
	else if (type == kLockSymbolTable)
		{
		if (itsUpdatePG->ProcessRunning())
			{
			itsUpdatePG->ProcessFinished();
			}

		(JXGetApplication())->DisplayBusyCursor();

		itsUpdateCounter->Hide();
		itsUpdateCleanUpIndicator->Hide();
		itsUpdateLabel->SetText(JGetString("ReloadingSymbols::CBProjectDocument"));
		GetWindow()->Redraw();

		*itsUpdateStream << kSymbolTableLocked << std::endl;

		jdelete itsWaitForUpdateTask;
		itsWaitForUpdateTask = jnew CBWaitForSymbolUpdateTask(itsUpdateProcess);
		assert( itsWaitForUpdateTask != NULL );
		ClearWhenGoingAway(itsWaitForUpdateTask, &itsWaitForUpdateTask);
		itsWaitForUpdateTask->Go();
		}
	else if (type == kSymbolTableWritten)
		{
		JBoolean onDisk;
		const JString symName = GetSymbolFileName(GetFullName(&onDisk));

		std::ifstream symInput(symName);

		const JString symSignature = JRead(symInput, kSymbolFileSignatureLength);

		JFileVersion symVers;
		symInput >> symVers;

		if (symSignature == kSymbolFileSignature &&
			symVers == kCurrentProjectFileVersion)
			{
			const int timerStatus = StartSymbolLoadTimer();

			itsAllFileDirector->GetFileListTable()->UpdateFinished();
			itsAllFileDirector->GetFileListTable()->ReadSetup(symInput, symVers);
			itsSymbolDirector->ReadSetup(symInput, symVers);
			itsCTreeDirector->ReloadSetup(symInput, symVers);
			itsJavaTreeDirector->ReloadSetup(symInput, symVers);
			itsPHPTreeDirector->ReloadSetup(symInput, symVers);

			StopSymbolLoadTimer(timerStatus);
			}

		if (itsWaitForUpdateTask != NULL)
			{
			itsWaitForUpdateTask->StopWaiting();
			}
		itsWaitForUpdateTask = NULL;

		SymbolUpdateFinished();
		}
}

/******************************************************************************
 SymbolUpdateFinished (private)

 ******************************************************************************/

void
CBProjectDocument::SymbolUpdateFinished()
{
	ShowUpdatePG(kJFalse);

	DeleteUpdateLink();

	jdelete itsUpdateStream;
	itsUpdateStream = NULL;

	jdelete itsUpdateProcess;
	itsUpdateProcess = NULL;

	if (itsWaitForUpdateTask != NULL)
		{
		itsWaitForUpdateTask->StopWaiting();
		}
	itsWaitForUpdateTask = NULL;
}

/******************************************************************************
 ShowUpdatePG (private)

 ******************************************************************************/

void
CBProjectDocument::ShowUpdatePG
	(
	const JBoolean visible
	)
{
	if (visible)
		{
		itsUpdateContainer->Show();
		itsToolBar->AdjustSize(0, -itsUpdateContainer->GetFrameHeight());
		}
	else
		{
		itsUpdateContainer->Hide();
		itsToolBar->AdjustSize(0, itsUpdateContainer->GetFrameHeight());

		itsUpdateLabel->SetText("");
		itsUpdateCounter->Hide();
		itsUpdateCleanUpIndicator->Hide();
		}
}

/******************************************************************************
 DelayUpdateSymbolDatabase

	This is used when we expect more user actions that would trigger yet
	another update.

 ******************************************************************************/

void
CBProjectDocument::DelayUpdateSymbolDatabase()
{
	jdelete itsDelaySymbolUpdateTask;
	itsDelaySymbolUpdateTask = jnew CBDelaySymbolUpdateTask(this);
	assert( itsDelaySymbolUpdateTask != NULL );
	itsDelaySymbolUpdateTask->Start();
}

/******************************************************************************
 CancelUpdateSymbolDatabase

 ******************************************************************************/

void
CBProjectDocument::CancelUpdateSymbolDatabase()
{
	jdelete itsDelaySymbolUpdateTask;
	itsDelaySymbolUpdateTask = NULL;

	if (itsUpdateLink != NULL)
		{
		itsUpdateProcess->Kill();
		SymbolUpdateFinished();
		}
}

/******************************************************************************
 UpdateSymbolDatabase

 ******************************************************************************/

void
CBProjectDocument::UpdateSymbolDatabase()
{
	CancelUpdateSymbolDatabase();

	int fd[2][2];
	JError err = JCreatePipe(fd[0]);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	err = JCreatePipe(fd[1]);
	if (!err.OK())
		{
		close(fd[0][0]);
		close(fd[0][1]);
		err.ReportIfError();
		return;
		}

	pid_t pid;
	err = JThisProcess::Fork(&pid);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	// child

	else if (pid == 0)
		{
		close(fd[0][0]);
		close(fd[1][1]);

		JThisProcess::Instance()->SetPriority(19);

		// get rid of JXCreatePG, since we must not use X connection
		JInitCore(jnew ChildAssertHandler());
		CBSetUpdateThread();

		JOutPipeStream output(fd[0][1], kJTrue);

		// update symbol table, trees, etc.
		// (exit immediately if no changes)

		const double prevSymbolLoadTime = itsLastSymbolLoadTime;
		itsLastSymbolLoadTime           = -1;
		const int timerStatus           = StartSymbolLoadTimer();

		if (!itsAllFileDirector->GetFileListTable()->Update(
				output, itsFileTree, *itsDirList, itsSymbolDirector,
				itsCTreeDirector, itsJavaTreeDirector, itsPHPTreeDirector))
			{
			exit(0);
			}

		// tell parent to do it if it takes less time than last full read

		StopSymbolLoadTimer(timerStatus);
		if (itsLastSymbolLoadTime < prevSymbolLoadTime)
			{
			output << kDoItYourself << std::endl;
			output.close();

			JWait(15);	// give last message a chance to be received
			exit(0);
			}

		// no need to clear itimer, since we will soon exit

		// obtain lock on .jst

		output << kLockSymbolTable << std::endl;

		if (!JWaitForInput(fd[1][0], 5*60))		// 5 minutes; in case of blocking dialog
			{
			exit(jerrno());
			}

		JReadUntil(fd[1][0], '\n');

		// write .jst

		std::ostringstream projOutput;

		JBoolean onDisk;
		const JString symName = GetSymbolFileName(GetFullName(&onDisk));

		std::ostream* symOutput = jnew std::ofstream(symName);
		assert( symOutput != NULL );

		WriteFiles(projOutput, NULL, NULL, symName, symOutput);

		output << kSymbolTableWritten << std::endl;
		output.close();

		JWait(15);	// give last message a chance to be received
		exit(0);
		}

	// parent

	else
		{
		close(fd[0][1]);
		close(fd[1][0]);

		itsUpdateStream = jnew JOutPipeStream(fd[1][1], kJTrue);
		assert( itsUpdateStream != NULL );

		ShowUpdatePG(kJTrue);

		itsUpdateProcess = jnew JProcess(pid);
		assert( itsUpdateProcess != NULL );
		ListenTo(itsUpdateProcess);
		itsUpdateProcess->KillAtExit();

		itsUpdateLink = new CBExecOutputDocument::RecordLink(fd[0][0]);
		assert( itsUpdateLink != NULL );
		ListenTo(itsUpdateLink);
		}
}
