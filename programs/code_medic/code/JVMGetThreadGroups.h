/******************************************************************************
 JVMGetThreadGroups.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadGroups
#define _H_JVMGetThreadGroups

#include "CMCommand.h"
#include <JBroadcaster.h>

class JTreeNode;
class JVMThreadNode;

class JVMGetThreadGroups : public CMCommand, virtual public JBroadcaster
{
public:

	JVMGetThreadGroups(JTreeNode* root, JVMThreadNode* parent);

	virtual	~JVMGetThreadGroups();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JTreeNode*		itsRoot;
	JVMThreadNode*	itsParent;	// can be nullptr

private:

	// not allowed

	JVMGetThreadGroups(const JVMGetThreadGroups& source);
	const JVMGetThreadGroups& operator=(const JVMGetThreadGroups& source);

};

#endif
