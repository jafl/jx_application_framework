/******************************************************************************
 CMGetAssembly.h

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetAssembly
#define _H_CMGetAssembly

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMSourceDirector;

class CMGetAssembly : public CMCommand
{
public:

	CMGetAssembly(CMSourceDirector* dir, const JCharacter* cmd);

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
