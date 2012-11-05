/******************************************************************************
 CMGetFrame.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetFrame
#define _H_CMGetFrame

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMGetFrame : public CMCommand
{
public:

	CMGetFrame(const JCharacter* cmd);

	virtual	~CMGetFrame();

private:

	// not allowed

	CMGetFrame(const CMGetFrame& source);
	const CMGetFrame& operator=(const CMGetFrame& source);
};

#endif
