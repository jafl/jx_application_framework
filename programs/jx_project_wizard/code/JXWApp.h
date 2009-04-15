/******************************************************************************
 JXWApp.h

	Copyright © 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_JXWApp
#define _H_JXWApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>

class JXWApp : public JXApplication
{
public:

	JXWApp(int* argc, char* argv[], JBoolean* displayLicense);

	virtual ~JXWApp();

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	// not allowed

	JXWApp(const JXWApp& source);
	const JXWApp& operator=(const JXWApp& source);

};

#endif
