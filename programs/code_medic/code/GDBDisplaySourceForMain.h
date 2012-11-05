/******************************************************************************
 GDBDisplaySourceForMain.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBDisplaySourceForMain
#define _H_GDBDisplaySourceForMain

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMDisplaySourceForMain.h"

class GDBDisplaySourceForMain : public CMDisplaySourceForMain
{
public:

	GDBDisplaySourceForMain(CMSourceDirector* sourceDir);

	virtual	~GDBDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsHasCoreFlag;
	JIndex		itsNextCmdIndex;

private:

	// not allowed

	GDBDisplaySourceForMain(const GDBDisplaySourceForMain& source);
	const GDBDisplaySourceForMain& operator=(const GDBDisplaySourceForMain& source);

};

#endif
