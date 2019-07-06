/******************************************************************************
 GDBGetStopLocationForAsm.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationForAsm
#define _H_GDBGetStopLocationForAsm

#include "GDBGetStopLocation.h"

class GDBGetStopLocationForAsm : public GDBGetStopLocation
{
public:

	GDBGetStopLocationForAsm();

	virtual	~GDBGetStopLocationForAsm();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	GDBGetStopLocationForAsm(const GDBGetStopLocationForAsm& source);
	const GDBGetStopLocationForAsm& operator=(const GDBGetStopLocationForAsm& source);
};

#endif
