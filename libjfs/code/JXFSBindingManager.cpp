/******************************************************************************
 JXFSBindingManager.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include "JXFSBindingManager.h"
#include "JFSBindingList.h"
#include "JXFSEditBindingsDialog.h"
#include "JXFSRunCommandDialog.h"
#include "JXFSRunFileDialog.h"
#include "JXFSRunScriptDialog.h"
#include <JXHelpManager.h>
#include <JXTimerTask.h>
#include <jXGlobals.h>
#include <JSimpleProcess.h>
#include <JOrderedSetUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

JXFSBindingManager* JXFSBindingManager::itsSelf = NULL;

const Time kUpdateInterval = 1000;	// milliseconds

static const JCharacter* kJFSBindingEditorHelpName = "JFSBindingEditorHelp";
static const JCharacter* kJFSRunCommandHelpName    = "JFSRunCommandHelp";
static const JCharacter* kJFSRunFileHelpName       = "JFSRunFileHelp";
static const JCharacter* kJXRegexHelpName          = "JXRegexHelp";
static const JCharacter* kJXRegexQRefHelpName      = "JXRegexQRef";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean initSelf = kJFalse;

JXFSBindingManager*
JXFSBindingManager::Instance()
{
	if (itsSelf == NULL && !initSelf)
		{
		initSelf = kJTrue;

		const JCharacter* needUserCheck;
		itsSelf = jnew JXFSBindingManager(&needUserCheck);
		assert( itsSelf != NULL );

		initSelf = kJFalse;

		if (!JStringEmpty(needUserCheck))
			{
			EditBindings();
			(JGetUserNotification())->DisplayMessage(needUserCheck);
			(itsSelf->itsBindingList)->Save();	// only display message once
			}
		}

	return itsSelf;
}

/******************************************************************************
 Destroy (static)

 ******************************************************************************/

void
JXFSBindingManager::Destroy()
{
	jdelete itsSelf;
	itsSelf = NULL;

	initSelf = kJFalse;
}

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

JXFSBindingManager::JXFSBindingManager
	(
	const JCharacter** needUserCheck
	)
{
	itsRunFileDialog   = NULL;
	itsFileList        = NULL;
	itsRunScriptDialog = NULL;
	itsEditDialog      = NULL;

	itsBindingList = JFSBindingList::Create(needUserCheck);

	itsUpdateBindingListTask = jnew JXTimerTask(kUpdateInterval);
	assert( itsUpdateBindingListTask != NULL );
	itsUpdateBindingListTask->Start();
	ListenTo(itsUpdateBindingListTask);

	itsRunCmdDialog = jnew JXFSRunCommandDialog;
	assert( itsRunCmdDialog != NULL );

	(JXGetHelpManager())->RegisterSection(kJFSBindingEditorHelpName);
	(JXGetHelpManager())->RegisterSection(kJFSRunCommandHelpName);
	(JXGetHelpManager())->RegisterSection(kJFSRunFileHelpName);
	(JXGetHelpManager())->RegisterSection(kJXRegexHelpName);
	(JXGetHelpManager())->RegisterSection(kJXRegexQRefHelpName);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXFSBindingManager::~JXFSBindingManager()
{
	jdelete itsBindingList;
	jdelete itsUpdateBindingListTask;
	jdelete itsFileList;
}

/******************************************************************************
 EditBindings (static)

 *****************************************************************************/

void
JXFSBindingManager::EditBindings()
{
	JXFSBindingManager* me = Instance();

	if (me->itsEditDialog == NULL)
		{
		me->itsEditDialog = jnew JXFSEditBindingsDialog(me->itsBindingList);
		assert( me->itsEditDialog != NULL );
		me->ListenTo(me->itsEditDialog);
		}

	me->itsEditDialog->Activate();
}

/******************************************************************************
 Exec (static)

	Let the user choose which program to run in the given directory.

 ******************************************************************************/

void
JXFSBindingManager::Exec
	(
	const JCharacter* path
	)
{
	JXFSBindingManager* me = Instance();

	me->itsRunCmdDialog->Activate();
	me->itsRunCmdDialog->SetPath(path);
}

/******************************************************************************
 Exec (static)

	Run the given *program* in its directory.  To run a program for a given
	*file*, use Exec(JPtrArray<JString>,...).

 ******************************************************************************/

void
JXFSBindingManager::Exec
	(
	const JCharacter*	fullProgramName,
	const JBoolean		askForArgs
	)
{
	JXFSBindingManager* me = Instance();

	JString cmd;
	JSplitPathAndName(fullProgramName, &(me->itsScriptPath), &cmd);

	#ifdef _J_UNIX
	cmd.Prepend("./");
	#endif

	if (askForArgs && me->itsRunScriptDialog == NULL)
		{
		me->itsRunScriptDialog = jnew JXFSRunScriptDialog(cmd);
		assert( me->itsRunScriptDialog != NULL );
		me->ListenTo(me->itsRunScriptDialog);
		me->itsRunScriptDialog->BeginDialog();
		}
	else if (!askForArgs)
		{
		(JXGetApplication())->DisplayBusyCursor();
		Exec(me->itsScriptPath, cmd, JFSBinding::kRunPlain);
		}
}

/******************************************************************************
 Exec (static)

	Run a program on each of the specified files.

 ******************************************************************************/

void
JXFSBindingManager::Exec
	(
	const JPtrArray<JString>&	fileList,
	const JBoolean				ignoreBindings
	)
{
	if (fileList.IsEmpty())
		{
		return;
		}

	JXFSBindingManager* me = Instance();

	if (me->itsFileList == NULL)
		{
		me->itsFileList = jnew JPtrArray<JFSBinding>(JPtrArrayT::kDeleteAll);
		assert( me->itsFileList != NULL );
		me->itsFileList->SetCompareFunction(ComparePatterns);
		}

	const JSize count = fileList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* fileName = fileList.NthElement(i);

		JFSBinding* f = jnew JFSBinding(*fileName, "", JFSBinding::kRunPlain, kJTrue, kJFalse);
		assert( f != NULL );
		if (!me->itsFileList->InsertSorted(f, kJFalse))
			{
			jdelete f;
			}
		}

	me->itsIgnoreBindingsFlag = ignoreBindings;
	me->ProcessFiles();
}

/******************************************************************************
 HasFiles (private)

 ******************************************************************************/

inline JBoolean
JXFSBindingManager::HasFiles()
	const
{
	return JI2B( itsFileList != NULL && !itsFileList->IsEmpty() );
}

/******************************************************************************
 ProcessFiles (private)

 ******************************************************************************/

void
JXFSBindingManager::ProcessFiles()
{
	if (!HasFiles() || itsRunFileDialog != NULL)
		{
		return;
		}

	// check for file with no command

	JSize count = itsFileList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JFSBinding* f           = itsFileList->NthElement(i);
		const JString& fileName = f->GetPattern();

		JFSBinding::CommandType type;
		JBoolean singleFile;
		const JString& cmd = f->GetCommand(&type, &singleFile);

		const JFSBinding* b;
		if (!itsIgnoreBindingsFlag &&
			cmd.IsEmpty() && itsBindingList->GetBinding(fileName, &b))
			{
			const JString& cmd = b->GetCommand(&type, &singleFile);
			f->SetCommand(cmd, type, singleFile);
			}
		else if (cmd.IsEmpty())
			{
			assert( itsRunFileDialog == NULL );
			itsRunFileDialog =
				jnew JXFSRunFileDialog(fileName, JNegate(count > 1 && itsIgnoreBindingsFlag));
			assert( itsRunFileDialog != NULL );
			ListenTo(itsRunFileDialog);
			itsRunFileDialog->BeginDialog();
			itsRunFileIndex = i;
			return;
			}
		}

	// exec one-at-a-time cmds

	(JXGetApplication())->DisplayBusyCursor();

	for (JIndex i=1; i<=count; i++)
		{
		JFSBinding::CommandType t;
		JBoolean singleFile;
		(itsFileList->NthElement(i))->GetCommand(&t, &singleFile);

		if (singleFile)
			{
			Exec(i, i);
			itsFileList->DeleteElement(i);
			count--;
			i--;	// compensate for shift
			}
		}

	// group remaining files and exec

	if (count > 0)
		{
		itsFileList->SetCompareFunction(CompareCommands);
		itsFileList->Sort();

		JIndex startIndex = 1;
		JString cmd;
		JFSBinding::CommandType type = JFSBinding::kRunPlain;

		for (JIndex i=1; i<=count; i++)
			{
			JFSBinding* f           = itsFileList->NthElement(i);
			const JString& fileName = f->GetPattern();

			JFSBinding::CommandType t;
			JBoolean singleFile;
			const JString& c = f->GetCommand(&t, &singleFile);
			assert( !singleFile );

			if (i == startIndex)
				{
				cmd  = c;
				type = t;
				}
			else if (c != cmd || type != t)
				{
				Exec(startIndex, i-1);

				startIndex = i;
				cmd        = c;
				type       = t;
				}
			}

		Exec(startIndex, count);
		}

	jdelete itsFileList;
	itsFileList = NULL;
}

/******************************************************************************
 Exec (private)

	The directory and command *must* be the same for all items in
	itsFileList between startIndex and endIndex, inclusive.

 ******************************************************************************/

void
JXFSBindingManager::Exec
	(
	const JIndex startIndex,
	const JIndex endIndex
	)
{
	// check if same path for all files

	JBoolean samePath = kJTrue;

	JString path, name;
	if (endIndex > startIndex)
		{
		JString p;
		for (JIndex i=startIndex; i<=endIndex; i++)
			{
			const JString& fullName = (itsFileList->NthElement(i))->GetPattern();
			JSplitPathAndName(fullName, &p, &name);
			if (i > startIndex && p != path)
				{
				samePath = kJFalse;
				break;
				}
			path = p;
			}
		}

	// build $q, $u, $qf, $uf

	JString q, u, qf, uf;
	for (JIndex i=startIndex; i<=endIndex; i++)
		{
		const JString& fullName = (itsFileList->NthElement(i))->GetPattern();

		if (samePath)
			{
			JSplitPathAndName(fullName, &path, &name);
			}
		else
			{
			name = fullName;
			}

		q += JPrepArgForExec(name);
		q += " ";

		u += name;
		u += " ";

		qf += JPrepArgForExec(fullName);
		qf += " ";

		uf += fullName;
		uf += " ";
		}

	// run command

	JFSBinding* f = itsFileList->NthElement(startIndex);
	JSplitPathAndName(f->GetPattern(), &path, &name);

	JFSBinding::CommandType type;
	JBoolean singleFile;
	JString cmd = f->GetCommand(&type, &singleFile);

	assert( ( singleFile && startIndex == endIndex) ||
			(!singleFile && startIndex <= endIndex) );

	BuildCommand(&cmd, q, u, qf, uf);
	Exec(path, cmd, type);
}

/******************************************************************************
 Exec (static)

 ******************************************************************************/

static const JCharacter* kShellMetaCharList = "|&;()<>$`'!{}*?@#~[]%";

JError
JXFSBindingManager::Exec
	(
	const JCharacter*				path,
	const JCharacter*				origCmd,
	const JFSBinding::CommandType	type
	)
{
	JXFSBindingManager* me = Instance();

	JString cmd = origCmd, u, q;

	// build shell command

	if (type == JFSBinding::kRunInShell || type == JFSBinding::kRunInWindow ||
		(me->itsBindingList->WillAutoUseShellCommand() &&
		 strpbrk(origCmd, kShellMetaCharList) != NULL))
		{
		u   = cmd;
		q   = JPrepArgForExec(cmd);
		cmd = me->itsBindingList->GetShellCommand();
		BuildCommand(&cmd, q, u, "", "");
		}

	// build window command

	if (type == JFSBinding::kRunInWindow)
		{
		u   = cmd;
		q   = JPrepArgForExec(cmd);
		cmd = me->itsBindingList->GetWindowCommand();
		BuildCommand(&cmd, q, u, "", "");
		}

	// exec command

	return JSimpleProcess::Create(path, cmd, kJTrue);
}

/******************************************************************************
 BuildCommand (static private)

 ******************************************************************************/

void
JXFSBindingManager::BuildCommand
	(
	JString*			cmd,
	const JCharacter*	q,
	const JCharacter*	u,
	const JCharacter*	qf,
	const JCharacter*	uf
	)
{
	if (cmd->Contains("$"))
		{
		const JCharacter* map[] =
			{
			"q",  q,
			"u",  u,
			"qf", qf,
			"uf", uf
			};
		(JGetStringManager())->Replace(cmd, map, sizeof(map));
		}
	else
		{
		*cmd += " ";
		*cmd += qf;
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXFSBindingManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		itsEditDialog = NULL;
		}

	else if (sender == itsRunFileDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			assert( HasFiles() );

			JFSBinding::CommandType type;
			JBoolean singleFile, saveBinding;
			const JString& cmd = itsRunFileDialog->GetCommand(&type, &singleFile, &saveBinding);

			if (itsIgnoreBindingsFlag && itsRunFileIndex == 1)
				{
				JSize count = itsFileList->GetElementCount();
				for (JIndex i=1; i<=count; i++)
					{
					JFSBinding* f = itsFileList->NthElement(i);
					f->SetCommand(cmd, type, singleFile);
					}
				}
			else
				{
				JFSBinding* f = itsFileList->NthElement(itsRunFileIndex);
				f->SetCommand(cmd, type, singleFile);
				}

			if (saveBinding)
				{
				JFSBinding* f = itsFileList->NthElement(itsRunFileIndex);
				SaveBinding(f->GetPattern(), cmd, type, singleFile);
				}

			itsRunFileDialog = NULL;
			ProcessFiles();
			}
		else
			{
			jdelete itsFileList;
			itsFileList      = NULL;
			itsRunFileDialog = NULL;
			}
		}

	else if (sender == itsRunScriptDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			JFSBinding::CommandType type;
			const JString& cmd = itsRunScriptDialog->GetCommand(&type);
			Exec(itsScriptPath, cmd, type);
			}
		itsRunScriptDialog = NULL;
		}

	else if (sender == itsUpdateBindingListTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		if (itsEditDialog == NULL && itsRunFileDialog == NULL)
			{
			itsBindingList->RevertIfModified();
			}
		else if (itsEditDialog != NULL)
			{
			itsEditDialog->CheckIfNeedRevert();
			}
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 SaveBinding (private)

 ******************************************************************************/

void
JXFSBindingManager::SaveBinding
	(
	const JCharacter*				fileName,
	const JCharacter*				cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile
	)
{
	JString root, suffix;
	if (JSplitRootAndSuffix(fileName, &root, &suffix))
		{
		JFSBindingList::CleanFileName(&suffix);		// ignore # and ~ on end
		suffix.PrependCharacter('.');

		itsBindingList->RevertIfModified();			// make sure we Save() latest
		itsBindingList->SetCommand(suffix, cmd, type, singleFile);
		itsBindingList->Save();						// ignore error:  no point in complaining

		if (itsEditDialog != NULL)
			{
			itsEditDialog->AddBinding(suffix, cmd, type, singleFile);
			}
		}
}

/******************************************************************************
 ComparePatterns (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JXFSBindingManager::ComparePatterns
	(
	JFSBinding* const & b1,
	JFSBinding* const & b2
	)
{
	return JCompareStringsCaseSensitive(const_cast<JString*>(&(b1->GetPattern())),
										const_cast<JString*>(&(b2->GetPattern())));
}

/******************************************************************************
 CompareCommands (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JXFSBindingManager::CompareCommands
	(
	JFSBinding* const & b1,
	JFSBinding* const & b2
	)
{
	JFSBinding::CommandType t1, t2;
	JBoolean s1, s2;
	const JString& c1 = b1->GetCommand(&t1, &s1);
	const JString& c2 = b2->GetCommand(&t2, &s2);

	JOrderedSetT::CompareResult result =
		JCompareStringsCaseSensitive(const_cast<JString*>(&c1),
									 const_cast<JString*>(&c2));
	if (result == JOrderedSetT::kFirstEqualSecond)
		{
		result = JCompareIndices(t1, t2);
		}
	return result;
}
