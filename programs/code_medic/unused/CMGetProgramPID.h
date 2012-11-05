/******************************************************************************
 CMGetProgramPID.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetProgramPID
#define _H_CMGetProgramPID

#include "CMCommand.h"

class CMGetProgramPID : public CMCommand
{
public:

	CMGetProgramPID();

	virtual	~CMGetProgramPID();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	CMGetProgramPID(const CMGetProgramPID& source);
	const CMGetProgramPID& operator=(const CMGetProgramPID& source);
};

#endif
