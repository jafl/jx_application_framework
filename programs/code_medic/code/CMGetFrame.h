/******************************************************************************
 CMGetFrame.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetFrame
#define _H_CMGetFrame

#include "CMCommand.h"

class CMGetFrame : public CMCommand
{
public:

	CMGetFrame(const JString& cmd);

	virtual	~CMGetFrame();

private:

	// not allowed

	CMGetFrame(const CMGetFrame& source);
	const CMGetFrame& operator=(const CMGetFrame& source);
};

#endif
