/******************************************************************************
 PrefsManager.h

	Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#ifndef _H_PrefsManager
#define _H_PrefsManager

#include <jx-af/jx/JXPrefsManager.h>

class JString;

// Preferences -- do not change ID's once they are assigned

enum
{
	kProgramVersionID = 1,
	kgCSFSetupID,
	kMainDlogID
};

class PrefsManager : public JXPrefsManager
{
public:

	PrefsManager(bool* isNew);

	~PrefsManager() override;

	JString GetPrevVersionStr() const;

protected:

	void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	void	SaveAllBeforeDestruct() override;
};

#endif
