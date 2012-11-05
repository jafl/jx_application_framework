/******************************************************************************
 JVMDisplaySourceForMain.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMDisplaySourceForMain
#define _H_JVMDisplaySourceForMain

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMDisplaySourceForMain.h"

class JVMDisplaySourceForMain : public CMDisplaySourceForMain
{
public:

	JVMDisplaySourceForMain(CMSourceDirector* sourceDir);

	virtual	~JVMDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	JVMDisplaySourceForMain(const JVMDisplaySourceForMain& source);
	const JVMDisplaySourceForMain& operator=(const JVMDisplaySourceForMain& source);

};

#endif
