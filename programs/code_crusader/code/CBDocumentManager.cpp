/******************************************************************************
 CBDocumentManager.cpp

	BASE CLASS = JXDocumentManager, JPrefObject

	Copyright (C) 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBDocumentManager.h"
#include "CBProjectDocument.h"
#include "CBProjectTree.h"
#include "CBProjectNode.h"
#include "CBExecOutputDocument.h"
#include "CBShellDocument.h"
#include "CBTextEditor.h"
#include "CBExtEditorDialog.h"
#include "CBFileHistoryMenu.h"
#include "cbFileVersions.h"
#include "cbGlobals.h"
#include "cbmUtil.h"
#include <JXWindow.h>
#include <JXStandAlonePG.h>
#include <JXWebBrowser.h>
#include <JXChooseSaveFile.h>
#include <JXSearchTextDialog.h>
#include <JXCloseDirectorTask.h>
#include <JRegex.h>
#include <JSubstitute.h>
#include <JLatentPG.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <JDirInfo.h>
#include <JSimpleProcess.h>
#include <jAssert.h>

static const JCharacter* kSaveNewFilePrompt = "Save file as:";
static const JCharacter* kTextTemplateDir   = "text_templates";

// External editors

static const JCharacter* kDefEditTextFileCmd     = "emacsclient $f";
static const JCharacter* kDefEditTextFileLineCmd = "emacsclient +$l $f";

static const JCharacter* kDefEditBinaryFileCmd = "khexedit $f"; // "xterm -title \"$f\" -e vi $f";

// setup information

const JFileVersion kCurrentSetupVersion = 6;

	// version 6 removes itsUpdateSymbolDBAfterBuildFlag
	// version 5 stores itsEditBinaryLocalFlag and itsEditBinaryFileCmd
	// version 4 stores itsUpdateSymbolDBAfterBuildFlag
	// version 3 converts variable marker from % to $
	// version 2 stores recent projects and recent text documents
	// version 1 stores itsWarnBeforeSaveAllFlag, itsWarnBeforeCloseAllFlag

// state information

const JFileVersion kCurrentStateVersion = 2;

	// version  2 stores open CBTextDocuments even if projects are open
	// version  1 stores project count instead of boolean

// JBroadcaster message types

const JCharacter* CBDocumentManager::kProjectDocumentCreated =
	"ProjectDocumentCreated::CBDocumentManager";

const JCharacter* CBDocumentManager::kProjectDocumentDeleted =
	"ProjectDocumentDeleted::CBDocumentManager";

const JCharacter* CBDocumentManager::kProjectDocumentActivated =
	"ProjectDocumentActivated::CBDocumentManager";

const JCharacter* CBDocumentManager::kAddFileToHistory =
	"AddFileToHistory::CBDocumentManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDocumentManager::CBDocumentManager()
	:
	JXDocumentManager(),
	JPrefObject(CBGetPrefsManager(), kCBDocMgrID),
	itsEditTextFileCmd(kDefEditTextFileCmd),
	itsEditTextFileLineCmd(kDefEditTextFileLineCmd),
	itsEditBinaryFileCmd(kDefEditBinaryFileCmd)
{
	itsProjectDocuments = jnew JPtrArray<CBProjectDocument>(JPtrArrayT::kForgetAll);
	assert( itsProjectDocuments != NULL );
	ListenTo(itsProjectDocuments);

	itsTextDocuments = jnew JPtrArray<CBTextDocument>(JPtrArrayT::kForgetAll);
	assert( itsTextDocuments != NULL );

	itsListDocument = NULL;

	itsWarnBeforeSaveAllFlag  = kJFalse;
	itsWarnBeforeCloseAllFlag = kJFalse;

	itsTextNeedsSaveFlag   = kJFalse;
	itsEditTextLocalFlag   = kJTrue;
	itsEditBinaryLocalFlag = kJFalse;

	itsRecentProjectMenu = NULL;
	itsRecentTextMenu    = NULL;

	itsExtEditorDialog = NULL;

	CBPrefsManager* prefsMgr = CBGetPrefsManager();
	ListenTo(prefsMgr);
	// ReadPrefs() called in CreateHistoryMenus()
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDocumentManager::~CBDocumentManager()
{
	// CBDeleteGlobals() calls WritePrefs()

	jdelete itsProjectDocuments;		// objects deleted by JXDirector
	jdelete itsTextDocuments;		// objects deleted by JXDirector
}

/******************************************************************************
 CreateFileHistoryMenus

	Called by CBCreateGlobals() after permanent windows have been created.

	*** This must only be called once.

 ******************************************************************************/

void
CBDocumentManager::CreateFileHistoryMenus
	(
	JXWindow* window
	)
{
	assert( itsRecentProjectMenu == NULL &&
			itsRecentTextMenu    == NULL );

	itsRecentProjectMenu =
		jnew CBFileHistoryMenu(kProjectFileHistory, "", window,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  0,0, 10,10);
	assert( itsRecentProjectMenu != NULL );
	itsRecentProjectMenu->Hide();

	itsRecentTextMenu =
		jnew CBFileHistoryMenu(kTextFileHistory, "", window,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  0,0, 10,10);
	assert( itsRecentTextMenu != NULL );
	itsRecentTextMenu->Hide();

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 GetFileHistoryMenu

	Called by CBFileHistoryMenu.

 ******************************************************************************/

CBFileHistoryMenu*
CBDocumentManager::GetFileHistoryMenu
	(
	const FileHistoryType type
	)
	const
{
	if (type == kProjectFileHistory)
		{
		return itsRecentProjectMenu;
		}
	else
		{
		assert( type == kTextFileHistory );
		return itsRecentTextMenu;
		}
}

/******************************************************************************
 AddToFileHistoryMenu

 ******************************************************************************/

void
CBDocumentManager::AddToFileHistoryMenu
	(
	const FileHistoryType	type,
	const JCharacter*		fullName
	)
{
	Broadcast(AddFileToHistory(type, fullName));
}

/******************************************************************************
 RefreshCVSStatus

 ******************************************************************************/

void
CBDocumentManager::RefreshCVSStatus()
{
	JSize count = itsProjectDocuments->GetElementCount();
	JIndex i;
	for (i=1; i<=count; i++)
		{
		(itsProjectDocuments->GetElement(i))->RefreshCVSStatus();
		}

	count = itsTextDocuments->GetElementCount();
	for (i=1; i<=count; i++)
		{
		(itsTextDocuments->GetElement(i))->RefreshCVSStatus();
		}
}

/******************************************************************************
 NewProjectDocument

	If doc != NULL, *doc is the new document, if one is successfully created.

 ******************************************************************************/

JBoolean
CBDocumentManager::NewProjectDocument
	(
	CBProjectDocument** doc
	)
{
	CBProjectDocument* d;
	if (CBProjectDocument::Create(&d))
		{
		if (doc != NULL)
			{
			*doc = d;
			}
		return kJTrue;
		}
	else
		{
		if (doc != NULL)
			{
			*doc = NULL;
			}
		return kJFalse;
		}
}

/******************************************************************************
 ProjDocCreated

	Called by CBProjectDocument.

 ******************************************************************************/

void
CBDocumentManager::ProjDocCreated
	(
	CBProjectDocument* doc
	)
{
	itsProjectDocuments->Prepend(doc);
	Broadcast(ProjectDocumentCreated(doc));
}

/******************************************************************************
 ProjDocDeleted

	Called by CBProjectDocument.

 ******************************************************************************/

void
CBDocumentManager::ProjDocDeleted
	(
	CBProjectDocument* doc
	)
{
	itsProjectDocuments->Remove(doc);
	Broadcast(ProjectDocumentDeleted(doc));

	JBoolean onDisk;
	const JString fullName = doc->GetFullName(&onDisk);
	if (onDisk)
		{
		Broadcast(AddFileToHistory(kProjectFileHistory, fullName));
		}
}

/******************************************************************************
 GetActiveProjectDocument

 ******************************************************************************/

JBoolean
CBDocumentManager::GetActiveProjectDocument
	(
	CBProjectDocument** doc
	)
	const
{
	if (!itsProjectDocuments->IsEmpty())
		{
		*doc = itsProjectDocuments->FirstElement();
		return kJTrue;
		}
	else
		{
		*doc = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 ProjectDocumentIsOpen

	If there is a CBProjectDocument that uses the specified file, we return it.

 ******************************************************************************/

JBoolean
CBDocumentManager::ProjectDocumentIsOpen
	(
	const JCharacter*	fileName,
	CBProjectDocument**	doc
	)
	const
{
	*doc = NULL;

	// check that the file exists

	if (!JFileExists(fileName))
		{
		return kJFalse;
		}

	// search for an open CBProjectDocument that uses this file

	const JSize count = itsProjectDocuments->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		*doc = itsProjectDocuments->GetElement(i);

		JBoolean onDisk;
		const JString docName = (**doc).GetFullName(&onDisk);

		if (onDisk && JSameDirEntry(fileName, docName))
			{
			return kJTrue;
			}
		}

	*doc = NULL;
	return kJFalse;
}

/******************************************************************************
 CloseProjectDocuments

 ******************************************************************************/

JBoolean
CBDocumentManager::CloseProjectDocuments()
{
	const JSize count = itsProjectDocuments->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		CBProjectDocument* doc = itsProjectDocuments->GetElement(i);
		if (!doc->Close())
			{
			break;
			}
		}

	return itsProjectDocuments->IsEmpty();
}

/******************************************************************************
 UpdateSymbolDatabases

 ******************************************************************************/

void
CBDocumentManager::UpdateSymbolDatabases()
{
	const JSize count = itsProjectDocuments->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsProjectDocuments->GetElement(i))->DelayUpdateSymbolDatabase();
		}
}

/******************************************************************************
 CancelUpdateSymbolDatabases

 ******************************************************************************/

void
CBDocumentManager::CancelUpdateSymbolDatabases()
{
	const JSize count = itsProjectDocuments->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsProjectDocuments->GetElement(i))->CancelUpdateSymbolDatabase();
		}
}

/******************************************************************************
 NewTextDocument

 ******************************************************************************/

void
CBDocumentManager::NewTextDocument()
{
	JString newName;
	if (itsEditTextLocalFlag)
		{
		CBTextDocument* doc = jnew CBTextDocument;
		assert( doc != NULL );
		doc->Activate();
		}
	else if ((JXGetChooseSaveFile())->SaveFile(
				"Name of new file:", NULL, "", &newName))
		{
		std::ofstream output(newName);
		output.close();
		OpenTextDocument(newName);
		}
}

/******************************************************************************
 NewTextDocumentFromTemplate

 ******************************************************************************/

void
CBDocumentManager::NewTextDocumentFromTemplate()
{
	JString tmplDir;
	const JBoolean exists = GetTextTemplateDirectory(kJFalse, &tmplDir);
	if (!exists)
		{
		JString msg = "There are no templates in ~/.jxcb/";
		msg += kTextTemplateDir;
		(JGetUserNotification())->ReportError(msg);
		return;
		}

	tmplDir += ACE_DIRECTORY_SEPARATOR_STR "*";		// make ChooseFile() happy

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	JString tmplName;
	if (csf->ChooseFile("", NULL, tmplDir, &tmplName))
		{
		JString newName;
		if (itsEditTextLocalFlag)
			{
			CBTextDocument* doc = jnew CBTextDocument(tmplName, kCBUnknownFT, kJTrue);
			assert( doc != NULL );
			doc->Activate();
			}
		else if (csf->SaveFile("Name of new file:", NULL, "", &newName))
			{
			JString tmplText;
			JReadFile(tmplName, &tmplText);
			std::ofstream output(newName);
			tmplText.Print(output);
			output.close();
			OpenTextDocument(newName);
			}
		}
}

/******************************************************************************
 GetTemplateDirectory

	If create==kJTrue, tries to create the directory if it doesn't exist.

 ******************************************************************************/

JBoolean
CBDocumentManager::GetTextTemplateDirectory
	(
	const JBoolean	create,
	JString*		tmplDir
	)
{
	return GetTemplateDirectory(kTextTemplateDir, create, tmplDir);
}

JBoolean
CBDocumentManager::GetTemplateDirectory
	(
	const JCharacter*	dirName,
	const JBoolean		create,
	JString*			fullName
	)
{
	fullName->Clear();

	JString sysDir, userDir;
	const JBoolean hasHomeDir   = JXGetProgramDataDirectories(dirName, &sysDir, &userDir);
	const JBoolean hasUserDir   = JI2B(hasHomeDir && JDirectoryExists(userDir));
	const JBoolean hasUserFiles = JI2B(hasUserDir && !JDirInfo::Empty(userDir));

	if (!hasHomeDir)
		{
		if (!JDirInfo::Empty(sysDir))
			{
			*fullName = sysDir;
			return kJTrue;
			}
		if (create)
			{
			(JGetUserNotification())->ReportError(
				"Unable to create a directory for storing templates "
				"because you do not have a home directory.");
			}
		return kJFalse;
		}
	else if (hasUserDir)
		{
		if (!hasUserFiles && !JDirInfo::Empty(sysDir))
			{
			*fullName = sysDir;
			return kJTrue;
			}
		*fullName = userDir;
		return JI2B(create || hasUserFiles);
		}
	else
		{
		if (!JDirInfo::Empty(sysDir))
			{
			*fullName = sysDir;
			return kJTrue;
			}
		if (!create)
			{
			return kJFalse;
			}

		const JError err = JCreateDirectory(userDir);
		if (err.OK())
			{
			*fullName = userDir;
			return kJTrue;
			}
		else
			{
			JString msg = "Unable to create the directory ";
			msg += userDir;
			msg += " for storing templates because:\n\n";
			msg += err.GetMessage();
			(JGetUserNotification())->ReportError(msg);
			return kJFalse;
			}
		}
}

/******************************************************************************
 TextDocumentCreated

	Called by CBTextDocument.

 ******************************************************************************/

void
CBDocumentManager::TextDocumentCreated
	(
	CBTextDocument* doc
	)
{
	itsTextDocuments->Append(doc);
}

/******************************************************************************
 TextDocumentDeleted

	Called by CBTextDocument.

 ******************************************************************************/

void
CBDocumentManager::TextDocumentDeleted
	(
	CBTextDocument* doc
	)
{
	JIndex i;
	if (itsTextDocuments->Find(doc, &i))
		{
		itsTextDocuments->RemoveElement(i);

		if (i == 1 && !itsTextDocuments->IsEmpty())
			{
			(JXGetSearchTextDialog())->SetActiveTE(
				(itsTextDocuments->FirstElement())->GetTextEditor());
			}
		}

	if (doc == itsListDocument)
		{
		itsListDocument = NULL;
		}

	JBoolean onDisk;
	const JString fullName = doc->GetFullName(&onDisk);
	if (onDisk)
		{
		Broadcast(AddFileToHistory(kTextFileHistory, fullName));
		}
}

/******************************************************************************
 GetActiveTextDocument

 ******************************************************************************/

JBoolean
CBDocumentManager::GetActiveTextDocument
	(
	CBTextDocument** doc
	)
	const
{
	if (!itsTextDocuments->IsEmpty())
		{
		*doc = itsTextDocuments->FirstElement();
		return kJTrue;
		}
	else
		{
		*doc = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 SetActiveTextDocument

	Called by CBTextDocument.

 ******************************************************************************/

void
CBDocumentManager::SetActiveTextDocument
	(
	CBTextDocument* doc
	)
{
	JIndex i;
	if (itsTextDocuments->Find(doc, &i) && i != 1)
		{
		itsTextDocuments->MoveElementToIndex(i, 1);
		doc->CheckIfModifiedByOthers();
		(JXGetSearchTextDialog())->SetActiveTE(doc->GetTextEditor());
		}
}

/******************************************************************************
 GetActiveListDocument

 ******************************************************************************/

JBoolean
CBDocumentManager::GetActiveListDocument
	(
	CBExecOutputDocument** doc
	)
	const
{
	*doc = itsListDocument;
	return JI2B( itsListDocument != NULL );
}

/******************************************************************************
 SetActiveListDocument

	Called by CBSearchDocument and CBCompileDocument.

 ******************************************************************************/

void
CBDocumentManager::SetActiveListDocument
	(
	CBExecOutputDocument* doc
	)
{
	itsListDocument = doc;
}

/******************************************************************************
 NewShellDocument

 ******************************************************************************/

void
CBDocumentManager::NewShellDocument()
{
	CBShellDocument* doc;
	CBShellDocument::Create(&doc);
}

/******************************************************************************
 OpenSomething

	Determines the file type and creates the appropriate document.

	If fileName is NULL or empty, we ask the user to choose a file.
	If lineIndex is not zero and we open a CBTextDocument, we go to that line.

 ******************************************************************************/

void
CBDocumentManager::OpenSomething
	(
	const JCharacter*	fileName,
	const JIndexRange	lineRange,		// not reference because of default value
	const JBoolean		iconify,
	const JBoolean		forceReload
	)
{
	if (!JString::IsEmpty(fileName))
		{
		if (JFileReadable(fileName))
			{
			PrivateOpenSomething(fileName, lineRange, iconify, forceReload);
			}
		else if (!JFileExists(fileName))
			{
			JString msg = fileName;
			msg.PrependCharacter('"');
			msg += "\" does not exist.";
			(JGetUserNotification())->ReportError(msg);
			}
		else
			{
			JString msg = "You do not have permission to read \"";
			msg += fileName;
			msg += "\".";
			(JGetUserNotification())->ReportError(msg);
			}
		}
	else
		{
		JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
		if ((JGetChooseSaveFile())->ChooseFiles("Files to open:", NULL, &fullNameList))
			{
			OpenSomething(fullNameList);
			}
		}
}

void
CBDocumentManager::OpenSomething
	(
	const JPtrArray<JString>& fileNameList
	)
{
	const JSize count = fileNameList.GetElementCount();
	if (count > 0)
		{
		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(count, "Opening files...", kJTrue, kJFalse);

		for (JIndex i=1; i<=count; i++)
			{
			const JString* fileName = fileNameList.GetElement(i);
			if (!fileName->IsEmpty())
				{
				OpenSomething(*fileName);
				}
			if (!pg.IncrementProgress())
				{
				break;
				}
			}

		pg.ProcessFinished();
		}
}

/******************************************************************************
 PrivateOpenSomething (private)

 ******************************************************************************/

void
CBDocumentManager::PrivateOpenSomething
	(
	const JCharacter*	fileName,
	const JIndexRange&	lineRange,
	const JBoolean		iconify,
	const JBoolean		forceReload
	)
{
	CBProjectDocument* doc;
	if (CBProjectDocument::Create(fileName, kJFalse, &doc) ==
		JXFileDocument::kNotMyFile)
		{
		JString cmd;
		if (CBGetPrefsManager()->EditWithOtherProgram(fileName, &cmd))
			{
			JSimpleProcess::Create(cmd, kJTrue);
			}
		else if (CBGetPrefsManager()->GetFileType(fileName) == kCBBinaryFT ||
				 CBTextDocument::OpenAsBinaryFile(fileName))
			{
			PrivateOpenBinaryDocument(fileName, iconify, forceReload);
			}
		else
			{
			PrivateOpenTextDocument(fileName, lineRange, iconify, forceReload, NULL);
			}
		}
	else if (doc != NULL && iconify)
		{
		(doc->GetWindow())->Iconify();
		}
}

/******************************************************************************
 OpenTextDocument

	Open the given file, assuming that it is a text file.
	If lineIndex is not zero, we go to that line.

	Returns kJTrue if the file was successfully opened.  If doc != NULL,
	*doc is set to the document.

 ******************************************************************************/

JBoolean
CBDocumentManager::OpenTextDocument
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex,
	CBTextDocument**	doc,
	const JBoolean		iconify,
	const JBoolean		forceReload
	)
{
	return OpenTextDocument(fileName, JIndexRange(lineIndex, lineIndex),
							doc, iconify, forceReload);
}

JBoolean
CBDocumentManager::OpenTextDocument
	(
	const JCharacter*	fileName,
	const JIndexRange&	lineRange,
	CBTextDocument**	doc,
	const JBoolean		iconify,
	const JBoolean		forceReload
	)
{
	if (doc != NULL)
		{
		*doc = NULL;
		}

	if (JString::IsEmpty(fileName))
		{
		return kJFalse;
		}

	const JCharacter* map[] =
		{
		"f", fileName
		};

	const JBoolean isFile = JFileExists(fileName);
	if (!isFile && JNameUsed(fileName))
		{
		const JString msg = JGetString("NotAFile::CBDocumentManager", map, sizeof(map));
		(JGetUserNotification())->ReportError(msg);
		return kJFalse;
		}
	else if (!isFile)
		{
		const JString msg = JGetString("FileDoesNotExist::CBDocumentManager", map, sizeof(map));
		if (!(JGetUserNotification())->AskUserYes(msg))
			{
			return kJFalse;
			}
		else
			{
			std::ofstream temp(fileName);
			if (!temp.good())
				{
				const JString msg = JGetString("CannotCreateFile::CBDocumentManager", map, sizeof(map));
				(JGetUserNotification())->ReportError(msg);
				return kJFalse;
				}
			}
		}

	return PrivateOpenTextDocument(fileName, lineRange, iconify, forceReload, doc);
}

/******************************************************************************
 PrivateOpenTextDocument (private)

	lineIndex can be zero.

	Returns kJTrue if the file was successfully opened locally.
	If doc != NULL, *doc is set to the document.

 ******************************************************************************/

JBoolean
CBDocumentManager::PrivateOpenTextDocument
	(
	const JCharacter*	fullName,
	const JIndexRange&	lineRange,
	const JBoolean		iconify,
	const JBoolean		forceReload,
	CBTextDocument**	returnDoc
	)
	const
{
	assert( !JString::IsEmpty(fullName) );

	if (returnDoc != NULL)
		{
		*returnDoc = NULL;
		}

	if (itsEditTextLocalFlag)
		{
		JXFileDocument* doc;
		if (!FileDocumentIsOpen(fullName, &doc))
			{
			if (!WarnFileSize(fullName))
				{
				return kJFalse;
				}
			doc = jnew CBTextDocument(fullName);
			}
		assert( doc != NULL );
		if (iconify && !doc->IsActive())
			{
			(doc->GetWindow())->Iconify();
			}
		if (!iconify || !doc->IsActive())
			{
			doc->Activate();
			}

		CBTextDocument* textDoc = dynamic_cast<CBTextDocument*>(doc);
		if (textDoc != NULL)
			{
			textDoc->RevertIfChangedByOthers(forceReload);
			if (!lineRange.IsEmpty())
				{
				textDoc->SelectLines(lineRange);
				}
			}

		if (returnDoc != NULL)
			{
			*returnDoc = textDoc;
			}
		return JI2B( textDoc != NULL );
		}
	else
		{
		JString cmd;
		if (!lineRange.IsEmpty())
			{
			cmd = itsEditTextFileLineCmd;
			}
		else
			{
			cmd = itsEditTextFileCmd;
			}

		const JString name = JPrepArgForExec(fullName);
		const JString lineIndexStr(lineRange.first, 0);

		JSubstitute sub;
		sub.IgnoreUnrecognized();
		sub.DefineVariable("f", name);
		sub.DefineVariable("l", lineIndexStr);
		sub.Substitute(&cmd);

		JSimpleProcess::Create(cmd, kJTrue);
		return kJFalse;
		}
}

/******************************************************************************
 WarnFileSize (static)

 ******************************************************************************/

JBoolean
CBDocumentManager::WarnFileSize
	(
	const JCharacter* fullName
	)
{
	JSize size;
	if (JGetFileLength(fullName, &size) == kJNoError &&
		size > kMinWarnFileSize)
		{
		JString path, fileName;
		JSplitPathAndName(fullName, &path, &fileName);

		const JCharacter* map[] =
			{
			"f", fileName
			};
		const JString msg = JGetString("WarnFileTooLarge::CBDocumentManager", map, sizeof(map));
		if (!(JGetUserNotification())->AskUserNo(msg))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 SaveTextDocuments

 ******************************************************************************/

JBoolean
CBDocumentManager::SaveTextDocuments
	(
	const JBoolean saveUntitled
	)
{
	if (!itsWarnBeforeSaveAllFlag ||
		(JGetUserNotification())->AskUserNo(
			"Are you sure you want to save all text documents?"))
		{
		itsTextNeedsSaveFlag = kJFalse;

		const JSize count = itsTextDocuments->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			CBTextDocument* doc = itsTextDocuments->GetElement(i);
			if (CBIsExecOutput(doc->GetFileType()))
				{
				CBExecOutputDocument* execDoc =
					dynamic_cast<CBExecOutputDocument*>(doc);
				assert( execDoc != NULL );
				if (execDoc->ProcessRunning())
					{
					continue;
					}
				}

			if ((saveUntitled || doc->ExistsOnDisk()) && !doc->Save())
				{
				itsTextNeedsSaveFlag = kJTrue;
				}
			}
		}

	return !itsTextNeedsSaveFlag;
}

/******************************************************************************
 ReloadTextDocuments

 ******************************************************************************/

void
CBDocumentManager::ReloadTextDocuments
	(
	const JBoolean force
	)
{
	const JSize count = itsTextDocuments->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBTextDocument* doc = itsTextDocuments->GetElement(i);
		doc->RevertIfChangedByOthers(force);
		}
}

/******************************************************************************
 CloseTextDocuments

 ******************************************************************************/

JBoolean
CBDocumentManager::CloseTextDocuments()
{
	if (!itsWarnBeforeCloseAllFlag ||
		(JGetUserNotification())->AskUserNo(
			"Are you sure you want to close all text documents?"))
		{
		// This is safe because there are no dependencies between documents.

		JIndex i=1;
		while (i <= itsTextDocuments->GetElementCount())
			{
			CBTextDocument* doc = itsTextDocuments->GetElement(i);
			if (CBIsExecOutput(doc->GetFileType()))
				{
				CBExecOutputDocument* execDoc =
					dynamic_cast<CBExecOutputDocument*>(doc);
				assert( execDoc != NULL );
				if (execDoc->ProcessRunning())
					{
					i++;
					continue;
					}
				}

			if (!doc->Close())
				{
				break;
				}
			}
		}

	return itsTextDocuments->IsEmpty();
}

/******************************************************************************
 FileRenamed

 ******************************************************************************/

void
CBDocumentManager::FileRenamed
	(
	const JCharacter* origFullName,
	const JCharacter* newFullName
	)
{
	const JSize count = itsProjectDocuments->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsProjectDocuments->GetElement(i)->
			GetFileTree()->GetProjectRoot()->
				FileRenamed(origFullName, newFullName);
		}
}

/******************************************************************************
 StylerChanged

 ******************************************************************************/

void
CBDocumentManager::StylerChanged
	(
	JTEStyler* styler
	)
{
	const JSize count = itsTextDocuments->GetElementCount();

	JLatentPG pg;
	pg.FixedLengthProcessBeginning(count, "Adjusting styles...", kJFalse, kJFalse);

	for (JIndex i=1; i<=count; i++)
		{
		(itsTextDocuments->GetElement(i))->StylerChanged(styler);
		pg.IncrementProgress();
		}

	pg.ProcessFinished();
}

/******************************************************************************
 OpenBinaryDocument

	Open the given file, assuming that it is a binary file.

 ******************************************************************************/

void
CBDocumentManager::OpenBinaryDocument
	(
	const JCharacter* fileName
	)
{
	if (!JString::IsEmpty(fileName) && JFileExists(fileName))
		{
		PrivateOpenBinaryDocument(fileName, kJFalse, kJFalse);
		}
}

/******************************************************************************
 PrivateOpenBinaryDocument (private)

 ******************************************************************************/

void
CBDocumentManager::PrivateOpenBinaryDocument
	(
	const JCharacter*	fullName,
	const JBoolean		iconify,
	const JBoolean		forceReload
	)
	const
{
	assert( !JString::IsEmpty(fullName) );

	JString cmd;
	if (CBGetPrefsManager()->EditWithOtherProgram(fullName, &cmd))
		{
		JSimpleProcess::Create(cmd, kJTrue);
//		return kJFalse;
		}
/*	else if (itsEditBinaryLocalFlag)
		{
		// some day...
//		return kJTrue;
		}
*/	else
		{
		cmd = itsEditBinaryFileCmd;

		JSubstitute sub;
		sub.IgnoreUnrecognized();
		sub.DefineVariable("f", fullName);
		sub.Substitute(&cmd);

		JSimpleProcess::Create(cmd, kJTrue);
//		return kJFalse;
		}
}

/******************************************************************************
 SearchFile

	If the document is open, search it in memory.

 ******************************************************************************/

JBoolean
CBDocumentManager::SearchFile
	(
	const JCharacter*	fileName,
	const JCharacter*	searchPattern,
	const JBoolean		caseSensitive,
	JIndex*				lineIndex
	)
	const
{
	JRegex pattern(searchPattern);
	pattern.SetCaseSensitive(caseSensitive);

	JXFileDocument* doc;
	if (FileDocumentIsOpen(fileName, &doc))
		{
		CBTextDocument* textDoc = dynamic_cast<CBTextDocument*>(doc);
		if (textDoc != NULL)
			{
			CBTextEditor* te = textDoc->GetTextEditor();
			JIndex charIndex;
			if (SearchLine(te->GetText(), pattern, &charIndex))
				{
				*lineIndex = te->GetLineForChar(charIndex);
				return kJTrue;
				}
			}
		return kJFalse;
		}
	else
		{
		std::ifstream input(fileName);

		*lineIndex = 0;
		while (!input.eof())
			{
			(*lineIndex)++;
			const JString line = JReadLine(input);
			if (input.fail())
				{
				break;
				}
			JIndex charIndex;
			if (SearchLine(line, pattern, &charIndex))
				{
				return kJTrue;
				}
			}

		return kJFalse;
		}
}

// private

JBoolean
CBDocumentManager::SearchLine
	(
	const JString&	text,
	const JRegex&	pattern,
	JIndex*			charIndex
	)
	const
{
	JIndexRange r;
	while (pattern.MatchAfter(text, r, &r))
		{
		if (!(r.first > 1 &&
			  CBMIsCharacterInWord(text, r.first) &&
			  CBMIsCharacterInWord(text, r.first-1)) &&
			!(r.last < text.GetLength() &&
			  CBMIsCharacterInWord(text, r.last) &&
			  CBMIsCharacterInWord(text, r.last+1)))
			{
			*charIndex = r.first;
			return kJTrue;
			}
		}

	*charIndex = 0;
	return kJFalse;
}

/******************************************************************************
 OpenComplementFile

 ******************************************************************************/

JBoolean
CBDocumentManager::OpenComplementFile
	(
	const JString&			fullName,
	const CBTextFileType	type,
	CBProjectDocument*		projDoc,
	const JBoolean			searchDirs
	)
{
	JString complName;
	if (GetComplementFile(fullName, type, &complName, projDoc, searchDirs))
		{
		return OpenTextDocument(complName);
		}
	else if (CBHasComplementType(type))
		{
		JString path, fileName;
		JSplitPathAndName(fullName, &path, &fileName);

		JString msg = "Unable to find complement of \"";
		msg += fileName;
		msg += "\".  Press the Save button if you wish to create it.";

		CBPrefsManager* prefsMgr = CBGetPrefsManager();
		JIndex suffixIndex;
		const JString origName =
			prefsMgr->GetDefaultComplementSuffix(fullName, type, &suffixIndex);

		JString newFullName;
		if ((JGetChooseSaveFile())->
				SaveFile(kSaveNewFilePrompt, msg, origName, &newFullName))
			{
			prefsMgr->SetDefaultComplementSuffix(suffixIndex, newFullName);

			std::ofstream temp(newFullName);
			temp.close();
			return OpenTextDocument(newFullName);
			}
		}

	return kJFalse;
}

/******************************************************************************
 Get open complement file

	Return the document for the source file corresponding to the given header file,
	or visa versa.

 ******************************************************************************/

JBoolean
CBDocumentManager::GetOpenComplementFile
	(
	const JString&			inputName,
	const CBTextFileType	inputType,
	JXFileDocument**		doc
	)
	const
{
	CBTextFileType outputType;
	return JI2B(CBGetComplementType(inputType, &outputType) &&
				(FindOpenComplementFile(inputName, outputType, doc) ||
				 (CBIsHeaderType(inputType) &&
				  FindOpenComplementFile(inputName, kCBDocumentationFT, doc))));
}

// private

JBoolean
CBDocumentManager::FindOpenComplementFile
	(
	const JString&			inputName,
	const CBTextFileType	outputType,
	JXFileDocument**		doc
	)
	const
{
JIndex i;

	JString baseName;
	JIndex dotIndex;
	if (inputName.LocateLastSubstring(".", &dotIndex) && dotIndex > 1)
		{
		baseName = inputName.GetSubstring(1, dotIndex-1);
		}
	else
		{
		baseName = inputName;
		}

	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	CBGetPrefsManager()->GetFileSuffixes(outputType, &suffixList);
	const JSize suffixCount = suffixList.GetElementCount();

	// check full name of each document

	JString fullName;
	for (i=1; i<=suffixCount; i++)
		{
		fullName = baseName + *(suffixList.GetElement(i));
		if (FileDocumentIsOpen(fullName, doc))
			{
			return kJTrue;
			}
		}

	// check file name of each document, in case complement is in different directory

	JString path, base, name;
	JSplitPathAndName(baseName, &path, &base);
	const JSize docCount = itsTextDocuments->GetElementCount();
	for (i=1; i<=suffixCount; i++)
		{
		name = base + *(suffixList.GetElement(i));
		for (JIndex j=1; j<=docCount; j++)
			{
			CBTextDocument* d = itsTextDocuments->GetElement(j);
			if (d->GetFileName() == name)
				{
				*doc = d;
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/******************************************************************************
 Get complement file

	Get the name of the source file corresponding to the given header file,
	or visa versa.

	If projDoc == NULL, the active project document is used.

 ******************************************************************************/

JBoolean
CBDocumentManager::GetComplementFile
	(
	const JString&			inputName,
	const CBTextFileType	inputType,
	JString*				outputName,
	CBProjectDocument*		projDoc,
	const JBoolean			searchDirs
	)
	const
{
	CBTextFileType outputType;
	return JI2B(CBGetComplementType(inputType, &outputType) &&
				(FindComplementFile(inputName, outputType, outputName, projDoc, searchDirs) ||
				 (CBIsHeaderType(inputType) &&
				  FindComplementFile(inputName, kCBDocumentationFT, outputName, projDoc, searchDirs))));
}

// private -- projDoc can be NULL

JBoolean
CBDocumentManager::FindComplementFile
	(
	const JString&			inputName,
	const CBTextFileType	outputType,
	JString*				outputName,
	CBProjectDocument*		projDoc,
	const JBoolean			searchDirs
	)
	const
{
	JString baseName;
	JIndex dotIndex;
	if (inputName.LocateLastSubstring(".", &dotIndex) && dotIndex > 1)
		{
		baseName = inputName.GetSubstring(1, dotIndex-1);
		}
	else
		{
		baseName = inputName;
		}

	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	CBGetPrefsManager()->GetFileSuffixes(outputType, &suffixList);
	const JSize suffixCount = suffixList.GetElementCount();

	JBoolean found = kJFalse;
	for (JIndex i=1; i<=suffixCount; i++)
		{
		*outputName = baseName + *(suffixList.GetElement(i));
		if (JFileExists(*outputName))
			{
			found = kJTrue;
			break;
			}
		}

	if (!found && searchDirs &&
		(projDoc != NULL || GetActiveProjectDocument(&projDoc)))
		{
		found = SearchForComplementFile(projDoc, inputName, baseName,
										suffixList, outputName);
		}

	return found;
}

/******************************************************************************
 SearchForComplementFile (private)

	Sort the project directories by how close they are to the given file
	and then search each one.

	e.g. starting with /a/b/c, we could get { /a/b/d, /a/f/g, /e/h }

 ******************************************************************************/

struct DirMatchInfo
{
	JString*	path;
	JBoolean	recurse;
	JSize		matchLength;
};

static JOrderedSetT::CompareResult
	CompareMatchLengths(const DirMatchInfo& i1, const DirMatchInfo& i2);

JBoolean
CBDocumentManager::SearchForComplementFile
	(
	CBProjectDocument*			projDoc,
	const JString&				origFullName,
	const JString&				baseFullName,
	const JPtrArray<JString>&	suffixList,
	JString*					outputName
	)
	const
{
JIndex i;

	JString origPath, baseName;
	JSplitPathAndName(baseFullName, &origPath, &baseName);

	// sort the directories

	const CBDirList& origDirList = projDoc->GetDirectories();
	JSize dirCount               = origDirList.GetElementCount();
	if (dirCount == 0)
		{
		return kJFalse;
		}

	JArray<DirMatchInfo> dirList(dirCount);
	dirList.SetSortOrder(JOrderedSetT::kSortDescending);
	dirList.SetCompareFunction(CompareMatchLengths);

	DirMatchInfo info;
	for (i=1; i<=dirCount; i++)
		{
		info.path = jnew JString;
		assert( info.path != NULL );

		if (origDirList.GetTruePath(i, info.path, &(info.recurse)))
			{
			info.matchLength = JCalcMatchLength(origPath, *(info.path));
			dirList.InsertSorted(info);
			}
		else
			{
			jdelete info.path;
			}
		}

	dirCount = dirList.GetElementCount();
	if (dirCount == 0)
		{
		return kJFalse;
		}

	// search for each suffix in all directories

	JBoolean found          = kJFalse;
	JBoolean cancelled      = kJFalse;
	const JSize suffixCount = suffixList.GetElementCount();

	JLatentPG pg;
	JString origFilePath, origFileName;
	JSplitPathAndName(origFullName, &origFilePath, &origFileName);
	JString msg = "Searching for complement of \"";
	msg += origFileName;
	msg += "\"...";
	pg.FixedLengthProcessBeginning(suffixCount * dirCount, msg, kJTrue, kJFalse);

	JString searchName, newPath, newName;
	for (i=1; i<=suffixCount; i++)
		{
		searchName = baseName + *(suffixList.GetElement(i));
		for (JIndex j=1; j<=dirCount; j++)
			{
			const DirMatchInfo info = dirList.GetElement(j);
			if (info.recurse &&
				JSearchSubdirs(*info.path, searchName, kJTrue, kJTrue,
							   &newPath, &newName))
				{
				*outputName = JCombinePathAndName(newPath, newName);
				found       = kJTrue;
				break;
				}
			else if (!info.recurse)
				{
				const JString fullName = JCombinePathAndName(*(info.path), searchName);
				if (JFileExists(fullName))
					{
					*outputName = fullName;
					found       = kJTrue;
					break;
					}
				}

			if (!pg.IncrementProgress())
				{
				cancelled = kJTrue;
				break;
				}
			}

		if (found || cancelled)
			{
			break;
			}
		}

	pg.ProcessFinished();

	// clean up

	for (i=1; i<=dirCount; i++)
		{
		jdelete (dirList.GetElement(i)).path;
		}

	return found;
}

// static

JOrderedSetT::CompareResult
CompareMatchLengths
	(
	const DirMatchInfo& i1,
	const DirMatchInfo& i2
	)
{
	if (i1.matchLength < i2.matchLength)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (i1.matchLength == i2.matchLength)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 ReadFromProject

	This must be at the end of the file so it can be cancelled.

 ******************************************************************************/

void
CBDocumentManager::ReadFromProject
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	JSize textCount;
	input >> textCount;

	if (textCount > 0)
		{
		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(textCount, "Opening files...", kJTrue, kJFalse);

		for (JIndex i=1; i<=textCount; i++)
			{
			JBoolean keep;
			CBTextDocument* doc = jnew CBTextDocument(input, vers, &keep);
			assert( doc != NULL );
			if (!keep)
				{
				doc->Close();
				}
			else
				{
				doc->Activate();
				}

			if (!pg.IncrementProgress())
				{
				break;
				}
			}

		pg.ProcessFinished();
		}
}

/******************************************************************************
 WriteForProject

	This must be at the end of the file so it can be cancelled.

 ******************************************************************************/

void
CBDocumentManager::WriteForProject
	(
	std::ostream& output
	)
	const
{
	const JSize textCount = itsTextDocuments->GetElementCount();
	output << textCount;

	for (JIndex i=1; i<=textCount; i++)
		{
		output << ' ';
		CBTextDocument* doc = itsTextDocuments->GetElement(i);
		doc->WriteForProject(output);
		}
}

/******************************************************************************
 RestoreState

	Reopens files that were open when we quit the last time.  Returns kJFalse
	if nothing is opened.

 ******************************************************************************/

JBoolean
CBDocumentManager::RestoreState
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentStateVersion)
		{
		return kJFalse;
		}

	JSize projCount;
	if (vers == 0)
		{
		JBoolean projectWasOpen;
		input >> projectWasOpen;
		projCount = projectWasOpen ? 1 : 0;
		}
	else
		{
		input >> projCount;
		}

	if (projCount > 0)
		{
		JBoolean saveReopen = CBProjectDocument::WillReopenTextFiles();
		if (kCurrentStateVersion >= 2)
			{
			CBProjectDocument::ShouldReopenTextFiles(kJFalse);
			}

		JBoolean onDisk = kJTrue;
		JString fileName;
		for (JIndex i=1; i<=projCount; i++)
			{
			if (vers >= 1)
				{
				input >> onDisk;
				}
			input >> fileName;
			if (onDisk && JFileReadable(fileName))
				{
				OpenSomething(fileName);
				}
			}

		CBProjectDocument::ShouldReopenTextFiles(saveReopen);
		}

	if (kCurrentStateVersion >= 2 || projCount == 0)
		{
		JFileVersion projVers;
		input >> projVers;
		if (projVers <= kCurrentProjectFileVersion)
			{
			ReadFromProject(input, projVers);
			}
		}

	return JI2B( HasProjectDocuments() || HasTextDocuments() );
}

/******************************************************************************
 SaveState

	Saves files that are currently open so they can be reopened next time.
	Returns kJFalse if there is nothing to save.

	Always calls WriteForProject() because project documents might be
	write protected, e.g., CVS.

 ******************************************************************************/

JBoolean
CBDocumentManager::SaveState
	(
	std::ostream& output
	)
	const
{
	if (itsProjectDocuments->IsEmpty() &&
		itsTextDocuments->IsEmpty())
		{
		return kJFalse;
		}

	output << kCurrentStateVersion;

	const JSize projCount = itsProjectDocuments->GetElementCount();
	output << ' ' << projCount;

	if (projCount > 0)
		{
		JBoolean onDisk;
		JString fullName;
		for (JIndex i=1; i<=projCount; i++)
			{
			fullName = (itsProjectDocuments->GetElement(i))->GetFullName(&onDisk);
			output << ' ' << onDisk << ' ' << fullName;
			}
		}

	output << ' ' << kCurrentProjectFileVersion;
	output << ' ';
	WriteForProject(output);

	return kJTrue;
}

/******************************************************************************
 Receive (virtual protected)

	By calling CBTextDocument::UpdateFileType() here, we don't have to store
	another copy of the document list in JBroadcaster, and we display a
	progress display, since it can take a while.

 ******************************************************************************/

void
CBDocumentManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProjectDocuments && message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast<const JOrderedSetT::ElementsInserted*>(&message);
		assert( info != NULL );
		if (info->Contains(1))
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JOrderedSetT::kElementsRemoved))
		{
		const JOrderedSetT::ElementsRemoved* info =
			dynamic_cast<const JOrderedSetT::ElementsRemoved*>(&message);
		assert( info != NULL );
		if (info->Contains(1))
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JOrderedSetT::kElementMoved))
		{
		const JOrderedSetT::ElementMoved* info =
			dynamic_cast<const JOrderedSetT::ElementMoved*>(&message);
		assert( info != NULL );
		if (info->GetOrigIndex() == 1 || info->GetNewIndex() == 1)
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JOrderedSetT::kElementsSwapped))
		{
		const JOrderedSetT::ElementsSwapped* info =
			dynamic_cast<const JOrderedSetT::ElementsSwapped*>(&message);
		assert( info != NULL );
		if (info->GetIndex1() == 1 || info->GetIndex2() == 1)
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JOrderedSetT::kElementChanged))
		{
		const JOrderedSetT::ElementChanged* info =
			dynamic_cast<const JOrderedSetT::ElementChanged*>(&message);
		assert( info != NULL );
		if (info->Contains(1))
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JOrderedSetT::kSorted))
		{
		assert( 0 /* not allowed */ );
		}

	else if (sender == CBGetPrefsManager() &&
			 message.Is(CBPrefsManager::kFileTypesChanged))
		{
		const JSize count = itsTextDocuments->GetElementCount();
		if (count > 0)
			{
			JLatentPG pg;
			pg.FixedLengthProcessBeginning(count, "Updating open files...",
										   kJFalse, kJFalse);

			for (JIndex i=1; i<=count; i++)
				{
				(itsTextDocuments->GetElement(i))->UpdateFileType();
				pg.IncrementProgress();
				}

			pg.ProcessFinished();
			}
		}

	else if (sender == itsExtEditorDialog &&
		message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsExtEditorDialog->GetPrefs(&itsEditTextLocalFlag, &itsEditTextFileCmd,
										 &itsEditTextFileLineCmd,
										 &itsEditBinaryLocalFlag, &itsEditBinaryFileCmd);
			}
		itsExtEditorDialog = NULL;
		}

	else
		{
		JXDocumentManager::Receive(sender, message);
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

static const JCharacter* kVarNameList = "fl";

inline void
CBTrimBkgdFlag
	(
	JString* str
	)
{
	str->TrimWhitespace();
	if (str->EndsWith("&"))
		{
		const JSize length = str->GetLength();
		str->RemoveSubstring(length, length);
		str->TrimWhitespace();
		}
}

void
CBDocumentManager::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	input >> itsEditTextLocalFlag >> itsEditTextFileCmd >> itsEditTextFileLineCmd;

	CBTrimBkgdFlag(&itsEditTextFileCmd);
	CBTrimBkgdFlag(&itsEditTextFileLineCmd);

	if (vers < 3)
		{
		JXWebBrowser::ConvertVarNames(&itsEditTextFileCmd,     kVarNameList);
		JXWebBrowser::ConvertVarNames(&itsEditTextFileLineCmd, kVarNameList);
		}

	if (vers >= 5)
		{
		input >> itsEditBinaryLocalFlag >> itsEditBinaryFileCmd;
		}

	if (vers >= 1)
		{
		input >> itsWarnBeforeSaveAllFlag >> itsWarnBeforeCloseAllFlag;
		}

	if (4 <= vers && vers < 6)
		{
		JBoolean updateSymbolDBAfterBuild;
		input >> updateSymbolDBAfterBuild;
		}

	if (vers >= 2)
		{
		itsRecentProjectMenu->ReadSetup(input);
		itsRecentTextMenu->ReadSetup(input);
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBDocumentManager::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ' << itsEditTextLocalFlag;
	output << ' ' << itsEditTextFileCmd;
	output << ' ' << itsEditTextFileLineCmd;

	output << ' ' << itsEditBinaryLocalFlag;
	output << ' ' << itsEditBinaryFileCmd;

	output << ' ' << itsWarnBeforeSaveAllFlag;
	output << ' ' << itsWarnBeforeCloseAllFlag;

	output << ' ';
	itsRecentProjectMenu->WriteSetup(output);

	output << ' ';
	itsRecentTextMenu->WriteSetup(output);
}

/******************************************************************************
 ChooseEditors

 ******************************************************************************/

void
CBDocumentManager::ChooseEditors()
{
	assert( itsExtEditorDialog == NULL );

	itsExtEditorDialog =
		jnew CBExtEditorDialog(CBGetApplication(), itsEditTextLocalFlag,
							  itsEditTextFileCmd, itsEditTextFileLineCmd,
							  itsEditBinaryLocalFlag, itsEditBinaryFileCmd);
	assert( itsExtEditorDialog != NULL );
	itsExtEditorDialog->BeginDialog();
	ListenTo(itsExtEditorDialog);
}
