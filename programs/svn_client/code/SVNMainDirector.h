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

	SVNMainDirector(JXDirector* supervisor, const JCharacter* path);
	SVNMainDirector(JXDirector* supervisor, std::istream& input, const JFileVersion vers);

	virtual	~SVNMainDirector();

	const JString&	GetPath() const;	// explodes if !HasPath()
	JBoolean		HasPath() const;
	JBoolean		GetPath(JString* path) const;
	JBoolean		GetRepoPath(JString* path) const;
	void			RefreshRepo();
	void			BrowseRepo(const JCharacter* rev);
	JBoolean		GetRepoWidget(SVNRepoView** widget);
	void			RefreshStatus();
	void			ScheduleStatusRefresh();
	void			UpdateWorkingCopy();
	void			Commit(const JCharacter* cmd);
	void			ShowInfoLog(SVNTabBase* tab);
	void			ShowInfoLog(const JCharacter* fullName,
								const JCharacter* rev = nullptr);
	void			ShowProperties(SVNTabBase* tab);
	void			ShowProperties(const JCharacter* fullName);

	JBoolean	OKToStartActionProcess() const;
	void		RegisterActionProcess(SVNTabBase* tab, JProcess* p,
									  const JBoolean refreshRepo,
									  const JBoolean refreshStatus,
									  const JBoolean reload);

	static void	CheckOut(const JCharacter* url);

	void	Execute(const JCharacter* tabStringID, const JCharacter* cmd,
					const JBoolean refreshRepo, const JBoolean refreshStatus,
					const JBoolean reloadOpenFiles);

	void	StreamOut(std::ostream& output);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JString	itsPath;

	JProcess*	itsActionProcess;
	JProcess*	itsCheckOutProcess;
	JBoolean	itsRefreshRepoFlag;
	JBoolean	itsRefreshStatusFlag;
	JBoolean	itsReloadOpenFilesFlag;

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
	void			UpdateWindowTitle(const JCharacter* title);

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

inline JBoolean
SVNMainDirector::HasPath()
	const
{
	return JNegate( itsPath.IsEmpty() );
}

/******************************************************************************
 GetPath

 ******************************************************************************/

inline JBoolean
SVNMainDirector::GetPath
	(
	JString* path
	)
	const
{
	*path = itsPath;
	return JNegate( itsPath.IsEmpty() );
}

/******************************************************************************
 GetRepoWidget

 ******************************************************************************/

inline JBoolean
SVNMainDirector::GetRepoWidget
	(
	SVNRepoView** widget
	)
{
	*widget = itsRepoWidget;
	return JI2B( itsRepoWidget != nullptr );
}

/******************************************************************************
 OKToStartActionProcess

 ******************************************************************************/

inline JBoolean
SVNMainDirector::OKToStartActionProcess()
	const
{
	return JI2B( itsActionProcess == nullptr );
}

#endif
