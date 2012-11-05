/******************************************************************************
 JVMGetInitArgs.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetInitArgs
#define _H_JVMGetInitArgs

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetInitArgs.h"

class JXInputField;

class JVMGetInitArgs : public CMGetInitArgs
{
public:

	JVMGetInitArgs(JXInputField* argInput);

	virtual	~JVMGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	JVMGetInitArgs(const JVMGetInitArgs& source);
	const JVMGetInitArgs& operator=(const JVMGetInitArgs& source);

};

#endif
