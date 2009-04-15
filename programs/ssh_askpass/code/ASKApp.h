/******************************************************************************
 ASKApp.h

	Copyright © 2006 by New Planet Software, Inc.. All rights reserved.

 *****************************************************************************/

#ifndef _H_ASKApp
#define _H_ASKApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>

class ASKApp : public JXApplication
{
public:

	ASKApp(int* argc, char* argv[]);

	virtual	~ASKApp();

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

private:

	// not allowed

	ASKApp(const ASKApp& source);
	const ASKApp& operator=(const ASKApp& source);
};

#endif
