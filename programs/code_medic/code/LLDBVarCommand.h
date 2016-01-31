/******************************************************************************
 LLDBVarCommand.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBVarCommand
#define _H_LLDBVarCommand

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMVarCommand.h"

class LLDBVarCommand : public CMVarCommand
{
public:

	LLDBVarCommand(const JCharacter* cmd);

	virtual	~LLDBVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBVarCommand(const LLDBVarCommand& source);
	const LLDBVarCommand& operator=(const LLDBVarCommand& source);
};

#endif
