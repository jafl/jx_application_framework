/******************************************************************************
 CMApp.h

	Interface for the CMApp class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CMApp
#define _H_CMApp

#include <JXApplication.h>

class CMApp : public JXApplication
{
public:

	CMApp(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);

	virtual	~CMApp();

	void	EditFile(const JCharacter* fileName, const JIndex lineIndex = 0) const;

	void	DisplayAbout(const JCharacter* prevVersStr = nullptr,
						 const JBoolean init = kJFalse);

	static void					InitStrings();
	static const JCharacter*	GetAppSignature();

protected:

	virtual JBoolean	HandleCustomEvent();
	virtual void		CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	// not allowed

	CMApp(const CMApp& source);
	const CMApp& operator=(const CMApp& source);
};

#endif
