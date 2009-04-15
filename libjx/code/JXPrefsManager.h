/******************************************************************************
 JXPrefsManager.h

	Copyright © 2000 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPrefsManager
#define _H_JXPrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPrefsManager.h>

class JXPrefsManager : public JPrefsManager
{
public:

	JXPrefsManager(const JFileVersion currentVersion,
				   const JBoolean eraseFileIfOpen);

	virtual ~JXPrefsManager();

private:

	// not allowed

	JXPrefsManager(const JXPrefsManager& source);
	JXPrefsManager& operator=(const JXPrefsManager& source);
};

#endif
