/******************************************************************************
 CBCommandManager.cpp

	Stores a list of user-defined commands.

	BASE CLASS = JPrefObject, virtual JBroadcaster

	Copyright (C) 2001-02 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCommandManager.h"
#include "CBCommand.h"
#include "CBProjectDocument.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "CBCompileDocument.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXHistoryMenuBase.h>
#include <JXWebBrowser.h>
#include <JKLRand.h>
#include <JRegex.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

static const JCharacter* kDefaultMakeDependCmd = "echo Makefile must be updated manually";
JPtrArray<CBTextDocument> CBCommandManager::theExecDocList(JPtrArrayT::kForgetAll);

// setup information

const JFileVersion kCurrentSetupVersion = 4;

	// version  1 removes makeDepend flag, since Makefile now rebuilt once a day
	// version  2 saves menuID
	// version  3 saves isVCS
	// version  4 converts count to keepGoing, for CVS merge

// JBroadcaster message types

const JCharacter* CBCommandManager::kUpdateCommandMenu = "UpdateCommandMenu::CBCommandManager";

/******************************************************************************
 Constructor

	Only cbGlobals actually invokes Read/WritePrefs().

 ******************************************************************************/

CBCommandManager::CBCommandManager
	(
	CBDocumentManager* docMgr
	)
	:
	JPrefObject(CBGetPrefsManager(), kCBGlobalCommandsID),
	itsMakeDependCmd(kDefaultMakeDependCmd),
	itsCompileDoc(NULL)
{
	itsCmdList = new CmdList;
	assert( itsCmdList != NULL );

	if (docMgr != NULL)
		{
		InitCommandList();
		ListenTo(docMgr);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCommandManager::~CBCommandManager()
{
	itsCmdList->DeleteAll();
	delete itsCmdList;
}

/******************************************************************************
 GetMakeDependCmdStr

 ******************************************************************************/

JBoolean
CBCommandManager::GetMakeDependCmdStr
	(
	CBProjectDocument*	projDoc,
	const JBoolean		reportError,
	JString*			cmdStr
	)
	const
{
	*cmdStr = itsMakeDependCmd;
	return Substitute(projDoc, reportError, cmdStr);
}

/******************************************************************************
 Substitute

 ******************************************************************************/

JBoolean
CBCommandManager::Substitute
	(
	CBProjectDocument*	projDoc,
	const JBoolean		reportError,
	JString*			cmdStr
	)
	const
{
	assert( projDoc != NULL );

	JString cmdPath = projDoc->GetFilePath();
	return Substitute(&cmdPath, cmdStr, projDoc, NULL, 0, kJFalse, reportError);
}

/******************************************************************************
 MakeDepend

 ******************************************************************************/

JBoolean
CBCommandManager::MakeDepend
	(
	CBProjectDocument*		projDoc,
	CBExecOutputDocument*	compileDoc,
	CBCommand**				resultCmd
	)
{
	assert( projDoc != NULL );

	JString cmdPath = projDoc->GetFilePath();
	JString cmdStr  = itsMakeDependCmd;
	if (Substitute(&cmdPath, &cmdStr, projDoc, NULL, 0, kJFalse))
		{
		CBCommand* cmd = new CBCommand(cmdPath, kJFalse, kJFalse, projDoc);
		assert( cmd != NULL );

		if (!cmd->Add(cmdStr, NULL, NULL, NULL, NULL))	// subroutines not allowed
			{
			delete cmd;
			}
		else if (cmd->StartMakeProcess(compileDoc))
			{
			if (resultCmd != NULL)
				{
				*resultCmd = cmd;
				}
			return kJTrue;
			}
		}

	if (resultCmd != NULL)
		{
		*resultCmd = NULL;
		}
	return kJFalse;
}

/******************************************************************************
 Exec (textDoc)

	projDoc can be NULL

 ******************************************************************************/

void
CBCommandManager::Exec
	(
	const JIndex		cmdIndex,
	CBProjectDocument*	projDoc,
	CBTextDocument*		textDoc
	)
{
	const CmdInfo info = itsCmdList->GetElement(cmdIndex);
	Exec(info, projDoc, textDoc);
}

// static

void
CBCommandManager::Exec
	(
	const CmdInfo&		info,
	CBProjectDocument*	projDoc,
	CBTextDocument*		textDoc
	)
{
	CBCommand* cmd = NULL;
	JPtrArray<JString> cmdList(JPtrArrayT::kDeleteAll);
	if (Prepare(info, projDoc, textDoc, &cmd, &cmdList))
		{
		cmd->Start(info);
		}
}

/******************************************************************************
 Prepare (textDoc)

	projDoc can be NULL

 ******************************************************************************/

JBoolean
CBCommandManager::Prepare
	(
	const char*			cmdName,
	CBProjectDocument*	projDoc,
	CBTextDocument*		textDoc,
	CBCommand**			cmd,
	CmdInfo**			returnInfo,
	JPtrArray<JString>*	cmdList
	)
{
	*cmd        = NULL;
	*returnInfo = NULL;

	CmdInfo info;
	if (FindCommandName(cmdName, &info))
		{
		if (Prepare(info, projDoc, textDoc, cmd, cmdList))
			{
			*returnInfo = new CmdInfo;
			assert( *returnInfo != NULL );
			**returnInfo = info.Copy();
			}
		return JI2B(*cmd != NULL);
		}
	else if (this != CBGetCommandManager())
		{
		return (CBGetCommandManager())->Prepare(cmdName, projDoc, textDoc,
												cmd, returnInfo, cmdList);
		}
	else
		{
		const JCharacter* map[] =
			{
			"cmd", cmdName
			};
		const JString msg = JGetString("UnknownCmd::CBCommandManager", map, sizeof(map));
		(JGetUserNotification())->ReportError(msg);
		return kJFalse;
		}
}

// static private

JBoolean
CBCommandManager::Prepare
	(
	const CmdInfo&		info,
	CBProjectDocument*	projDoc,
	CBTextDocument*		textDoc,
	CBCommand**			cmd,
	JPtrArray<JString>*	cmdList
	)
{
	assert( textDoc != NULL );

	JBoolean onDisk;
	const JString fullName = textDoc->GetFullName(&onDisk);

	JTextEditor* te        = textDoc->GetTextEditor();
	const JIndex lineIndex = te->VisualLineIndexToCRLineIndex(te->GetLineForChar(te->GetInsertionIndex()));

	JString cmdPath = *(info.path);
	JString cmdStr  = *(info.cmd);
	return JI2B(Substitute(&cmdPath, &cmdStr, projDoc, fullName, lineIndex, onDisk) &&
				Add(cmdPath, cmdStr, info, projDoc, textDoc, NULL, NULL, cmd, cmdList));
}

/******************************************************************************
 Exec (file list)

	projDoc can be NULL

 ******************************************************************************/

void
CBCommandManager::Exec
	(
	const JIndex				cmdIndex,
	CBProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList
	)
{
	const CmdInfo info = itsCmdList->GetElement(cmdIndex);
	Exec(info, projDoc, fullNameList, lineIndexList);
}

// static

void
CBCommandManager::Exec
	(
	const CmdInfo&				info,
	CBProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList
	)
{
	CBCommand* cmd = NULL;
	JPtrArray<JString> cmdList(JPtrArrayT::kDeleteAll);
	if (Prepare(info, projDoc, fullNameList, lineIndexList, &cmd, &cmdList))
		{
		cmd->Start(info);
		}
}

/******************************************************************************
 Prepare (file list)

	projDoc can be NULL

 ******************************************************************************/

JBoolean
CBCommandManager::Prepare
	(
	const char*					cmdName,
	CBProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	CBCommand**					cmd,
	CmdInfo**					returnInfo,
	JPtrArray<JString>*			cmdList
	)
{
	*cmd        = NULL;
	*returnInfo = NULL;

	CmdInfo info;
	if (FindCommandName(cmdName, &info))
		{
		if (Prepare(info, projDoc, fullNameList, lineIndexList, cmd, cmdList))
			{
			*returnInfo = new CmdInfo;
			assert( *returnInfo != NULL );
			**returnInfo = info.Copy();
			}
		return JI2B(*cmd != NULL);
		}
	else if (this != CBGetCommandManager())
		{
		return (CBGetCommandManager())->Prepare(cmdName, projDoc, fullNameList,
												lineIndexList, cmd, returnInfo, cmdList);
		}
	else
		{
		const JCharacter* map[] =
			{
			"cmd", cmdName
			};
		const JString msg = JGetString("UnknownCmd::CBCommandManager", map, sizeof(map));
		(JGetUserNotification())->ReportError(msg);
		return kJFalse;
		}
}

// static private

JBoolean
CBCommandManager::Prepare
	(
	const CmdInfo&				info,
	CBProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	CBCommand**					cmd,
	JPtrArray<JString>*			cmdList
	)
{
	const JBoolean hasLines = !lineIndexList.IsEmpty();
	assert( !hasLines || lineIndexList.GetElementCount() == fullNameList.GetElementCount() );

	JSize count = 0;
	if ((info.cmd)->Contains("$full_name")        ||
		(info.cmd)->Contains("$relative_name")    ||
		(info.cmd)->Contains("$file_name")        ||
		(info.cmd)->Contains("$file_name_root")   ||
		(info.cmd)->Contains("$file_name_suffix") ||
		(info.cmd)->Contains("$full_path")        ||
		(info.cmd)->Contains("$relative_path")    ||
		(info.cmd)->Contains("$line")             ||
		(info.path)->GetFirstCharacter() == '@')
		{
		count = fullNameList.GetElementCount();
		JString cmdPath, cmdStr;
		for (JIndex i=1; i<=count; i++)
			{
			cmdPath = *(info.path);
			cmdStr  = *(info.cmd);
			if (Substitute(&cmdPath, &cmdStr, projDoc,
						   *(fullNameList.NthElement(i)),
						   (hasLines ? lineIndexList.GetElement(i) : 0),
						   kJTrue) &&
				Add(cmdPath, cmdStr, info, projDoc, NULL,
					&fullNameList, &lineIndexList, cmd, cmdList))
				{
				(**cmd).MarkEndOfSequence();
				}
			else
				{
				delete *cmd;
				*cmd = NULL;
				break;
				}
			}
		}

	if (count == 0)
		{
		JString cmdPath = *(info.path);
		JString cmdStr  = *(info.cmd);
		if (Substitute(&cmdPath, &cmdStr, projDoc, NULL, 0, kJTrue))
			{
			Add(cmdPath, cmdStr, info, projDoc, NULL,
				&fullNameList, &lineIndexList, cmd, cmdList);
			}
		}

	return JI2B(*cmd != NULL);
}

/******************************************************************************
 Add (static private)

 ******************************************************************************/

JBoolean
CBCommandManager::Add
	(
	const JCharacter*			path,
	const JCharacter*			cmdStr,
	const CmdInfo&				info,
	CBProjectDocument*			projDoc,
	CBTextDocument*				textDoc,
	const JPtrArray<JString>*	fullNameList,
	const JArray<JIndex>*		lineIndexList,
	CBCommand**					cmd,
	JPtrArray<JString>*			cmdList
	)
{
	if (*cmd == NULL)
		{
		*cmd = new CBCommand(path, info.isVCS, info.beepWhenFinished, projDoc);
		assert( *cmd != NULL );
		}

	if (path != (**cmd).GetPath())
		{
		CBCommand* subCmd = new CBCommand(path, info.isVCS, kJFalse, projDoc);
		if (subCmd->Add(cmdStr, textDoc, fullNameList, lineIndexList, cmdList))
			{
			(**cmd).Add(subCmd, info);
			return kJTrue;
			}
		else
			{
			delete subCmd;
			delete *cmd;
			*cmd = NULL;
			return kJFalse;
			}
		}
	else if ((**cmd).Add(cmdStr, textDoc, fullNameList, lineIndexList, cmdList))
		{
		return kJTrue;
		}
	else
		{
		delete *cmd;
		*cmd = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 Substitute (static private)

 ******************************************************************************/

JBoolean
CBCommandManager::Substitute
	(
	JString*			cmdPath,
	JString*			cmd,
	CBProjectDocument*	projDoc,
	const JCharacter*	fullName,
	const JIndex		lineIndex,
	const JBoolean		onDisk,
	const JBoolean		reportError
	)
{
	if (!cmdPath->IsEmpty() && cmdPath->GetFirstCharacter() == '@')
		{
		if (JStringEmpty(fullName) || !onDisk)
			{
			if (reportError)
				{
				(JGetUserNotification())->ReportError(
					JGetString(onDisk ? "RequiresFile::CBCommandManager" : "MustSaveText::CBCommandManager"));
				}
			return kJFalse;
			}

		JString p, n;
		JSplitPathAndName(fullName, &p, &n);
		cmdPath->ReplaceSubstring(1, 1, p);
		}

	if (projDoc == NULL && JIsRelativePath(*cmdPath))
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(JGetString("RequiresProject::CBCommandManager"));
			}
		return kJFalse;
		}
	else
		{
		const JCharacter* basePath = NULL;
		if (projDoc != NULL)
			{
			basePath = projDoc->GetFilePath();
			}

		JString fullPath;
		if (!JConvertToAbsolutePath(*cmdPath, basePath, &fullPath) ||
			!JDirectoryExists(fullPath))
			{
			if (reportError)
				{
				const JCharacter* map[] =
					{
					"path", cmdPath->GetCString()
					};
				const JString msg = JGetString("InvalidPath::CBCommandManager", map, sizeof(map));
				(JGetUserNotification())->ReportError(msg);
				}
			return kJFalse;
			}

		*cmdPath = fullPath;
		}

	JString projectPath, projectName, programName;
	if (projDoc != NULL)
		{
		projectPath = projDoc->GetFilePath();
		projectName = projDoc->GetName();
		programName = projDoc->GetBuildTargetName();
		}
	else if (cmd->Contains("$project_path")  ||
			 cmd->Contains("$project_name")  ||
			 cmd->Contains("$program")       ||
			 cmd->Contains("$relative_name") ||
			 cmd->Contains("$relative_path"))
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(JGetString("RequiresProject::CBCommandManager"));
			}
		return kJFalse;
		}

	if ((JStringEmpty(fullName) || !onDisk) &&
		(cmd->Contains("$full_name")        ||
		 cmd->Contains("$relative_name")    ||
		 cmd->Contains("$file_name")        ||
		 cmd->Contains("$file_name_root")   ||
		 cmd->Contains("$file_name_suffix") ||
		 cmd->Contains("$full_path")        ||
		 cmd->Contains("$relative_path")    ||
		 cmd->Contains("$line")))
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(
				JGetString(onDisk ? "RequiresFile::CBCommandManager" : "MustSaveText::CBCommandManager"));
			}
		return kJFalse;
		}

	JString fullPath, fileName, quotedFullName, relativePath, relativeName,
			fileNameRoot, fileNameSuffix;
	if (onDisk && !JStringEmpty(fullName))
		{
		JSplitPathAndName(fullName, &fullPath, &fileName);
		quotedFullName = JPrepArgForExec(fullName);

		if (!projectPath.IsEmpty())
			{
			relativePath = JConvertToRelativePath(fullPath, projectPath);
			relativeName = JConvertToRelativePath(fullName, projectPath);
			}

		if (JSplitRootAndSuffix(fileName, &fileNameRoot, &fileNameSuffix))
			{
			fileNameSuffix.PrependCharacter('.');
			}
		}

	projectPath    = JPrepArgForExec(projectPath);
	projectName    = JPrepArgForExec(projectName);
	programName    = JPrepArgForExec(programName);

	fullPath       = JPrepArgForExec(fullPath);
	relativePath   = JPrepArgForExec(relativePath);
	relativeName   = JPrepArgForExec(relativeName);
	fileName       = JPrepArgForExec(fileName);
	fileNameRoot   = JPrepArgForExec(fileNameRoot);
	fileNameSuffix = JPrepArgForExec(fileNameSuffix);

	const JString lineIndexStr(lineIndex, 0);

	const JCharacter* map[] =
		{
		"project_path",     projectPath.GetCString(),
		"project_name",     projectName.GetCString(),
		"program",          programName.GetCString(),

		// remember to update check in Exec()

		"full_name",        quotedFullName.GetCString(),
		"relative_name",    relativeName.GetCString(),
		"file_name",        fileName.GetCString(),
		"file_name_root",   fileNameRoot.GetCString(),
		"file_name_suffix", fileNameSuffix.GetCString(),

		"full_path",        fullPath.GetCString(),
		"relative_path",    relativePath.GetCString(),

		"line",             lineIndexStr.GetCString()
		};

	(JGetStringManager())->Replace(cmd, map, sizeof(map));
	return kJTrue;
}

/******************************************************************************
 FindCommandName (private)

 ******************************************************************************/

JBoolean
CBCommandManager::FindCommandName
	(
	const JCharacter*	name,
	CmdInfo*			info
	)
	const
{
	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		*info = itsCmdList->GetElement(i);
		if (*(info->name) == name)
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 AppendCommand

 ******************************************************************************/

void
CBCommandManager::AppendCommand
	(
	const JCharacter*	path,
	const JCharacter*	cmd,
	const JCharacter*	cmdName,

	const JBoolean		isMake,
	const JBoolean		isVCS,
	const JBoolean		saveAll,
	const JBoolean		oneAtATime,
	const JBoolean		useWindow,
	const JBoolean		raise,
	const JBoolean		beep,

	const JCharacter*	menuText,
	const JCharacter*	menuShortcut,
	const JBoolean		separator
	)
{
	JString* p = new JString(path);
	assert( p != NULL );

	JString* c = new JString(cmd);
	assert( c != NULL );

	JString* cn = new JString(cmdName);
	assert( cn != NULL );

	JString* mt = new JString(menuText);
	assert( mt != NULL );

	JString* ms = new JString(menuShortcut);
	assert( ms != NULL );

	JString* mi = new JString;
	assert( mi != NULL );

	CmdInfo info(p, c, cn,
				 isMake, isVCS, saveAll, oneAtATime,
				 useWindow, raise, beep,
				 mt, ms, mi, separator);
	itsCmdList->AppendElement(info);
}

/******************************************************************************
 AppendMenuItems

 ******************************************************************************/

void
CBCommandManager::AppendMenuItems
	(
	JXTextMenu*		menu,
	const JBoolean	hasProject
	)
	const
{
	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CmdInfo info = itsCmdList->GetElement(i);
		menu->AppendItem((info.menuText)->IsEmpty() ? *(info.cmd) : *(info.menuText));
		menu->SetItemNMShortcut(menu->GetItemCount(), *(info.menuShortcut));
		menu->SetItemID(menu->GetItemCount(), *(info.menuID));
		if (info.separator)
			{
			menu->ShowSeparatorAfter(menu->GetItemCount());
			}

		if (!hasProject && info.isMake)
			{
			menu->DisableItem(menu->GetItemCount());
			}
		}

	menu->ShowSeparatorAfter(menu->GetItemCount());
}

/******************************************************************************
 GetCurrentCmdInfoFileVersion (static)

 ******************************************************************************/

JFileVersion
CBCommandManager::GetCurrentCmdInfoFileVersion()
{
	return kCurrentSetupVersion;
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBCommandManager::ReadSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	if (ReadCommands(input, &itsMakeDependCmd, itsCmdList, &vers) && vers < 2)
		{
		UpdateMenuIDs();
		}
}

/******************************************************************************
 ReadCommands (static)

 ******************************************************************************/

JBoolean
CBCommandManager::ReadCommands
	(
	istream&		input,
	JString*		makeDependCmd,
	CmdList*		cmdList,
	JFileVersion*	returnVers
	)
{
	JFileVersion vers;
	input >> vers;
	if (input.fail() || vers > kCurrentSetupVersion)
		{
		return kJFalse;
		}

	input >> *makeDependCmd;

	cmdList->DeleteAll();

	if (vers <= 3)
		{
		JSize count;
		input >> count;

		for (JIndex i=1; i<=count; i++)
			{
			CmdInfo info = ReadCmdInfo(input, vers);
			UpgradeCommand(&info);
			cmdList->AppendElement(info);

			if (input.fail())
				{
				break;
				}
			}
		}
	else
		{
		while (1)
			{
			JBoolean keepGoing;
			input >> keepGoing;
			if (input.fail() || !keepGoing)
				{
				break;
				}

			CmdInfo info = ReadCmdInfo(input, vers);
			UpgradeCommand(&info);
			cmdList->AppendElement(info);
			}
		}

	if (returnVers != NULL)
		{
		*returnVers = vers;
		}
	return kJTrue;
}

/******************************************************************************
 ReadCmdInfo (static)

 ******************************************************************************/

CBCommandManager::CmdInfo
CBCommandManager::ReadCmdInfo
	(
	istream&			input,
	const JFileVersion	vers
	)
{
	JString* path = new JString;
	assert( path != NULL );
	input >> *path;

	JString* cmd = new JString;
	assert( cmd != NULL );
	input >> *cmd;

	JString* cmdName = new JString;
	assert( cmdName != NULL );
	input >> *cmdName;

	JBoolean isMake, saveAll, oneAtATime, useWindow, raise, beep;
	JBoolean isVCS = kJFalse;
	input >> isMake;
	if (vers >= 3)
		{
		input >> isVCS;
		}
	if (vers == 0)
		{
		JBoolean makeDepend;
		input >> makeDepend;
		}
	input >> saveAll >> oneAtATime >> useWindow >> raise >> beep;

	JString* menuText = new JString;
	assert( menuText != NULL );
	input >> *menuText;

	JString* menuShortcut = new JString;
	assert( menuShortcut != NULL );
	input >> *menuShortcut;

	JString* menuID = new JString;
	assert( menuID != NULL );
	if (vers >= 2)
		{
		input >> *menuID;
		}

	JBoolean separator;
	input >> separator;

	return CmdInfo(path, cmd, cmdName,
				   isMake, isVCS, saveAll, oneAtATime,
				   useWindow, raise, beep,
				   menuText, menuShortcut, menuID, separator);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBCommandManager::WriteSetup
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion << '\n';
	output << itsMakeDependCmd << '\n';

	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		output << kJTrue;
		WriteCmdInfo(output, itsCmdList->GetElement(i));
		}

	output << kJFalse << '\n';
}

/******************************************************************************
 WriteCmdInfo (static)

 ******************************************************************************/

void
CBCommandManager::WriteCmdInfo
	(
	ostream&		output,
	const CmdInfo&	info
	)
{
	output << ' ' << *(info.path);
	output << ' ' << *(info.cmd);
	output << ' ' << *(info.name);

	output << ' ' << info.isMake;
	output << ' ' << info.isVCS;
	output << ' ' << info.saveAll;
	output << ' ' << info.oneAtATime;
	output << ' ' << info.useWindow;
	output << ' ' << info.raiseWindowWhenStart;
	output << ' ' << info.beepWhenFinished;

	output << ' ' << *(info.menuText);
	output << ' ' << *(info.menuShortcut);
	output << ' ' << *(info.menuID);
	output << ' ' << info.separator;
	output << '\n';
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBCommandManager::ReadPrefs
	(
	istream& input
	)
{
	ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBCommandManager::WritePrefs
	(
	ostream& output
	)
	const
{
	WriteSetup(output);
}

/******************************************************************************
 UpgradeCommand (private static)

 ******************************************************************************/

static const JRegex svnChangeFullPathPattern =
	"\\bsvn (resolved|revert|commit|add|remove) \\$full_name\\b";

static const JRegex svnChangeRelativePathPattern =
	"\\bsvn (resolved|revert|commit|add|remove)\\b";

void
CBCommandManager::UpgradeCommand
	(
	CmdInfo* info
	)
{
	if (JProgramAvailable("nps_svn_client") &&
		!info->cmd->Contains("nps_svn_client"))
		{
		if (*(info->cmd) == "svn info $file_name; svn log $file_name" ||
			*(info->cmd) == "svn info $file_name"                     ||
			*(info->cmd) == "svn log $file_name"                      ||
			*(info->cmd) == "svn info $full_name; svn log $full_name" ||
			*(info->cmd) == "svn info $full_name"                     ||
			*(info->cmd) == "svn log $full_name")
			{
			*(info->cmd)               = "nps_svn_client --info-log $file_name";
			*(info->path)              = "@";
			info->oneAtATime           = kJTrue;
			info->useWindow            = kJFalse;
			info->raiseWindowWhenStart = kJFalse;
			info->beepWhenFinished     = kJFalse;
			}
		else if (svnChangeFullPathPattern.Match(*(info->cmd)))
			{
			*(info->cmd) += "; nps_svn_client --refresh-status $full_path";
			}
		else if (svnChangeRelativePathPattern.Match(*(info->cmd)))
			{
			*(info->cmd) += "; nps_svn_client --refresh-status .";
			}
		else if (*(info->cmd) == "svn update; jcc --reload-open" &&
				 (*(info->path) == "." || *(info->path) == "./"))
			{
			*(info->cmd)               = "nps_svn_client --update .";
			info->useWindow            = kJFalse;
			info->raiseWindowWhenStart = kJFalse;
			info->beepWhenFinished     = kJFalse;
			}
		else if (*(info->cmd) == "svn status" &&
				 (*(info->path) == "." || *(info->path) == "./"))
			{
			*(info->cmd)               = "nps_svn_client --status .";
			info->useWindow            = kJFalse;
			info->raiseWindowWhenStart = kJFalse;
			info->beepWhenFinished     = kJFalse;
			}
		}
}

/******************************************************************************
 ReadTemplate

 ******************************************************************************/

void
CBCommandManager::ReadTemplate
	(
	istream&				input,
	const JFileVersion		tmplVers,
	const JFileVersion		projVers
	)
{
	if (tmplVers >= 3)
		{
		ReadSetup(input);
		}
}

/******************************************************************************
 WriteTemplate

 ******************************************************************************/

void
CBCommandManager::WriteTemplate
	(
	ostream& output
	)
	const
{
	WriteSetup(output);
}

/******************************************************************************
 InitCommandList (private)

 ******************************************************************************/

struct DefCmd
{
	const JCharacter*	path;
	const JCharacter*	cmd;
	const JCharacter*	name;
	JBoolean			isMake;
	JBoolean			isVCS;
	JBoolean			saveAll;
	JBoolean			oneAtATime;
	JBoolean			useWindow;
	JBoolean			raiseWindowWhenStart;
	JBoolean			beepWhenFinished;
	const JCharacter*	menuTextID;
	const JCharacter*	menuShortcutID;
	JBoolean			separator;
};

static const DefCmd kDefCmd[] =
{
	// make

	{ "./", J_MAKE_BINARY_NAME " -kw all",
		"make_default",
		kJTrue, kJFalse, kJTrue, kJFalse, kJTrue, kJFalse, kJTrue,
		"DefCmdBuildText::CBCommandManager", "DefCmdBuildShortcut::CBCommandManager",
		kJFalse },
	{ "./", J_MAKE_BINARY_NAME " $relative_path$file_name_root.o",
		"",
		kJTrue, kJFalse, kJTrue, kJTrue, kJTrue, kJTrue, kJFalse,
		"DefCmdCompileText::CBCommandManager", "DefCmdCompileShortcut::CBCommandManager",
		kJFalse },
	{ "./", J_MAKE_BINARY_NAME " tidy",
		"",
		kJTrue, kJFalse, kJTrue, kJFalse, kJTrue, kJFalse, kJFalse,
		"DefCmdRemoveObjFilesText::CBCommandManager", "DefCmdRemoveObjFilesShortcut::CBCommandManager",
		kJTrue },

	// run

	{ "./", "$program",
		"",
		kJFalse, kJFalse, kJFalse, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdRunText::CBCommandManager", "DefCmdRunShortcut::CBCommandManager",
		kJFalse },
	{ "./", "java $program",
		"",
		kJFalse, kJFalse, kJFalse, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdRunJavaText::CBCommandManager", "DefCmdRunJavaShortcut::CBCommandManager",
		kJFalse },
	{ "./", "medic $program",
		"",
		kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdDebugText::CBCommandManager", "DefCmdDebugShorcut::CBCommandManager",
		kJFalse },
	{ "@", "medic -b +$line $file_name",
		"",
		kJFalse, kJFalse, kJFalse, kJTrue, kJFalse, kJFalse, kJFalse,
		"DefCmdSetBreakpointText::CBCommandManager", "DefCmdSetBreakpointShortcut::CBCommandManager",
		kJFalse },
	{ "@", "medic -f +$line $file_name",
		"",
		kJFalse, kJFalse, kJFalse, kJTrue, kJFalse, kJFalse, kJFalse,
		"DefCmdOpenDebuggerText::CBCommandManager", "DefCmdOpenDebuggerShortcut::CBCommandManager",
		kJTrue },

	// make & run

	{ "./", "&make_default ; $program",
		"",
		kJFalse, kJFalse, kJFalse, kJFalse, kJTrue, kJFalse, kJFalse,
		"DefCmdBuildRunText::CBCommandManager", "DefCmdBuildRunShortcut::CBCommandManager",
		kJFalse },
	{ "./", "&make_default ; medic $program",
		"",
		kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdBuildDebugText::CBCommandManager", "DefCmdBuildDebugShortcut::CBCommandManager",
		kJTrue },

	// git

	{ "@", "git checkout $file_name; jcc --reload-open",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdGitRevertText::CBCommandManager", "DefCmdVCSRevertShortcut::CBCommandManager",
		kJFalse },
	{ "@", "git commit $file_name",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdGitCheckInText::CBCommandManager", "DefCmdVCSCheckInShortcut::CBCommandManager",
		kJFalse },
	{ "@", "git add $file_name",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdGitAddText::CBCommandManager", "DefCmdVCSAddShortcut::CBCommandManager",
		kJFalse },
	{ "@", "git rm -f $file_name",
		"",
		kJFalse, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdGitRemoveText::CBCommandManager", "DefCmdVCSRemoveShortcut::CBCommandManager",
		kJTrue },

	{ "./", "git pull; jcc --reload-open",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdGitUpdateAllText::CBCommandManager", "DefCmdVCSUpdateAllShortcut::CBCommandManager",
		kJFalse },
	{ "./", "git status",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdGitDiffAllText::CBCommandManager", "DefCmdVCSDiffAllShortcut::CBCommandManager",
		kJFalse },
	{ "./", "git commit",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdGitCommitAllText::CBCommandManager", "DefCmdVCSCommitAllShortcut::CBCommandManager",
		kJTrue },

	// svn

	{ "@", "svn info $file_name; svn log $file_name",
		"",
		kJFalse, kJTrue, kJFalse, kJTrue, kJTrue, kJTrue, kJFalse,
		"DefCmdSVNInfoText::CBCommandManager", "DefCmdVCSInfoShortcut::CBCommandManager",
		kJFalse },
	{ "@", "svn resolved $file_name",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdSVNResolveText::CBCommandManager", "DefCmdVCSCheckOutShortcut::CBCommandManager",
		kJFalse },
	{ "@", "svn revert $file_name; jcc --reload-open",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdSVNRevertText::CBCommandManager", "DefCmdVCSRevertShortcut::CBCommandManager",
		kJFalse },
	{ "@", "svn commit $file_name",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdSVNCheckInText::CBCommandManager", "DefCmdVCSCheckInShortcut::CBCommandManager",
		kJFalse },
	{ "@", "svn add $file_name",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdSVNAddText::CBCommandManager", "DefCmdVCSAddShortcut::CBCommandManager",
		kJFalse },
	{ "@", "svn remove $file_name",
		"",
		kJFalse, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdSVNRemoveText::CBCommandManager", "DefCmdVCSRemoveShortcut::CBCommandManager",
		kJTrue },

	{ "./", "svn update; jcc --reload-open",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdSVNUpdateAllText::CBCommandManager", "DefCmdVCSUpdateAllShortcut::CBCommandManager",
		kJFalse },
	{ "./", "svn status",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdSVNStatusText::CBCommandManager", "DefCmdVCSStatusShortcut::CBCommandManager",
		kJFalse },
	{ "./", "svn diff --diff-cmd diff -x --brief",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdSVNDiffAllText::CBCommandManager", "DefCmdVCSDiffAllShortcut::CBCommandManager",
		kJFalse },
	{ "./", "svn commit",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdSVNCommitAllText::CBCommandManager", "DefCmdVCSCommitAllShortcut::CBCommandManager",
		kJTrue },
/*
	{ "./", "snv commit $project_name.jcc",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdSVNCheckInProjectText::CBCommandManager", "DefCmdVCSCheckInProjectShortcut::CBCommandManager",
		kJFalse },
	{ "./", "svn add $project_name.jcc",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdSVNAddProjectText::CBCommandManager", "DefCmdVCSAddProjectShortcut::CBCommandManager",
		kJTrue },
*/
	// cvs
/*
	{ "@", "cvs edit $file_name",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdCVSCheckOutText::CBCommandManager", "DefCmdVCSCheckOutShortcut::CBCommandManager",
		kJFalse },
	{ "@", "cvs unedit $file_name; jcc --reload-open",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdCVSRevertText::CBCommandManager", "DefCmdVCSRevertShortcut::CBCommandManager",
		kJFalse },
	{ "@", "cvs commit $file_name",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdCVSCheckInText::CBCommandManager", "DefCmdVCSCheckInShortcut::CBCommandManager",
		kJFalse },
	{ "@", "cvs add $file_name",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdCVSAddText::CBCommandManager", "DefCmdVCSAddShortcut::CBCommandManager",
		kJFalse },
	{ "@", "cvs remove -f $file_name",
		"",
		kJFalse, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdCVSRemoveText::CBCommandManager", "DefCmdVCSRemoveShortcut::CBCommandManager",
		kJTrue },

	{ "./", "cvs -q update -P -d; jcc --reload-open",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdCVSUpdateAllText::CBCommandManager", "DefCmdVCSUpdateAllShortcut::CBCommandManager",
		kJFalse },
	{ "./", "cvs -q diff --brief",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdCVSDiffAllText::CBCommandManager", "DefCmdVCSDiffAllShortcut::CBCommandManager",
		kJFalse },
	{ "./", "cvs -q commit",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdCVSCommitAllText::CBCommandManager", "DefCmdVCSCommitAllShortcut::CBCommandManager",
		kJTrue },
*//*
	{ "./", "cvs commit $project_name.jcc",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
		"DefCmdCVSCheckInProjectText::CBCommandManager", "DefCmdVCSCheckInProjectShortcut::CBCommandManager",
		kJFalse },
	{ "./", "cvs add $project_name.jcc",
		"",
		kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse,
		"DefCmdCVSAddProjectText::CBCommandManager", "DefCmdVCSAddProjectShortcut::CBCommandManager",
		kJTrue },
*/
};

const JSize kDefCmdCount = sizeof(kDefCmd) / sizeof(DefCmd);

void
CBCommandManager::InitCommandList()
{
	for (JIndex i=0; i<kDefCmdCount; i++)
		{
		AppendCommand(kDefCmd[i].path, kDefCmd[i].cmd, kDefCmd[i].name,
					  kDefCmd[i].isMake, kDefCmd[i].isVCS, kDefCmd[i].saveAll,
					  kDefCmd[i].oneAtATime, kDefCmd[i].useWindow,
					  kDefCmd[i].raiseWindowWhenStart,
					  kDefCmd[i].beepWhenFinished,
					  JGetString(kDefCmd[i].menuTextID),
					  JGetString(kDefCmd[i].menuShortcutID),
					  kDefCmd[i].separator);
		}

	for (JIndex i=1; i<=kDefCmdCount; i++)
		{
		*((itsCmdList->GetElement(i)).menuID) = GetUniqueMenuID();
		}
}

/******************************************************************************
 UpdateMenuIDs

 ******************************************************************************/

void
CBCommandManager::UpdateMenuIDs()
{
	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JString* id = (itsCmdList->GetElement(i)).menuID;
		if (id->IsEmpty())
			{
			*id = GetUniqueMenuID();
			}
		}
}

/******************************************************************************
 GetUniqueMenuID (private)

 ******************************************************************************/

JString
CBCommandManager::GetUniqueMenuID()
{
	JKLRand r;

	JString id;
	while (1)
		{
		id = "::CBCommandManager";

		for (JIndex i=1; i<=20; i++)
			{
			id.InsertCharacter((JCharacter) r.UniformLong(32, 126), 1);
			}

		JBoolean found = kJFalse;

		const JSize count = itsCmdList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (id == *((itsCmdList->GetElement(i)).menuID))
				{
				found = kJTrue;
				}
			}

		if (!found)
			{
			break;
			}
		}

	return id;
}

/******************************************************************************
 ConvertCompileDialog

 ******************************************************************************/

void
CBCommandManager::ConvertCompileDialog
	(
	istream&			input,
	const JFileVersion	vers,
	CBBuildManager*		buildMgr,
	const JBoolean		readWindGeom
	)
{
	if (vers < 3 || 61 < vers)
		{
		return;
		}

	if (vers >= 23 && readWindGeom)
		{
		JXWindow::SkipGeometry(input);
		}

	// path input
	JString path;
	input >> path;

	JString str;
	JPtrArray<JString> itemList(JPtrArrayT::kDeleteAll),
					   nmShortcutList(JPtrArrayT::kDeleteAll);
	if (vers >= 8)
		{
		// path history menu
		JXHistoryMenuBase::ReadSetup(input, &itemList, &nmShortcutList);
		}

	if (vers >= 14)
		{
		input >> itsMakeDependCmd;

		// make depend history menu
		JXHistoryMenuBase::ReadSetup(input, &itemList, &nmShortcutList);

		JBoolean makeDepend;
		input >> makeDepend;
		}

	// make command -- at top of menu
	input >> str;

	// make history menu
	JPtrArray<JString> makeList(JPtrArrayT::kDeleteAll);
	JXHistoryMenuBase::ReadSetup(input, &makeList, &nmShortcutList);

	// compile command -- at top of menu
	input >> str;

	// compile history menu
	JPtrArray<JString> compileList(JPtrArrayT::kDeleteAll);
	JXHistoryMenuBase::ReadSetup(input, &compileList, &nmShortcutList);

	JBoolean saveAll;
	input >> saveAll;

	if (vers >= 29)
		{
		JBoolean doubleSpace;
		input >> doubleSpace;
		}

	buildMgr->ConvertCompileDialog(input, vers);

	UpdateFileMarkers(JI2B(vers < 26), &itsMakeDependCmd);

	// save commands in menu

	JSize count = makeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		UpdateFileMarkers(JI2B(vers < 26), makeList.NthElement(i));
		AppendCommand(path, *(makeList.NthElement(i)), "",
					  kJTrue, kJFalse, kJTrue, kJFalse, kJTrue, kJFalse, kJTrue,
					  "", "", JI2B(i == count));
		}

	count = compileList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		UpdateFileMarkers(JI2B(vers < 26), compileList.NthElement(i));
		AppendCommand(path, *(compileList.NthElement(i)), "",
					  kJTrue, kJFalse, kJTrue, kJTrue, kJTrue, kJTrue, kJFalse,
					  "", "", JI2B(i == count));
		}
}

/******************************************************************************
 ConvertRunDialog

 ******************************************************************************/

void
CBCommandManager::ConvertRunDialog
	(
	istream&			input,
	const JFileVersion	vers,
	const JBoolean		readWindGeom
	)
{
	if (vers < 5 || 61 < vers)
		{
		return;
		}

	if (vers >= 23 && readWindGeom)
		{
		JXWindow::SkipGeometry(input);
		}

	// path input
	JString path;
	input >> path;

	JString str;
	JPtrArray<JString> itemList(JPtrArrayT::kDeleteAll),
					   nmShortcutList(JPtrArrayT::kDeleteAll);
	if (vers >= 8)
		{
		// path history menu
		JXHistoryMenuBase::ReadSetup(input, &itemList, &nmShortcutList);
		}

	// run command -- at top of menu
	input >> str;

	// run history menu
	JPtrArray<JString> runList(JPtrArrayT::kDeleteAll);
	JXHistoryMenuBase::ReadSetup(input, &runList, &nmShortcutList);

	// debug command -- at top of menu
	input >> str;

	// debug history menu
	JPtrArray<JString> debugList(JPtrArrayT::kDeleteAll);
	JXHistoryMenuBase::ReadSetup(input, &debugList, &nmShortcutList);

	JBoolean makeBeforeRun;
	input >> makeBeforeRun;

	if (vers >= 39)
		{
		JBoolean useOutputDoc;
		input >> useOutputDoc;
		}

	// save commands in menu

	JSize count = runList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		UpdateFileMarkers(JI2B(vers < 26), runList.NthElement(i));
		AppendCommand(path, *(runList.NthElement(i)), "",
					  kJFalse, kJFalse, kJFalse, kJFalse, kJTrue, kJTrue, kJFalse,
					  "", "", JI2B(i == count));
		}

	count = debugList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		UpdateFileMarkers(JI2B(vers < 26), debugList.NthElement(i));
		AppendCommand(path, *(debugList.NthElement(i)), "",
					  kJFalse, kJFalse, kJFalse, kJTrue, kJFalse, kJFalse, kJFalse,
					  "", "", JI2B(i == count));
		}
}

/******************************************************************************
 UpdateFileMarkers (private)

	Convert %[fpnrl] to $[fpnrl], when upgrading from vers < 26.

 ******************************************************************************/

static const JCharacter* kVarNameList = "fpnrl";

static const JCharacter* kOldVar[] =
{
	"$f", "$p", "$n", "$r", "$l"
};

static const JCharacter* kNewVar[] =
{
	"$full_name", "$full_path", "$file_name", "$file_name_root", "$line"
};

const JSize kVarCount = strlen(kVarNameList);

void
CBCommandManager::UpdateFileMarkers
	(
	const JBoolean	convertFromAncient,
	JString*		s
	)
{
	if (convertFromAncient)
		{
		JXWebBrowser::ConvertVarNames(s, kVarNameList);
		}

	// convert variable names

	for (JIndex j=0; j<kVarCount; j++)
		{
		JIndex i = 1;
		while (s->LocateNextSubstring(kOldVar[j], &i))
			{
			s->ReplaceSubstring(i, i+strlen(kOldVar[j])-1, kNewVar[j]);
			i += strlen(kNewVar[j]);
			}
		}
}

/******************************************************************************
 GetCompileDoc

 ******************************************************************************/

CBCompileDocument*
CBCommandManager::GetCompileDoc
	(
	CBProjectDocument* projDoc
	)
{
	if (itsCompileDoc == NULL)
		{
		itsCompileDoc = new CBCompileDocument(projDoc);
		assert( itsCompileDoc != NULL );
		ListenTo(itsCompileDoc);
		}

	return itsCompileDoc;
}

/******************************************************************************
 GetOutputDoc (static)

 ******************************************************************************/

CBExecOutputDocument*
CBCommandManager::GetOutputDoc()
{
	const JSize count = theExecDocList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBExecOutputDocument* doc =
			dynamic_cast<CBExecOutputDocument*>(theExecDocList.NthElement(i));
		assert( doc != NULL );
		if (!doc->ProcessRunning())
			{
			return doc;
			}
		}

	CBExecOutputDocument* doc = new CBExecOutputDocument();
	assert( doc != NULL );
	theExecDocList.Append(doc);
	(CBGetCommandManager())->ListenTo(doc);
	return doc;
}

/******************************************************************************
 Receive (virtual protected)

	Listen for new active project document.

 ******************************************************************************/

void
CBCommandManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(CBDocumentManager::kProjectDocumentActivated))
		{
		UpdateAllCommandMenus();
		}
	else if (message.Is(CBTextDocument::kSaved))
		{
		DocumentDeleted(sender);
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	Listen for closed CBExecOutputDocument

 ******************************************************************************/

void
CBCommandManager::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!DocumentDeleted(sender))
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 DocumentDeleted (private)

 ******************************************************************************/

JBoolean
CBCommandManager::DocumentDeleted
	(
	JBroadcaster* sender
	)
{
	if (sender == itsCompileDoc)
		{
		itsCompileDoc = NULL;
		return kJTrue;
		}

	const JSize count = theExecDocList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (sender == theExecDocList.NthElement(i))
			{
			theExecDocList.RemoveElement(i);
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 CmdList functions

 ******************************************************************************/

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
CBCommandManager::CmdList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdInfo info = GetElement(i);
		info.Free();
		}
	RemoveAll();
}

/******************************************************************************
 CmdInfo functions

 ******************************************************************************/

/******************************************************************************
 Copy

 ******************************************************************************/

CBCommandManager::CmdInfo
CBCommandManager::CmdInfo::Copy()
	const
{
	CmdInfo info = *this;

	info.path = new JString(*(this->path));
	assert( info.path != NULL );

	info.cmd = new JString(*(this->cmd));
	assert( info.cmd != NULL );

	info.name = new JString(*(this->name));
	assert( info.name != NULL );

	info.menuText = new JString(*(this->menuText));
	assert( info.menuText != NULL );

	info.menuShortcut = new JString(*(this->menuShortcut));
	assert( info.menuShortcut != NULL );

	info.menuID = new JString(*(this->menuID));
	assert( info.menuID != NULL );

	return info;
}

/******************************************************************************
 Free

 ******************************************************************************/

void
CBCommandManager::CmdInfo::Free()
{
	delete path;
	path = NULL;

	delete cmd;
	cmd = NULL;

	delete name;
	name = NULL;

	delete menuText;
	menuText = NULL;

	delete menuShortcut;
	menuShortcut = NULL;

	delete menuID;
	menuID = NULL;
}
