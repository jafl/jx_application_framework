/******************************************************************************
 MDApp.h

	Copyright (C) 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_MDApp
#define _H_MDApp

#include <JXApplication.h>

class MDApp : public JXApplication
{
public:

	MDApp(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);

	virtual	~MDApp();

	void	DisplayAbout(const JCharacter* prevVersStr = NULL);

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	// not allowed

	MDApp(const MDApp& source);
	const MDApp& operator=(const MDApp& source);
};

#endif
