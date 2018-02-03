/******************************************************************************
 GDBGetStopLocation.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocation
#define _H_GDBGetStopLocation

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
