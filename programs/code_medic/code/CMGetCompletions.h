/******************************************************************************
 CMGetCompletions.h

	Copyright © 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_CMGetCompletions
#define _H_CMGetCompletions

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMGetCompletions : public CMCommand
{
public:

	CMGetCompletions(const JString& cmd);

	virtual	~CMGetCompletions();

private:

	// not allowed

	CMGetCompletions(const CMGetCompletions& source);
	const CMGetCompletions& operator=(const CMGetCompletions& source);
};

#endif
