/******************************************************************************
 JVMGetInitArgs.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetInitArgs
#define _H_JVMGetInitArgs

#include "CMGetInitArgs.h"

class JXInputField;

class JVMGetInitArgs : public CMGetInitArgs
{
public:

	JVMGetInitArgs(JXInputField* argInput);

	virtual	~JVMGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMGetInitArgs(const JVMGetInitArgs& source);
	const JVMGetInitArgs& operator=(const JVMGetInitArgs& source);

};

#endif
