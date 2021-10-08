/******************************************************************************
 JXPrefsManager.h

	Copyright (C) 2000 John Lindal.

 ******************************************************************************/

#ifndef _H_JXPrefsManager
#define _H_JXPrefsManager

#include <jx-af/jcore/JPrefsManager.h>
#include "jx-af/jx/JXDocumentManager.h"	// need definition of SafetySaveReason

class JXPrefsManager : public JPrefsManager
{
public:

	JXPrefsManager(const JFileVersion currentVersion,
				   const bool eraseFileIfOpen);

	~JXPrefsManager();

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

protected:

	virtual void	SaveAllBeforeDestruct();
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTimerTask*	itsSafetySaveTask;
};

#endif
