/******************************************************************************
 JVMGetThreadName.h

	Copyright © 2011 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetThreadName
#define _H_JVMGetThreadName

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
