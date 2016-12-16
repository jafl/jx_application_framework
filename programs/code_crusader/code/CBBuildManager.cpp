/******************************************************************************
 CBBuildManager.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1997-2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBBuildManager.h"
#include "CBProjectDocument.h"
#include "CBProjectTree.h"
#include "CBProjectNode.h"
#include "CBCommandManager.h"
#include "CBProjectConfigDialog.h"
#include "CBCommand.h"
#include "cbGlobals.h"
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jVCSUtil.h>
#include <sstream>
#include <jAssert.h>

JBoolean CBBuildManager::itsRebuildMakefileDailyFlag = kJTrue;

static const JCharacter* kMakeHeaderName = "Make.header";
static const JCharacter* kMakeFilesName  = "Make.files";

static const JCharacter* kCMakeHeaderName = "CMake.header";
static const JCharacter* kCMakeInputName  = "CMakeLists.txt";

static const JCharacter* kQMakeHeaderName      = "QMake.header";
static const JCharacter* kQMakeInputFileSuffix = ".pro";

static const JCharacter* kDefaultSubProjectBuildCmd = "make -k all";
static const JCharacter* kSubProjectBuildSuffix     = ".jmk";

static const JCharacter* kMakefileName[] =
{
	"m3makefile", "Makefile", "makefile"
};

const JIndex kFirstDefaultMakefileIndex = 2;	// first one that makemake can overwrite
const JSize kMakefileNameCount          = sizeof(kMakefileName) / sizeof(JCharacter*);

const time_t kUpdateMakefileInterval = 24*3600;	// 1 day (seconds)

// string ID's

static const JCharacter* kNoMakeFileID = "NoMakeFile::CBBuildManager";

static const JCharacter* kSubProjectBuildInitTextID  = "SubProjectBuildInitText::CBBuildManager";

static const JCharacter* kMakeHeaderInitTextID         = "MakeHeaderInitText::CBBuildManager";
static const JCharacter* kMakeFilesInitTextID          = "MakeFilesInitText::CBBuildManager";
static const JCharacter* kDefaultMakemakeDependCmdID   = "DefaultMakemakeDependCmd::CBBuildManager";
static const JCharacter* kMakeHeaderMarkerID           = "MakeHeaderMarker::CBBuildManager";
static const JCharacter* kMakeHeaderLibTargetWarningID = "MakeHeaderLibTargetWarning::CBBuildManager";
static const JCharacter* kMakeHeaderLibTargetID        = "MakeHeaderLibTarget::CBBuildManager";

static const JCharacter* kCMakeHeaderInitTextID   = "CMakeHeaderInitText::CBBuildManager";
static const JCharacter* kDefaultCMakeDependCmdID = "DefaultCMakeDependCmd::CBBuildManager";
static const JCharacter* kCMakeInsertMarkerID     = "CMakeInsertMarker::CBBuildManager";
static const JCharacter* kCMakeDataID             = "CMakeData::CBBuildManager";

static const JCharacter* kQMakeHeaderInitTextID   = "QMakeHeaderInitText::CBBuildManager";
static const JCharacter* kDefaultQMakeDependCmdID = "DefaultQMakeDependCmd::CBBuildManager";
static const JCharacter* kQMakeInsertMarkerID     = "QMakeInsertMarker::CBBuildManager";
static const JCharacter* kQMakeDataID             = "QMakeData::CBBuildManager";

static const JCharacter* kMissingBuildTargetID = "MissingBuildTarget::CBBuildManager";
static const JCharacter* kMissingSourceFilesID = "MissingSourceFiles::CBBuildManager";
static const JCharacter* kNoSourceFilesID      = "NoSourceFiles::CBBuildManager";
static const JCharacter* kMakeFilesWarningID   = "MakeFilesWarning::CBBuildManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBBuildManager::CBBuildManager
	(
	CBProjectDocument*		doc,
	const MakefileMethod	method,
	const JBoolean			needWriteMakeFiles,
	const JCharacter*		targetName,
	const JCharacter*		depListExpr
	)
	:
	itsMakefileMethod(method),
	itsNeedWriteMakeFilesFlag(needWriteMakeFiles),
	itsTargetName(targetName),
	itsDepListExpr(depListExpr),
	itsSubProjectBuildCmd(kDefaultSubProjectBuildCmd),
	itsLastMakefileUpdateTime(0),
	itsProjDoc(doc),
	itsMakeDependCmd(NULL),
	itsProjectConfigDialog(NULL)
{
	if (itsTargetName.IsEmpty())
		{
		JString suffix;
		JSplitRootAndSuffix(itsProjDoc->GetFileName(), &itsTargetName, &suffix);
		}
}

CBBuildManager::CBBuildManager
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	CBProjectDocument*	doc
	)
	:
	itsMakefileMethod(kManual),
	itsNeedWriteMakeFilesFlag(kJTrue),
	itsSubProjectBuildCmd(kDefaultSubProjectBuildCmd),
	itsLastMakefileUpdateTime(0),
	itsProjDoc(doc),
	itsMakeDependCmd(NULL),
	itsProjectConfigDialog(NULL)
{
	ReadSetup(projInput, projVers, setInput, setVers, doc->GetFilePath());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBBuildManager::~CBBuildManager()
{
}

/******************************************************************************
 UpdateMakefile

	Returns kJTrue if Makefile should be updated.  If this process has been
	successfully started, returns *cmd != NULL.

 ******************************************************************************/

JBoolean
CBBuildManager::UpdateMakefile
	(
	CBExecOutputDocument*	compileDoc,
	CBCommand**				cmd,
	const JBoolean			force
	)
{
	if (itsMakeDependCmd != NULL)
		{
		if (cmd != NULL)
			{
			*cmd = itsMakeDependCmd;
			}
		return kJTrue;
		}

	if (!UpdateMakeFiles())
		{
		if (cmd != NULL)
			{
			*cmd = NULL;
			}
		return kJTrue;
		}

	const time_t now = time(NULL);
	if (itsMakefileMethod != kManual &&
		(force || MakeFilesChanged() ||
		 (itsRebuildMakefileDailyFlag &&
		  now > itsLastMakefileUpdateTime + kUpdateMakefileInterval)))
		{
		if ((itsProjDoc->GetCommandManager())->MakeDepend(itsProjDoc, compileDoc,
														  &itsMakeDependCmd))
			{
			itsNewModTime             = SaveMakeFileModTimes();
			itsLastMakefileUpdateTime = now;
			ListenTo(itsMakeDependCmd);
			}

		if (cmd != NULL)
			{
			*cmd = itsMakeDependCmd;
			}
		return kJTrue;
		}
	else
		{
		if (cmd != NULL)
			{
			*cmd = NULL;
			}
		return kJFalse;
		}
}

/******************************************************************************
 UpdateMakeFiles

 ******************************************************************************/

JBoolean
CBBuildManager::UpdateMakeFiles
	(
	const JBoolean reportError
	)
{
	JString makeHeaderName, makeFilesName;
	GetMakemakeFileNames(&makeHeaderName, &makeFilesName);

	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);

	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);

	if ((itsMakefileMethod == kMakemake && !SaveOpenFile(makeHeaderName))  ||
		(itsMakefileMethod == kCMake    && !SaveOpenFile(cmakeHeaderName)) ||
		(itsMakefileMethod == kQMake    && !SaveOpenFile(qmakeHeaderName)) ||
		(itsMakefileMethod == kManual &&
		 (!SaveOpenFile(makeHeaderName) ||
		  !SaveOpenFile(makeFilesName)  ||
		  !SaveOpenFile(cmakeInputName) ||
		  !SaveOpenFile(qmakeInputName))))
		{
		return kJFalse;
		}

	if (itsMakefileMethod == kManual)	// after saving files related to make
		{
		return kJTrue;
		}

	RecreateMakeHeaderFile();

	if (!WriteSubProjectBuildFile(reportError))
		{
		return kJFalse;
		}

	if (itsProjDoc->GetFilePath() != itsMakePath ||				// jnew directory
		(itsMakefileMethod == kCMake && CMakeHeaderChanged()) ||// update CMakeLists.txt
		(itsMakefileMethod == kQMake && QMakeHeaderChanged()) ||// update .pro file
		itsMakefileMethod == kMakemake)							// always call UpdateMakeHeader()
		{
		itsNeedWriteMakeFilesFlag = kJTrue;
		}

	if (!itsNeedWriteMakeFilesFlag &&
		((itsMakefileMethod == kMakemake && JFileExists(makeFilesName))  ||
		 (itsMakefileMethod == kCMake    && JFileExists(cmakeInputName)) ||
		 (itsMakefileMethod == kQMake    && JFileExists(qmakeInputName))))
		{
		return kJTrue;
		}
	else if (itsTargetName.IsEmpty())
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(JGetString(kMissingBuildTargetID));
			EditProjectConfig();
			}
		return kJFalse;
		}

	CBProjectTree* fileTree = itsProjDoc->GetFileTree();

	JString text, src, hdr;
	JPtrArray<JTreeNode> invalidList(JPtrArrayT::kForgetAll);
	JPtrArray<JString> libFileList(JPtrArrayT::kDeleteAll),
					   libProjPathList(JPtrArrayT::kDeleteAll);
	if (itsMakefileMethod == kMakemake &&
		fileTree->BuildMakeFiles(&text, &invalidList, &libFileList, &libProjPathList) &&
		!text.IsEmpty())
		{
		UpdateMakeHeader(makeHeaderName, libFileList, libProjPathList);
		UpdateMakeFiles(makeFilesName, text);
		itsNeedWriteMakeFilesFlag = kJFalse;
		return kJTrue;
		}
	else if (itsMakefileMethod == kCMake &&
			 fileTree->BuildCMakeData(&src, &hdr, &invalidList) &&
			 !src.IsEmpty())
		{
		WriteCMakeInput(cmakeHeaderName, src, hdr, cmakeInputName);
		itsNeedWriteMakeFilesFlag = kJFalse;
		return kJTrue;
		}
	else if (itsMakefileMethod == kQMake &&
			 fileTree->BuildQMakeData(&src, &hdr, &invalidList) &&
			 !src.IsEmpty())
		{
		WriteQMakeInput(qmakeHeaderName, src, hdr, qmakeInputName);
		itsNeedWriteMakeFilesFlag = kJFalse;
		return kJTrue;
		}
	else if (!invalidList.IsEmpty())
		{
		if (reportError)
			{
			CBProjectTable*	fileTable = itsProjDoc->GetFileTable();
			fileTable->ClearSelection();
			fileTable->SelectFileNodes(invalidList);
			(JGetUserNotification())->ReportError(JGetString(kMissingSourceFilesID));
			itsProjDoc->Activate();
			}
		return kJFalse;
		}
	else
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(JGetString(kNoSourceFilesID));
			itsProjDoc->Activate();
			}
		return kJFalse;
		}
}

/******************************************************************************
 UpdateMakeHeader (private)

 ******************************************************************************/

void
CBBuildManager::UpdateMakeHeader
	(
	const JCharacter*			fileName,
	const JPtrArray<JString>&	libFileList,
	const JPtrArray<JString>&	libProjPathList
	)
	const
{
	// read original Make.header data

	JString text;
	JReadFile(fileName, &text);

	// build new Make.header data

	std::ostringstream output;

	const JString& marker = JGetString(kMakeHeaderMarkerID);
	JIndex i;
	const JBoolean foundMarker = text.LocateSubstring(marker, &i);
	if (foundMarker)
		{
		output.write(text, i-1);
		}
	else
		{
		text.TrimWhitespace();
		text.Print(output);
		output << "\n\n";
		}

	marker.Print(output);
	output << "\n\n";

	(JGetString(kMakeHeaderLibTargetWarningID)).Print(output);

	const JSize libCount = libFileList.GetElementCount();
	JString libPath, libName;
	for (JIndex i=1; i<=libCount; i++)
		{
		const JCharacter* map[] =
			{
			"lib_full_name", (libFileList.NthElement(i))->GetCString(),
			"lib_proj_path", (libProjPathList.NthElement(i))->GetCString(),
			};
		const JString target = JGetString(kMakeHeaderLibTargetID, map, sizeof(map));
		target.Print(output);
		}

	output << '\n';
	marker.Print(output);

	i++;
	if (foundMarker && text.LocateNextSubstring(marker, &i))
		{
		const JSize offset = i-1 + marker.GetLength();
		output.write(text.GetCString() + offset, text.GetLength() - offset);
		}
	else
		{
		output << '\n';
		}

	// if new Make.header data is different, update file

	const std::string s = output.str();
	if (s.c_str() != text)
		{
		JEditVCS(fileName);
		std::ofstream f(fileName);
		f << s.c_str();
		f.close();

		// force reload of Make.header, if open

		JXFileDocument* doc;
		if ((JXGetDocumentManager())->FileDocumentIsOpen(fileName, &doc))
			{
			doc->RevertIfChangedByOthers(kJTrue);
			}
		}
}

/******************************************************************************
 UpdateMakeFiles (private)

 ******************************************************************************/

void
CBBuildManager::UpdateMakeFiles
	(
	const JCharacter* fileName,
	const JCharacter* text
	)
	const
{
	std::ostringstream data;
	(JGetString(kMakeFilesWarningID)).Print(data);
	PrintTargetName(data);
	data << '\n';
	data << text;
	if (!itsDepListExpr.IsEmpty())
		{
		data << "\nliteral: ";
		itsDepListExpr.Print(data);
		data << '\n';
		}

	const std::string s = data.str();

	JString origText;
	JReadFile(fileName, &origText);
	if (s.c_str() != origText)
		{
		JEditVCS(fileName);
		std::ofstream output(fileName);
		output << s.c_str();
		}
}

/******************************************************************************
 WriteCMakeInput (private)

 ******************************************************************************/

void
CBBuildManager::WriteCMakeInput
	(
	const JCharacter* inputFileName,
	const JCharacter* src,
	const JCharacter* hdr,
	const JCharacter* outputFileName
	)
	const
{
	const JCharacter* map[] =
		{
		"t", itsTargetName.GetCString(),
		"s", src,
		"h", hdr
		};
	const JString cmakeData = JGetString(kCMakeDataID, map, sizeof(map));

	JString cmakeHeader;
	JReadFile(inputFileName, &cmakeHeader);
	const JCharacter* marker = JGetString(kCMakeInsertMarkerID);
	JIndex i;
	if (cmakeHeader.LocateSubstring(marker, &i))
		{
		cmakeHeader.ReplaceSubstring(i, i+strlen(marker)-1, cmakeData);
		}
	else
		{
		cmakeHeader += cmakeData;
		}

	JEditVCS(outputFileName);
	std::ofstream output(outputFileName);
	(JGetString(kMakeFilesWarningID)).Print(output);
	cmakeHeader.Print(output);
}

/******************************************************************************
 WriteQMakeInput (private)

 ******************************************************************************/

void
CBBuildManager::WriteQMakeInput
	(
	const JCharacter* inputFileName,
	const JCharacter* src,
	const JCharacter* hdr,
	const JCharacter* outputFileName
	)
	const
{
	const JCharacter* map[] =
		{
		"t", itsTargetName.GetCString(),
		"s", src,
		"h", hdr
		};
	const JString qmakeData = JGetString(kQMakeDataID, map, sizeof(map));

	JString qmakeHeader;
	JReadFile(inputFileName, &qmakeHeader);
	const JCharacter* marker = JGetString(kQMakeInsertMarkerID);
	JIndex i;
	if (qmakeHeader.LocateSubstring(marker, &i))
		{
		qmakeHeader.ReplaceSubstring(i, i+strlen(marker)-1, qmakeData);
		}
	else
		{
		qmakeHeader += qmakeData;
		}

	JEditVCS(outputFileName);
	std::ofstream output(outputFileName);
	(JGetString(kMakeFilesWarningID)).Print(output);
	qmakeHeader.Print(output);
}

/******************************************************************************
 PrintTargetName (private)

	Parses itsTargetName as comma separated list and prints each item
	for use in Make.files.

 ******************************************************************************/

void
CBBuildManager::PrintTargetName
	(
	std::ostream& output
	)
	const
{
	JString s = itsTargetName, target;
	JIndex commaIndex;
	while (s.LocateSubstring(",", &commaIndex))
		{
		target = s.GetSubstring(1, commaIndex-1);
		s.RemoveSubstring(1, commaIndex);

		target.TrimWhitespace();
		output << '@';
		target.Print(output);
		output << '\n';
		}

	s.TrimWhitespace();
	output << '@';
	s.Print(output);
	output << '\n';
}

/******************************************************************************
 SaveOpenFile (private)

 ******************************************************************************/

JBoolean
CBBuildManager::SaveOpenFile
	(
	const JCharacter* fileName
	)
{
	JXFileDocument* doc;
	return JI2B(!(CBGetDocumentManager())->FileDocumentIsOpen(fileName, &doc) ||
				doc->Save());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBBuildManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsMakeDependCmd && message.Is(CBCommand::kFinished))
		{
		const CBCommand::Finished* info =
			dynamic_cast<const CBCommand::Finished*>(&message);
		assert( info != NULL );

		itsMakeDependCmd = NULL;
		if (info->Successful())
			{
			itsModTime = itsNewModTime;
			}
		}

	else if (sender == itsProjectConfigDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			UpdateProjectConfig();
			}
		itsProjectConfigDialog = NULL;
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ProjectChanged

	node can be NULL

 ******************************************************************************/

void
CBBuildManager::ProjectChanged
	(
	const CBProjectNode* node
	)
{
	if (node == NULL ||
		(itsMakefileMethod == kMakemake && node->IncludedInMakefile())  ||
		(itsMakefileMethod == kCMake    && node->IncludedInCMakeData()) ||
		(itsMakefileMethod == kQMake    && node->IncludedInQMakeData()))
		{
		itsNeedWriteMakeFilesFlag = kJTrue;
		}
}

/******************************************************************************
 EditProjectConfig

 ******************************************************************************/

void
CBBuildManager::EditProjectConfig()
{
	assert( itsProjectConfigDialog == NULL );

	itsProjectConfigDialog =
		jnew CBProjectConfigDialog(itsProjDoc, itsMakefileMethod,
								  itsTargetName, itsDepListExpr,
								  (itsProjDoc->GetCommandManager())->GetMakeDependCommand(),
								  itsSubProjectBuildCmd);
	assert( itsProjectConfigDialog != NULL );
	itsProjectConfigDialog->BeginDialog();
	ListenTo(itsProjectConfigDialog);
}

/******************************************************************************
 UpdateProjectConfig (private)

 ******************************************************************************/

void
CBBuildManager::UpdateProjectConfig()
{
	assert( itsProjectConfigDialog != NULL );

	MakefileMethod method;
	JString targetName, depListExpr, updateMakefileCmd;
	itsProjectConfigDialog->GetConfig(&method, &targetName, &depListExpr,
									  &updateMakefileCmd, &itsSubProjectBuildCmd);

	if (method      != itsMakefileMethod ||
		targetName  != itsTargetName     ||
		depListExpr != itsDepListExpr)
		{
		itsNeedWriteMakeFilesFlag = kJTrue;

		itsMakefileMethod = method;
		itsTargetName     = targetName;
		itsDepListExpr    = depListExpr;
		}

	(itsProjDoc->GetCommandManager())->SetMakeDependCommand(updateMakefileCmd);
}

/******************************************************************************
 UpdateMakeDependCmd (static)

	Returns kJTrue if *cmd was changed.

 ******************************************************************************/

JBoolean
CBBuildManager::UpdateMakeDependCmd
	(
	const MakefileMethod	oldMethod,
	const MakefileMethod	newMethod,
	JString*				cmd
	)
{
	if (oldMethod == newMethod)
		{
		return kJFalse;
		}

	if (newMethod == kMakemake)
		{
		*cmd = JGetString(kDefaultMakemakeDependCmdID);
		return kJTrue;
		}
	else if (newMethod == kCMake)
		{
		*cmd = JGetString(kDefaultCMakeDependCmdID);
		return kJTrue;
		}
	else if (newMethod == kQMake)
		{
		*cmd = JGetString(kDefaultQMakeDependCmdID);
		return kJTrue;
		}
	// if manual, leave as is, since input files for other methods will still exist

	return kJFalse;
}

/******************************************************************************
 EditMakeConfig

 ******************************************************************************/

JBoolean
CBBuildManager::EditMakeConfig()
{
	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);

	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);

	if (itsMakefileMethod == kMakemake)
		{
		RecreateMakeHeaderFile();

		JString makeHeaderName, makeFilesName;
		GetMakemakeFileNames(&makeHeaderName, &makeFilesName);

		if ((CBGetDocumentManager())->OpenTextDocument(makeHeaderName))
			{
			return kJTrue;
			}
		}
	else if (itsMakefileMethod == kCMake)
		{
		RecreateMakeHeaderFile();

		if ((CBGetDocumentManager())->OpenTextDocument(cmakeHeaderName))
			{
			return kJTrue;
			}
		}
	else if (itsMakefileMethod == kQMake)
		{
		RecreateMakeHeaderFile();

		if ((CBGetDocumentManager())->OpenTextDocument(qmakeHeaderName))
			{
			return kJTrue;
			}
		}

	JPtrArray<JString> makefileNameList(JPtrArrayT::kDeleteAll);
	GetMakefileNames(&makefileNameList);
	makefileNameList.Prepend(cmakeInputName);
	makefileNameList.Prepend(qmakeInputName);
	makefileNameList.Append(JCombinePathAndName(itsProjDoc->GetFilePath(), "pom.xml"));		// Maven
	makefileNameList.Append(JCombinePathAndName(itsProjDoc->GetFilePath(), "build.xml"));	// ant

	const JSize count = makefileNameList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JString* fileName = makefileNameList.NthElement(i);
		if (JFileReadable(*fileName) &&
			(CBGetDocumentManager())->OpenTextDocument(*fileName))
			{
			return kJTrue;
			}
		}

	for (JIndex i=1; i<=count; i++)
		{
		if ((CBGetDocumentManager())->OpenTextDocument(*(makefileNameList.NthElement(i))))
			{
			return kJTrue;
			}
		}

	(JGetUserNotification())->ReportError(JGetString(kNoMakeFileID));
	return kJFalse;
}

/******************************************************************************
 ReadSetup (private)

 ******************************************************************************/

void
CBBuildManager::ReadSetup
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	const JCharacter*	projPath
	)
{
	if (projVers < 62)
		{
		// default values set by ctor

		if (projVers >= 57)
			{
			projInput >> itsMakefileMethod >> itsNeedWriteMakeFilesFlag;
			projInput >> itsTargetName;
			}
		else if (projVers >= 35)
			{
			JBoolean shouldWriteMakeFilesFlag;
			projInput >> shouldWriteMakeFilesFlag >> itsNeedWriteMakeFilesFlag;
			projInput >> itsTargetName;

			itsMakefileMethod = (shouldWriteMakeFilesFlag ? CBBuildManager::kMakemake :
															CBBuildManager::kManual);
			}

		if (projVers >= 37)
			{
			JString origPath;
			projInput >> origPath;
			if (JString::IsEmpty(projPath) || origPath != projPath)
				{
				itsNeedWriteMakeFilesFlag = kJTrue;
				}
			}

		if (projVers >= 39)
			{
			projInput >> itsDepListExpr;
			}
		}
	else
		{
		if (projVers >= 71)
			{
			projInput >> std::ws;
			JIgnoreLine(projInput);
			}

		projInput >> itsMakefileMethod;
		projInput >> itsNeedWriteMakeFilesFlag;
		projInput >> itsTargetName;
		projInput >> itsDepListExpr;

		if (projVers < 71)
			{
			projInput >> itsMakePath;
			projInput >> itsModTime.makeHeaderModTime;
			projInput >> itsModTime.makeFilesModTime;

			if (projVers >= 67)
				{
				projInput >> itsModTime.cmakeHeaderModTime;
				projInput >> itsModTime.cmakeInputModTime;
				}
			else
				{
				itsModTime.cmakeHeaderModTime = 0;
				itsModTime.cmakeInputModTime  = 0;
				}

			projInput >> itsModTime.qmakeHeaderModTime;
			projInput >> itsModTime.qmakeInputModTime;
			projInput >> itsLastMakefileUpdateTime;
			}

		if (projVers < 64)
			{
			itsSubProjectBuildCmd = kDefaultSubProjectBuildCmd;
			}
		else
			{
			projInput >> itsSubProjectBuildCmd;
			}
		}

	if (setInput != NULL && setVers >= 71)
		{
		*setInput >> itsMakePath;
		*setInput >> itsModTime.makeHeaderModTime;
		*setInput >> itsModTime.makeFilesModTime;
		*setInput >> itsModTime.cmakeHeaderModTime;
		*setInput >> itsModTime.cmakeInputModTime;
		*setInput >> itsModTime.qmakeHeaderModTime;
		*setInput >> itsModTime.qmakeInputModTime;
		*setInput >> itsLastMakefileUpdateTime;
		}
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
CBBuildManager::StreamOut
	(
	std::ostream& projOutput,
	std::ostream* setOutput
	)
	const
{
	projOutput << "# build settings\n";
	projOutput << itsMakefileMethod << '\n';
	projOutput << itsNeedWriteMakeFilesFlag << '\n';
	projOutput << itsTargetName << '\n';
	projOutput << itsDepListExpr << '\n';
	projOutput << itsSubProjectBuildCmd << '\n';

	if (setOutput != NULL)
		{
		*setOutput << ' ' << itsMakePath;
		*setOutput << ' ' << itsModTime.makeHeaderModTime;
		*setOutput << ' ' << itsModTime.makeFilesModTime;
		*setOutput << ' ' << itsModTime.cmakeHeaderModTime;
		*setOutput << ' ' << itsModTime.cmakeInputModTime;
		*setOutput << ' ' << itsModTime.qmakeHeaderModTime;
		*setOutput << ' ' << itsModTime.qmakeInputModTime;
		*setOutput << ' ' << itsLastMakefileUpdateTime;
		*setOutput << ' ';
		}
}

/******************************************************************************
 ReadTemplate

 ******************************************************************************/

void
CBBuildManager::ReadTemplate
	(
	std::istream&				input,
	const JFileVersion		tmplVers,
	const JFileVersion		projVers,
	const MakefileMethod	method,
	const JCharacter*		targetName,
	const JCharacter*		depListExpr
	)
{
	itsMakefileMethod = method;
	itsTargetName     = targetName;
	itsDepListExpr    = depListExpr;

	if (projVers >= 62)
		{
		ReadSetup(input, projVers, NULL, 0, NULL);
		}
	else
		{
		(itsProjDoc->GetCommandManager())->ConvertCompileDialog(input, projVers,
																this, kJFalse);
		}

	JBoolean hasMakeHeader = kJTrue, hasMakeFiles = kJTrue,
			 hasCMakeHeader = kJFalse, hasQMakeHeader = kJFalse;
	JString makeHeaderText, makeFilesText, cmakeHeaderText, qmakeHeaderText;
	if (tmplVers > 0)
		{
		input >> hasMakeHeader;
		}
	if (hasMakeHeader)
		{
		input >> makeHeaderText;
		}
	if (tmplVers > 0)
		{
		input >> hasMakeFiles;
		}
	if (hasMakeFiles)
		{
		input >> makeFilesText;
		}

	if (tmplVers >= 4)
		{
		input >> hasCMakeHeader;
		if (hasCMakeHeader)
			{
			input >> cmakeHeaderText;
			}
		}

	if (tmplVers >= 2)
		{
		input >> hasQMakeHeader;
		if (hasQMakeHeader)
			{
			input >> qmakeHeaderText;
			}

		JBoolean hasOtherFiles;
		input >> hasOtherFiles;
		if (hasOtherFiles)
			{
			JSize fileCount;
			input >> fileCount;

			JString fileName, fullName, text;
			JBoolean hasFile;
			for (JIndex i=1; i<=fileCount; i++)
				{
				input >> fileName >> hasFile;
				if (hasFile)
					{
					input >> text;

					fullName = JCombinePathAndName(itsProjDoc->GetFilePath(), fileName);
					if (!JNameUsed(fullName))
						{
						std::ofstream output(fullName);
						text.Print(output);
						}
					}
				}
			}
		}

	if (hasMakeHeader || hasMakeFiles)
		{
		CreateMakemakeFiles(makeHeaderText, makeFilesText, kJTrue);
		}
	if (hasCMakeHeader)
		{
		CreateCMakeFiles(cmakeHeaderText, kJTrue);
		}
	if (hasQMakeHeader)
		{
		CreateQMakeFiles(qmakeHeaderText, kJTrue);
		}

	if (projVers < 62)
		{
		(itsProjDoc->GetCommandManager())->ConvertRunDialog(input, projVers, kJFalse);
		}
}

/******************************************************************************
 WriteTemplate

 ******************************************************************************/

inline void
cbSaveFile
	(
	std::ostream&			output,
	const JCharacter*	fileName
	)
{
	if (JFileExists(fileName))
		{
		JString text;
		JReadFile(fileName, &text);
		output << ' ' << kJTrue << ' ' << text;
		}
	else
		{
		output << ' ' << kJFalse;
		}
}

void
CBBuildManager::WriteTemplate
	(
	std::ostream& output
	)
	const
{
	StreamOut(output, NULL);

	JBoolean saveMakefile = kJTrue;

	JString makeHeaderName, makeFilesName;
	GetMakemakeFileNames(&makeHeaderName, &makeFilesName);

	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);

	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);

	cbSaveFile(output, makeHeaderName);
	cbSaveFile(output, makeFilesName);
	cbSaveFile(output, cmakeHeaderName);
	cbSaveFile(output, qmakeHeaderName);

	if (itsMakefileMethod == kManual)
		{
		output << ' ' << kJTrue;

		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
		GetMakefileNames(&list);
		list.Append(cmakeInputName);
		list.Append(qmakeInputName);

		const JSize count = list.GetElementCount();
		output << ' ' << count;

		JString path, name;
		for (JIndex i=1; i<=count; i++)
			{
			const JString* fullName = list.NthElement(i);
			JSplitPathAndName(*fullName, &path, &name);
			output << ' ' << name;
			cbSaveFile(output, *fullName);
			}
		}
	else
		{
		output << ' ' << kJFalse;
		}
}

/******************************************************************************
 ConvertCompileDialog

 ******************************************************************************/

void
CBBuildManager::ConvertCompileDialog
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	if (vers >= 27)
		{
		input >> itsMakePath;
		input >> itsModTime.makeHeaderModTime;
		input >> itsModTime.makeFilesModTime;

		if (vers >= 57)
			{
			input >> itsModTime.qmakeHeaderModTime;
			input >> itsModTime.qmakeInputModTime;
			}
		}
}

/******************************************************************************
 CreateMakeFiles

	Returns a safe MakefileMethod to avoid overwriting existing make files.

 ******************************************************************************/

void
CBBuildManager::CreateMakeFiles
	(
	const MakefileMethod method
	)
{
	itsMakefileMethod = method;

	if (itsMakefileMethod == kMakemake)
		{
		CreateMakemakeFiles(JGetString(kMakeHeaderInitTextID),
							JGetString(kMakeFilesInitTextID), kJFalse);
		}
	else if (itsMakefileMethod == kCMake)
		{
		CreateCMakeFiles(JGetString(kCMakeHeaderInitTextID), kJFalse);
		}
	else if (itsMakefileMethod == kQMake)
		{
		CreateQMakeFiles(JGetString(kQMakeHeaderInitTextID), kJFalse);
		}
}

// private

void
CBBuildManager::CreateMakemakeFiles
	(
	const JCharacter*	makeHeaderText,
	const JCharacter*	makeFilesText,
	const JBoolean		readingTemplate
	)
{
	// don't overwrite existing Make.files or Make.header

	JString makeHeaderName, makeFilesName;
	GetMakemakeFileNames(&makeHeaderName, &makeFilesName);
	if (!JFileExists(makeHeaderName) && !JFileExists(makeFilesName))
		{
		std::ofstream output1(makeHeaderName);
		output1 << makeHeaderText;

		std::ofstream output2(makeFilesName);
		output2 << makeFilesText;
		}

	if (!readingTemplate)
		{
		(itsProjDoc->GetCommandManager())->SetMakeDependCommand(
			JGetString(kDefaultMakemakeDependCmdID));
		}

	itsModTime = SaveMakeFileModTimes();
}

void
CBBuildManager::CreateCMakeFiles
	(
	const JCharacter*	cmakeHeaderText,
	const JBoolean		readingTemplate
	)
{
	// don't overwrite existing CMake.header file

	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);
	if (!JFileExists(cmakeHeaderName))
		{
		std::ofstream output(cmakeHeaderName);
		output << cmakeHeaderText;
		}

	if (!readingTemplate)
		{
		(itsProjDoc->GetCommandManager())->SetMakeDependCommand(
			JGetString(kDefaultCMakeDependCmdID));
		}

	itsModTime = SaveMakeFileModTimes();
}

void
CBBuildManager::CreateQMakeFiles
	(
	const JCharacter*	qmakeHeaderText,
	const JBoolean		readingTemplate
	)
{
	// don't overwrite existing QMake.header file

	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);
	if (!JFileExists(qmakeHeaderName))
		{
		std::ofstream output(qmakeHeaderName);
		output << qmakeHeaderText;
		}

	if (!readingTemplate)
		{
		(itsProjDoc->GetCommandManager())->SetMakeDependCommand(
			JGetString(kDefaultQMakeDependCmdID));
		}

	itsModTime = SaveMakeFileModTimes();
}

/******************************************************************************
 RecreateMakeHeaderFile (private)

	Recreates *Make.header if user accidentally deleted it.

 ******************************************************************************/

void
CBBuildManager::RecreateMakeHeaderFile()
{
	if (itsMakefileMethod == kMakemake)
		{
		JString makeHeaderName, makeFilesName;
		GetMakemakeFileNames(&makeHeaderName, &makeFilesName);
		if (!JFileExists(makeHeaderName))
			{
			std::ofstream output(makeHeaderName);
			(JGetString(kMakeHeaderInitTextID)).Print(output);
			}
		}
	else if (itsMakefileMethod == kCMake)
		{
		JString cmakeHeaderName, cmakeInputName;
		GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);
		if (!JFileExists(cmakeHeaderName))
			{
			std::ofstream output(cmakeHeaderName);
			(JGetString(kCMakeHeaderInitTextID)).Print(output);
			}
		}
	else if (itsMakefileMethod == kQMake)
		{
		JString qmakeHeaderName, qmakeInputName;
		GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);
		if (!JFileExists(qmakeHeaderName))
			{
			std::ofstream output(qmakeHeaderName);
			(JGetString(kQMakeHeaderInitTextID)).Print(output);
			}
		}
}

/******************************************************************************
 WriteSubProjectBuildFile (private)

 ******************************************************************************/

JBoolean
CBBuildManager::WriteSubProjectBuildFile
	(
	const JBoolean reportError
	)
{
	CBCommandManager* cmdMgr = itsProjDoc->GetCommandManager();

	JString updateCmd;
	if (itsMakefileMethod != kManual)
		{
		if (!cmdMgr->GetMakeDependCmdStr(itsProjDoc, reportError, &updateCmd))
			{
			return kJFalse;
			}

		if (itsMakefileMethod == kMakemake)
			{
			updateCmd.Append(" --check");
			}
		}

	JString buildCmd = itsSubProjectBuildCmd;
	if (!cmdMgr->Substitute(itsProjDoc, reportError, &buildCmd))
		{
		return kJFalse;
		}

	const JCharacter* map[] =
		{
		"update_cmd", updateCmd.GetCString(),
		"build_cmd",  buildCmd.GetCString(),
		};
	const JString data = JGetString(kSubProjectBuildInitTextID, map, sizeof(map));

	JString fileName = JCombinePathAndName(itsProjDoc->GetFilePath(),
										   itsProjDoc->GetName());
	fileName += kSubProjectBuildSuffix;

	JString origData;
	JReadFile(fileName, &origData);
	if (origData != data)
		{
		JEditVCS(fileName);
		std::ofstream output(fileName);
		data.Print(output);
		}

	mode_t perms;
	if ((JGetPermissions(fileName, &perms)).OK())
		{
		perms |= S_IXUSR | S_IXGRP | S_IXOTH;
		JSetPermissions(fileName, perms);
		}

	return kJTrue;
}

/******************************************************************************
 GetSubProjectBuildSuffix (static)

 ******************************************************************************/

const JCharacter*
CBBuildManager::GetSubProjectBuildSuffix()
{
	return kSubProjectBuildSuffix;
}

/******************************************************************************
 MakeFilesChanged (private)

 ******************************************************************************/

JBoolean
CBBuildManager::MakeFilesChanged()
	const
{
	// If the directory changed, we don't have a previous time stamp with
	// which we can compare.

	if (itsProjDoc->GetFilePath() != itsMakePath)
		{
		return kJTrue;
		}

	JString makeHeaderName, makeFilesName;
	GetMakemakeFileNames(&makeHeaderName, &makeFilesName);

	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);

	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);

	const JBoolean cmakeInputExists = JFileExists(cmakeInputName);
	const JBoolean qmakeInputExists = JFileExists(qmakeInputName);
	const JBoolean makefileExists   = MakefileExists();
	const JBoolean needsMakefile =
		JI2B((JFileExists(makeHeaderName) &&
			  JFileExists(makeFilesName) &&
			  !makefileExists) ||
			 (JFileExists(cmakeHeaderName) && !cmakeInputExists) ||
			 (JFileExists(qmakeHeaderName) && !qmakeInputExists) ||
			 ((cmakeInputExists || qmakeInputExists) && !makefileExists));

	time_t t;
	return JI2B(
		needsMakefile ||
		(JGetModificationTime(makeHeaderName, &t) == kJNoError &&
		 itsModTime.makeHeaderModTime != t)  ||
		(JGetModificationTime(makeFilesName, &t) == kJNoError &&
		 itsModTime.makeFilesModTime != t)   ||
		(JGetModificationTime(cmakeHeaderName, &t) == kJNoError &&
		 itsModTime.cmakeHeaderModTime != t) ||
		(JGetModificationTime(cmakeInputName, &t) == kJNoError &&
		 itsModTime.cmakeInputModTime != t)  ||
		(JGetModificationTime(qmakeHeaderName, &t) == kJNoError &&
		 itsModTime.qmakeHeaderModTime != t) ||
		(JGetModificationTime(qmakeInputName, &t) == kJNoError &&
		 itsModTime.qmakeInputModTime != t));
}

/******************************************************************************
 CMakeHeaderChanged (private)

 ******************************************************************************/

JBoolean
CBBuildManager::CMakeHeaderChanged()
	const
{
	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);

	time_t t;
	return JI2B(JGetModificationTime(cmakeHeaderName, &t) == kJNoError &&
				itsModTime.cmakeHeaderModTime != t);
}

/******************************************************************************
 QMakeHeaderChanged (private)

 ******************************************************************************/

JBoolean
CBBuildManager::QMakeHeaderChanged()
	const
{
	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);

	time_t t;
	return JI2B(JGetModificationTime(qmakeHeaderName, &t) == kJNoError &&
				itsModTime.qmakeHeaderModTime != t);
}

/******************************************************************************
 SaveMakeFileModTimes (private)

 ******************************************************************************/

CBBuildManager::ModTime
CBBuildManager::SaveMakeFileModTimes()
{
	itsMakePath = itsProjDoc->GetFilePath();

	ModTime t;

	JString makeHeaderName, makeFilesName;
	GetMakemakeFileNames(&makeHeaderName, &makeFilesName);

	JGetModificationTime(makeHeaderName, &(t.makeHeaderModTime));
	JGetModificationTime(makeFilesName,  &(t.makeFilesModTime));

	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);

	JGetModificationTime(cmakeHeaderName, &(t.cmakeHeaderModTime));
	JGetModificationTime(cmakeInputName,  &(t.cmakeInputModTime));

	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);

	JGetModificationTime(qmakeHeaderName, &(t.qmakeHeaderModTime));
	JGetModificationTime(qmakeInputName,  &(t.qmakeInputModTime));

	return t;
}

/******************************************************************************
 GetMakemakeFileNames (private)

 ******************************************************************************/

void
CBBuildManager::GetMakemakeFileNames
	(
	JString* makeHeaderName,
	JString* makeFilesName
	)
	const
{
	*makeHeaderName = JCombinePathAndName(itsProjDoc->GetFilePath(), kMakeHeaderName);
	*makeFilesName  = JCombinePathAndName(itsProjDoc->GetFilePath(), kMakeFilesName);
}

/******************************************************************************
 GetMakeFilesName (static)

 ******************************************************************************/

JString
CBBuildManager::GetMakeFilesName
	(
	const JCharacter* path
	)
{
	return JCombinePathAndName(path, kMakeFilesName);
}

/******************************************************************************
 GetCMakeFileNames (private)

 ******************************************************************************/

void
CBBuildManager::GetCMakeFileNames
	(
	JString* cmakeHeaderName,
	JString* cmakeInputName
	)
	const
{
	*cmakeHeaderName = JCombinePathAndName(itsProjDoc->GetFilePath(), kCMakeHeaderName);
	*cmakeInputName  = JCombinePathAndName(itsProjDoc->GetFilePath(), kCMakeInputName);
}

/******************************************************************************
 GetCMakeInputName (static)

 ******************************************************************************/

JString
CBBuildManager::GetCMakeInputName
	(
	const JCharacter* path,
	const JCharacter* projName
	)
{
	return JCombinePathAndName(path, kCMakeInputName);
}

/******************************************************************************
 GetQMakeFileNames (private)

 ******************************************************************************/

void
CBBuildManager::GetQMakeFileNames
	(
	JString* qmakeHeaderName,
	JString* qmakeInputName
	)
	const
{
	*qmakeHeaderName = JCombinePathAndName(itsProjDoc->GetFilePath(), kQMakeHeaderName);

	*qmakeInputName  = itsProjDoc->GetName();
	*qmakeInputName  = JCombinePathAndName(itsProjDoc->GetFilePath(), *qmakeInputName);
	*qmakeInputName += kQMakeInputFileSuffix;
}

/******************************************************************************
 GetQMakeInputName (static)

 ******************************************************************************/

JString
CBBuildManager::GetQMakeInputName
	(
	const JCharacter* path,
	const JCharacter* projName
	)
{
	JString qmakeInputName = JCombinePathAndName(path, projName);
	qmakeInputName        += kQMakeInputFileSuffix;
	return qmakeInputName;
}

/******************************************************************************
 GetMakefileNames (private)

 ******************************************************************************/

void
CBBuildManager::GetMakefileNames
	(
	JPtrArray<JString>* list
	)
	const
{
	const JString& path = itsProjDoc->GetFilePath();
	return GetMakefileNames(path, list);
}

// static

void
CBBuildManager::GetMakefileNames
	(
	const JCharacter*	path,
	JPtrArray<JString>*	list
	)
{
	JString name;
	for (JIndex i=0; i<kMakefileNameCount; i++)
		{
		name = JCombinePathAndName(path, kMakefileName[i]);
		list->Append(name);
		}
}

/******************************************************************************
 MakefileExists (private)

 ******************************************************************************/

JBoolean
CBBuildManager::MakefileExists()
	const
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetMakefileNames(&list);
	for (JIndex i=kFirstDefaultMakefileIndex; i<=kMakefileNameCount; i++)
		{
		if (JFileExists(*(list.NthElement(i))))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetMakefileMethodName (static)

 ******************************************************************************/

static const JCharacter* kMakefileMethodName[] =
{
	"manual",
	"makemake",
	"qmake",
	"CMake"
};

const JCharacter*
CBBuildManager::GetMakefileMethodName
	(
	const MakefileMethod method
	)
{
	assert( method == kManual || method == kMakemake ||
			method == kCMake || method == kQMake );
	return kMakefileMethodName[ method ];
}

/******************************************************************************
 Stream operators for MakefileMethod

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&						input,
	CBBuildManager::MakefileMethod&	method
	)
{
	long temp;
	input >> temp;
	method = (CBBuildManager::MakefileMethod) temp;
	assert( method == CBBuildManager::kManual   ||
			method == CBBuildManager::kMakemake ||
			method == CBBuildManager::kCMake    ||
			method == CBBuildManager::kQMake );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&								output,
	const CBBuildManager::MakefileMethod	method
	)
{
	output << (long) method;
	return output;
}
