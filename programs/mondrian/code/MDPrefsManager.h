/******************************************************************************
 MDPrefsManager.h

	Copyright © 2008 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_MDPrefsManager
#define _H_MDPrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPrefsManager.h>
#include <JXDocumentManager.h>	// need definition of SafetySaveReason

class JPoint;
class JXWindow;
class MDPrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	kMDProgramVersionID = 1,
	kMDgCSFSetupID,
	kMDMainDirectorID,
	kMDMainToolBarID,
	kMDMainDirectorWindSizeID
};

class MDPrefsManager : public JXPrefsManager
{
public:

	MDPrefsManager(JBoolean* isNew);

	virtual	~MDPrefsManager();

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	JBoolean	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
							  JCoordinate* width, JCoordinate* height) const;
	void		SaveWindowSize(const JPrefID& id, JXWindow* window);

	// called by MDCleanUpBeforeSuddenDeath

	void CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	MDPrefsDialog*	itsPrefsDialog;

private:

	void	SaveAllBeforeDestruct();

	// not allowed

	MDPrefsManager(const MDPrefsManager& source);
	const MDPrefsManager& operator=(const MDPrefsManager& source);
};

#endif
