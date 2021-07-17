/******************************************************************************
 SVNMainDirector.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_SVNMainDirector
#define _H_SVNMainDirector

#include <JXWindowDirector.h>
#include <JPrefObject.h>
#include <JString.h>

class JProcess;
class JXContainer;
class JXScrollbarSet;
class JXTextMenu;
class JXToolBar;
class JXGetStringDialog;
class SVNTabGroup;
class SVNTabBase;
class SVNRepoView;
class SVNGetRepoDialog;
class SVNStatusList;
class SVNRefreshStatusTask;

class SVNMainDirector : public JXWindowDirector
{
public:

	SVNMainDirector(JXDirector* supervisor, const JString& path);
	SVNMainDirector(JXDirector* supervisor, std::istream& input, const JFileVersion vers);

	virtual	~SVNMainDirector();

	const JString&	GetPath() const;	// explodes if !HasPath()
	bool		HasPath() const;
	bool		GetPath(JString* path) const;
	bool		GetRepoPath(JString* path) const;
	void			RefreshRepo();
	void			BrowseRepo(const JString& rev);
	bool		GetRepoWidget(SVNRepoView** widget);
	void			RefreshStatus();
	void			ScheduleStatusRefresh();
	void			UpdateWorkingCopy();
	void			Commit(const JString& cmd);
	void			ShowInfoLog(SVNTabBase* tab);
	void			ShowInfoLog(const JString& fullName,
								const JString& rev = JString::empty);
	void			ShowProperties(SVNTabBase* tab);
	void			ShowProperties(const JString& fullName);

	bool	OKToStartActionProcess() const;
	void		RegisterActionProcess(SVNTabBase* tab, JProcess* p,
									  const bool refreshRepo,
									  const bool refreshStatus,
									  const bool reload);

	static void	CheckOut(const JString& url);

	void	Execute(const JUtf8Byte* tabStringID, const JString& cmd,
					const bool refreshRepo, const bool refreshStatus,
					const bool reloadOpenFiles);

	void	StreamOut(std::ostream& output);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JString	itsPath;

	JProcess*	itsActionProcess;
	JProcess*	itsCheckOutProcess;
	bool	itsRefreshRepoFlag;
	bool	itsRefreshStatusFlag;
	bool	itsReloadOpenFilesFlag;

	SVNTabGroup*			itsTabGroup;
	SVNRepoView*			itsRepoWidget;		// can be nullptr
	SVNStatusList*			itsStatusWidget;	// can be nullptr
	JPtrArray<SVNTabBase>*	itsTabList;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsEditMenu;
	JXTextMenu*	itsActionsMenu;
	JXTextMenu*	itsInfoMenu;
	JXTextMenu*	itsPrefsMenu;
	JXTextMenu*	itsHelpMenu;

	SVNGetRepoDialog*	itsBrowseRepoDialog;
	JXGetStringDialog*	itsBrowseRepoRevisionDialog;
	SVNGetRepoDialog*	itsCheckOutRepoDialog;

	SVNRefreshStatusTask*	itsRefreshStatusTask;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void			SVNMainDirectorX();
	void			BuildWindow();
	JXScrollbarSet*	BuildScrollbarSet(JXContainer* widget);
	void			UpdateWindowTitle(const JString& title);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	CheckOut();
	void	CreateStatusTab();

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	void	CleanUpWorkingCopy();
	void	CommitAll();
	void	RevertAll();

	void	UpdateInfoMenu();
	void	HandleInfoMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex index);

	// not allowed

	SVNMainDirector(const SVNMainDirector& source);
	const SVNMainDirector& operator=(const SVNMainDirector& source);
};


/******************************************************************************
 HasPath

 ******************************************************************************/

inline bool
SVNMainDirector::HasPath()
	const
{
	return !itsPath.IsEmpty();
}

/******************************************************************************
 GetPath

 ******************************************************************************/

inline bool
SVNMainDirector::GetPath
	(
	JString* path
	)
	const
{
	*path = itsPath;
	return !itsPath.IsEmpty();
}

/******************************************************************************
 GetRepoWidget

 ******************************************************************************/

inline bool
SVNMainDirector::GetRepoWidget
	(
	SVNRepoView** widget
	)
{
	*widget = itsRepoWidget;
	return itsRepoWidget != nullptr;
}

/******************************************************************************
 OKToStartActionProcess

 ******************************************************************************/

inline bool
SVNMainDirector::OKToStartActionProcess()
	const
{
	return itsActionProcess == nullptr;
}

#endif
