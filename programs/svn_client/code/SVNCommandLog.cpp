/******************************************************************************
 SVNCommandLog.cpp

	BASE CLASS = SVNTextBase

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#include "SVNCommandLog.h"
#include "SVNMainDirector.h"
#include <JProcess.h>
#include <JStdError.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNCommandLog::SVNCommandLog
	(
	SVNMainDirector*	director,
	JXTextMenu*			editMenu,
	const JString&		cmd,
	const bool		refreshRepo,
	const bool		refreshStatus,
	const bool		reloadOpenFiles,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	SVNTextBase(director, editMenu, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	itsCmd(cmd),
	itsHasRefreshedFlag(false),
	itsRefreshRepoFlag(refreshRepo),
	itsRefreshStatusFlag(refreshStatus),
	itsReloadOpenFilesFlag(reloadOpenFiles)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNCommandLog::~SVNCommandLog()
{
}

/******************************************************************************
 RefreshContent (virtual)

 ******************************************************************************/

void
SVNCommandLog::RefreshContent()
{
	if (!itsHasRefreshedFlag)
	{
		SVNTextBase::RefreshContent();
		itsHasRefreshedFlag = true;
	}
}

/******************************************************************************
 StartProcess (virtual protected)

 ******************************************************************************/

JError
SVNCommandLog::StartProcess
	(
	JProcess**	p,
	int*		outFD
	)
{
	if (!GetDirector()->OKToStartActionProcess())
		{
		return JWouldHaveBlocked();
		}

	JError err = JNoError();
	if (GetDirector()->HasPath())
		{
		err = JProcess::Create(p, GetPath(), itsCmd,
							   kJIgnoreConnection, nullptr,
							   kJCreatePipe, outFD,
							   kJAttachToFromFD);
		}
	else
		{
		err = JProcess::Create(p, itsCmd,
							   kJIgnoreConnection, nullptr,
							   kJCreatePipe, outFD,
							   kJAttachToFromFD);
		}

	if (err.OK())
		{
		GetDirector()->RegisterActionProcess(this, *p, itsRefreshRepoFlag,
											   itsRefreshStatusFlag, itsReloadOpenFilesFlag);
		}

	return err;
}

/******************************************************************************
 UpdateActionsMenu (virtual)

 ******************************************************************************/

void
SVNCommandLog::UpdateActionsMenu
	(
	JXTextMenu* menu
	)
{
}

/******************************************************************************
 GetSelectedFiles (virtual)

 ******************************************************************************/

void
SVNCommandLog::GetSelectedFiles
	(
	JPtrArray<JString>*	fullNameList,
	const bool		includeDeleted
	)
{
	fullNameList->CleanOut();
}
