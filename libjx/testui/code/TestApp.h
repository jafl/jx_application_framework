/******************************************************************************
 TestApp.h

	Interface for the TestApp class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestApp
#define _H_TestApp

#include <jx-af/jx/JXApplication.h>

class TestDirector;

class TestApp : public JXApplication
{
public:

	TestApp(int* argc, char* argv[], const bool wantMDI);

	virtual ~TestApp();

	void			OpenDocuments();
	bool	Close() override;

	void	DisplayAbout(JXDisplay* display);

	bool	GetMainDirector(TestDirector** dir) const;
	void	SetMainDirector(TestDirector* dir);

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason) override;

	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	TestDirector*	itsMainDirector;	// deleted by JXDirector
};


/******************************************************************************
 GetMainDirector

 ******************************************************************************/

inline bool
TestApp::GetMainDirector
	(
	TestDirector** dir
	)
	const
{
	*dir = itsMainDirector;
	return *dir != nullptr;
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
