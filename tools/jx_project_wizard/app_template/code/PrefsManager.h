/******************************************************************************
 PrefsManager.h

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#ifndef _H_PrefsManager
#define _H_PrefsManager

#include <jx-af/jx/JXPrefsManager.h>

class JPoint;
class JXWindow;

// Preferences -- do not change ID's once they are assigned

enum
{
	kProgramVersionID = 1,
	kVersionCheckID,
	kgCSFSetupID,
	kMainDirectorID,
	kMainToolBarID,
	kMainDirectorWindSizeID
};

class PrefsManager : public JXPrefsManager
{
public:

	PrefsManager(bool* isNew);

	~PrefsManager() override;

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	bool	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
						  JCoordinate* width, JCoordinate* height) const;
	void	SaveWindowSize(const JPrefID& id, JXWindow* window);

protected:

	void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	void	SaveAllBeforeDestruct() override;
};

#endif
