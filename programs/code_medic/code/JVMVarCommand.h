/******************************************************************************
 JVMVarCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMVarCommand
#define _H_JVMVarCommand

#include "CMVarCommand.h"

class JVMVarCommand : public CMVarCommand
{
public:

	JVMVarCommand(const JString& cmd);

	virtual	~JVMVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMVarCommand(const JVMVarCommand& source);
	const JVMVarCommand& operator=(const JVMVarCommand& source);
};

#endif
