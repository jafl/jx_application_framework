/******************************************************************************
 TestApp.h

	Interface for the TestApp class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestApp
#define _H_TestApp

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

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason) override;

	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

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
	return JConvertToBoolean( *dir != nullptr );
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
