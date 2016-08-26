/******************************************************************************
 LLDBVarCommand.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBVarCommand
#define _H_LLDBVarCommand

#include "CMVarCommand.h"

class LLDBVarCommand : public CMVarCommand
{
public:

	LLDBVarCommand(const JCharacter* cmd);

	virtual	~LLDBVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	JString	itsExpr;

private:

	// not allowed

	LLDBVarCommand(const LLDBVarCommand& source);
	const LLDBVarCommand& operator=(const LLDBVarCommand& source);
};

#endif
