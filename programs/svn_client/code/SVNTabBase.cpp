/******************************************************************************
 SVNTabBase.cpp

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

bool
SVNTabBase::GetBaseRevision
	(
	JString* rev
	)
{
	rev->Clear();
	return false;
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
	const JString& rev
	)
{
	Compare(rev, false);
}

/******************************************************************************
 CompareCurrent

 ******************************************************************************/

void
SVNTabBase::CompareCurrent
	(
	const JString& rev
	)
{
	JString r;
	if (!rev.IsEmpty())
		{
		r = rev;
		if (itsDirector->HasPath())
			{
			r += ":BASE";
			}
		}

	Compare(r, false);
}

/******************************************************************************
 ComparePrev

 ******************************************************************************/

void
SVNTabBase::ComparePrev
	(
	const JString& revStr
	)
{
	JString r("PREV");
	if (!revStr.IsEmpty())
		{
		JUInt rev;
		if (revStr.ConvertToUInt(&rev) && rev > 0)
			{
			r  = JString((JUInt64) rev-1);
			r += ":";
			r += JString((JUInt64) rev);
			}
		else
			{
			r += ":";
			r += revStr;
			}
		}

	Compare(r, true);
}

/******************************************************************************
 Compare (private)

 ******************************************************************************/

void
SVNTabBase::Compare
	(
	const JString&	rev,
	const bool	isPrev
	)
{
	SVNPrefsManager::Integration type;
	JString cmd;
	const bool hasCmd =
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

bool
SVNTabBase::ExecuteDiff
	(
	const JString&	origCmd,
	const JString&	rev,
	const bool	isPrev
	)
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	JArray<JIndex> revList;
	GetSelectedFilesForDiff(&fileList, &revList);
	if (fileList.IsEmpty())
		{
		return false;
		}

	JXGetApplication()->DisplayBusyCursor();

	JSubstitute subst;
	subst.DefineVariable("rev_option", rev);
	const bool customPrev = isPrev && !revList.IsEmpty();

	const JSize count = fileList.GetElementCount();
	JString cmd, fullName, r;
	for (JIndex i=1; i<=count; i++)
		{
		cmd      = origCmd;
		fullName = JPrepArgForExec(*(fileList.GetElement(i)));

		if (customPrev)
			{
			const JIndex j = revList.GetElement(i);
			r  = JString((JUInt64) j-1);
			r += ":";
			r += JString((JUInt64) j);
			subst.DefineVariable("rev_option", r);
			}

		subst.DefineVariable("file_name", fullName);
		subst.Substitute(&cmd);

		if (itsDirector->HasPath())
			{
			JSimpleProcess::Create(itsDirector->GetPath(), cmd, true);
			}
		else
			{
			JSimpleProcess::Create(cmd, true);
			}
		}

	return true;
}

/******************************************************************************
 ExecuteJCCDiff (private)

 ******************************************************************************/

bool
SVNTabBase::ExecuteJCCDiff
	(
	const JString&	rev,
	const bool	isPrev
	)
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	JArray<JIndex> revList;
	GetSelectedFilesForDiff(&fileList, &revList);
	if (fileList.IsEmpty())
		{
		return false;
		}

	const bool customPrev = isPrev && !revList.IsEmpty();

	const JSize count = fileList.GetElementCount();
	JString cmd, s, fullName;
	JSubstitute subst;
	for (JIndex i=1; i<=count; i++)
		{
		if (customPrev)
			{
			const JIndex j = revList.GetElement(i);
			s  = JString((JUInt64) j-1);
			s += ":";
			s += JString((JUInt64) j);
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
		JSimpleProcess::Create(itsDirector->GetPath(), cmd, true);
		}
	else
		{
		JSimpleProcess::Create(cmd, true);
		}

	return true;
}

/******************************************************************************
 ScheduleForAdd (virtual)

 ******************************************************************************/

bool
SVNTabBase::ScheduleForAdd()
{
	if (Execute(JString("svn add $file_name", JString::kNoCopy)))
		{
		itsDirector->RefreshStatus();
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 ScheduleForRemove (virtual)

 ******************************************************************************/

bool
SVNTabBase::ScheduleForRemove()
{
	if (Execute(JString("svn remove $file_name", JString::kNoCopy), "WarnRemove::SVNTabBase", true))
		{
		itsDirector->RefreshStatus();
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 ForceScheduleForRemove (virtual)

 ******************************************************************************/

bool
SVNTabBase::ForceScheduleForRemove()
{
	if (Execute(JString("svn remove --force $file_name", JString::kNoCopy), "WarnRemove::SVNTabBase", true))
		{
		itsDirector->RefreshStatus();
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 Resolved (virtual)

 ******************************************************************************/

bool
SVNTabBase::Resolved()
{
	if (Execute(JString("svn resolved $file_name", JString::kNoCopy)))
		{
		itsDirector->RefreshStatus();
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 Commit (virtual)

 ******************************************************************************/

void
SVNTabBase::Commit()
{
	JString cmd("svn commit $file_name");
	if (Prepare(&cmd, nullptr, true))
		{
		itsDirector->Commit(cmd);
		}
}

/******************************************************************************
 Revert (virtual)

 ******************************************************************************/

bool
SVNTabBase::Revert()
{
	if (Execute(JString("svn revert $file_name", JString::kNoCopy), "WarnRevert::SVNTabBase", true))
		{
		itsDirector->RefreshStatus();
		(SVNGetApplication())->ReloadOpenFilesInIDE();
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 CreateDirectory (virtual)

 ******************************************************************************/

bool
SVNTabBase::CreateDirectory()
{
	return false;
}

/******************************************************************************
 DuplicateItem (virtual)

 ******************************************************************************/

bool
SVNTabBase::DuplicateItem()
{
	return false;
}

/******************************************************************************
 CreateProperty (virtual)

 ******************************************************************************/

bool
SVNTabBase::CreateProperty()
{
	return false;
}

/******************************************************************************
 SchedulePropertiesForRemove (virtual)

 ******************************************************************************/

bool
SVNTabBase::SchedulePropertiesForRemove()
{
	return false;
}

/******************************************************************************
 Ignore (virtual)

 ******************************************************************************/

bool
SVNTabBase::Ignore()
{
	return false;
}

/******************************************************************************
 CanCheckOutSelection (virtual)

 ******************************************************************************/

bool
SVNTabBase::CanCheckOutSelection()
	const
{
	return false;
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

bool
SVNTabBase::Prepare
	(
	JString*			cmd,
	const JUtf8Byte*	warnMsgID,
	const bool		includeDeleted
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	GetSelectedFiles(&list, includeDeleted);
	if (list.IsEmpty())
		{
		return false;
		}

	if (warnMsgID != nullptr && !JGetUserNotification()->AskUserNo(JGetString(warnMsgID)))
		{
		return false;
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

	return true;
}

/******************************************************************************
 Execute (private)

 ******************************************************************************/

bool
SVNTabBase::Execute
	(
	const JString&		origCmd,
	const JUtf8Byte*	warnMsgID,
	const bool		includeDeleted,
	const bool		blocking
	)
{
	JString cmd = origCmd;
	if (!Prepare(&cmd, warnMsgID, includeDeleted))
		{
		return false;
		}

	if (blocking)
		{
		JXGetApplication()->DisplayBusyCursor();

		JSimpleProcess* p;
		if (itsDirector->HasPath())
			{
			JSimpleProcess::Create(&p, itsDirector->GetPath(), cmd, true);
			}
		else
			{
			JSimpleProcess::Create(&p, cmd, true);
			}
		p->WaitUntilFinished();
		}
	else if (itsDirector->HasPath())
		{
		JSimpleProcess::Create(itsDirector->GetPath(), cmd, true);
		}
	else
		{
		JSimpleProcess::Create(cmd, true);
		}

	return true;
}
