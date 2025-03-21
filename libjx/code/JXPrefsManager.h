/******************************************************************************
 JXPrefsManager.h

	Copyright (C) 2000 John Lindal.

 ******************************************************************************/

#ifndef _H_JXPrefsManager
#define _H_JXPrefsManager

#include <jx-af/jcore/JPrefsManager.h>
#include "JXDocumentManager.h"	// for SafetySaveReason

class JXPrefsManager : public JPrefsManager
{
public:

	JXPrefsManager(const JFileVersion currentVersion,
				   const bool eraseFileIfOpen,
				   const JPrefID& csfID);

	~JXPrefsManager() override;

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

protected:

	void			DataLoaded() override;
	virtual void	SaveAllBeforeDestruct();

private:

	JXIdleTask*		itsSafetySaveTask;
	const JPrefID	itsCSFPrefID;
};

#endif
