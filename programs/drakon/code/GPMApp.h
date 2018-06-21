/******************************************************************************
 GPMApp.h

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GPMApp
#define _H_GPMApp

#include <JXApplication.h>

class GPMApp : public JXApplication
{
public:

	GPMApp(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);

	virtual ~GPMApp();

	void	DisplayAbout(const JCharacter* prevVersStr = nullptr);

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	// not allowed

	GPMApp(const GPMApp& source);
	const GPMApp& operator=(const GPMApp& source);

};

#endif
