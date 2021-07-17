/******************************************************************************
 <PRE>PrefsManager.h

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#ifndef _H_<PRE>PrefsManager
#define _H_<PRE>PrefsManager

#include <JXPrefsManager.h>

class JPoint;
class JXWindow;
class <PRE>PrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	k<PRE>ProgramVersionID = 1,
	k<PRE>VersionCheckID,
	k<PRE>gCSFSetupID,
	k<PRE>MainDirectorID,
	k<PRE>MainToolBarID,
	k<PRE>MainDirectorWindSizeID
};

class <PRE>PrefsManager : public JXPrefsManager
{
public:

	<PRE>PrefsManager(bool* isNew);

	virtual	~<PRE>PrefsManager();

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	bool	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
							  JCoordinate* width, JCoordinate* height) const;
	void		SaveWindowSize(const JPrefID& id, JXWindow* window);

protected:

	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion);
	virtual void	SaveAllBeforeDestruct();
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	<PRE>PrefsDialog*	itsPrefsDialog;

private:

	// not allowed

	<PRE>PrefsManager(const <PRE>PrefsManager& source);
	const <PRE>PrefsManager& operator=(const <PRE>PrefsManager& source);
};

#endif
