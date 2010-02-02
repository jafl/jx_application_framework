/******************************************************************************
 GPMPrefsManager.h

	Copyright © 2001 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_GPMPrefsManager
#define _H_GPMPrefsManager

#include <JXPrefsManager.h>
#include <JXDocumentManager.h>	// need definition of SafetySaveReason

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

	GPMPrefsManager(JBoolean* isNew);

	virtual ~GPMPrefsManager();

	JString GetPrevVersionStr() const;

	// called by GPMCleanUpBeforeSuddenDeath

	void CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);

private:

	void	SaveAllBeforeDestruct();

	// not allowed

	GPMPrefsManager(const GPMPrefsManager& source);
	const GPMPrefsManager& operator=(const GPMPrefsManager& source);
};

#endif
