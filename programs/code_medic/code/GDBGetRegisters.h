/******************************************************************************
 GDBGetRegisters.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetRegisters
#define _H_GDBGetRegisters

#include "CMGetRegisters.h"

class GDBGetRegisters : public CMGetRegisters
{
public:

	GDBGetRegisters(CMRegistersDir* dir);

	virtual	~GDBGetRegisters();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	GDBGetRegisters(const GDBGetRegisters& source);
	const GDBGetRegisters& operator=(const GDBGetRegisters& source);
};

#endif
