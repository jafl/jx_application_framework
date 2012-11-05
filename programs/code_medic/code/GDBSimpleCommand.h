/******************************************************************************
 GDBSimpleCommand.h

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBSimpleCommand
#define _H_GDBSimpleCommand

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class GDBSimpleCommand : public CMCommand
{
public:

	GDBSimpleCommand(const JCharacter* cmd);

	virtual	~GDBSimpleCommand();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBSimpleCommand(const GDBSimpleCommand& source);
	const GDBSimpleCommand& operator=(const GDBSimpleCommand& source);
};

#endif
