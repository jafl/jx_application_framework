/******************************************************************************
 GDBGetProgramName.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetProgramName
#define _H_GDBGetProgramName

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class GDBGetProgramName : public CMCommand
{
public:

	GDBGetProgramName();

	virtual	~GDBGetProgramName();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBGetProgramName(const GDBGetProgramName& source);
	const GDBGetProgramName& operator=(const GDBGetProgramName& source);
};

#endif
