/******************************************************************************
 SymcirApp.h

	Interface for the SymcirApp class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_SymcirApp
#define _H_SymcirApp

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
