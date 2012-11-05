/******************************************************************************
 XDDisplaySourceForMain.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDDisplaySourceForMain
#define _H_XDDisplaySourceForMain

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMDisplaySourceForMain.h"

class XDDisplaySourceForMain : public CMDisplaySourceForMain
{
public:

	XDDisplaySourceForMain(CMSourceDirector* sourceDir);

	virtual	~XDDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	XDDisplaySourceForMain(const XDDisplaySourceForMain& source);
	const XDDisplaySourceForMain& operator=(const XDDisplaySourceForMain& source);

};

#endif
