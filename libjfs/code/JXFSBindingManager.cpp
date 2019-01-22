/******************************************************************************
 JXFSBindingManager.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1999 by Glenn W. Bach.

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
#include <JListUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

JXFSBindingManager* JXFSBindingManager::itsSelf = nullptr;

const Time kUpdateInterval = 1000;	// milliseconds

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean initSelf = kJFalse;

JXFSBindingManager*
JXFSBindingManager::Instance()
{
	if (itsSelf == nullptr && !initSelf)
		{
		initSelf = kJTrue;

		JString needUserCheck;
		itsSelf = jnew JXFSBindingManager(&needUserCheck);
		assert( itsSelf != nullptr );

		initSelf = kJFalse;

		if (!needUserCheck.IsEmpty())
			{
			EditBindings();
			JGetUserNotification()->DisplayMessage(needUserCheck);
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
	itsSelf = nullptr;

	initSelf = kJFalse;
}

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

JXFSBindingManager::JXFSBindingManager
	(
	JString* needUserCheck
	)
{
	itsRunFileDialog   = nullptr;
	itsFileList        = nullptr;
	itsRunScriptDialog = nullptr;
	itsEditDialog      = nullptr;

	itsBindingList = JFSBindingList::Create(needUserCheck);

	itsUpdateBindingListTask = jnew JXTimerTask(kUpdateInterval);
	assert( itsUpdateBindingListTask != nullptr );
	itsUpdateBindingListTask->Start();
	ListenTo(itsUpdateBindingListTask);

	itsRunCmdDialog = jnew JXFSRunCommandDialog;
	assert( itsRunCmdDialog != nullptr );
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

	if (me->itsEditDialog == nullptr)
		{
		me->itsEditDialog = jnew JXFSEditBindingsDialog(me->itsBindingList);
		assert( me->itsEditDialog != nullptr );
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
	const JString& path
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
	const JString&	fullProgramName,
	const JBoolean	askForArgs
	)
{
	JXFSBindingManager* me = Instance();

	JString cmd;
	JSplitPathAndName(fullProgramName, &(me->itsScriptPath), &cmd);

	#ifdef _J_UNIX
	cmd.Prepend("./");
	#endif

	if (askForArgs && me->itsRunScriptDialog == nullptr)
		{
		me->itsRunScriptDialog = jnew JXFSRunScriptDialog(cmd);
		assert( me->itsRunScriptDialog != nullptr );
		me->ListenTo(me->itsRunScriptDialog);
		me->itsRunScriptDialog->BeginDialog();
		}
	else if (!askForArgs)
		{
		JXGetApplication()->DisplayBusyCursor();
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

	if (me->itsFileList == nullptr)
		{
		me->itsFileList = jnew JPtrArray<JFSBinding>(JPtrArrayT::kDeleteAll);
		assert( me->itsFileList != nullptr );
		me->itsFileList->SetCompareFunction(ComparePatterns);
		}

	for (const JString* fileName : fileList)
		{
		JFSBinding* f = jnew JFSBinding(*fileName, JString::empty, JFSBinding::kRunPlain, kJTrue, kJFalse);
		assert( f != nullptr );
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
	return JI2B( itsFileList != nullptr && !itsFileList->IsEmpty() );
}

/******************************************************************************
 ProcessFiles (private)

 ******************************************************************************/

void
JXFSBindingManager::ProcessFiles()
{
	if (!HasFiles() || itsRunFileDialog != nullptr)
		{
		return;
		}

	// check for file with no command

	JIndex i = 1;
	for (JFSBinding* f : *itsFileList)
		{
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
			assert( itsRunFileDialog == nullptr );
			itsRunFileDialog =
				jnew JXFSRunFileDialog(fileName,
					JNegate(itsFileList->GetElementCount() > 1 && itsIgnoreBindingsFlag));
			assert( itsRunFileDialog != nullptr );
			ListenTo(itsRunFileDialog);
			itsRunFileDialog->BeginDialog();
			itsRunFileIndex = i;
			return;
			}

		i++;
		}

	// exec one-at-a-time cmds

	JXGetApplication()->DisplayBusyCursor();

	for (i=itsFileList->GetElementCount(); i>=1; i--)
		{
		JFSBinding::CommandType t;
		JBoolean singleFile;
		itsFileList->GetElement(i)->GetCommand(&t, &singleFile);

		if (singleFile)
			{
			Exec(i, i);
			itsFileList->DeleteElement(i);
			}
		}

	// group remaining files and exec

	if (!itsFileList->IsEmpty())
		{
		itsFileList->SetCompareFunction(CompareCommands);
		itsFileList->Sort();

		JIndex startIndex = 1;
		JString cmd;
		JFSBinding::CommandType type = JFSBinding::kRunPlain;

		i = 1;
		for (JFSBinding* f : *itsFileList)
			{
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

			i++;
			}

		Exec(startIndex, itsFileList->GetElementCount());
		}

	jdelete itsFileList;
	itsFileList = nullptr;
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
			const JString& fullName = (itsFileList->GetElement(i))->GetPattern();
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
		const JString& fullName = (itsFileList->GetElement(i))->GetPattern();

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

	JFSBinding* f = itsFileList->GetElement(startIndex);
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

static const JUtf8Byte* kShellMetaCharList = "|&;()<>$`'!{}*?@#~[]%";

JError
JXFSBindingManager::Exec
	(
	const JString&					path,
	const JString&					origCmd,
	const JFSBinding::CommandType	type
	)
{
	JXFSBindingManager* me = Instance();

	JString cmd = origCmd, u, q;

	// build shell command

	if (type == JFSBinding::kRunInShell || type == JFSBinding::kRunInWindow ||
		(me->itsBindingList->WillAutoUseShellCommand() &&
		 strpbrk(origCmd.GetBytes(), kShellMetaCharList) != nullptr))
		{
		u   = cmd;
		q   = JPrepArgForExec(cmd);
		cmd = me->itsBindingList->GetShellCommand();
		BuildCommand(&cmd, q, u, JString::empty, JString::empty);
		}

	// build window command

	if (type == JFSBinding::kRunInWindow)
		{
		u   = cmd;
		q   = JPrepArgForExec(cmd);
		cmd = me->itsBindingList->GetWindowCommand();
		BuildCommand(&cmd, q, u, JString::empty, JString::empty);
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
	JString*		cmd,
	const JString&	q,
	const JString&	u,
	const JString&	qf,
	const JString&	uf
	)
{
	if (cmd->Contains("$"))
		{
		const JUtf8Byte* map[] =
			{
			"q",  q.GetBytes(),
			"u",  u.GetBytes(),
			"qf", qf.GetBytes(),
			"uf", uf.GetBytes()
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
		itsEditDialog = nullptr;
		}

	else if (sender == itsRunFileDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != nullptr);
		if (info->Successful())
			{
			assert( HasFiles() );

			JFSBinding::CommandType type;
			JBoolean singleFile, saveBinding;
			const JString& cmd = itsRunFileDialog->GetCommand(&type, &singleFile, &saveBinding);

			if (itsIgnoreBindingsFlag && itsRunFileIndex == 1)
				{
				for (JFSBinding* f : *itsFileList)
					{
					f->SetCommand(cmd, type, singleFile);
					}
				}
			else
				{
				JFSBinding* f = itsFileList->GetElement(itsRunFileIndex);
				f->SetCommand(cmd, type, singleFile);
				}

			if (saveBinding)
				{
				JFSBinding* f = itsFileList->GetElement(itsRunFileIndex);
				SaveBinding(f->GetPattern(), cmd, type, singleFile);
				}

			itsRunFileDialog = nullptr;
			ProcessFiles();
			}
		else
			{
			jdelete itsFileList;
			itsFileList      = nullptr;
			itsRunFileDialog = nullptr;
			}
		}

	else if (sender == itsRunScriptDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != nullptr);
		if (info->Successful())
			{
			JFSBinding::CommandType type;
			const JString& cmd = itsRunScriptDialog->GetCommand(&type);
			Exec(itsScriptPath, cmd, type);
			}
		itsRunScriptDialog = nullptr;
		}

	else if (sender == itsUpdateBindingListTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		if (itsEditDialog == nullptr && itsRunFileDialog == nullptr)
			{
			itsBindingList->RevertIfModified();
			}
		else if (itsEditDialog != nullptr)
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
	const JString&					fileName,
	const JString&					cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile
	)
{
	JString root, suffix;
	if (JSplitRootAndSuffix(fileName, &root, &suffix))
		{
		JFSBindingList::CleanFileName(&suffix);		// ignore # and ~ on end
		suffix.Prepend(".");

		itsBindingList->RevertIfModified();			// make sure we Save() latest
		itsBindingList->SetCommand(suffix, cmd, type, singleFile);
		itsBindingList->Save();						// ignore error:  no point in complaining

		if (itsEditDialog != nullptr)
			{
			itsEditDialog->AddBinding(suffix, cmd, type, singleFile);
			}
		}
}

/******************************************************************************
 ComparePatterns (static private)

 ******************************************************************************/

JListT::CompareResult
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

JListT::CompareResult
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

	JListT::CompareResult result =
		JCompareStringsCaseSensitive(const_cast<JString*>(&c1),
									 const_cast<JString*>(&c2));
	if (result == JListT::kFirstEqualSecond)
		{
		result = JCompareIndices(t1, t2);
		}
	return result;
}
