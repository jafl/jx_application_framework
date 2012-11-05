/******************************************************************************
 SVNApp.h

	Copyright © 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_SVNApp
#define _H_SVNApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>

class SVNApp : public JXApplication
{
public:

	SVNApp(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);

	virtual	~SVNApp();

	void	DisplayAbout(const JCharacter* prevVersStr = NULL);

	void	ReloadOpenFilesInIDE();

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual JBoolean	Close();
	virtual void		CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	// not allowed

	SVNApp(const SVNApp& source);
	const SVNApp& operator=(const SVNApp& source);
};

#endif
