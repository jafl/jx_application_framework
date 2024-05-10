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
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

JXFSBindingManager* JXFSBindingManager::itsSelf = nullptr;

const Time kUpdateInterval = 1000;	// milliseconds

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool initSelf = false;

JXFSBindingManager*
JXFSBindingManager::Instance()
{
	if (itsSelf == nullptr && !initSelf)
	{
		initSelf = true;

		JString needUserCheck;
		itsSelf = jnew JXFSBindingManager(&needUserCheck);

		initSelf = false;

		if (!needUserCheck.IsEmpty())
		{
			JScheduleTask([needUserCheck]()
			{
				EditBindings();
				JGetUserNotification()->DisplayMessage(needUserCheck);
				Instance()->itsBindingList->Save();	// only display message once
			});
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

	initSelf = false;
}

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

JXFSBindingManager::JXFSBindingManager
	(
	JString* needUserCheck
	)
	:
	itsRunFileDialog(nullptr),
	itsEditDialog(nullptr)
{
	itsBindingList = JFSBindingList::Create(needUserCheck);

	itsUpdateBindingListTask = jnew JXFunctionTask(kUpdateInterval, [this]()
	{
		if (itsEditDialog == nullptr && itsRunFileDialog == nullptr)
		{
			itsBindingList->RevertIfModified();
		}
		else if (itsEditDialog != nullptr)
		{
			itsEditDialog->CheckIfNeedRevert();
		}
	},
	"JXFSBindingManager::UpdateBindingListTask");
	itsUpdateBindingListTask->Start();

	itsRunCmdDialog = jnew JXFSRunCommandDialog;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXFSBindingManager::~JXFSBindingManager()
{
	jdelete itsBindingList;
	jdelete itsUpdateBindingListTask;
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
	}

	me->itsEditDialog->Activate();
	me->ClearWhenGoingAway(me->itsEditDialog, &me->itsEditDialog);
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
	const bool		askForArgs
	)
{
	JString path, cmd;
	JSplitPathAndName(fullProgramName, &path, &cmd);

	#ifdef _J_UNIX
	cmd.Prepend("./");
	#endif

	if (askForArgs)
	{
		auto* dlog = jnew JXFSRunScriptDialog(cmd);
		if (dlog->DoDialog())
		{
			JFSBinding::CommandType type;
			cmd = dlog->GetCommand(&type);
			Exec(path, cmd, type);
		}
	}
	else
	{
		JXGetApplication()->DisplayBusyCursor();
		Exec(path, cmd, JFSBinding::kRunPlain);
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
	const bool					ignoreBindings
	)
{
	if (fileList.IsEmpty())
	{
		return;
	}

	JPtrArray<JFSBinding> bindingList(JPtrArrayT::kDeleteAll);
	bindingList.SetCompareFunction(ComparePatterns);

	for (const auto* fileName : fileList)
	{
		auto* f = jnew JFSBinding(*fileName, JString::empty, JFSBinding::kRunPlain, true, false);
		if (!bindingList.InsertSorted(f, false))
		{
			jdelete f;
		}
	}

	Instance()->ProcessFiles(&bindingList, ignoreBindings);
}

/******************************************************************************
 ProcessFiles (private)

 ******************************************************************************/

void
JXFSBindingManager::ProcessFiles
	(
	JPtrArray<JFSBinding>*	fileList,
	const bool				ignoreBindings
	)
{
	if (itsRunFileDialog != nullptr)	// paranoia
	{
		return;
	}

	// check for file with no command

	JIndex i = 1;
	for (auto* f : *fileList)
	{
		const JString& fileName = f->GetPattern();

		JFSBinding::CommandType type;
		bool singleFile;
		const JString& cmd = f->GetCommand(&type, &singleFile);

		const JFSBinding* b;
		if (!ignoreBindings &&
			cmd.IsEmpty() && itsBindingList->GetBinding(fileName, &b))
		{
			const JString& c = b->GetCommand(&type, &singleFile);
			f->SetCommand(c, type, singleFile);
		}
		else if (cmd.IsEmpty())
		{
			itsRunFileDialog =
				jnew JXFSRunFileDialog(fileName,
					fileList->GetItemCount() <= 1 || !ignoreBindings);
			if (itsRunFileDialog->DoDialog())
			{
				bool saveBinding;
				const JString& cmd = itsRunFileDialog->GetCommand(&type, &singleFile, &saveBinding);

				if (ignoreBindings && i == 1)
				{
					for (auto* f1 : *fileList)
					{
						f1->SetCommand(cmd, type, singleFile);
					}
				}
				else
				{
					f->SetCommand(cmd, type, singleFile);
				}

				if (saveBinding)
				{
					SaveBinding(f->GetPattern(), cmd, type, singleFile);
				}

				itsRunFileDialog = nullptr;
			}
			else
			{
				itsRunFileDialog = nullptr;
				return;
			}
		}

		i++;
	}

	// exec one-at-a-time cmds

	JXGetApplication()->DisplayBusyCursor();

	for (i=fileList->GetItemCount(); i>=1; i--)
	{
		JFSBinding::CommandType t;
		bool singleFile;
		fileList->GetItem(i)->GetCommand(&t, &singleFile);

		if (singleFile)
		{
			Exec(*fileList, i, i);
			fileList->DeleteItem(i);
		}
	}

	// group remaining files and exec

	if (!fileList->IsEmpty())
	{
		fileList->SetCompareFunction(CompareCommands);
		fileList->Sort();

		JIndex startIndex = 1;
		JString cmd;
		JFSBinding::CommandType type = JFSBinding::kRunPlain;

		i = 1;
		for (const auto* f : *fileList)
		{
			JFSBinding::CommandType t;
			bool singleFile;
			const JString& c = f->GetCommand(&t, &singleFile);
			assert( !singleFile );

			if (i == startIndex)
			{
				cmd  = c;
				type = t;
			}
			else if (c != cmd || type != t)
			{
				Exec(*fileList, startIndex, i-1);

				startIndex = i;
				cmd        = c;
				type       = t;
			}

			i++;
		}

		Exec(*fileList, startIndex, fileList->GetItemCount());
	}
}

/******************************************************************************
 Exec (private)

	The directory and command *must* be the same for all items in
	fileList between startIndex and endIndex, inclusive.

 ******************************************************************************/

void
JXFSBindingManager::Exec
	(
	const JPtrArray<JFSBinding>&	fileList,
	const JIndex					startIndex,
	const JIndex					endIndex
	)
{
	// check if same path for all files

	bool samePath = true;

	JString path, name;
	if (endIndex > startIndex)
	{
		JString p;
		JIndex i = 1;
		for (auto* f : fileList)
		{
			JSplitPathAndName(f->GetPattern(), &p, &name);
			if (i > startIndex && p != path)
			{
				samePath = false;
				break;
			}
			path = p;
			i++;
		}
	}

	// build $q, $u, $qf, $uf

	JString q, u, qf, uf;
	for (auto* f : fileList)
	{
		const JString& fullName = f->GetPattern();

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

	JFSBinding* f = fileList.GetItem(startIndex);
	JSplitPathAndName(f->GetPattern(), &path, &name);

	JFSBinding::CommandType type;
	bool singleFile;
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

	return JSimpleProcess::Create(path, cmd, true);
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
		JGetStringManager()->Replace(cmd, map, sizeof(map));
	}
	else
	{
		*cmd += " ";
		*cmd += qf;
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
	const bool						singleFile
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

std::weak_ordering
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

std::weak_ordering
JXFSBindingManager::CompareCommands
	(
	JFSBinding* const & b1,
	JFSBinding* const & b2
	)
{
	JFSBinding::CommandType t1, t2;
	bool s1, s2;
	const JString& c1 = b1->GetCommand(&t1, &s1);
	const JString& c2 = b2->GetCommand(&t2, &s2);

	auto result =
		JCompareStringsCaseSensitive(const_cast<JString*>(&c1),
									 const_cast<JString*>(&c2));
	if (result == std::weak_ordering::equivalent)
	{
		result = t1 <=> t2;
	}
	return result;
}
