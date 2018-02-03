/******************************************************************************
 THXPrefsManager.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_THXPrefsManager
#define _H_THXPrefsManager

#include <JXPrefsManager.h>

// Preferences -- do not change ID's once they are assigned

enum
{
	kTHXVersionCheckID = 1
};

class THXPrefsManager : public JXPrefsManager
{
public:

	THXPrefsManager(JBoolean* isNew);

	virtual	~THXPrefsManager();

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);

private:

	// not allowed

	THXPrefsManager(const THXPrefsManager& source);
	const THXPrefsManager& operator=(const THXPrefsManager& source);
};

#endif
