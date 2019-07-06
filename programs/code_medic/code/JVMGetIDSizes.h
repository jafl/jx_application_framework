/******************************************************************************
 JVMGetIDSizes.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetIDSizes
#define _H_JVMGetIDSizes

#include "CMCommand.h"

class JVMGetIDSizes : public CMCommand
{
public:

	JVMGetIDSizes();

	virtual	~JVMGetIDSizes();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMGetIDSizes(const JVMGetIDSizes& source);
	const JVMGetIDSizes& operator=(const JVMGetIDSizes& source);

};

#endif
