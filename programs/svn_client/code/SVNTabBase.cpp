/******************************************************************************
 SVNTabBase.cc

	BASE CLASS = none

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#include "SVNTabBase.h"
#include "SVNMainDirector.h"
#include "svnGlobals.h"
#include <JXFSBindingManager.h>
#include <JXWebBrowser.h>
#include <JSimpleProcess.h>
#include <JSubstitute.h>
#include <jDirUtil.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kWarnRemoveID = "WarnRemove::SVNTabBase";
static const JCharacter* kWarnRevertID = "WarnRevert::SVNTabBase";

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNTabBase::SVNTabBase
	(
	SVNMainDirector* director
	)
	:
	itsDirector(director)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNTabBase::~SVNTabBase()
{
}

/******************************************************************************
 GetPath

 ******************************************************************************/

const JString&
SVNTabBase::GetPath()
	const
{
	return itsDirector->GetPath();
}

/******************************************************************************
 GetSelectedFilesForDiff (virtual)

 ******************************************************************************/

void
SVNTabBase::GetSelectedFilesForDiff
	(
	JPtrArray<JString>* fullNameList,
	JArray<JIndex>*		revList
	)
{
	revList->RemoveAll();
	return GetSelectedFiles(fullNameList);
}

/******************************************************************************
 OpenFiles (virtual)

 ******************************************************************************/

void
SVNTabBase::OpenFiles()
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list);

	JPtrArray<JString> pathList(JPtrArrayT::kDeleteAll);

	const JSize count = list.GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		JString* s = list.GetElement(i);
		if (JDirectoryExists(*s))
			{
			pathList.Append(s);
			list.RemoveElement(i);
			}
		}

	(JXGetWebBrowser())->ShowFileLocations(pathList);
	JXFSBindingManager::Exec(list);
}

/******************************************************************************
 ShowFiles (virtual)

 ******************************************************************************/

void
SVNTabBase::ShowFiles()
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list);
	(JXGetWebBrowser())->ShowFileLocations(list);
}

/******************************************************************************
 GetBaseRevision (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::GetBaseRevision
	(
	JString* rev
	)
{
	rev->Clear();
	return kJFalse;
}

/******************************************************************************
 CompareEdited

	Most of the time, it doesn't make sense to compare with edited, since
	the edited version is being referenced.  We therefore require a base
	revision.

 ******************************************************************************/

void
SVNTabBase::CompareEdited
	(
	const JCharacter* rev
	)
{
	Compare(rev, kJFalse);
}

/******************************************************************************
 CompareCurrent

 ******************************************************************************/

void
SVNTabBase::CompareCurrent
	(
	const JCharacter* rev
	)
{
	JString r;
	if (!JStringEmpty(rev))
		{
		r = rev;
		if (itsDirector->HasPath())
			{
			r += ":BASE";
			}
		}

	Compare(r, kJFalse);
}

/******************************************************************************
 ComparePrev

 ******************************************************************************/

void
SVNTabBase::ComparePrev
	(
	const JCharacter* revStr
	)
{
	JString r = "PREV";
	if (!JStringEmpty(revStr))
		{
		JString s = revStr;
		JUInt rev;
		if (s.ConvertToUInt(&rev) && rev > 0)
			{
			r  = JString(rev-1, JString::kBase10);
			r += ":";
			r += JString(rev, JString::kBase10);
			}
		else
			{
			r += ":";
			r += revStr;
			}
		}

	Compare(r, kJTrue);
}

/******************************************************************************
 Compare (private)

 ******************************************************************************/

void
SVNTabBase::Compare
	(
	const JString&	rev,
	const JBoolean	isPrev
	)
{
	SVNPrefsManager::Integration type;
	JString cmd;
	const JBoolean hasCmd =
		(SVNGetPrefsManager())->GetCommand(SVNPrefsManager::kDiffCmd, &type, &cmd);

	if (type == SVNPrefsManager::kCodeCrusader)
		{
		ExecuteJCCDiff(rev, isPrev);
		}
	else if (hasCmd)
		{
		JString r = rev;
		if (!r.IsEmpty())
			{
			r.Prepend("-r ");
			}
		ExecuteDiff(cmd, r, isPrev);
		}
}

/******************************************************************************
 ExecuteDiff (private)

 ******************************************************************************/

JBoolean
SVNTabBase::ExecuteDiff
	(
	const JCharacter*	origCmd,
	const JCharacter*	rev,
	const JBoolean		isPrev
	)
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	JArray<JIndex> revList;
	GetSelectedFilesForDiff(&fileList, &revList);
	if (fileList.IsEmpty())
		{
		return kJFalse;
		}

	(JXGetApplication())->DisplayBusyCursor();

	JSubstitute subst;
	subst.DefineVariable("rev_option", rev);
	const JBoolean customPrev = JI2B( isPrev && !revList.IsEmpty() );

	const JSize count = fileList.GetElementCount();
	JString cmd, fullName, r;
	for (JIndex i=1; i<=count; i++)
		{
		cmd      = origCmd;
		fullName = JPrepArgForExec(*(fileList.GetElement(i)));

		if (customPrev)
			{
			const JIndex j = revList.GetElement(i);
			r  = JString(j-1, JString::kBase10);
			r += ":";
			r += JString(j, JString::kBase10);
			subst.DefineVariable("rev_option", r);
			}

		subst.DefineVariable("file_name", fullName);
		subst.Substitute(&cmd);

		if (itsDirector->HasPath())
			{
			JSimpleProcess::Create(itsDirector->GetPath(), cmd, kJTrue);
			}
		else
			{
			JSimpleProcess::Create(cmd, kJTrue);
			}
		}

	return kJTrue;
}

/******************************************************************************
 ExecuteJCCDiff (private)

 ******************************************************************************/

JBoolean
SVNTabBase::ExecuteJCCDiff
	(
	const JString&	rev,
	const JBoolean	isPrev
	)
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	JArray<JIndex> revList;
	GetSelectedFilesForDiff(&fileList, &revList);
	if (fileList.IsEmpty())
		{
		return kJFalse;
		}

	const JBoolean customPrev = JI2B( isPrev && !revList.IsEmpty() );

	const JSize count = fileList.GetElementCount();
	JString cmd, s, fullName;
	JSubstitute subst;
	for (JIndex i=1; i<=count; i++)
		{
		if (customPrev)
			{
			const JIndex j = revList.GetElement(i);
			s  = JString(j-1, JString::kBase10);
			s += ":";
			s += JString(j, JString::kBase10);
			}
		else
			{
			s = rev;
			}
		s += " $file_name";

		fullName = JPrepArgForExec(*(fileList.GetElement(i)));
		subst.DefineVariable("file_name", fullName);
		subst.Substitute(&s);

		if (count == 1)
			{
			s.Prepend(" --svn-diff ");
			}
		else
			{
			s.Prepend(" --svn-diff-silent ");
			}

		cmd += s;
		}

	cmd.Prepend("jcc");

	if (itsDirector->HasPath())
		{
		JSimpleProcess::Create(itsDirector->GetPath(), cmd, kJTrue);
		}
	else
		{
		JSimpleProcess::Create(cmd, kJTrue);
		}

	return kJTrue;
}

/******************************************************************************
 ScheduleForAdd (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::ScheduleForAdd()
{
	if (Execute("svn add $file_name"))
		{
		itsDirector->RefreshStatus();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ScheduleForRemove (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::ScheduleForRemove()
{
	if (Execute("svn remove $file_name", kWarnRemoveID, kJTrue))
		{
		itsDirector->RefreshStatus();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ForceScheduleForRemove (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::ForceScheduleForRemove()
{
	if (Execute("svn remove --force $file_name", kWarnRemoveID, kJTrue))
		{
		itsDirector->RefreshStatus();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Resolved (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::Resolved()
{
	if (Execute("svn resolved $file_name"))
		{
		itsDirector->RefreshStatus();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Commit (virtual)

 ******************************************************************************/

void
SVNTabBase::Commit()
{
	JString cmd = "svn commit $file_name";
	if (Prepare(&cmd, nullptr, kJTrue))
		{
		itsDirector->Commit(cmd);
		}
}

/******************************************************************************
 Revert (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::Revert()
{
	if (Execute("svn revert $file_name", kWarnRevertID, kJTrue))
		{
		itsDirector->RefreshStatus();
		(SVNGetApplication())->ReloadOpenFilesInIDE();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 CreateDirectory (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::CreateDirectory()
{
	return kJFalse;
}

/******************************************************************************
 DuplicateItem (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::DuplicateItem()
{
	return kJFalse;
}

/******************************************************************************
 CreateProperty (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::CreateProperty()
{
	return kJFalse;
}

/******************************************************************************
 SchedulePropertiesForRemove (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::SchedulePropertiesForRemove()
{
	return kJFalse;
}

/******************************************************************************
 Ignore (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::Ignore()
{
	return kJFalse;
}

/******************************************************************************
 CanCheckOutSelection (virtual)

 ******************************************************************************/

JBoolean
SVNTabBase::CanCheckOutSelection()
	const
{
	return kJFalse;
}

/******************************************************************************
 CheckOutSelection (virtual)

 ******************************************************************************/

void
SVNTabBase::CheckOutSelection()
{
}

/******************************************************************************
 Prepare (private)

 ******************************************************************************/

JBoolean
SVNTabBase::Prepare
	(
	JString*			cmd,
	const JCharacter*	warnMsgID,
	const JBoolean		includeDeleted
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list, includeDeleted);
	if (list.IsEmpty())
		{
		return kJFalse;
		}

	if (warnMsgID != nullptr && !(JGetUserNotification())->AskUserNo(JGetString(warnMsgID)))
		{
		return kJFalse;
		}

	const JSize count = list.GetElementCount();
	JString fileList;
	for (JIndex i=1; i<=count; i++)
		{
		fileList += JPrepArgForExec(*(list.GetElement(i)));
		fileList += " ";
		}

	JSubstitute subst;
	subst.DefineVariable("file_name", fileList);
	subst.Substitute(cmd);

	return kJTrue;
}

/******************************************************************************
 Execute (private)

 ******************************************************************************/

JBoolean
SVNTabBase::Execute
	(
	const JCharacter*	origCmd,
	const JCharacter*	warnMsgID,
	const JBoolean		includeDeleted,
	const JBoolean		blocking
	)
{
	JString cmd = origCmd;
	if (!Prepare(&cmd, warnMsgID, includeDeleted))
		{
		return kJFalse;
		}

	if (blocking)
		{
		(JXGetApplication())->DisplayBusyCursor();

		JSimpleProcess* p;
		if (itsDirector->HasPath())
			{
			JSimpleProcess::Create(&p, itsDirector->GetPath(), cmd, kJTrue);
			}
		else
			{
			JSimpleProcess::Create(&p, cmd, kJTrue);
			}
		p->WaitUntilFinished();
		}
	else if (itsDirector->HasPath())
		{
		JSimpleProcess::Create(itsDirector->GetPath(), cmd, kJTrue);
		}
	else
		{
		JSimpleProcess::Create(cmd, kJTrue);
		}

	return kJTrue;
}
