/******************************************************************************
 JVMGetLocalVars.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetLocalVars
#define _H_JVMGetLocalVars

#include "CMGetLocalVars.h"

class CMVarNode;

class JVMGetLocalVars : public CMGetLocalVars
{
public:

	JVMGetLocalVars(CMVarNode* rootNode);

	virtual	~JVMGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	CMVarNode*	itsRootNode;	// not owned

private:

	// not allowed

	JVMGetLocalVars(const JVMGetLocalVars& source);
	const JVMGetLocalVars& operator=(const JVMGetLocalVars& source);
};

#endif
