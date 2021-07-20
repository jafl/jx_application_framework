/******************************************************************************
 <PRE>App.h

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#ifndef _H_<PRE>App
#define _H_<PRE>App

#include <JXApplication.h>

class <PRE>App : public JXApplication
{
public:

	<PRE>App(int* argc, char* argv[], bool* displayAbout, JString* prevVersStr);

	virtual	~<PRE>App();

	void	DisplayAbout(const JString& prevVersStr = JString::empty);

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	// not allowed

	<PRE>App(const <PRE>App& source);
	const <PRE>App& operator=(const <PRE>App& source);
};

#endif
