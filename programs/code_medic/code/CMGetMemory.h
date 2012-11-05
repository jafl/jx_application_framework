/******************************************************************************
 CMGetMemory.h

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetMemory
#define _H_CMGetMemory

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMMemoryDir;

class CMGetMemory : public CMCommand
{
public:

	CMGetMemory(CMMemoryDir* dir);

	virtual	~CMGetMemory();

	CMMemoryDir*	GetDirector();

protected:

	virtual void	HandleFailure();

private:

	CMMemoryDir*	itsDir;

private:

	// not allowed

	CMGetMemory(const CMGetMemory& source);
	const CMGetMemory& operator=(const CMGetMemory& source);
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline CMMemoryDir*
CMGetMemory::GetDirector()
{
	return itsDir;
}

#endif
