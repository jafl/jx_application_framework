/******************************************************************************
 GDBGetStopLocation.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocation
#define _H_GDBGetStopLocation

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMLocation;

class GDBGetStopLocation : public CMCommand
{
public:

	GDBGetStopLocation();

	virtual	~GDBGetStopLocation();

protected:

	CMLocation	GetLocation() const;

private:

	// not allowed

	GDBGetStopLocation(const GDBGetStopLocation& source);
	const GDBGetStopLocation& operator=(const GDBGetStopLocation& source);
};

#endif
