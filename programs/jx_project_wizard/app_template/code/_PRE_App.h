/******************************************************************************
 <PRE>App.h

	Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#ifndef _H_<PRE>App
#define _H_<PRE>App

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
