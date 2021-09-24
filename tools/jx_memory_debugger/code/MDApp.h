/******************************************************************************
 MDApp.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_MDApp
#define _H_MDApp

#include <JXApplication.h>

class JString;

class MDApp : public JXApplication
{
public:

	MDApp(int* argc, char* argv[], bool* displayAbout, JString* prevVersStr);

	virtual	~MDApp();

	void	DisplayAbout(const JString& prevVersStr = JString::empty);
	void	OpenFile(const JString& fileName, const JSize lineIndex);

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);
};

#endif
