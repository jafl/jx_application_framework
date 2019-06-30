/******************************************************************************
 CMGetRegisters.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetRegisters
#define _H_CMGetRegisters

#include "CMCommand.h"

class CMRegistersDir;

class CMGetRegisters : public CMCommand
{
public:

	CMGetRegisters(const JString& cmd, CMRegistersDir* dir);

	virtual	~CMGetRegisters();

	CMRegistersDir*	GetDirector();

protected:

	virtual void	HandleFailure();

private:

	CMRegistersDir*	itsDir;

private:

	// not allowed

	CMGetRegisters(const CMGetRegisters& source);
	const CMGetRegisters& operator=(const CMGetRegisters& source);
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline CMRegistersDir*
CMGetRegisters::GetDirector()
{
	return itsDir;
}

#endif
