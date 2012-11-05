/******************************************************************************
 JVMGetLocalVars.h

	Copyright � 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetLocalVars
#define _H_JVMGetLocalVars

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetLocalVars.h"

class CMVarNode;

class JVMGetLocalVars : public CMGetLocalVars
{
public:

	JVMGetLocalVars(CMVarNode* rootNode);

	virtual	~JVMGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMVarNode*	itsRootNode;	// not owned

private:

	// not allowed

	JVMGetLocalVars(const JVMGetLocalVars& source);
	const JVMGetLocalVars& operator=(const JVMGetLocalVars& source);
};

#endif
