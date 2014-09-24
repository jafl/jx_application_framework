/******************************************************************************
 SVNCommandLog.h

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNCommandLog
#define _H_SVNCommandLog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SVNTextBase.h"

class SVNCommandLog : public SVNTextBase
{
public:

	SVNCommandLog(SVNMainDirector* director,  JXTextMenu* editMenu,
				  const JCharacter* cmd, const JBoolean refreshRepo,
				  const JBoolean refreshStatus, const JBoolean reloadOpenFiles,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~SVNCommandLog();

	virtual void	UpdateActionsMenu(JXTextMenu* menu);
	virtual void	RefreshContent();
	virtual void	GetSelectedFiles(JPtrArray<JString>* fullNameList,
									 const JBoolean includeDeleted = kJFalse);

protected:

	virtual JError	StartProcess(JProcess** p, int* outFD);

private:

	const JString	itsCmd;
	JBoolean		itsHasRefreshedFlag;
	JBoolean		itsRefreshRepoFlag;
	JBoolean		itsRefreshStatusFlag;
	JBoolean		itsReloadOpenFilesFlag;

private:

	// not allowed

	SVNCommandLog(const SVNCommandLog& source);
	const SVNCommandLog& operator=(const SVNCommandLog& source);
};

#endif
