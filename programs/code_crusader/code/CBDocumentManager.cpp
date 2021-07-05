/******************************************************************************
 CBDocumentManager.cpp

	BASE CLASS = JXDocumentManager, JPrefObject

	Copyright © 1997-99 by John Lindal.

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
#include <JRegex.h>
#include <JSubstitute.h>
#include <JLatentPG.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <JDirInfo.h>
#include <JSimpleProcess.h>
#include <JStringIterator.h>
#include <jAssert.h>

static const JUtf8Byte* kTextTemplateDir = "text_templates";

// External editors

static const JString kDefEditTextFileCmd("emacsclient $f", kJFalse);
static const JString kDefEditTextFileLineCmd("emacsclient +$l $f", kJFalse);

#ifdef _J_OSX
static const JString kDefEditBinaryFileCmd("open -a \"Hex Fiend\" $f", kJFalse);
#else
static const JString kDefEditBinaryFileCmd("khexedit $f", kJFalse); // "xterm -title \"$f\" -e vi $f";
#endif

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

const JUtf8Byte* CBDocumentManager::kProjectDocumentCreated =
	"ProjectDocumentCreated::CBDocumentManager";

const JUtf8Byte* CBDocumentManager::kProjectDocumentDeleted =
	"ProjectDocumentDeleted::CBDocumentManager";

const JUtf8Byte* CBDocumentManager::kProjectDocumentActivated =
	"ProjectDocumentActivated::CBDocumentManager";

const JUtf8Byte* CBDocumentManager::kAddFileToHistory =
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
	assert( itsProjectDocuments != nullptr );
	ListenTo(itsProjectDocuments);

	itsTextDocuments = jnew JPtrArray<CBTextDocument>(JPtrArrayT::kForgetAll);
	assert( itsTextDocuments != nullptr );

	itsListDocument = nullptr;

	itsWarnBeforeSaveAllFlag  = kJFalse;
	itsWarnBeforeCloseAllFlag = kJFalse;

	itsTextNeedsSaveFlag   = kJFalse;
	itsEditTextLocalFlag   = kJTrue;
	itsEditBinaryLocalFlag = kJFalse;

	itsRecentProjectMenu = nullptr;
	itsRecentTextMenu    = nullptr;

	itsExtEditorDialog = nullptr;

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
	assert( itsRecentProjectMenu == nullptr &&
			itsRecentTextMenu    == nullptr );

	itsRecentProjectMenu =
		jnew CBFileHistoryMenu(kProjectFileHistory, JString::empty, window,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  -20,0, 10,10);
	assert( itsRecentProjectMenu != nullptr );
	itsRecentProjectMenu->Hide();

	itsRecentTextMenu =
		jnew CBFileHistoryMenu(kTextFileHistory, JString::empty, window,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  -20,0, 10,10);
	assert( itsRecentTextMenu != nullptr );
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
	const JString&			fullName
	)
{
	Broadcast(AddFileToHistory(type, fullName));
}

/******************************************************************************
 RefreshVCSStatus

 ******************************************************************************/

void
CBDocumentManager::RefreshVCSStatus()
{
	JSize count = itsProjectDocuments->GetElementCount();
	JIndex i;
	for (i=1; i<=count; i++)
		{
		(itsProjectDocuments->GetElement(i))->RefreshVCSStatus();
		}

	count = itsTextDocuments->GetElementCount();
	for (i=1; i<=count; i++)
		{
		(itsTextDocuments->GetElement(i))->RefreshVCSStatus();
		}
}

/******************************************************************************
 NewProjectDocument

	If doc != nullptr, *doc is the new document, if one is successfully created.

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
		if (doc != nullptr)
			{
			*doc = d;
			}
		return kJTrue;
		}
	else
		{
		if (doc != nullptr)
			{
			*doc = nullptr;
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
		*doc = itsProjectDocuments->GetFirstElement();
		return kJTrue;
		}
	else
		{
		*doc = nullptr;
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
	const JString&		fileName,
	CBProjectDocument**	doc
	)
	const
{
	*doc = nullptr;

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

	*doc = nullptr;
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
		assert( doc != nullptr );
		doc->Activate();
		}
	else if ((JXGetChooseSaveFile())->SaveFile(
				JGetString("NewFilePrompt::CBDocumentManager"),
				JString::empty, JString::empty, &newName))
		{
		std::ofstream output(newName.GetBytes());
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
		const JUtf8Byte* map[] =
		{
			"name", kTextTemplateDir
		};
		const JString msg = JGetString("NoTextTemplates::CBDocumentManager", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return;
		}

	tmplDir += ACE_DIRECTORY_SEPARATOR_STR "*";		// make ChooseFile() happy

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	JString tmplName;
	if (csf->ChooseFile(JString::empty, JString::empty, tmplDir, &tmplName))
		{
		JString newName;
		if (itsEditTextLocalFlag)
			{
			CBTextDocument* doc = jnew CBTextDocument(tmplName, kCBUnknownFT, kJTrue);
			assert( doc != nullptr );
			doc->Activate();
			}
		else if (csf->SaveFile(JGetString("NewFilePrompt::CBDocumentManager"),
							   JString::empty, JString::empty, &newName))
			{
			JString tmplText;
			JReadFile(tmplName, &tmplText);
			std::ofstream output(newName.GetBytes());
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
	const JUtf8Byte*	dirName,
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
			JGetUserNotification()->ReportError(JGetString("NoHomeDirectory::CBDocumentManager"));
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
			const JUtf8Byte* map[] =
			{
				"name", userDir.GetBytes(),
				"err",  err.GetMessage().GetBytes()
			};
			const JString msg = JGetString("CannotCreateTemplates::CBDocumentManager", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
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
			JXGetSearchTextDialog()->SetActiveTE(
				itsTextDocuments->GetFirstElement()->GetTextEditor());
			}
		}

	if (doc == itsListDocument)
		{
		itsListDocument = nullptr;
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
		*doc = itsTextDocuments->GetFirstElement();
		return kJTrue;
		}
	else
		{
		*doc = nullptr;
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
		JXGetSearchTextDialog()->SetActiveTE(doc->GetTextEditor());
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
	return JI2B( itsListDocument != nullptr );
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

	If fileName is nullptr or empty, we ask the user to choose a file.
	If lineIndex is not zero and we open a CBTextDocument, we go to that line.

 ******************************************************************************/

void
CBDocumentManager::OpenSomething
	(
	const JString&		fileName,
	const JIndexRange	lineRange,		// not reference because of default value
	const JBoolean		iconify,
	const JBoolean		forceReload
	)
{
	if (!fileName.IsEmpty())
		{
		if (JFileReadable(fileName))
			{
			PrivateOpenSomething(fileName, lineRange, iconify, forceReload);
			}
		else if (!JFileExists(fileName))
			{
			const JUtf8Byte* map[] =
			{
				"f", fileName.GetBytes()
			};
			const JString msg = JGetString("FileDoesNotExist::CBDocumentManager", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			}
		else
			{
			const JUtf8Byte* map[] =
			{
				"f", fileName.GetBytes()
			};
			const JString msg = JGetString("CannotReadFile::CBDocumentManager", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			}
		}
	else
		{
		JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
		if (JGetChooseSaveFile()->ChooseFiles(
					JGetString("OpenFilesPrompt::CBDocumentManager"),
					JString::empty, &fullNameList))
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
		pg.FixedLengthProcessBeginning(count, JGetString("OpeningFilesProgress::CBDocumentManager"), kJTrue, kJFalse);

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
	const JString&		fileName,
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
			PrivateOpenTextDocument(fileName, lineRange, iconify, forceReload, nullptr);
			}
		}
	else if (doc != nullptr && iconify)
		{
		(doc->GetWindow())->Iconify();
		}
}

/******************************************************************************
 OpenTextDocument

	Open the given file, assuming that it is a text file.
	If lineIndex is not zero, we go to that line.

	Returns kJTrue if the file was successfully opened.  If doc != nullptr,
	*doc is set to the document.

 ******************************************************************************/

JBoolean
CBDocumentManager::OpenTextDocument
	(
	const JString&		fileName,
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
	const JString&		fileName,
	const JIndexRange&	lineRange,
	CBTextDocument**	doc,
	const JBoolean		iconify,
	const JBoolean		forceReload
	)
{
	if (doc != nullptr)
		{
		*doc = nullptr;
		}

	if (fileName.IsEmpty())
		{
		return kJFalse;
		}

	const JUtf8Byte* map[] =
		{
		"f", fileName.GetBytes()
		};

	const JBoolean isFile = JFileExists(fileName);
	if (!isFile && JNameUsed(fileName))
		{
		const JString msg = JGetString("NotAFile::CBDocumentManager", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return kJFalse;
		}
	else if (!isFile)
		{
		const JString msg = JGetString("FileDoesNotExistCreate::CBDocumentManager", map, sizeof(map));
		if (!JGetUserNotification()->AskUserYes(msg))
			{
			return kJFalse;
			}
		else
			{
			std::ofstream temp(fileName.GetBytes());
			if (!temp.good())
				{
				const JString msg = JGetString("CannotCreateFile::CBDocumentManager", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
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
	If doc != nullptr, *doc is set to the document.

 ******************************************************************************/

JBoolean
CBDocumentManager::PrivateOpenTextDocument
	(
	const JString&		fullName,
	const JIndexRange&	lineRange,
	const JBoolean		iconify,
	const JBoolean		forceReload,
	CBTextDocument**	returnDoc
	)
	const
{
	assert( !fullName.IsEmpty() );

	if (returnDoc != nullptr)
		{
		*returnDoc = nullptr;
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
		assert( doc != nullptr );
		if (iconify && !doc->IsActive())
			{
			(doc->GetWindow())->Iconify();
			}
		if (!iconify || !doc->IsActive())
			{
			doc->Activate();
			}

		CBTextDocument* textDoc = dynamic_cast<CBTextDocument*>(doc);
		if (textDoc != nullptr)
			{
			textDoc->RevertIfChangedByOthers(forceReload);
			if (!lineRange.IsEmpty())
				{
				textDoc->SelectLines(lineRange);
				}
			}

		if (returnDoc != nullptr)
			{
			*returnDoc = textDoc;
			}
		return JI2B( textDoc != nullptr );
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
	const JString& fullName
	)
{
	JSize size;
	if (JGetFileLength(fullName, &size) == kJNoError &&
		size > kMinWarnFileSize)
		{
		JString path, fileName;
		JSplitPathAndName(fullName, &path, &fileName);

		const JUtf8Byte* map[] =
			{
			"f", fileName.GetBytes()
			};
		const JString msg = JGetString("WarnFileTooLarge::CBDocumentManager", map, sizeof(map));
		if (!JGetUserNotification()->AskUserNo(msg))
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
		JGetUserNotification()->AskUserNo(JGetString("WarnSaveAll::CBDocumentManager")))
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
				assert( execDoc != nullptr );
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
		JGetUserNotification()->AskUserNo(JGetString("WarnCloseAll::CBDocumentManager")))
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
				assert( execDoc != nullptr );
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
	const JString& origFullName,
	const JString& newFullName
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
	JSTStyler* styler
	)
{
	const JSize count = itsTextDocuments->GetElementCount();

	JLatentPG pg;
	pg.FixedLengthProcessBeginning(count, JGetString("AdjustStylesProgress::CBDocumentManager"), kJFalse, kJFalse);

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
	const JString& fileName
	)
{
	if (!fileName.IsEmpty() && JFileExists(fileName))
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
	const JString&	fullName,
	const JBoolean	iconify,
	const JBoolean	forceReload
	)
	const
{
	assert( !fullName.IsEmpty() );

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

		const JUtf8Byte* map[] =
		{
			"name", fileName.GetBytes()
		};
		const JString msg = JGetString("ComplFileNotFound::CBDocumentManager", map, sizeof(map));

		CBPrefsManager* prefsMgr = CBGetPrefsManager();
		JIndex suffixIndex;
		const JString origName =
			prefsMgr->GetDefaultComplementSuffix(fullName, type, &suffixIndex);

		JString newFullName;
		if (JGetChooseSaveFile()->
				SaveFile(JGetString("SaveNewFilePrompt::CBDocumentManager"),
						 msg, origName, &newFullName))
			{
			prefsMgr->SetDefaultComplementSuffix(suffixIndex, newFullName);

			std::ofstream temp(newFullName.GetBytes());
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

	JString baseName, suffix;
	JSplitRootAndSuffix(inputName, &baseName, &suffix);

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

	If projDoc == nullptr, the active project document is used.

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

// private -- projDoc can be nullptr

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
	JString baseName, suffix;
	JSplitRootAndSuffix(inputName, &baseName, &suffix);

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
		(projDoc != nullptr || GetActiveProjectDocument(&projDoc)))
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

static JListT::CompareResult
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
	dirList.SetSortOrder(JListT::kSortDescending);
	dirList.SetCompareFunction(CompareMatchLengths);

	DirMatchInfo info;
	for (i=1; i<=dirCount; i++)
		{
		info.path = jnew JString;
		assert( info.path != nullptr );

		if (origDirList.GetTruePath(i, info.path, &(info.recurse)))
			{
			info.matchLength = JString::CalcCharacterMatchLength(origPath, *(info.path));
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

	const JUtf8Byte* map[] =
	{
		"name", origFileName.GetBytes()
	};
	const JString msg = JGetString("ComplFileProgress::CBDocumentManager", map, sizeof(map));
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

JListT::CompareResult
CompareMatchLengths
	(
	const DirMatchInfo& i1,
	const DirMatchInfo& i2
	)
{
	if (i1.matchLength < i2.matchLength)
		{
		return JListT::kFirstLessSecond;
		}
	else if (i1.matchLength == i2.matchLength)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
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
		pg.FixedLengthProcessBeginning(textCount, JGetString("OpeningFiles::CBMDIServer"), kJTrue, kJFalse);

		for (JIndex i=1; i<=textCount; i++)
			{
			JBoolean keep;
			CBTextDocument* doc = jnew CBTextDocument(input, vers, &keep);
			assert( doc != nullptr );
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
		input >> JBoolFromString(projectWasOpen);
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
				input >> JBoolFromString(onDisk);
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
			output << ' ' << JBoolToString(onDisk) << ' ' << fullName;
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
	if (sender == itsProjectDocuments && message.Is(JListT::kElementsInserted))
		{
		const JListT::ElementsInserted* info =
			dynamic_cast<const JListT::ElementsInserted*>(&message);
		assert( info != nullptr );
		if (info->Contains(1))
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JListT::kElementsRemoved))
		{
		const JListT::ElementsRemoved* info =
			dynamic_cast<const JListT::ElementsRemoved*>(&message);
		assert( info != nullptr );
		if (info->Contains(1))
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JListT::kElementMoved))
		{
		const JListT::ElementMoved* info =
			dynamic_cast<const JListT::ElementMoved*>(&message);
		assert( info != nullptr );
		if (info->GetOrigIndex() == 1 || info->GetNewIndex() == 1)
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JListT::kElementsSwapped))
		{
		const JListT::ElementsSwapped* info =
			dynamic_cast<const JListT::ElementsSwapped*>(&message);
		assert( info != nullptr );
		if (info->GetIndex1() == 1 || info->GetIndex2() == 1)
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JListT::kElementChanged))
		{
		const JListT::ElementChanged* info =
			dynamic_cast<const JListT::ElementChanged*>(&message);
		assert( info != nullptr );
		if (info->Contains(1))
			{
			Broadcast(ProjectDocumentActivated(itsProjectDocuments));
			}
		}
	else if (sender == itsProjectDocuments && message.Is(JListT::kSorted))
		{
		assert_msg( 0, "not allowed" );
		}

	else if (sender == CBGetPrefsManager() &&
			 message.Is(CBPrefsManager::kFileTypesChanged))
		{
		const JSize count = itsTextDocuments->GetElementCount();
		if (count > 0)
			{
			JLatentPG pg;
			pg.FixedLengthProcessBeginning(count, JGetString("UpdateTypesProgress::CBDocumentManager"),
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
		assert( info != nullptr );
		if (info->Successful())
			{
			itsExtEditorDialog->GetPrefs(&itsEditTextLocalFlag, &itsEditTextFileCmd,
										 &itsEditTextFileLineCmd,
										 &itsEditBinaryLocalFlag, &itsEditBinaryFileCmd);
			}
		itsExtEditorDialog = nullptr;
		}

	else
		{
		JXDocumentManager::Receive(sender, message);
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

static const JUtf8Byte* kVarNameList = "fl";

inline void
cbTrimBkgdFlag
	(
	JString* str
	)
{
	str->TrimWhitespace();
	if (str->EndsWith("&"))
		{
		JStringIterator iter(str, kJIteratorStartAtEnd);
		iter.RemovePrev();

		str->TrimWhitespace();	// invalidates iter
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

	input >> JBoolFromString(itsEditTextLocalFlag);
	input >> itsEditTextFileCmd;
	input >> itsEditTextFileLineCmd;

	cbTrimBkgdFlag(&itsEditTextFileCmd);
	cbTrimBkgdFlag(&itsEditTextFileLineCmd);

	if (vers < 3)
		{
		JXWebBrowser::ConvertVarNames(&itsEditTextFileCmd,     kVarNameList);
		JXWebBrowser::ConvertVarNames(&itsEditTextFileLineCmd, kVarNameList);
		}

	if (vers >= 5)
		{
		input >> JBoolFromString(itsEditBinaryLocalFlag);
		input >> itsEditBinaryFileCmd;
		}

	if (vers >= 1)
		{
		input >> JBoolFromString(itsWarnBeforeSaveAllFlag)
			  >> JBoolFromString(itsWarnBeforeCloseAllFlag);
		}

	if (4 <= vers && vers < 6)
		{
		JBoolean updateSymbolDBAfterBuild;
		input >> JBoolFromString(updateSymbolDBAfterBuild);
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

	output << ' ' << JBoolToString(itsEditTextLocalFlag);
	output << ' ' << itsEditTextFileCmd;
	output << ' ' << itsEditTextFileLineCmd;

	output << ' ' << JBoolToString(itsEditBinaryLocalFlag);
	output << ' ' << itsEditBinaryFileCmd;

	output << ' ' << JBoolToString(itsWarnBeforeSaveAllFlag)
				  << JBoolToString(itsWarnBeforeCloseAllFlag);

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
	assert( itsExtEditorDialog == nullptr );

	itsExtEditorDialog =
		jnew CBExtEditorDialog(CBGetApplication(), itsEditTextLocalFlag,
							  itsEditTextFileCmd, itsEditTextFileLineCmd,
							  itsEditBinaryLocalFlag, itsEditBinaryFileCmd);
	assert( itsExtEditorDialog != nullptr );
	itsExtEditorDialog->BeginDialog();
	ListenTo(itsExtEditorDialog);
}
