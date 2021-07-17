/******************************************************************************
 SyGApplication.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_SyGApplication
#define _H_SyGApplication

#include <JXApplication.h>
#include <JPrefObject.h>
#include <JStringPtrMap.h>
#include <jMountUtil.h>

class JMountPointList;
class JXWindowDirector;
class JXTextMenu;
class JXToolBar;
class SyGTreeDir;

class SyGApplication : public JXApplication, public JPrefObject
{
public:

	SyGApplication(int* argc, char* argv[], bool* displayAbout,
				   JString* prevVersStr);

	virtual ~SyGApplication();

	virtual bool	Close() override;

	bool	OpenDirectory();
	bool	OpenDirectory(const JString& pathName,
							  SyGTreeDir** dir = nullptr, JIndex* row = nullptr,
							  const bool deiconify = true,
							  const bool reportError = true,
							  const bool forceNew = true,
							  const bool clearSelection = true);

	JSize	GetWindowCount() const;
	void	GetWindowNames(JPtrArray<JString>* names);

	void	UpdateShortcutMenu(JXTextMenu* menu) const;
	void	LoadToolBarDefaults(JXToolBar* toolBar, JXTextMenu* shorcutMenu,
								const JIndex firstIndex);
	void	OpenShortcut(const JIndex index);
	void	AddShortcut(const JString& shortcut);
	void	RemoveShortcut(const JString& shortcut);
	void	RemoveAllShortcuts();

	bool	IsMountPoint(const JString& path, JMountType* type = nullptr) const;
	bool	GetMountPointPrefs(const JString& path, const JString** prefs) const;
	void		SetMountPointPrefs(const JString& path, const JString& prefs);

	void	DirectoryRenamed(const Message& message);
	void	UpdateTrash();

	void			OpenTerminal(const JString& path);
	const JString&	GetTerminalCommand() const;
	void			SetTerminalCommand(const JString& cmd);

	const JString&	GetGitStatusCommand() const;
	void			SetGitStatusCommand(const JString& cmd);

	const JString&	GetGitHistoryCommand() const;
	void			SetGitHistoryCommand(const JString& cmd);

	const JString&	GetPostCheckoutCommand() const;
	void			SetPostCheckoutCommand(const JString& cmd);

	void	DisplayAbout(const JString& prevVersStr = JString::empty);

	bool	RestoreProgramState();
	void		SaveProgramState();

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	DirectorClosed(JXDirector* theDirector) override;
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<SyGTreeDir>*	itsWindowList;
	JMountPointList*		itsMountPointList;
	mutable JMountState		itsMountPointState;
	JPtrArray<JString>*		itsShortcutList;
	JStringPtrMap<JString>*	itsMountPointPrefs;

	JString	itsTermCmd;
	JString	itsGitStatusCmd;
	JString	itsGitHistoryCmd;
	JString	itsPostCheckoutCmd;

private:

	JString	GetNMShortcut(JIndex* i) const;

	void	UpdateMountPointList() const;
	JString	GetMountPointPrefsPath(const JString& path) const;

	static JListT::CompareResult
		CompareShortcuts(JString* const & s1, JString* const & s2);

	// not allowed

	SyGApplication(const SyGApplication& source);
	const SyGApplication& operator=(const SyGApplication& source);

public:

	static const JUtf8Byte* kTrashNeedsUpdate;
	static const JUtf8Byte* kShortcutsChanged;

	class TrashNeedsUpdate : public JBroadcaster::Message
		{
		public:

			TrashNeedsUpdate()
				:
				JBroadcaster::Message(kTrashNeedsUpdate)
			{ };
		};

	class ShortcutsChanged : public JBroadcaster::Message
		{
		public:

			ShortcutsChanged()
				:
				JBroadcaster::Message(kShortcutsChanged)
			{ };
		};
};


/******************************************************************************
 GetWindowCount

 ******************************************************************************/

inline JSize
SyGApplication::GetWindowCount()
	const
{
	return itsWindowList->GetElementCount();
}

/******************************************************************************
 RemoveAllShortcuts

 ******************************************************************************/

inline void
SyGApplication::RemoveAllShortcuts()
{
	itsShortcutList->CleanOut();
}

/******************************************************************************
 Terminal command

 ******************************************************************************/

inline const JString&
SyGApplication::GetTerminalCommand()
	const
{
	return itsTermCmd;
}

inline void
SyGApplication::SetTerminalCommand
	(
	const JString& cmd
	)
{
	itsTermCmd = cmd;
	itsTermCmd.TrimWhitespace();
}

/******************************************************************************
 Git status

 ******************************************************************************/

inline const JString&
SyGApplication::GetGitStatusCommand()
	const
{
	return itsGitStatusCmd;
}

inline void
SyGApplication::SetGitStatusCommand
	(
	const JString& cmd
	)
{
	itsGitStatusCmd = cmd;
	itsGitStatusCmd.TrimWhitespace();
}

/******************************************************************************
 Git history

 ******************************************************************************/

inline const JString&
SyGApplication::GetGitHistoryCommand()
	const
{
	return itsGitHistoryCmd;
}

inline void
SyGApplication::SetGitHistoryCommand
	(
	const JString& cmd
	)
{
	itsGitHistoryCmd = cmd;
	itsGitHistoryCmd.TrimWhitespace();
}

/******************************************************************************
 Post-checkout command

 ******************************************************************************/

inline const JString&
SyGApplication::GetPostCheckoutCommand()
	const
{
	return itsPostCheckoutCmd;
}

inline void
SyGApplication::SetPostCheckoutCommand
	(
	const JString& cmd
	)
{
	itsPostCheckoutCmd = cmd;
	itsPostCheckoutCmd.TrimWhitespace();
}

#endif
