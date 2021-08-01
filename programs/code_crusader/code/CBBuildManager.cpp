/******************************************************************************
 CBBuildManager.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright © 1997-2002 by John Lindal.

 ******************************************************************************/

#include "CBBuildManager.h"
#include "CBProjectDocument.h"
#include "CBProjectTree.h"
#include "CBProjectNode.h"
#include "CBCommandManager.h"
#include "CBProjectConfigDialog.h"
#include "CBCommand.h"
#include "cbGlobals.h"
#include <JStringIterator.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jVCSUtil.h>
#include <sstream>
#include <jAssert.h>

bool CBBuildManager::itsRebuildMakefileDailyFlag = true;

static const JString kMakeHeaderName("Make.header", JString::kNoCopy);
static const JString kMakeFilesName("Make.files", JString::kNoCopy);

static const JString kCMakeHeaderName("CMake.header", JString::kNoCopy);
static const JString kCMakeInputName("CMakeLists.txt", JString::kNoCopy);

static const JString kQMakeHeaderName("QMake.header", JString::kNoCopy);
static const JString kQMakeInputFileSuffix(".pro", JString::kNoCopy);

static const JString kDefaultSubProjectBuildCmd("make -k all", JString::kNoCopy);
static const JString kSubProjectBuildSuffix(".jmk", JString::kNoCopy);

static const JUtf8Byte* kMakefileName[] =
{
	"m3makefile", "Makefile", "makefile"
};

const JIndex kFirstDefaultMakefileIndex = 2;	// first one that makemake can overwrite
const JSize kMakefileNameCount          = sizeof(kMakefileName) / sizeof(JUtf8Byte*);

const time_t kUpdateMakefileInterval = 24*3600;	// 1 day (seconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

CBBuildManager::CBBuildManager
	(
	CBProjectDocument*		doc,
	const MakefileMethod	method,
	const bool			needWriteMakeFiles,
	const JString&			targetName,
	const JString&			depListExpr
	)
	:
	itsMakefileMethod(method),
	itsNeedWriteMakeFilesFlag(needWriteMakeFiles),
	itsTargetName(targetName),
	itsDepListExpr(depListExpr),
	itsSubProjectBuildCmd(kDefaultSubProjectBuildCmd),
	itsLastMakefileUpdateTime(0),
	itsProjDoc(doc),
	itsMakeDependCmd(nullptr),
	itsProjectConfigDialog(nullptr)
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
	itsNeedWriteMakeFilesFlag(true),
	itsSubProjectBuildCmd(kDefaultSubProjectBuildCmd),
	itsLastMakefileUpdateTime(0),
	itsProjDoc(doc),
	itsMakeDependCmd(nullptr),
	itsProjectConfigDialog(nullptr)
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

	Returns true if Makefile should be updated.  If this process has been
	successfully started, returns *cmd != nullptr.

 ******************************************************************************/

bool
CBBuildManager::UpdateMakefile
	(
	CBExecOutputDocument*	compileDoc,
	CBCommand**				cmd,
	const bool			force
	)
{
	if (itsMakeDependCmd != nullptr)
		{
		if (cmd != nullptr)
			{
			*cmd = itsMakeDependCmd;
			}
		return true;
		}

	if (!UpdateMakeFiles())
		{
		if (cmd != nullptr)
			{
			*cmd = nullptr;
			}
		return true;
		}

	const time_t now = time(nullptr);
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

		if (cmd != nullptr)
			{
			*cmd = itsMakeDependCmd;
			}
		return true;
		}
	else
		{
		if (cmd != nullptr)
			{
			*cmd = nullptr;
			}
		return false;
		}
}

/******************************************************************************
 UpdateMakeFiles

 ******************************************************************************/

bool
CBBuildManager::UpdateMakeFiles
	(
	const bool reportError
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
		return false;
		}

	if (itsMakefileMethod == kManual)	// after saving files related to make
		{
		return true;
		}

	RecreateMakeHeaderFile();

	if (!WriteSubProjectBuildFile(reportError))
		{
		return false;
		}

	if (itsProjDoc->GetFilePath() != itsMakePath ||				// jnew directory
		(itsMakefileMethod == kCMake && CMakeHeaderChanged()) ||// update CMakeLists.txt
		(itsMakefileMethod == kQMake && QMakeHeaderChanged()) ||// update .pro file
		itsMakefileMethod == kMakemake)							// always call UpdateMakeHeader()
		{
		itsNeedWriteMakeFilesFlag = true;
		}

	if (!itsNeedWriteMakeFilesFlag &&
		((itsMakefileMethod == kMakemake && JFileExists(makeFilesName))  ||
		 (itsMakefileMethod == kCMake    && JFileExists(cmakeInputName)) ||
		 (itsMakefileMethod == kQMake    && JFileExists(qmakeInputName))))
		{
		return true;
		}
	else if (itsTargetName.IsEmpty())
		{
		if (reportError)
			{
			JGetUserNotification()->ReportError(JGetString("MissingBuildTarget::CBBuildManager"));
			EditProjectConfig();
			}
		return false;
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
		itsNeedWriteMakeFilesFlag = false;
		return true;
		}
	else if (itsMakefileMethod == kCMake &&
			 fileTree->BuildCMakeData(&src, &hdr, &invalidList) &&
			 !src.IsEmpty())
		{
		WriteCMakeInput(cmakeHeaderName, src, hdr, cmakeInputName);
		itsNeedWriteMakeFilesFlag = false;
		return true;
		}
	else if (itsMakefileMethod == kQMake &&
			 fileTree->BuildQMakeData(&src, &hdr, &invalidList) &&
			 !src.IsEmpty())
		{
		WriteQMakeInput(qmakeHeaderName, src, hdr, qmakeInputName);
		itsNeedWriteMakeFilesFlag = false;
		return true;
		}
	else if (!invalidList.IsEmpty())
		{
		if (reportError)
			{
			CBProjectTable*	fileTable = itsProjDoc->GetFileTable();
			fileTable->ClearSelection();
			fileTable->SelectFileNodes(invalidList);
			JGetUserNotification()->ReportError(JGetString("MissingSourceFiles::CBBuildManager"));
			itsProjDoc->Activate();
			}
		return false;
		}
	else
		{
		if (reportError)
			{
			JGetUserNotification()->ReportError(JGetString("NoSourceFiles::CBBuildManager"));
			itsProjDoc->Activate();
			}
		return false;
		}
}

/******************************************************************************
 UpdateMakeHeader (private)

 ******************************************************************************/

void
CBBuildManager::UpdateMakeHeader
	(
	const JString&				fileName,
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

	const JString& marker = JGetString("MakeHeaderMarker::CBBuildManager");

	JStringIterator iter(text);
	const bool foundMarker = iter.Next(marker);
	if (foundMarker)
		{
		output.write(text.GetBytes(), iter.GetPrevByteIndex() - marker.GetByteCount());
		}
	else
		{
		text.TrimWhitespace();
		text.Print(output);
		output << "\n\n";
		}

	marker.Print(output);
	output << "\n\n";

	JGetString("MakeHeaderLibTargetWarning::CBBuildManager").Print(output);

	const JSize libCount = libFileList.GetElementCount();
	JString libPath, libName;
	for (JIndex i=1; i<=libCount; i++)
		{
		const JUtf8Byte* map[] =
			{
			"lib_full_name", libFileList.GetElement(i)->GetBytes(),
			"lib_proj_path", libProjPathList.GetElement(i)->GetBytes(),
			};
		JGetString("MakeHeaderLibTarget::CBBuildManager", map, sizeof(map)).Print(output);
		}

	output << '\n';
	marker.Print(output);

	if (foundMarker && iter.Next(marker) && !iter.AtEnd())
		{
		const JSize offset = iter.GetPrevByteIndex();
		output.write(text.GetBytes() + offset, text.GetByteCount() - offset);
		}
	else
		{
		output << '\n';
		}

	iter.Invalidate();

	// if new Make.header data is different, update file

	const std::string s = output.str();
	if (s.c_str() != text)
		{
		JEditVCS(fileName);
		std::ofstream f(fileName.GetBytes());
		f << s.c_str();
		f.close();

		// force reload of Make.header, if open

		JXFileDocument* doc;
		if ((JXGetDocumentManager())->FileDocumentIsOpen(fileName, &doc))
			{
			doc->RevertIfChangedByOthers(true);
			}
		}
}

/******************************************************************************
 UpdateMakeFiles (private)

 ******************************************************************************/

void
CBBuildManager::UpdateMakeFiles
	(
	const JString& fileName,
	const JString& text
	)
	const
{
	std::ostringstream data;
	JGetString("MakeFilesWarning::CBBuildManager").Print(data);
	PrintTargetName(data);
	data << '\n';
	text.Print(data);
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
		std::ofstream output(fileName.GetBytes());
		output << s.c_str();
		}
}

/******************************************************************************
 WriteCMakeInput (private)

 ******************************************************************************/

void
CBBuildManager::WriteCMakeInput
	(
	const JString& inputFileName,
	const JString& src,
	const JString& hdr,
	const JString& outputFileName
	)
	const
{
	const JUtf8Byte* map[] =
		{
		"t", itsTargetName.GetBytes(),
		"s", src.GetBytes(),
		"h", hdr.GetBytes()
		};
	const JString cmakeData = JGetString("CMakeData::CBBuildManager", map, sizeof(map));

	JString cmakeHeader;
	JReadFile(inputFileName, &cmakeHeader);
	const JString& marker = JGetString("CMakeInsertMarker::CBBuildManager");

	JStringIterator iter(&cmakeHeader);
	if (iter.Next(marker))
		{
		iter.ReplaceLastMatch(cmakeData);
		}
	else
		{
		cmakeHeader += cmakeData;
		}
	iter.Invalidate();

	JEditVCS(outputFileName);
	std::ofstream output(outputFileName.GetBytes());
	JGetString("MakeFilesWarning::CBBuildManager").Print(output);
	cmakeHeader.Print(output);
}

/******************************************************************************
 WriteQMakeInput (private)

 ******************************************************************************/

void
CBBuildManager::WriteQMakeInput
	(
	const JString& inputFileName,
	const JString& src,
	const JString& hdr,
	const JString& outputFileName
	)
	const
{
	const JUtf8Byte* map[] =
		{
		"t", itsTargetName.GetBytes(),
		"s", src.GetBytes(),
		"h", hdr.GetBytes()
		};
	const JString qmakeData = JGetString("QMakeData::CBBuildManager", map, sizeof(map));

	JString qmakeHeader;
	JReadFile(inputFileName, &qmakeHeader);
	const JString& marker = JGetString("QMakeInsertMarker::CBBuildManager");

	JStringIterator iter(&qmakeHeader);
	if (iter.Next(marker))
		{
		iter.ReplaceLastMatch(qmakeData);
		}
	else
		{
		qmakeHeader += qmakeData;
		}
	iter.Invalidate();

	JEditVCS(outputFileName);
	std::ofstream output(outputFileName.GetBytes());
	JGetString("MakeFilesWarning::CBBuildManager").Print(output);
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
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	itsTargetName.Split(",", &list);

	for (const auto* s : list)
		{
		output << '@';
		s->Print(output);
		output << '\n';
		}
}

/******************************************************************************
 SaveOpenFile (private)

 ******************************************************************************/

bool
CBBuildManager::SaveOpenFile
	(
	const JString& fileName
	)
{
	JXFileDocument* doc;
	return !CBGetDocumentManager()->FileDocumentIsOpen(fileName, &doc) ||
				doc->Save();
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
		const auto* info =
			dynamic_cast<const CBCommand::Finished*>(&message);
		assert( info != nullptr );

		itsMakeDependCmd = nullptr;
		if (info->Successful())
			{
			itsModTime = itsNewModTime;
			}
		}

	else if (sender == itsProjectConfigDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateProjectConfig();
			}
		itsProjectConfigDialog = nullptr;
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ProjectChanged

	node can be nullptr

 ******************************************************************************/

void
CBBuildManager::ProjectChanged
	(
	const CBProjectNode* node
	)
{
	if (node == nullptr ||
		(itsMakefileMethod == kMakemake && node->IncludedInMakefile())  ||
		(itsMakefileMethod == kCMake    && node->IncludedInCMakeData()) ||
		(itsMakefileMethod == kQMake    && node->IncludedInQMakeData()))
		{
		itsNeedWriteMakeFilesFlag = true;
		}
}

/******************************************************************************
 EditProjectConfig

 ******************************************************************************/

void
CBBuildManager::EditProjectConfig()
{
	assert( itsProjectConfigDialog == nullptr );

	itsProjectConfigDialog =
		jnew CBProjectConfigDialog(itsProjDoc, itsMakefileMethod,
								  itsTargetName, itsDepListExpr,
								  (itsProjDoc->GetCommandManager())->GetMakeDependCommand(),
								  itsSubProjectBuildCmd);
	assert( itsProjectConfigDialog != nullptr );
	itsProjectConfigDialog->BeginDialog();
	ListenTo(itsProjectConfigDialog);
}

/******************************************************************************
 UpdateProjectConfig (private)

 ******************************************************************************/

void
CBBuildManager::UpdateProjectConfig()
{
	assert( itsProjectConfigDialog != nullptr );

	MakefileMethod method;
	JString targetName, depListExpr, updateMakefileCmd;
	itsProjectConfigDialog->GetConfig(&method, &targetName, &depListExpr,
									  &updateMakefileCmd, &itsSubProjectBuildCmd);

	if (method      != itsMakefileMethod ||
		targetName  != itsTargetName     ||
		depListExpr != itsDepListExpr)
		{
		itsNeedWriteMakeFilesFlag = true;

		itsMakefileMethod = method;
		itsTargetName     = targetName;
		itsDepListExpr    = depListExpr;
		}

	(itsProjDoc->GetCommandManager())->SetMakeDependCommand(updateMakefileCmd);
}

/******************************************************************************
 UpdateMakeDependCmd (static)

	Returns true if *cmd was changed.

 ******************************************************************************/

bool
CBBuildManager::UpdateMakeDependCmd
	(
	const MakefileMethod	oldMethod,
	const MakefileMethod	newMethod,
	JString*				cmd
	)
{
	if (oldMethod == newMethod)
		{
		return false;
		}

	if (newMethod == kMakemake)
		{
		*cmd = JGetString("DefaultMakemakeDependCmd::CBBuildManager");
		return true;
		}
	else if (newMethod == kCMake)
		{
		*cmd = JGetString("DefaultCMakeDependCmd::CBBuildManager");
		return true;
		}
	else if (newMethod == kQMake)
		{
		*cmd = JGetString("DefaultQMakeDependCmd::CBBuildManager");
		return true;
		}
	// if manual, leave as is, since input files for other methods will still exist

	return false;
}

/******************************************************************************
 EditMakeConfig

 ******************************************************************************/

bool
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

		if (CBGetDocumentManager()->OpenTextDocument(makeHeaderName))
			{
			return true;
			}
		}
	else if (itsMakefileMethod == kCMake)
		{
		RecreateMakeHeaderFile();

		if (CBGetDocumentManager()->OpenTextDocument(cmakeHeaderName))
			{
			return true;
			}
		}
	else if (itsMakefileMethod == kQMake)
		{
		RecreateMakeHeaderFile();

		if (CBGetDocumentManager()->OpenTextDocument(qmakeHeaderName))
			{
			return true;
			}
		}

	JPtrArray<JString> makefileNameList(JPtrArrayT::kDeleteAll);
	GetMakefileNames(&makefileNameList);
	makefileNameList.Prepend(cmakeInputName);
	makefileNameList.Prepend(qmakeInputName);
	makefileNameList.Append(JCombinePathAndName(itsProjDoc->GetFilePath(), JString("pom.xml", JString::kNoCopy)));	// Maven
	makefileNameList.Append(JCombinePathAndName(itsProjDoc->GetFilePath(), JString("build.xml", JString::kNoCopy)));	// ant

	const JSize count = makefileNameList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JString* fileName = makefileNameList.GetElement(i);
		if (JFileReadable(*fileName) &&
			CBGetDocumentManager()->OpenTextDocument(*fileName))
			{
			return true;
			}
		}

	for (JIndex i=1; i<=count; i++)
		{
		if (CBGetDocumentManager()->OpenTextDocument(*(makefileNameList.GetElement(i))))
			{
			return true;
			}
		}

	JGetUserNotification()->ReportError(JGetString("NoMakeFile::CBBuildManager"));
	return false;
}

/******************************************************************************
 ReadSetup (private)

 ******************************************************************************/

void
CBBuildManager::ReadSetup
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	const JString&		projPath
	)
{
	if (projVers < 62)
		{
		// default values set by ctor

		if (projVers >= 57)
			{
			projInput >> itsMakefileMethod;
			projInput >> JBoolFromString(itsNeedWriteMakeFilesFlag);
			projInput >> itsTargetName;
			}
		else if (projVers >= 35)
			{
			bool shouldWriteMakeFilesFlag;
			projInput >> JBoolFromString(shouldWriteMakeFilesFlag);
			projInput >> JBoolFromString(itsNeedWriteMakeFilesFlag);
			projInput >> itsTargetName;

			itsMakefileMethod = (shouldWriteMakeFilesFlag ? CBBuildManager::kMakemake :
															CBBuildManager::kManual);
			}

		if (projVers >= 37)
			{
			JString origPath;
			projInput >> origPath;
			if (projPath.IsEmpty() || origPath != projPath)
				{
				itsNeedWriteMakeFilesFlag = true;
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
		projInput >> JBoolFromString(itsNeedWriteMakeFilesFlag);
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

	if (setInput != nullptr && setVers >= 71)
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
	projOutput << JBoolToString(itsNeedWriteMakeFilesFlag) << '\n';
	projOutput << itsTargetName << '\n';
	projOutput << itsDepListExpr << '\n';
	projOutput << itsSubProjectBuildCmd << '\n';

	if (setOutput != nullptr)
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
	std::istream&			input,
	const JFileVersion		tmplVers,
	const JFileVersion		projVers,
	const MakefileMethod	method,
	const JString&			targetName,
	const JString&			depListExpr
	)
{
	itsMakefileMethod = method;
	itsTargetName     = targetName;
	itsDepListExpr    = depListExpr;

	if (projVers >= 62)
		{
		ReadSetup(input, projVers, nullptr, 0, JString::empty);
		}
	else
		{
		(itsProjDoc->GetCommandManager())->ConvertCompileDialog(input, projVers,
																this, false);
		}

	bool hasMakeHeader = true, hasMakeFiles = true,
			 hasCMakeHeader = false, hasQMakeHeader = false;
	JString makeHeaderText, makeFilesText, cmakeHeaderText, qmakeHeaderText;
	if (tmplVers > 0)
		{
		input >> JBoolFromString(hasMakeHeader);
		}
	if (hasMakeHeader)
		{
		input >> makeHeaderText;
		}
	if (tmplVers > 0)
		{
		input >> JBoolFromString(hasMakeFiles);
		}
	if (hasMakeFiles)
		{
		input >> makeFilesText;
		}

	if (tmplVers >= 4)
		{
		input >> JBoolFromString(hasCMakeHeader);
		if (hasCMakeHeader)
			{
			input >> cmakeHeaderText;
			}
		}

	if (tmplVers >= 2)
		{
		input >> JBoolFromString(hasQMakeHeader);
		if (hasQMakeHeader)
			{
			input >> qmakeHeaderText;
			}

		bool hasOtherFiles;
		input >> JBoolFromString(hasOtherFiles);
		if (hasOtherFiles)
			{
			JSize fileCount;
			input >> fileCount;

			JString fileName, fullName, text;
			bool hasFile;
			for (JIndex i=1; i<=fileCount; i++)
				{
				input >> fileName >> JBoolFromString(hasFile);
				if (hasFile)
					{
					input >> text;

					fullName = JCombinePathAndName(itsProjDoc->GetFilePath(), fileName);
					if (!JNameUsed(fullName))
						{
						std::ofstream output(fullName.GetBytes());
						text.Print(output);
						}
					}
				}
			}
		}

	if (hasMakeHeader || hasMakeFiles)
		{
		CreateMakemakeFiles(makeHeaderText, makeFilesText, true);
		}
	if (hasCMakeHeader)
		{
		CreateCMakeFiles(cmakeHeaderText, true);
		}
	if (hasQMakeHeader)
		{
		CreateQMakeFiles(qmakeHeaderText, true);
		}

	if (projVers < 62)
		{
		(itsProjDoc->GetCommandManager())->ConvertRunDialog(input, projVers, false);
		}
}

/******************************************************************************
 WriteTemplate

 ******************************************************************************/

inline void
cbSaveFile
	(
	std::ostream&	output,
	const JString&	fileName
	)
{
	if (JFileExists(fileName))
		{
		JString text;
		JReadFile(fileName, &text);
		output << ' ' << JBoolToString(true) << ' ' << text;
		}
	else
		{
		output << ' ' << JBoolToString(false);
		}
}

void
CBBuildManager::WriteTemplate
	(
	std::ostream& output
	)
	const
{
	StreamOut(output, nullptr);

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
		output << ' ' << JBoolToString(true);

		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
		GetMakefileNames(&list);
		list.Append(cmakeInputName);
		list.Append(qmakeInputName);

		const JSize count = list.GetElementCount();
		output << ' ' << count;

		JString path, name;
		for (JIndex i=1; i<=count; i++)
			{
			const JString* fullName = list.GetElement(i);
			JSplitPathAndName(*fullName, &path, &name);
			output << ' ' << name;
			cbSaveFile(output, *fullName);
			}
		}
	else
		{
		output << ' ' << JBoolToString(false);
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
		CreateMakemakeFiles(JGetString("MakeHeaderInitText::CBBuildManager"),
							JGetString("MakeFilesInitText::CBBuildManager"), false);
		}
	else if (itsMakefileMethod == kCMake)
		{
		CreateCMakeFiles(JGetString("CMakeHeaderInitText::CBBuildManager"), false);
		}
	else if (itsMakefileMethod == kQMake)
		{
		CreateQMakeFiles(JGetString("QMakeHeaderInitText::CBBuildManager"), false);
		}
}

// private

void
CBBuildManager::CreateMakemakeFiles
	(
	const JString&	makeHeaderText,
	const JString&	makeFilesText,
	const bool	readingTemplate
	)
{
	// don't overwrite existing Make.files or Make.header

	JString makeHeaderName, makeFilesName;
	GetMakemakeFileNames(&makeHeaderName, &makeFilesName);
	if (!JFileExists(makeHeaderName) && !JFileExists(makeFilesName))
		{
		std::ofstream output1(makeHeaderName.GetBytes());
		output1 << makeHeaderText;

		std::ofstream output2(makeFilesName.GetBytes());
		output2 << makeFilesText;
		}

	if (!readingTemplate)
		{
		itsProjDoc->GetCommandManager()->SetMakeDependCommand(
			JGetString("DefaultMakemakeDependCmd::CBBuildManager"));
		}

	itsModTime = SaveMakeFileModTimes();
}

void
CBBuildManager::CreateCMakeFiles
	(
	const JString&	cmakeHeaderText,
	const bool	readingTemplate
	)
{
	// don't overwrite existing CMake.header file

	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);
	if (!JFileExists(cmakeHeaderName))
		{
		std::ofstream output(cmakeHeaderName.GetBytes());
		output << cmakeHeaderText;
		}

	if (!readingTemplate)
		{
		itsProjDoc->GetCommandManager()->SetMakeDependCommand(
			JGetString("DefaultCMakeDependCmd::CBBuildManager"));
		}

	itsModTime = SaveMakeFileModTimes();
}

void
CBBuildManager::CreateQMakeFiles
	(
	const JString&	qmakeHeaderText,
	const bool	readingTemplate
	)
{
	// don't overwrite existing QMake.header file

	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);
	if (!JFileExists(qmakeHeaderName))
		{
		std::ofstream output(qmakeHeaderName.GetBytes());
		output << qmakeHeaderText;
		}

	if (!readingTemplate)
		{
		itsProjDoc->GetCommandManager()->SetMakeDependCommand(
			JGetString("DefaultQMakeDependCmd::CBBuildManager"));
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
			std::ofstream output(makeHeaderName.GetBytes());
			JGetString("MakeHeaderInitText::CBBuildManager").Print(output);
			}
		}
	else if (itsMakefileMethod == kCMake)
		{
		JString cmakeHeaderName, cmakeInputName;
		GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);
		if (!JFileExists(cmakeHeaderName))
			{
			std::ofstream output(cmakeHeaderName.GetBytes());
			JGetString("CMakeHeaderInitText::CBBuildManager").Print(output);
			}
		}
	else if (itsMakefileMethod == kQMake)
		{
		JString qmakeHeaderName, qmakeInputName;
		GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);
		if (!JFileExists(qmakeHeaderName))
			{
			std::ofstream output(qmakeHeaderName.GetBytes());
			JGetString("QMakeHeaderInitText::CBBuildManager").Print(output);
			}
		}
}

/******************************************************************************
 WriteSubProjectBuildFile (private)

 ******************************************************************************/

bool
CBBuildManager::WriteSubProjectBuildFile
	(
	const bool reportError
	)
{
	CBCommandManager* cmdMgr = itsProjDoc->GetCommandManager();

	JString updateCmd;
	if (itsMakefileMethod != kManual)
		{
		if (!cmdMgr->GetMakeDependCmdStr(itsProjDoc, reportError, &updateCmd))
			{
			return false;
			}

		if (itsMakefileMethod == kMakemake)
			{
			updateCmd.Append(" --check");
			}
		}

	JString buildCmd = itsSubProjectBuildCmd;
	if (!cmdMgr->Substitute(itsProjDoc, reportError, &buildCmd))
		{
		return false;
		}

	const JUtf8Byte* map[] =
		{
		"update_cmd", updateCmd.GetBytes(),
		"build_cmd",  buildCmd.GetBytes(),
		};
	const JString data = JGetString("SubProjectBuildInitText::CBBuildManager", map, sizeof(map));

	JString fileName = JCombinePathAndName(itsProjDoc->GetFilePath(),
										   itsProjDoc->GetName());
	fileName += kSubProjectBuildSuffix;

	JString origData;
	JReadFile(fileName, &origData);
	if (origData != data)
		{
		JEditVCS(fileName);
		std::ofstream output(fileName.GetBytes());
		data.Print(output);
		}

	mode_t perms;
	if ((JGetPermissions(fileName, &perms)).OK())
		{
		perms |= S_IXUSR | S_IXGRP | S_IXOTH;
		JSetPermissions(fileName, perms);
		}

	return true;
}

/******************************************************************************
 GetSubProjectBuildSuffix (static)

 ******************************************************************************/

const JString&
CBBuildManager::GetSubProjectBuildSuffix()
{
	return kSubProjectBuildSuffix;
}

/******************************************************************************
 MakeFilesChanged (private)

 ******************************************************************************/

bool
CBBuildManager::MakeFilesChanged()
	const
{
	// If the directory changed, we don't have a previous time stamp with
	// which we can compare.

	if (itsProjDoc->GetFilePath() != itsMakePath)
		{
		return true;
		}

	JString makeHeaderName, makeFilesName;
	GetMakemakeFileNames(&makeHeaderName, &makeFilesName);

	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);

	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);

	const bool cmakeInputExists = JFileExists(cmakeInputName);
	const bool qmakeInputExists = JFileExists(qmakeInputName);
	const bool makefileExists   = MakefileExists();
	const bool needsMakefile =
		(JFileExists(makeHeaderName) &&
			  JFileExists(makeFilesName) &&
			  !makefileExists) ||
			 (JFileExists(cmakeHeaderName) && !cmakeInputExists) ||
			 (JFileExists(qmakeHeaderName) && !qmakeInputExists) ||
			 ((cmakeInputExists || qmakeInputExists) && !makefileExists);

	time_t t;
	return needsMakefile ||
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
		 itsModTime.qmakeInputModTime != t);
}

/******************************************************************************
 CMakeHeaderChanged (private)

 ******************************************************************************/

bool
CBBuildManager::CMakeHeaderChanged()
	const
{
	JString cmakeHeaderName, cmakeInputName;
	GetCMakeFileNames(&cmakeHeaderName, &cmakeInputName);

	time_t t;
	return JGetModificationTime(cmakeHeaderName, &t) == kJNoError &&
				itsModTime.cmakeHeaderModTime != t;
}

/******************************************************************************
 QMakeHeaderChanged (private)

 ******************************************************************************/

bool
CBBuildManager::QMakeHeaderChanged()
	const
{
	JString qmakeHeaderName, qmakeInputName;
	GetQMakeFileNames(&qmakeHeaderName, &qmakeInputName);

	time_t t;
	return JGetModificationTime(qmakeHeaderName, &t) == kJNoError &&
				itsModTime.qmakeHeaderModTime != t;
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
	const JString& path
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
	const JString& path,
	const JString& projName
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
	const JString& path,
	const JString& projName
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
	const JString&		path,
	JPtrArray<JString>*	list
	)
{
	JString name;
	for (JUnsignedOffset i=0; i<kMakefileNameCount; i++)
		{
		name = JCombinePathAndName(path, JString(kMakefileName[i], JString::kNoCopy));
		list->Append(name);
		}
}

/******************************************************************************
 MakefileExists (private)

 ******************************************************************************/

bool
CBBuildManager::MakefileExists()
	const
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetMakefileNames(&list);
	for (JIndex i=kFirstDefaultMakefileIndex; i<=kMakefileNameCount; i++)
		{
		if (JFileExists(*(list.GetElement(i))))
			{
			return true;
			}
		}

	return false;
}

/******************************************************************************
 GetMakefileMethodName (static)

 ******************************************************************************/

static const JUtf8Byte* kMakefileMethodName[] =
{
	"manual",
	"makemake",
	"qmake",
	"CMake"
};

JString
CBBuildManager::GetMakefileMethodName
	(
	const MakefileMethod method
	)
{
	assert( method == kManual || method == kMakemake ||
			method == kCMake || method == kQMake );
	return JString(kMakefileMethodName[ method ], JString::kNoCopy);
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
