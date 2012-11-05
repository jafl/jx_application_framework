/******************************************************************************
 GDBGetStopLocationForAsm.h

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationForAsm
#define _H_GDBGetStopLocationForAsm

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "GDBGetStopLocation.h"

class GDBGetStopLocationForAsm : public GDBGetStopLocation
{
public:

	GDBGetStopLocationForAsm();

	virtual	~GDBGetStopLocationForAsm();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBGetStopLocationForAsm(const GDBGetStopLocationForAsm& source);
	const GDBGetStopLocationForAsm& operator=(const GDBGetStopLocationForAsm& source);
};

#endif
