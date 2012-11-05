/******************************************************************************
 JVMVarCommand.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMVarCommand
#define _H_JVMVarCommand

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMVarCommand.h"

class JVMVarCommand : public CMVarCommand
{
public:

	JVMVarCommand(const JCharacter* cmd);

	virtual	~JVMVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	JVMVarCommand(const JVMVarCommand& source);
	const JVMVarCommand& operator=(const JVMVarCommand& source);
};

#endif
