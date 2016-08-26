/******************************************************************************
 SyGApplication.h

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

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

	SyGApplication(int* argc, char* argv[], JBoolean* displayAbout,
				   JString* prevVersStr);

	virtual ~SyGApplication();

	virtual JBoolean	Close();

	JBoolean	OpenDirectory();
	JBoolean	OpenDirectory(const JString& pathName,
							  SyGTreeDir** dir = NULL, JIndex* row = NULL,
							  const JBoolean deiconify = kJTrue,
							  const JBoolean reportError = kJTrue,
							  const JBoolean forceNew = kJTrue,
							  const JBoolean clearSelection = kJTrue);

	JSize	GetWindowCount() const;
	void	GetWindowNames(JPtrArray<JString>* names);

	void	UpdateShortcutMenu(JXTextMenu* menu) const;
	void	LoadToolBarDefaults(JXToolBar* toolBar, JXTextMenu* shorcutMenu,
								const JIndex firstIndex);
	void	OpenShortcut(const JIndex index);
	void	AddShortcut(const JString& shortcut);
	void	RemoveShortcut(const JString& shortcut);
	void	RemoveAllShortcuts();

	JBoolean	IsMountPoint(const JCharacter* path, JMountType* type = NULL) const;
	JBoolean	GetMountPointPrefs(const JCharacter* path, const JString** prefs) const;
	void		SetMountPointPrefs(const JCharacter* path, const JCharacter* prefs);

	void	DirectoryRenamed(const Message& message);
	void	UpdateTrash();

	void			OpenTerminal(const JCharacter* path);
	const JString&	GetTerminalCommand() const;
	void			SetTerminalCommand(const JCharacter* cmd);

	const JString&	GetGitStatusCommand() const;
	void			SetGitStatusCommand(const JCharacter* cmd);

	const JString&	GetGitHistoryCommand() const;
	void			SetGitHistoryCommand(const JCharacter* cmd);

	const JString&	GetPostCheckoutCommand() const;
	void			SetPostCheckoutCommand(const JCharacter* cmd);

	void	DisplayAbout(const JCharacter* prevVersStr = NULL);

	JBoolean	RestoreProgramState();
	void		SaveProgramState();

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	DirectorClosed(JXDirector* theDirector);
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

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

	const JCharacter*	GetNMShortcut(JIndex* i) const;

	void	UpdateMountPointList() const;
	JString	GetMountPointPrefsPath(const JCharacter* path) const;

	static JOrderedSetT::CompareResult
		CompareShortcuts(JString* const & s1, JString* const & s2);

	// not allowed

	SyGApplication(const SyGApplication& source);
	const SyGApplication& operator=(const SyGApplication& source);

public:

	static const JCharacter* kTrashNeedsUpdate;
	static const JCharacter* kShortcutsChanged;

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
	const JCharacter* cmd
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
	const JCharacter* cmd
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
	const JCharacter* cmd
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
	const JCharacter* cmd
	)
{
	itsPostCheckoutCmd = cmd;
	itsPostCheckoutCmd.TrimWhitespace();
}

#endif
