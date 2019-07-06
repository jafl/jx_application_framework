/******************************************************************************
 JVMGetCompletions.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JVMGetCompletions
#define _H_JVMGetCompletions

#include "CMGetCompletions.h"

class CMCommandInput;
class CMCommandOutputDisplay;

class JVMGetCompletions : public CMGetCompletions
{
public:

	JVMGetCompletions(CMCommandInput* input, CMCommandOutputDisplay* history);

	virtual	~JVMGetCompletions();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMGetCompletions(const JVMGetCompletions& source);
	const JVMGetCompletions& operator=(const JVMGetCompletions& source);
};

#endif
