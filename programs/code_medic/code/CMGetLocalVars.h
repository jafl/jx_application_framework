/******************************************************************************
 CMGetLocalVars.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetLocalVars
#define _H_CMGetLocalVars

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMGetLocalVars : public CMCommand
{
public:

	CMGetLocalVars(const JCharacter* cmd);

	virtual	~CMGetLocalVars();

private:

	// not allowed

	CMGetLocalVars(const CMGetLocalVars& source);
	const CMGetLocalVars& operator=(const CMGetLocalVars& source);
};

#endif
