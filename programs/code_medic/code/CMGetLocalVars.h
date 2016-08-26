/******************************************************************************
 CMGetLocalVars.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetLocalVars
#define _H_CMGetLocalVars

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
