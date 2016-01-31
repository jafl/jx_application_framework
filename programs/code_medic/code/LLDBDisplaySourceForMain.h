/******************************************************************************
 LLDBDisplaySourceForMain.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBDisplaySourceForMain
#define _H_LLDBDisplaySourceForMain

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMDisplaySourceForMain.h"

class LLDBDisplaySourceForMain : public CMDisplaySourceForMain
{
public:

	LLDBDisplaySourceForMain(CMSourceDirector* sourceDir);

	virtual	~LLDBDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	// not allowed

	LLDBDisplaySourceForMain(const LLDBDisplaySourceForMain& source);
	const LLDBDisplaySourceForMain& operator=(const LLDBDisplaySourceForMain& source);

};

#endif
