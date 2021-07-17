/******************************************************************************
 SVNApp.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_SVNApp
#define _H_SVNApp

#include <JXApplication.h>

class SVNApp : public JXApplication
{
public:

	SVNApp(int* argc, char* argv[], bool* displayAbout, JString* prevVersStr);

	virtual	~SVNApp();

	void	DisplayAbout(const JString& prevVersStr = JString::empty);

	void	ReloadOpenFilesInIDE();

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	virtual bool	Close();
	virtual void		CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	// not allowed

	SVNApp(const SVNApp& source);
	const SVNApp& operator=(const SVNApp& source);
};

#endif
