/******************************************************************************
 SymcirApp.h

	Interface for the SymcirApp class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SymcirApp
#define _H_SymcirApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>

class SymcirApp : public JXApplication
{
public:

	SymcirApp(int* argc, char* argv[]);

	virtual ~SymcirApp();

	void	DisplayAbout(JXDisplay* display);

private:

	// not allowed

	SymcirApp(const SymcirApp& source);
	const SymcirApp& operator=(const SymcirApp& source);
};

#endif
