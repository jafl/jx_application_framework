/******************************************************************************
 GDBVarCommand.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBVarCommand
#define _H_GDBVarCommand

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMVarCommand.h"

class GDBVarCommand : public CMVarCommand
{
public:

	GDBVarCommand(const JCharacter* cmd);

	virtual	~GDBVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBVarCommand(const GDBVarCommand& source);
	const GDBVarCommand& operator=(const GDBVarCommand& source);
};

#endif
