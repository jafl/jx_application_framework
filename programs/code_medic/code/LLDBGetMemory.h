/******************************************************************************
 LLDBGetMemory.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetMemory
#define _H_LLDBGetMemory

#include "CMGetMemory.h"

class LLDBGetMemory : public CMGetMemory
{
public:

	LLDBGetMemory(CMMemoryDir* dir);

	virtual	~LLDBGetMemory();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetMemory(const LLDBGetMemory& source);
	const LLDBGetMemory& operator=(const LLDBGetMemory& source);
};

#endif
