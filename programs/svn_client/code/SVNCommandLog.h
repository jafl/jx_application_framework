/******************************************************************************
 SVNCommandLog.h

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNCommandLog
#define _H_SVNCommandLog

#include "SVNTextBase.h"

class SVNCommandLog : public SVNTextBase
{
public:

	SVNCommandLog(SVNMainDirector* director,  JXTextMenu* editMenu,
				  const JString& cmd, const bool refreshRepo,
				  const bool refreshStatus, const bool reloadOpenFiles,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~SVNCommandLog();

	virtual void	UpdateActionsMenu(JXTextMenu* menu);
	virtual void	RefreshContent();
	virtual void	GetSelectedFiles(JPtrArray<JString>* fullNameList,
									 const bool includeDeleted = false);

protected:

	virtual JError	StartProcess(JProcess** p, int* outFD);

private:

	const JString	itsCmd;
	bool		itsHasRefreshedFlag;
	bool		itsRefreshRepoFlag;
	bool		itsRefreshStatusFlag;
	bool		itsReloadOpenFilesFlag;

private:

	// not allowed

	SVNCommandLog(const SVNCommandLog& source);
	const SVNCommandLog& operator=(const SVNCommandLog& source);
};

#endif
