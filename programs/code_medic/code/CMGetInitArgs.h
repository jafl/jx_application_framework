/******************************************************************************
 CMGetInitArgs.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetInitArgs
#define _H_CMGetInitArgs

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMGetInitArgs : public CMCommand
{
public:

	CMGetInitArgs(const JCharacter* cmd);

	virtual	~CMGetInitArgs();

private:

	// not allowed

	CMGetInitArgs(const CMGetInitArgs& source);
	const CMGetInitArgs& operator=(const CMGetInitArgs& source);

};

#endif
