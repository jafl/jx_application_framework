/******************************************************************************
 LLDBGetRegisters.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetRegisters
#define _H_LLDBGetRegisters

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetRegisters.h"

class LLDBGetRegisters : public CMGetRegisters
{
public:

	LLDBGetRegisters(CMRegistersDir* dir);

	virtual	~LLDBGetRegisters();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetRegisters(const LLDBGetRegisters& source);
	const LLDBGetRegisters& operator=(const LLDBGetRegisters& source);
};

#endif
