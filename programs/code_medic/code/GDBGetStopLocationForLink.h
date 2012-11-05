/******************************************************************************
 GDBGetStopLocationForLink.h

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationForLink
#define _H_GDBGetStopLocationForLink

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "GDBGetStopLocation.h"

class GDBGetStopLocationForLink : public GDBGetStopLocation
{
public:

	GDBGetStopLocationForLink();

	virtual	~GDBGetStopLocationForLink();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBGetStopLocationForLink(const GDBGetStopLocationForLink& source);
	const GDBGetStopLocationForLink& operator=(const GDBGetStopLocationForLink& source);
};

#endif
