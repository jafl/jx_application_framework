/******************************************************************************
 GFGApp.h

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GFGApp
#define _H_GFGApp

#include <JXApplication.h>

class GFGApp : public JXApplication
{
public:

	GFGApp(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);

	virtual	~GFGApp();

	void	DisplayAbout(const JCharacter* prevVersStr = nullptr);

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

	JBoolean		IsDeletingTemplate() const;
	void			ShouldBeDeletingTemplate(const JBoolean delTemplate);

protected:

	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	JBoolean	itsDeletingTemplate;

private:

	// not allowed

	GFGApp(const GFGApp& source);
	const GFGApp& operator=(const GFGApp& source);
};

#endif
