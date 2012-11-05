/******************************************************************************
 GDBCheckCoreStatus.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBCheckCoreStatus
#define _H_GDBCheckCoreStatus

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class GDBCheckCoreStatus : public CMCommand
{
public:

	GDBCheckCoreStatus();

	virtual	~GDBCheckCoreStatus();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBCheckCoreStatus(const GDBCheckCoreStatus& source);
	const GDBCheckCoreStatus& operator=(const GDBCheckCoreStatus& source);
};

#endif
