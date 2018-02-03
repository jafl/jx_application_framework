/******************************************************************************
 JVMGetThreadName.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadName
#define _H_JVMGetThreadName

#include "CMCommand.h"
#include <JBroadcaster.h>

class JVMThreadNode;

class JVMGetThreadName : public CMCommand, virtual public JBroadcaster
{
public:

	JVMGetThreadName(JVMThreadNode* node);

	virtual	~JVMGetThreadName();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);
	virtual void	HandleFailure();

private:

	JVMThreadNode*	itsNode;

private:

	// not allowed

	JVMGetThreadName(const JVMGetThreadName& source);
	const JVMGetThreadName& operator=(const JVMGetThreadName& source);

};

#endif
