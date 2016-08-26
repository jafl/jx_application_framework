/******************************************************************************
 CMDisplaySourceForMain.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMDisplaySourceForMain
#define _H_CMDisplaySourceForMain

#include "CMCommand.h"
#include <JBroadcaster.h>

class CMSourceDirector;

class CMDisplaySourceForMain : public CMCommand, virtual public JBroadcaster
{
public:

	CMDisplaySourceForMain(CMSourceDirector* sourceDir, const JCharacter* cmd);

	virtual	~CMDisplaySourceForMain();

	CMSourceDirector*	GetSourceDir();

private:

	CMSourceDirector*	itsSourceDir;		// not owned

private:

	// not allowed

	CMDisplaySourceForMain(const CMDisplaySourceForMain& source);
	const CMDisplaySourceForMain& operator=(const CMDisplaySourceForMain& source);

};


/******************************************************************************
 GetSourceDir

 ******************************************************************************/

inline CMSourceDirector*
CMDisplaySourceForMain::GetSourceDir()
{
	return itsSourceDir;
}

#endif
