/******************************************************************************
 JVMGetThreadParent.h

	Copyright (C) 2011 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetThreadParent
#define _H_JVMGetThreadParent

#include "CMCommand.h"
#include <JBroadcaster.h>

class JVMThreadNode;

class JVMGetThreadParent : public CMCommand, virtual public JBroadcaster
{
public:

	JVMGetThreadParent(JVMThreadNode* node, const JBoolean checkOnly = kJFalse);

	virtual	~JVMGetThreadParent();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);
	virtual void	HandleFailure();

private:

	JVMThreadNode*	itsNode;
	const JBoolean	itsCheckOnlyFlag;

private:

	// not allowed

	JVMGetThreadParent(const JVMGetThreadParent& source);
	const JVMGetThreadParent& operator=(const JVMGetThreadParent& source);

};

#endif
