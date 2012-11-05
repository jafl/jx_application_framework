/******************************************************************************
 GDBGetRegisters.h

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetRegisters
#define _H_GDBGetRegisters

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetRegisters.h"

class GDBGetRegisters : public CMGetRegisters
{
public:

	GDBGetRegisters(CMRegistersDir* dir);

	virtual	~GDBGetRegisters();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBGetRegisters(const GDBGetRegisters& source);
	const GDBGetRegisters& operator=(const GDBGetRegisters& source);
};

#endif
