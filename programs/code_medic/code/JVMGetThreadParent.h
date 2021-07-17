/******************************************************************************
 JVMGetThreadParent.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadParent
#define _H_JVMGetThreadParent

#include "CMCommand.h"
#include <JBroadcaster.h>

class JVMThreadNode;

class JVMGetThreadParent : public CMCommand, virtual public JBroadcaster
{
public:

	JVMGetThreadParent(JVMThreadNode* node, const bool checkOnly = false);

	virtual	~JVMGetThreadParent();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	HandleFailure() override;

private:

	JVMThreadNode*	itsNode;
	const bool	itsCheckOnlyFlag;

private:

	// not allowed

	JVMGetThreadParent(const JVMGetThreadParent& source);
	const JVMGetThreadParent& operator=(const JVMGetThreadParent& source);

};

#endif
