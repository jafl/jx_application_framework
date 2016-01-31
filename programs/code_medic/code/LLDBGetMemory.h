/******************************************************************************
 LLDBGetMemory.h

	Copyright � 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetMemory
#define _H_LLDBGetMemory

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetMemory.h"

class LLDBGetMemory : public CMGetMemory
{
public:

	LLDBGetMemory(CMMemoryDir* dir);

	virtual	~LLDBGetMemory();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetMemory(const LLDBGetMemory& source);
	const LLDBGetMemory& operator=(const LLDBGetMemory& source);
};

#endif
