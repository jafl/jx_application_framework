/******************************************************************************
 <PRE>App.h

	Copyright (C) <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#ifndef _H_<PRE>App
#define _H_<PRE>App

#include <JXApplication.h>

class <PRE>App : public JXApplication
{
public:

	<PRE>App(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);

	virtual	~<PRE>App();

	void	DisplayAbout(const JCharacter* prevVersStr = NULL);

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	// not allowed

	<PRE>App(const <PRE>App& source);
	const <PRE>App& operator=(const <PRE>App& source);
};

#endif
