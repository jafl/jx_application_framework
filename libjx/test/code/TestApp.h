/******************************************************************************
 TestApp.h

	Interface for the TestApp class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestApp
#define _H_TestApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>

class TestDirector;

class TestApp : public JXApplication
{
public:

	TestApp(int* argc, char* argv[], const JBoolean wantMDI);

	virtual ~TestApp();

	void	DisplayAbout(JXDisplay* display);

	JBoolean	GetMainDirector(TestDirector** dir) const;
	void		SetMainDirector(TestDirector* dir);

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

private:

	TestDirector*	itsMainDirector;	// deleted by JXDirector

private:

	// not allowed

	TestApp(const TestApp& source);
	const TestApp& operator=(const TestApp& source);
};


/******************************************************************************
 GetMainDirector

 ******************************************************************************/

inline JBoolean
TestApp::GetMainDirector
	(
	TestDirector** dir
	)
	const
{
	*dir = itsMainDirector;
	return JConvertToBoolean( *dir != NULL );
}

/******************************************************************************
 SetMainDirector

 ******************************************************************************/

inline void
TestApp::SetMainDirector
	(
	TestDirector* dir
	)
{
	itsMainDirector = dir;
}

#endif
