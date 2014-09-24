/******************************************************************************
 SVNCommandLog.cc

	BASE CLASS = SVNTextBase

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <svnStdInc.h>
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
	const JCharacter*	cmd,
	const JBoolean		refreshRepo,
	const JBoolean		refreshStatus,
	const JBoolean		reloadOpenFiles,
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
	itsHasRefreshedFlag(kJFalse),
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
		itsHasRefreshedFlag = kJTrue;
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
	if (!(GetDirector())->OKToStartActionProcess())
		{
		return JWouldHaveBlocked();
		}

	JError err = JNoError();
	if ((GetDirector())->HasPath())
		{
		err = JProcess::Create(p, GetPath(), itsCmd,
							   kJIgnoreConnection, NULL,
							   kJCreatePipe, outFD,
							   kJAttachToFromFD);
		}
	else
		{
		err = JProcess::Create(p, itsCmd,
							   kJIgnoreConnection, NULL,
							   kJCreatePipe, outFD,
							   kJAttachToFromFD);
		}

	if (err.OK())
		{
		(GetDirector())->RegisterActionProcess(this, *p, itsRefreshRepoFlag,
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
	const JBoolean		includeDeleted
	)
{
	fullNameList->CleanOut();
}
