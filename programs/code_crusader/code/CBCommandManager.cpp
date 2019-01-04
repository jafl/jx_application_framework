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
	itsCmdList = jnew CmdList;
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
	jdelete itsCmdList;
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

	return Substitute(cmdStr, projDoc, "", 0, reportError);
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
	CmdInfo info(&cmdPath, &itsMakeDependCmd, NULL, NULL, NULL, NULL);

	JPtrArray<JString> emptyFullNameList(JPtrArrayT::kForgetAll);
	JArray<JIndex> emptyLineIndexList;

	CBCommand* cmd = NULL;
	// subroutines not allowed
	if (Prepare(info, projDoc, emptyFullNameList, emptyLineIndexList, &cmd, NULL) &&
		cmd->StartMakeProcess(compileDoc))
		{
		if (resultCmd != NULL)
			{
			*resultCmd = cmd;
			}
		return kJTrue;
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
	JBoolean onDisk;
	JString fullName = textDoc->GetFullName(&onDisk);
	assert( onDisk == JIsAbsolutePath(fullName) );

	JTextEditor* te        = textDoc->GetTextEditor();
	const JIndex lineIndex = te->VisualLineIndexToCRLineIndex(te->GetLineForChar(te->GetInsertionIndex()));

	JPtrArray<JString> fullNameList(JPtrArrayT::kForgetAll);
	fullNameList.Append(&fullName);

	JArray<JIndex> lineIndexList;
	lineIndexList.AppendElement(lineIndex);

	CBCommand* cmd = NULL;
	CBFunctionStack fnStack;
	if (Prepare(info, projDoc, fullNameList, lineIndexList, &cmd, &fnStack))
		{
		cmd->Start(info);
		}
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
	CBFunctionStack fnStack;
	if (Prepare(info, projDoc, fullNameList, lineIndexList, &cmd, &fnStack))
		{
		cmd->Start(info);
		}
}

/******************************************************************************
 Prepare

	projDoc can be NULL

 ******************************************************************************/

JBoolean
CBCommandManager::Prepare
	(
	const JString&				cmdName,
	CBProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	CBCommand**					cmd,
	CmdInfo**					returnInfo,
	CBFunctionStack*			fnStack
	)
{
	*cmd        = NULL;
	*returnInfo = NULL;

	CmdInfo info;
	if (FindCommandName(cmdName, &info))
		{
		if (Prepare(info, projDoc, fullNameList, lineIndexList, cmd, fnStack))
			{
			*returnInfo = jnew CmdInfo;
			assert( *returnInfo != NULL );
			**returnInfo = info.Copy();
			}
		return JI2B(*cmd != NULL);
		}
	else if (this != CBGetCommandManager())
		{
		return (CBGetCommandManager())->Prepare(cmdName, projDoc, fullNameList,
												lineIndexList, cmd, returnInfo, fnStack);
		}
	else
		{
		const JCharacter* map[] =
			{
			"cmd", cmdName.GetCString()
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
	CBFunctionStack*			fnStack
	)
{
	const JBoolean hasLines = !lineIndexList.IsEmpty();
	assert( !hasLines || lineIndexList.GetElementCount() == fullNameList.GetElementCount() );

	const JBoolean usesFiles = JI2B(
		info.cmd->Contains("$full_name")        ||
		info.cmd->Contains("$relative_name")    ||
		info.cmd->Contains("$file_name")        ||
		info.cmd->Contains("$file_name_root")   ||
		info.cmd->Contains("$file_name_suffix") ||
		info.cmd->Contains("$full_path")        ||
		info.cmd->Contains("$relative_path")    ||
		info.cmd->Contains("$line"));

	if (usesFiles && fullNameList.IsEmpty())
		{
		(JGetUserNotification())->ReportError(JGetString("RequiresFile::CBCommandManager"));
		return kJFalse;
		}

	CBCmdQueue cmdQueue(JPtrArrayT::kDeleteAll);
	if (!Parse(*info.cmd, &cmdQueue, fnStack))
		{
		return kJFalse;
		}

	if (usesFiles && info.oneAtATime)
		{
		const JSize count = fullNameList.GetElementCount();
		JString cmdPath;
		JPtrArray<JString>* cmdArgs = NULL;
		for (JIndex i=1; i<=count; i++)
			{
			cmdPath = *info.path;

			JPtrArray<JString> subFullNameList(JPtrArrayT::kForgetAll);
			subFullNameList.Append(const_cast<JString*>(fullNameList.NthElement(i)));

			JArray<JIndex> subLineIndexList;
			if (hasLines)
				{
				subLineIndexList.AppendElement(lineIndexList.GetElement(i));
				}

			if (BuildCmdPath(&cmdPath, projDoc, *fullNameList.NthElement(i), kJTrue) &&
				ProcessCmdQueue(cmdPath, cmdQueue, info, projDoc,
								subFullNameList, subLineIndexList,
								kJTrue, cmd, fnStack))
				{
				(**cmd).MarkEndOfSequence();
				}
			else
				{
				jdelete *cmd;
				*cmd = NULL;
				break;
				}
			}
		}
	else if (usesFiles && info.path->GetFirstCharacter() == '@')
		{
		// group files in the same directory

		JPtrArray<JString> nameList(fullNameList, JPtrArrayT::kForgetAll);
		JArray<JIndex> lineList(lineIndexList);

		JPtrArray<JString> samePathNameList(JPtrArrayT::kForgetAll);
		JArray<JIndex> samePathLineList;

		JString cmdPath, filePath, p, n;
		while (!nameList.IsEmpty())
			{
			cmdPath = *info.path;
			samePathNameList.RemoveAll();
			samePathLineList.RemoveAll();
			filePath.Clear();

			JSize nameCount = nameList.GetElementCount();
			for (JIndex i=1; i<=nameCount; i++)
				{
				const JString* fullName = nameList.NthElement(i);
				JSplitPathAndName(*fullName, &p, &n);
				if (filePath.IsEmpty() || p == filePath)
					{
					samePathNameList.Append(const_cast<JString*>(fullName));
					nameList.RemoveElement(i);

					if (hasLines)
						{
						samePathLineList.AppendElement(lineList.GetElement(i));
						lineList.RemoveElement(i);
						}

					filePath = p;
					nameCount--;
					i--;
					}
				}

			if (BuildCmdPath(&cmdPath, projDoc, *samePathNameList.FirstElement(), kJTrue) &&
				ProcessCmdQueue(cmdPath, cmdQueue, info, projDoc,
								samePathNameList, samePathLineList,
								kJTrue, cmd, fnStack))
				{
				(**cmd).MarkEndOfSequence();
				}
			else
				{
				jdelete *cmd;
				*cmd = NULL;
				break;
				}
			}
		}
	else
		{
		JString cmdPath = *info.path;
		if (BuildCmdPath(&cmdPath, projDoc, "", kJTrue))
			{
			ProcessCmdQueue(cmdPath, cmdQueue, info, projDoc,
							fullNameList, lineIndexList,
							kJTrue, cmd, fnStack);
			}
		}

	return JI2B(*cmd != NULL);
}

/******************************************************************************
 Parse (static private)

	fnStack is used to detect infinite loops.

 ******************************************************************************/

JBoolean
CBCommandManager::Parse
	(
	const JString&		origCmd,
	CBCmdQueue*			cmdQueue,
	CBFunctionStack*	fnStack
	)
{
	if (origCmd.IsEmpty())
		{
		return kJFalse;
		}

	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	JParseArgsForExec(origCmd, &argList);

	if (!argList.IsEmpty() && *(argList.LastElement()) != ";")
		{
		argList.Append(";");	// catch all commands inside loop
		}

	JPtrArray<JString> cmdArgs(JPtrArrayT::kDeleteAll);

	while (!argList.IsEmpty())
		{
		JString* arg = argList.FirstElement();
		if (*arg == ";")
			{
			if (!cmdArgs.IsEmpty())
				{
				// TODO: use move constructor
				JPtrArray<JString>* a = jnew JPtrArray<JString>(cmdArgs, JPtrArrayT::kDeleteAll);
				assert( a != NULL );
				cmdArgs.RemoveAll();
				cmdQueue->Append(a);
				}

			argList.DeleteElement(1);
			}
		else if (!arg->IsEmpty() && arg->GetFirstCharacter() == '&' &&
				 cmdArgs.IsEmpty())
			{
			if (fnStack == NULL)
				{
				(JGetUserNotification())->ReportError(JGetString("FnsNotAllowed::CBCommandManager"));
				return kJFalse;
				}
			else if (*argList.NthElement(2) != ";")
				{
				(JGetUserNotification())->ReportError(JGetString("ArgsNotAllowed::CBCommandManager"));
				return kJFalse;
				}

			JPtrArray<JString>* a = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
			assert( a != NULL );
			a->Append(arg);
			cmdQueue->Append(a);

			argList.RemoveElement(1);
			}
		else
			{
			cmdArgs.Append(arg);
			argList.RemoveElement(1);
			}
		}

	return kJTrue;
}

/******************************************************************************
 ProcessCmdQueue (static private)

 ******************************************************************************/

JBoolean
CBCommandManager::ProcessCmdQueue
	(
	const JString&				cmdPath,
	const CBCmdQueue&			cmdQueue,
	const CmdInfo&				info,
	CBProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	const JBoolean				reportError,
	CBCommand**					cmd,
	CBFunctionStack*			fnStack
	)
{
	JPtrArray<JString> args(JPtrArrayT::kDeleteAll);

	const JSize nameCount = fullNameList.GetElementCount();

	const JBoolean hasLines = !lineIndexList.IsEmpty();
	assert( !hasLines || lineIndexList.GetElementCount() == fullNameList.GetElementCount() );

	const JSize cmdCount = cmdQueue.GetElementCount();
	for (JIndex i=1; i<=cmdCount; i++)
		{
		args.CleanOut();

		const JPtrArray<JString>* cmdArgs = cmdQueue.NthElement(i);
		const JSize argCount = cmdArgs->GetElementCount();
		for (JIndex j=1; j<=argCount; j++)
			{
			const JString* cmdArg = cmdArgs->NthElement(j);

			if (fullNameList.IsEmpty())
				{
				JString* arg = jnew JString(*cmdArg);
				assert( arg != NULL );

				if (!Substitute(arg, projDoc, "", 0, reportError))
					{
					jdelete *cmd;
					*cmd = NULL;
					return kJFalse;
					}

				args.Append(arg);
				}
			else
				{
				for (JIndex k=1; k<=nameCount; k++)
					{
					JString* arg = jnew JString(*cmdArg);
					assert( arg != NULL );

					if (!Substitute(arg, projDoc,
									*(fullNameList.GetElement(k)),
									(hasLines ? lineIndexList.GetElement(k) : 0),
									reportError))
						{
						jdelete *cmd;
						*cmd = NULL;
						return kJFalse;
						}

					args.Append(arg);
					if (!UsesFile(*cmdArg))
						{
						break;
						}
					}
				}
			}

		if (!Add(cmdPath, args, info, projDoc, fullNameList, lineIndexList, cmd, fnStack))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 Add (static private)

 ******************************************************************************/

JBoolean
CBCommandManager::Add
	(
	const JString&				path,
	const JPtrArray<JString>&	cmdArgs,
	const CmdInfo&				info,
	CBProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	CBCommand**					cmd,
	CBFunctionStack*			fnStack
	)
{
	if (*cmd == NULL)
		{
		*cmd = jnew CBCommand(path, info.isVCS, info.beepWhenFinished, projDoc);
		assert( *cmd != NULL );
		}

	if (path != (**cmd).GetPath())
		{
		CBCommand* subCmd = jnew CBCommand(path, info.isVCS, kJFalse, projDoc);
		if (subCmd->Add(cmdArgs, fullNameList, lineIndexList, fnStack))
			{
			(**cmd).Add(subCmd, info);
			return kJTrue;
			}
		else
			{
			jdelete subCmd;
			jdelete *cmd;
			*cmd = NULL;
			return kJFalse;
			}
		}
	else if ((**cmd).Add(cmdArgs, fullNameList, lineIndexList, fnStack))
		{
		return kJTrue;
		}
	else
		{
		jdelete *cmd;
		*cmd = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 BuildCmdPath (static private)

 ******************************************************************************/

JBoolean
CBCommandManager::BuildCmdPath
	(
	JString*			cmdPath,
	CBProjectDocument*	projDoc,
	const JString&		fullName,
	const JBoolean		reportError
	)
{
	const JBoolean onDisk = JI2B( !JStringEmpty(fullName) && JIsAbsolutePath(fullName) );

	if (!cmdPath->IsEmpty() && cmdPath->GetFirstCharacter() == '@')
		{
		if (!onDisk)
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
		JCleanPath(cmdPath);
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
		return kJTrue;
		}
}

/******************************************************************************
 Substitute (static private)

 ******************************************************************************/

JBoolean
CBCommandManager::Substitute
	(
	JString*			arg,
	CBProjectDocument*	projDoc,
	const JString&		fullName,
	const JIndex		lineIndex,
	const JBoolean		reportError
	)
{
	const JBoolean onDisk = JI2B( !JStringEmpty(fullName) && JIsAbsolutePath(fullName) );

	JString projectPath, projectName, programName;
	if (projDoc != NULL)
		{
		projectPath = projDoc->GetFilePath();
		projectName = projDoc->GetName();
		programName = projDoc->GetBuildTargetName();
		}
	else if (arg->Contains("$project_path")  ||
			 arg->Contains("$project_name")  ||
			 arg->Contains("$program")       ||
			 arg->Contains("$relative_name") ||
			 arg->Contains("$relative_path"))
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(JGetString("RequiresProject::CBCommandManager"));
			}
		return kJFalse;
		}

	if (!onDisk && UsesFile(*arg))
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(
				JGetString(JStringEmpty(fullName) ?
					"RequiresFile::CBCommandManager" :
					"MustSaveText::CBCommandManager"));
			}
		return kJFalse;
		}

	JString fullPath, fileName, relativePath, relativeName,
			fileNameRoot, fileNameSuffix;
	if (onDisk)
		{
		JSplitPathAndName(fullName, &fullPath, &fileName);

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

	const JString lineIndexStr(lineIndex, 0);

	const JCharacter* map[] =
		{
		"project_path",     projectPath.GetCString(),
		"project_name",     projectName.GetCString(),
		"program",          programName.GetCString(),

		// remember to update check in Exec()

		"full_name",        fullName,
		"relative_name",    relativeName.GetCString(),
		"file_name",        fileName.GetCString(),
		"file_name_root",   fileNameRoot.GetCString(),
		"file_name_suffix", fileNameSuffix.GetCString(),

		"full_path",        fullPath.GetCString(),
		"relative_path",    relativePath.GetCString(),

		"line",             lineIndexStr.GetCString()
		};

	(JGetStringManager())->Replace(arg, map, sizeof(map));
	return kJTrue;
}

/******************************************************************************
 UsesFile (static private)

 ******************************************************************************/

JBoolean
CBCommandManager::UsesFile
	(
	const JString& arg
	)
{
	return JI2B(
		arg.Contains("$full_name")        ||
		arg.Contains("$relative_name")    ||
		arg.Contains("$file_name")        ||
		arg.Contains("$file_name_root")   ||
		arg.Contains("$file_name_suffix") ||
		arg.Contains("$full_path")        ||
		arg.Contains("$relative_path")    ||
		arg.Contains("$line"));
}

/******************************************************************************
 FindCommandName (private)

 ******************************************************************************/

JBoolean
CBCommandManager::FindCommandName
	(
	const JString&	name,
	CmdInfo*		info
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
	const JString&	path,
	const JString&	cmd,
	const JString&	cmdName,

	const JBoolean	isMake,
	const JBoolean	isVCS,
	const JBoolean	saveAll,
	const JBoolean	oneAtATime,
	const JBoolean	useWindow,
	const JBoolean	raise,
	const JBoolean	beep,

	const JString&	menuText,
	const JString&	menuShortcut,
	const JBoolean	separator
	)
{
	JString* p = jnew JString(path);
	assert( p != NULL );

	JString* c = jnew JString(cmd);
	assert( c != NULL );

	JString* cn = jnew JString(cmdName);
	assert( cn != NULL );

	JString* mt = jnew JString(menuText);
	assert( mt != NULL );

	JString* ms = jnew JString(menuShortcut);
	assert( ms != NULL );

	JString* mi = jnew JString;
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
		menu->AppendItem((info.menuText)->IsEmpty() ? *info.cmd : *info.menuText);
		menu->SetItemNMShortcut(menu->GetItemCount(), *info.menuShortcut);
		menu->SetItemID(menu->GetItemCount(), *info.menuID);
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
	std::istream& input
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
	std::istream&	input,
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
	std::istream&			input,
	const JFileVersion	vers
	)
{
	JString* path = jnew JString;
	assert( path != NULL );
	input >> *path;

	JString* cmd = jnew JString;
	assert( cmd != NULL );
	input >> *cmd;

	JString* cmdName = jnew JString;
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

	JString* menuText = jnew JString;
	assert( menuText != NULL );
	input >> *menuText;

	JString* menuShortcut = jnew JString;
	assert( menuShortcut != NULL );
	input >> *menuShortcut;

	JString* menuID = jnew JString;
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
	std::ostream& output
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
	std::ostream&		output,
	const CmdInfo&	info
	)
{
	output << ' ' << *info.path;
	output << ' ' << *info.cmd;
	output << ' ' << *info.name;

	output << ' ' << info.isMake;
	output << ' ' << info.isVCS;
	output << ' ' << info.saveAll;
	output << ' ' << info.oneAtATime;
	output << ' ' << info.useWindow;
	output << ' ' << info.raiseWindowWhenStart;
	output << ' ' << info.beepWhenFinished;

	output << ' ' << *info.menuText;
	output << ' ' << *info.menuShortcut;
	output << ' ' << *info.menuID;
	output << ' ' << info.separator;
	output << '\n';
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBCommandManager::ReadPrefs
	(
	std::istream& input
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
	std::ostream& output
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
	std::istream&				input,
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
	std::ostream& output
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
		kJTrue, kJFalse, kJTrue, kJFalse, kJTrue, kJTrue, kJFalse,
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
	{ "./", "medic -b +$line $file_name",
		"",
		kJFalse, kJFalse, kJFalse, kJTrue, kJFalse, kJFalse, kJFalse,
		"DefCmdSetBreakpointText::CBCommandManager", "DefCmdSetBreakpointShortcut::CBCommandManager",
		kJFalse },
	{ "./", "medic -f +$line $file_name",
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

	{ "@", "git log -p $file_name",
		"",
		kJFalse, kJTrue, kJFalse, kJTrue, kJTrue, kJTrue, kJFalse,
		"DefCmdGitInfoText::CBCommandManager", "DefCmdVCSInfoShortcut::CBCommandManager",
		kJFalse },
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
	std::istream&			input,
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
	std::istream&			input,
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
		itsCompileDoc = jnew CBCompileDocument(projDoc);
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

	CBExecOutputDocument* doc = jnew CBExecOutputDocument();
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

	info.path = jnew JString(*(this->path));
	assert( info.path != NULL );

	info.cmd = jnew JString(*(this->cmd));
	assert( info.cmd != NULL );

	info.name = jnew JString(*(this->name));
	assert( info.name != NULL );

	info.menuText = jnew JString(*(this->menuText));
	assert( info.menuText != NULL );

	info.menuShortcut = jnew JString(*(this->menuShortcut));
	assert( info.menuShortcut != NULL );

	info.menuID = jnew JString(*(this->menuID));
	assert( info.menuID != NULL );

	return info;
}

/******************************************************************************
 Free

 ******************************************************************************/

void
CBCommandManager::CmdInfo::Free()
{
	jdelete path;
	path = NULL;

	jdelete cmd;
	cmd = NULL;

	jdelete name;
	name = NULL;

	jdelete menuText;
	menuText = NULL;

	jdelete menuShortcut;
	menuShortcut = NULL;

	jdelete menuID;
	menuID = NULL;
}
