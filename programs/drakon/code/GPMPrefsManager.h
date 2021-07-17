/******************************************************************************
 GPMPrefsManager.h

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GPMPrefsManager
#define _H_GPMPrefsManager

#include <JXPrefsManager.h>

// Preferences -- do not change ID's once they are assigned

enum
{
	kGPMProgramVersionID = 1,
	kGPMgCSFSetupID,
	kGPMMainDirectorID,
	kGPMMainToolBarID,
	kGPMVersionCheckID
};

class GPMPrefsManager : public JXPrefsManager
{
public:

	GPMPrefsManager(bool* isNew);

	virtual ~GPMPrefsManager();

	JString GetPrevVersionStr() const;

protected:

	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion);
	virtual void	SaveAllBeforeDestruct();

private:

	// not allowed

	GPMPrefsManager(const GPMPrefsManager& source);
	const GPMPrefsManager& operator=(const GPMPrefsManager& source);
};

#endif
