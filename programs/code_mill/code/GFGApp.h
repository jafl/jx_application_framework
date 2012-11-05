/******************************************************************************
 GFGApp.h

	Copyright © 2002 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_GFGApp
#define _H_GFGApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>

class GFGApp : public JXApplication
{
public:

	GFGApp(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);

	virtual	~GFGApp();

	void	DisplayAbout(const JCharacter* prevVersStr = NULL);

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
