/******************************************************************************
 CMGetAssembly.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetAssembly
#define _H_CMGetAssembly

#include "CMCommand.h"

class CMSourceDirector;

class CMGetAssembly : public CMCommand
{
public:

	CMGetAssembly(CMSourceDirector* dir, const JString& cmd);

	virtual	~CMGetAssembly();

	CMSourceDirector*	GetDirector();

protected:

	virtual void	HandleFailure();

private:

	CMSourceDirector*	itsDir;

private:

	// not allowed

	CMGetAssembly(const CMGetAssembly& source);
	const CMGetAssembly& operator=(const CMGetAssembly& source);
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline CMSourceDirector*
CMGetAssembly::GetDirector()
{
	return itsDir;
}

#endif
