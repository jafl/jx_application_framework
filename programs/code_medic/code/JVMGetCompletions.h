/******************************************************************************
 JVMGetCompletions.h

	Copyright © 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JVMGetCompletions
#define _H_JVMGetCompletions

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetCompletions.h"

class CMCommandInput;
class CMCommandOutputDisplay;

class JVMGetCompletions : public CMGetCompletions
{
public:

	JVMGetCompletions(CMCommandInput* input, CMCommandOutputDisplay* history);

	virtual	~JVMGetCompletions();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	JVMGetCompletions(const JVMGetCompletions& source);
	const JVMGetCompletions& operator=(const JVMGetCompletions& source);
};

#endif
