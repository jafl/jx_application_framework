/******************************************************************************
 GDBGetMemory.h

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetMemory
#define _H_GDBGetMemory

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetMemory.h"

class GDBGetMemory : public CMGetMemory
{
public:

	GDBGetMemory(CMMemoryDir* dir);

	virtual	~GDBGetMemory();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBGetMemory(const GDBGetMemory& source);
	const GDBGetMemory& operator=(const GDBGetMemory& source);
};

#endif
