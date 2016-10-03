/******************************************************************************
 GDBVarCommand.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBVarCommand
#define _H_GDBVarCommand

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
