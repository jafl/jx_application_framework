/******************************************************************************
 CMGetMemory.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetMemory
#define _H_CMGetMemory

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
