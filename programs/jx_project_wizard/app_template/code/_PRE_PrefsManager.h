/******************************************************************************
 <PRE>PrefsManager.h

	Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#ifndef _H_<PRE>PrefsManager
#define _H_<PRE>PrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPrefsManager.h>
#include <JXDocumentManager.h>	// need definition of SafetySaveReason

class JPoint;
class JXWindow;
class <PRE>PrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	k<PRE>ProgramVersionID = 1,
	k<PRE>gCSFSetupID,
	k<PRE>MainDirectorID,
	k<PRE>MainToolBarID,
	k<PRE>MainDirectorWindSizeID,
	k<PRE>VersionCheckID
};

class <PRE>PrefsManager : public JXPrefsManager
{
public:

	<PRE>PrefsManager(JBoolean* isNew);

	virtual	~<PRE>PrefsManager();

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	JBoolean	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
							  JCoordinate* width, JCoordinate* height) const;
	void		SaveWindowSize(const JPrefID& id, JXWindow* window);

	// called by <PRE>CleanUpBeforeSuddenDeath

	void CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	<PRE>PrefsDialog*	itsPrefsDialog;

private:

	void	SaveAllBeforeDestruct();

	// not allowed

	<PRE>PrefsManager(const <PRE>PrefsManager& source);
	const <PRE>PrefsManager& operator=(const <PRE>PrefsManager& source);
};

#endif
