/******************************************************************************
 MDApp.h

	Copyright © 2010 by John Lindal. All rights reserved.

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
	void	OpenFile(const JCharacter* fileName, const JSize lineIndex);

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
