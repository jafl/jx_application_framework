/******************************************************************************
 JXWPrefsManager.h

	Copyright © 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_JXWPrefsManager
#define _H_JXWPrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPrefsManager.h>

class JString;

// Preferences -- do not change ID's once they are assigned

enum
{
	kJXWProgramVersionID = 1,
	kJXWgCSFSetupID,
	kJXWMainDlogID
};

class JXWPrefsManager : public JXPrefsManager
{
public:

	JXWPrefsManager(JBoolean* isNew);

	virtual ~JXWPrefsManager();

	JString GetPrevVersionStr() const;

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	SaveAllBeforeDestruct();

private:

	// not allowed

	JXWPrefsManager(const JXWPrefsManager& source);
	const JXWPrefsManager& operator=(const JXWPrefsManager& source);
};

#endif
